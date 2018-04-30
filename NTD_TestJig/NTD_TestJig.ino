#include "Arduino.h"
#include "NixieDriver.h"
#include "ErrorHandling.h"
#include "PinDefs.h"
#include "Testing.h"

/*******************************************************************************
 * Macros
 ******************************************************************************/

/* Create library instances */
nixie nix(DATA, CLK, OE);
backlight rgb(R, G, B);

/*******************************************************************************
 * Globals
 ******************************************************************************/
extern uint16_t errorCount;

/* Colour fade to test RGBs */
/* NOTE - times to be adjusted when test length is known */
int cycleTest[4][4] = {
		{ RED, 		500 	},
		{ GREEN, 	500 	},
		{ BLUE, 	500		},
		{ ENDCYCLE			}
};

/*******************************************************************************
 * Function Prototypes
 ******************************************************************************/
void PinSetup();

/*******************************************************************************
 * Arduino Functions
 ******************************************************************************/

/*******************************************************************************
 * Name:	setup()
 * Inputs:	None.
 * Return:	None.
 * Notes: 	Runs once, performs the one time setup.
 ******************************************************************************/
void setup()
{
	PinSetup();
	Serial.begin(9600); //Diagnostics output
}

/*******************************************************************************
 * Name:	loop()
 * Inputs:	None.
 * Return:	None.
 * Notes: 	Runs repeatedly.
 ******************************************************************************/
void loop()
{
	/* Set up the queue */
	SetupQueue();

	/* Put segment 0 on so we can sense the NTD being placed on the jig */
	ChangeSegment(0);

	/* Wait for NTD */
	while(!digitalRead(SENSE_HV)) {
		digitalWrite(LED, 1);
		delay(50);
		digitalWrite(LED, 0);
		delay(50);
	}

	/* Turn LED on */
	digitalWrite(LED, 1);

	/* When a NTD is connected */
	Serial.println("NTD Detected... Starting Test in 100ms.");
	delay(100);
	Serial.println("Starting test.");

	Serial.println("RGB Test Begin.");
	rgb.setFade(cycleTest, 100);

	/* Test the digits */
	Serial.println("Functional Test Begin.");
	for(uint8_t i = 0; i < 6; i++)
	{
		Serial.print("Testing segment: ");
		Serial.println(i);
		TestSegment(i);
		Serial.print("Total Error Count: ");
		Serial.println(errorCount);
	}

	Serial.println("Functional Test End.");
	Serial.println("RGB Test End.");

	/* Stop the RGBs */
	rgb.stopFade(rgb.black, 100);

	Serial.println();
	Serial.println("**************************************************");

	if(errorCount == 0)
	{
		Serial.println("***                    PASS                    ***");
		Serial.println("**************************************************");
		Serial.println();
		rgb.setColour(rgb.green);
	} else {

		Serial.println("***                    FAIL                    ***");
		Serial.println("**************************************************");
		Serial.println();

		void ParseQueue(Error_t *node);

	}

	/* Free the queue */
	FreeQueue();

	/* Put segment 0 on so we can sense the NTD being removed from the jig */
	ChangeSegment(0);

	/* Turn the LED off */
	digitalWrite(LED, 0);

	/* Wait for NTD */
	while(digitalRead(SENSE_HV));

}

/*******************************************************************************
 * Custom Functions
 ******************************************************************************/

/*******************************************************************************
 * Name:	PinSetup()
 * Inputs:	None
 * Return:	None
 * Notes:	Sets the pins to their appropriate state.
 ******************************************************************************/
void PinSetup()
{
	/* Set the segment choice pins to outputs */
	pinMode(SC_EN, 	OUTPUT);
	pinMode(SC_0, 	OUTPUT);
	pinMode(SC_1, 	OUTPUT);
	pinMode(SC_2, 	OUTPUT);

	/* Write the segment choice enable LOW */
	digitalWrite(SC_EN, LOW);

	/* Set the LED to output */
	pinMode(LED, 	OUTPUT);

	/* Set the priority encoder pins to input */
	pinMode(ENC_0, 	INPUT);
	pinMode(ENC_1, 	INPUT);
	pinMode(ENC_2, 	INPUT);
	pinMode(ENC_3, 	INPUT);

	/* Set the other sense pins to input */
	pinMode(SENSE_0, 	INPUT);
	pinMode(SENSE_DP, 	INPUT);
	pinMode(SENSE_HV, 	INPUT);
}
