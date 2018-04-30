/*******************************************************************************
 * File:                    PinDefs.h
 * Project:                 NTD Test Jig
 * Date:                    27/04/2018
 * Author:                  Thomas Cousins
 * Target:                  Arduino Uno
 * Copyright:               Doayee 2018
 * Description:             Pin Definitions for the NTD Test Jig
 ******************************************************************************/

#ifndef PINDEFS_H_
#define PINDEFS_H_

/* Nixie Driver Pins */
#define DATA 		8
#define CLK 		9
#define OE			10

/* RGB Pins */
#define R 			3
#define G 			5
#define B 			6

/* Segment Choice Pins */
#define SC_EN		7
#define SC_0 		11
#define SC_1 		12
#define SC_2 		13

/* Priority Encoder Pins */
#define ENC_0		A0
#define ENC_1		A1
#define ENC_2		A2
#define ENC_3		A4

/* Other Sense Pins */
#define SENSE_0		A4
#define SENSE_DP	A5
#define SENSE_HV	2

/* LED Output */
#define LED			4

#endif /* PINDEFS_H_ */
