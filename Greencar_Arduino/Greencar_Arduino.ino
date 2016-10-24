/*
 Name:		Greencar_Arduino.ino
 Created:	10/25/2016 12:18:43 AM
 Author:	roland.kunz
*/

#include <IRremote.h>

int PinReceiveIR = 7;				//ARDUINO?7?------ L293D

/******************************************************************************************
* 
******************************************************************************************/
int PinEnableLeftPositive = 14;		//ARDUINO?A0?------ L293D (L+?  
int PinEnableLeftNegative = 15;		//ARDUINO?A1?------ L293D?L-?  
int PinEnableRightPositive = 16;    //ARDUINO?A2?------ L293D?R+? 
int PinEnableRightNegative = 17;    //ARDUINO?A3?------ L293D (R-?

/******************************************************************************************
* 
******************************************************************************************/

const long ReceivedValueForward = 0x00FF18E7;
const long ReceivedValueBackward = 0x00FF4AB5;
const long ReceivedValueLeft = 0x00FF5AA5;
const long ReceivedValueTurnLeft = 0x00FF30CF;
const long ReceivedValueRight = 0x00FF10EF;
const long ReceivedValueTurnRight = 0x00FF7A85;
const long ReceivedValueStop = 0x00FF38C7;

struct Drive {
	char *name;
	bool run;
	bool forward;
	int pinForward;
	int pinBackward;
};

struct Drive driveLeftSide = { "Left", false, true, PinEnableLeftPositive, PinEnableLeftNegative };
struct Drive driveRightSide = { "Right", false, true, PinEnableRightPositive, PinEnableRightNegative };


IRrecv irrecv(PinReceiveIR);  //
decode_results results;
void dump(decode_results *results)
{
	if (results->decode_type == UNKNOWN)
	{
		Serial.println("Could not decode message");
	}
	else
	{
		Serial.print(results->value, HEX);
		Serial.print(" (");
		Serial.print(results->bits, DEC);
		Serial.println(" bits)");
	}
}

/******************************************************************************************
* 
******************************************************************************************/
void setup()
{
	Serial.begin(9600);
	pinMode(PinReceiveIR, INPUT);
	pinMode(PinEnableLeftPositive, OUTPUT);
	pinMode(PinEnableLeftNegative, OUTPUT);
	pinMode(PinEnableRightPositive, OUTPUT);
	pinMode(PinEnableRightNegative, OUTPUT);
	irrecv.enableIRIn(); // Start the receiver
}

int on = 0;
unsigned long last = millis();

/******************************************************************************************
* 
******************************************************************************************/
void loop()
{
	if (irrecv.decode(&results))
	{
		if (millis() - last > 500)
		{
			on = !on;
			digitalWrite(13, on ? HIGH : LOW);
			dump(&results);
		}

		switch (results.value)
		{
			case ReceivedValueForward:
				driveLeftSide.forward = true;
				driveLeftSide.run = true;
				Move(&driveLeftSide);
				driveRightSide.forward = true;
				driveRightSide.run = true;
				Move(&driveRightSide);
				break;
			case ReceivedValueBackward:
				driveLeftSide.forward = false;
				driveLeftSide.run = true;
				Move(&driveLeftSide);
				driveRightSide.forward = false;
				driveRightSide.run = true;
				Move(&driveRightSide);
				break;
			case ReceivedValueLeft:
				driveLeftSide.run = false;
				Move(&driveLeftSide);
				driveRightSide.forward = true;
				driveRightSide.run = true;
				Move(&driveRightSide);
				break;
			case ReceivedValueTurnLeft:
				driveLeftSide.forward = false;
				driveLeftSide.run = true;
				Move(&driveLeftSide);
				driveRightSide.forward = true;
				driveRightSide.run = true;
				Move(&driveRightSide);
				break;
			case ReceivedValueRight:
				driveLeftSide.forward = true;
				driveLeftSide.run = true;
				Move(&driveLeftSide);
				driveRightSide.run = false;
				Move(&driveRightSide);
				break;
			case ReceivedValueTurnRight:
				driveLeftSide.forward = true;
				driveLeftSide.run = true;
				Move(&driveLeftSide);
				driveRightSide.forward = false;
				driveRightSide.run = true;
				Move(&driveRightSide);
				break;
			case ReceivedValueStop:
				driveLeftSide.run = false;
				Move(&driveLeftSide);
				driveRightSide.run = false;
				Move(&driveRightSide);
				break;
			default:
				break;
		}

		last = millis();
		irrecv.resume(); // Receive the next value
	}
}

void Move(struct Drive* drive)
{
	if ((*drive).run)
	{
		if ((*drive).forward)
		{
			digitalWrite((*drive).pinForward, LOW);
			digitalWrite((*drive).pinBackward, HIGH);
			Serial.print((*drive).name);
			Serial.print(": ");
			Serial.println("Move Forward");
		}
		else
		{
			digitalWrite((*drive).pinForward, HIGH);
			digitalWrite((*drive).pinBackward, LOW);
			Serial.print((*drive).name);
			Serial.print(": ");
			Serial.println("Move Backward");
		}
	}
	else
	{
		digitalWrite((*drive).pinForward, HIGH);
		digitalWrite((*drive).pinBackward, HIGH);
		Serial.print((*drive).name);
		Serial.print(": ");
		Serial.println("Stop");
	}
}
/******************************************************************************************
*                                       END
******************************************************************************************/
