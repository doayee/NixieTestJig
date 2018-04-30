/*******************************************************************************
 * File:                    ErrorHandling.cpp
 * Project:                 NTD Test Jig
 * Date:                    27/04/2018
 * Author:                  Thomas Cousins
 * Target:                  Arduino Uno
 * Copyright:               Doayee 2018
 * Description:             Handles the error queue, which stores any errors
 * 							encountered in the test. Has facility to set up the
 * 							queue, push errors onto the queue, erase the queue
 * 							and parse the queue to the Serial port.
 ******************************************************************************/

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "ErrorHandling.h"
#include "PinDefs.h"
#include "Arduino.h"

/*******************************************************************************
 * Constants and definitionss
 ******************************************************************************/

/*******************************************************************************
 * Module globals
 ******************************************************************************/
Error_t *queue;
Error_t *working;
uint16_t errorCount;

/*******************************************************************************
 * Functions
 ******************************************************************************/

/*******************************************************************************
 * Name:	SetupQueue()
 * Inputs:	None
 * Return:	None.
 * Notes: 	Builds the queue of error messages.
 ******************************************************************************/
void SetupQueue()
{
	queue = (Error_t *)malloc(sizeof(Error_t));
	if(queue == NULL)
	{
		Serial.print("Fatal error... Error Queue cannot be created. See line ");
		Serial.print(__LINE__);
		Serial.print(" of ");
		Serial.println(__FILE__);
		while(1) {
			digitalWrite(LED, HIGH);
			delay(1000);
			digitalWrite(LED, LOW);
			delay(1000);
		}
	}
	queue->next = NULL;
	working = queue;
	errorCount = 0;
}

/*******************************************************************************
 * Name:	EnqueueError(Error_t *toPush)
 * Inputs:	Error_t *toPush - the error to push onto the queue
 * Return:	None.
 * Notes: 	Adds an error to the queue.
 ******************************************************************************/
void EnqueueError(Error_t *toPush)
{
	/* Build new error */
	Error_t *newErr = (Error_t *)malloc(sizeof(Error_t));
	if(newErr == NULL) return;

	/* Copy error to newErr */
	memcpy((void *)newErr, (void *)toPush, sizeof(Error_t));

	/* Point the queue at the new entry */
	working->next = newErr;

	/* Move working node to new entry */
	working = newErr;

	errorCount++;
}

/*******************************************************************************
 * Name:	FreeQueue()
 * Inputs:	None.
 * Return:	None.
 * Notes: 	Wrapper for FreeQueueRecursive
 ******************************************************************************/
void FreeQueue()
{
	FreeQueueRecursive(queue);
}

/*******************************************************************************
 * Name:	FreeQueueRecursive(Error_t *node)
 * Inputs:	Error_t *node - a node on the queue
 * Return:	None.
 * Notes: 	Frees the queue recursively
 ******************************************************************************/
void FreeQueueRecursive(Error_t *node)
{
	if(node->next != NULL)
		FreeQueueRecursive(node->next);

	free(node);
	return;
}

/*******************************************************************************
 * Name:	ParseQueue(Error_t *node)
 * Inputs:	Error_t *node - the node to parse the queue from.
 * Return:	None.
 * Notes: 	Reads the queue type and prints the details to the UART.
 ******************************************************************************/
void ParseQueue(Error_t *node)
{
	/* Error type */
	Serial.print("Error Type: ");
	Serial.println((node->error == SHORT) ? "SHORT" : "OPEN");

	/* Segment */
	Serial.print("Segment: ");
	Serial.println(node->segment);

	/* Test Node */
	Serial.println("Test Node: ");
	switch(node->digit) {
	case BLANKED:
		Serial.println("BLANK DIGIT");
		break;
	case DP:
		Serial.println("DECIMAL POINT");
		break;
	case HV:
		Serial.println("HIGH VOLTAGE CONNECTION");
		break;
	default:
		Serial.println(node->testDigit % 10);
		break;
	}

	/* Error node */
	Serial.println("Error Node: ");
	switch(node->digit) {
	case DP:
		Serial.println("DECIMAL POINT");
		break;
	case HV:
		Serial.println("HIGH VOLTAGE CONNECTION");
		break;
	default:
		Serial.println(node->testDigit % 10);
		break;
	}

	Serial.println();
	Serial.println("--------------------------------------------------");
	Serial.println();

	if(node->next != NULL) ParseQueue(node->next);

	return;
}
