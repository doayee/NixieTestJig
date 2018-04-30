/*******************************************************************************
 * File:                    Testing.cpp
 * Project:                 NTD Test Jig
 * Date:                    27/04/2018
 * Author:                  Thomas Cousins
 * Target:                  Arduino Uno
 * Copyright:               Doayee 2018
 * Description:             Handles the test functionality of the NTD. Please
 * 							see [insert doc here] for detailed description of
 * 							functionality.
 ******************************************************************************/

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "Testing.h"
#include "Arduino.h"
#include "PinDefs.h"
#include "NixieDriver.h"
#include "ErrorHandling.h"

/*******************************************************************************
 * Constants and definitionss
 ******************************************************************************/

/*******************************************************************************
 * Module globals
 ******************************************************************************/

/* Bring nixie library object in */
extern nixie nix;

/*******************************************************************************
 * Functions
 ******************************************************************************/

/*******************************************************************************
 * Name:	ChangeSegment(uint8_t segment)
 * Inputs:	uint8_t segment - the segment we wish to change to (0 -> 5)
 * Return:	None
 * Notes:	Changes the segment under test to the one specified
 ******************************************************************************/
void ChangeSegment(uint8_t segment)
{
	/* Check input */
	if(segment > 5) return;

	/* Turn the outputs OFF */
	digitalWrite(SC_EN, LOW);

	/* Enable the correct segment */
	digitalWrite(SC_0, segment & (1 << 0));
	digitalWrite(SC_1, segment & (1 << 1));
	digitalWrite(SC_2, segment & (1 << 2));

	/* Turn the outputs ON */
	digitalWrite(SC_EN, HIGH);
}

/*******************************************************************************
 * Name:	SwitchDigit(uint8_t segment, uint8_t digit)
 * Inputs:	uint8_t segment - the current segment we are working on
 * 			uint8_t digit   - the digit to test
 * Return:	None.
 * Notes: 	Switches a single bit of the NTD to be "ON".
 ******************************************************************************/
void SwitchDigit(uint8_t segment, uint8_t digit)
{
	switch(segment)
	{
		case 0:
			nix.displayDigits((int) digit, BLANK, BLANK, BLANK, BLANK, BLANK);
			break;
		case 1:
			nix.displayDigits(BLANK, (int) digit, BLANK, BLANK, BLANK, BLANK);
			break;
		case 2:
			nix.displayDigits(BLANK, BLANK, (int) digit, BLANK, BLANK, BLANK);
			break;
		case 3:
			nix.displayDigits(BLANK, BLANK, BLANK, (int) digit, BLANK, BLANK);
			break;
		case 4:
			nix.displayDigits(BLANK, BLANK, BLANK, BLANK, (int) digit, BLANK);
			break;
		case 5:
			nix.displayDigits(BLANK, BLANK, BLANK, BLANK, BLANK, (int) digit);
			break;
		default:
			blank_display();
			break;
	}

	/* Wait for settle */
	delay(SETTLE_TIME);
}

/*******************************************************************************
 * Name:	ReadEncoder()
 * Inputs:	None
 * Return:	uint8_t - 	the number of the HIGHEST digit to be "ON" (i.e. pulled
 * 						low).
 * Notes: 	If by some chance there is a bug by which digit 0 comes on when
 * 			digit 3 is lit, but not the other way around, this will not catch
 * 			it.
 ******************************************************************************/
uint8_t ReadEncoder()
{
	uint8_t output =
			((digitalRead(ENC_3) << 3) |
			 (digitalRead(ENC_2) << 2) |
			 (digitalRead(ENC_1) << 1) |
			 (digitalRead(ENC_0)     ))| 0xFF00;
	return ~output;
}

/*******************************************************************************
 * Name:	TestSegment(uint8_t segment)
 * Inputs:	uint8_t segment - the segment we wish to test (0->5)
 * Return:	None.
 * Notes: 	Tests a segment and adds any errors to the queue.
 ******************************************************************************/
void TestSegment(uint8_t segment)
{
	Error_t err;
	err.segment = segment;
	err.next = NULL;

	ChangeSegment(segment);
	delay(10);

	if(!digitalRead(SENSE_HV))
	{
		err.testDigit = HV;
		err.digit = HV;
		err.error = OPEN;
		EnqueueError(&err);
	}

	/* Make note of the test digit */
	err.testDigit = BLANKED;

	/* Test that none of the digits are on by default */
	blank_display();

	/* If there is a digit on report it back */
	if(ReadEncoder() != 0) {
		err.digit = (Digit_et)ReadEncoder();
		err.error = SHORT;
		EnqueueError(&err);
	}

	/* If the 0 is on */
	if(!digitalRead(SENSE_0))
	{
		err.digit = ZERO;
		err.error = SHORT;
		EnqueueError(&err);
	}

	/* If the DP is on */
	if(!digitalRead(SENSE_DP))
	{
		err.digit = DP;
		err.error = SHORT;
		EnqueueError(&err);
	}

	/* Check each digit 1->9 */
	for(uint8_t index = 1; index < 10; index++)
	{
		/* Make note of the test digit */
		err.testDigit = (Digit_et)index;

		/* Turn the digit "ON" */
		SwitchDigit(segment, index);

		/* If the result is not what we expected */
		if(ReadEncoder() != index) {

			if(ReadEncoder() == 0)
			{
				/* Digit is not on */
				err.digit = (Digit_et)index;
				err.error = OPEN;
				EnqueueError(&err);

			} else {

				/* Another digit is on */
				err.digit = (Digit_et)ReadEncoder();
				err.error = SHORT;
				EnqueueError(&err);
			}
		}

		/* If the 0 is on */
		if(!digitalRead(SENSE_0))
		{
			err.digit = ZERO;
			err.error = SHORT;
			EnqueueError(&err);
		}

		/* If the DP is on */
		if(!digitalRead(SENSE_DP))
		{
			err.digit = DP;
			err.error = SHORT;
			EnqueueError(&err);
		}
	}

	/* Make note of the test digit */
	err.testDigit = ZERO;

	/* Check digit 0 */
	SwitchDigit(segment, 0);

	/* If 0 is "OFF" */
	if(digitalRead(SENSE_0))
	{
		err.digit = ZERO;
		err.error = OPEN;
		EnqueueError(&err);
	}

	/* If the DP is on */
	if(!digitalRead(SENSE_DP))
	{
		err.digit = DP;
		err.error = SHORT;
		EnqueueError(&err);
	}

	/* If a number is on */
	if(ReadEncoder() != 0)
	{
		/* Another digit is on */
		err.digit = (Digit_et)ReadEncoder();
		err.error = SHORT;
		EnqueueError(&err);
	}

	/* No need to do 1st segment DP */
	if(segment == 0) return;

	/* Make note of the test digit */
	err.testDigit = DP;

	/* Turn DP on */
    nix.setDecimalPoint(segment - 1, true); //setDecimalPoint(0, 1) will turn on the DP AFTER the
    										//0th digit i.e. segment 1.
    blank_display();

	/* If DP is "OFF" */
	if(digitalRead(SENSE_DP))
	{
		err.digit = DP;
		err.error = OPEN;
		EnqueueError(&err);
	}

	/* If the 0 is on */
	if(!digitalRead(SENSE_0))
	{
		err.digit = ZERO;
		err.error = SHORT;
		EnqueueError(&err);
	}

	/* If a number is on */
	if(ReadEncoder() != 0)
	{
		/* Another digit is on */
		err.digit = (Digit_et)ReadEncoder();
		err.error = SHORT;
		EnqueueError(&err);
	}
}

