/*******************************************************************************
 * File:                    ErrorHandling.h
 * Project:                 NTD Test Jig
 * Date:                    27/04/2018
 * Author:                  Thomas Cousins
 * Target:                  Arduino Uno
 * Copyright:               Doayee 2018
 * Description:             Header for the error queue functionality.
 ******************************************************************************/

#ifndef ERRORHANDLING_H_
#define ERRORHANDLING_H_

/*******************************************************************************
 * Module includes
 ******************************************************************************/
#include "stdint.h"

/*******************************************************************************
 * System Wide Module definitions
 ******************************************************************************/

/*******************************************************************************
 * Module Specific Macros
 ******************************************************************************/

/*******************************************************************************
 * Types
 ******************************************************************************/
typedef enum {
	NONE,
	SHORT,
	OPEN,
} Err_et;

typedef enum {
	BLANKED 	= (uint8_t)0,
	ONE			= (uint8_t)1,
	TWO			= (uint8_t)2,
	THREE		= (uint8_t)3,
	FOUR		= (uint8_t)4,
	FIVE		= (uint8_t)5,
	SIX			= (uint8_t)6,
	SEVEN		= (uint8_t)7,
	EIGHT		= (uint8_t)8,
	NINE		= (uint8_t)9,
	ZERO		= (uint8_t)10,
	DP			= (uint8_t)11,
	HV			= (uint8_t)12,
} Digit_et;

/* Weird declaration to get round whatever unholy compiler
 * warnings arduino has on by default...
 */
typedef struct Error_t {
	Err_et error;
	uint8_t segment;
	Digit_et digit;
	Digit_et testDigit;
	struct Error_t *next;
} Error_t;

/*******************************************************************************
 * Function Prototypes
 ******************************************************************************/
void SetupQueue();
void EnqueueError(Error_t *toPush);
void FreeQueue();
void FreeQueueRecursive(Error_t *node);
void ParseQueue(Error_t *node);

#endif /* ERRORHANDLING_H_ */
