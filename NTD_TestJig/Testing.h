/*******************************************************************************
 * File:                    Testing.h
 * Project:                 NTD Test Jig
 * Date:                    27/04/2018
 * Author:                  Thomas Cousins
 * Target:                  Arduino Uno
 * Copyright:               Doayee 2018
 * Description:             Header for the Test Jig specific functionality.
 ******************************************************************************/

#ifndef TESTING_H_
#define TESTING_H_

/*******************************************************************************
 * Module includes
 ******************************************************************************/
#include "stdint.h"

/*******************************************************************************
 * System Wide Module definitions
 ******************************************************************************/
#define SETTLE_TIME 10 //ms

/*******************************************************************************
 * Module Specific Macros
 ******************************************************************************/
#define blank_display() nix.displayDigits(BLANK, BLANK, BLANK, BLANK, BLANK, BLANK)

/*******************************************************************************
 * Types
 ******************************************************************************/

/*******************************************************************************
 * Function Prototypes
 ******************************************************************************/
void ChangeSegment(uint8_t segment);
void SwitchDigit(uint8_t segment, uint8_t digit);
uint8_t ReadEncoder();
void TestSegment(uint8_t segment);

#endif /* TESTING_H_ */
