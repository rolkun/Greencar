/*
 Name:		Greencar_Arduino.ino
 Created:	10/25/2016 12:18:43 AM
 Author:	roland.kunz
*/

#include <Wire.h>
#include <IRremote.h>

int PinReceiveIR = 7;	

/******************************************************************************************
* 
******************************************************************************************/
int PinEnableLeftPositive = 15;
int PinEnableLeftNegative = 14;
int PinEnableRightPositive = 17;
int PinEnableRightNegative = 16;

/******************************************************************************************
* 
******************************************************************************************/

const long ReceivedValueForward = 0x00FF18E7;
const long ReceivedValueBackward = 0x00FF4AB5;
const long ReceivedValueLeft = 0x00FF10EF;
const long ReceivedValueTurnLeft = 0x00FF30CF;
const long ReceivedValueRight = 0x00FF5AA5;
const long ReceivedValueTurnRight = 0x00FF7A85;
const long ReceivedValueStop = 0x00FF38C7;

String command = "stop";

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
	pinMode(13, OUTPUT);
	pinMode(PinReceiveIR, INPUT);
	pinMode(PinEnableLeftPositive, OUTPUT);
	pinMode(PinEnableLeftNegative, OUTPUT);
	pinMode(PinEnableRightPositive, OUTPUT);
	pinMode(PinEnableRightNegative, OUTPUT);
	irrecv.enableIRIn(); // Start the receiver

	Wire.begin(42);
	Wire.onReceive(receiveEvent);
	Wire.onRequest(requestEvent);
}

byte measureResult = 0;
boolean isDataReceived = 0;
unsigned long last = millis();

void requestEvent(void)
{
	Wire.write(measureResult);
}


// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany) {
	
	//while (1 < Wire.available())	// loop through all but the last
	//{								
		command = Wire.readStringUntil('\n');
	//}
	//String command = String(receivedCommand).substring(0, howMany-1);

	int x = Wire.read();			// receive byte as an integer
	Serial.println(x);				// print the integer
	
	isDataReceived = true;
}

/******************************************************************************************
* 
******************************************************************************************/
void loop()
{
	if (isDataReceived)
	{
		SetDriveParameter(&driveLeftSide, command);
		SetDriveParameter(&driveRightSide, command);
		CallDrive(&driveLeftSide);
		CallDrive(&driveRightSide);
		isDataReceived = false;
	}

	if (irrecv.decode(&results))
	{
		if (millis() - last > 500)
		{
			dump(&results);

			switch (results.value)
			{
			case ReceivedValueForward:
				command = "forward";
				break;
			case ReceivedValueBackward:
				command = "backward";
				break;
			case ReceivedValueLeft:
				break;
			case ReceivedValueTurnLeft:
				command = "turnleft";
				break;
			case ReceivedValueRight:
				command = "right";
				break;
			case ReceivedValueTurnRight:
				command = "turnright";
				break;
			default:
				command = "stop";
				break;
			}

			SetDriveParameter(&driveLeftSide, command);
			SetDriveParameter(&driveRightSide, command);
			CallDrive(&driveLeftSide);
			CallDrive(&driveRightSide);
		}

		last = millis();
		irrecv.resume(); // Receive the next value
	}
}

void SetDriveParameter(struct Drive* drive, String command)
{
	if (command.equals("forward"))
	{
		drive->run = true;
		drive->forward = true;
	}
	else if (command.equals("backward"))
	{
		drive->run = true;
		drive->forward = false;
	}
	else if (command.equals("left"))
	{
		if (drive->name == "Left")
		{
			drive->run = false;
		}
		else
		{
			drive->run = true;
		}

		drive->forward = true;
	}
	else if (command.equals("right"))
	{
		if (drive->name == "Right")
		{
			drive->run = false;
		}
		else
		{
			drive->run = true;
		}

		drive->forward = true;
	}
	else if (command.equals("turnleft"))
	{
		if (drive->name == "Left")
		{
			drive->forward = false;
		}
		else
		{
			drive->forward = true;
		}

		drive->run = true;
	}
	else if (command.equals("turnright"))
	{
		if (drive->name == "Right")
		{
			drive->forward = false;
		}
		else
		{
			drive->forward = true;
		}

		drive->run = true;
	}
	else
	{
		driveLeftSide.run = false;
		driveRightSide.run = false;
	}
}

void CallDrive(struct Drive* drive)
{
	if (drive->run)
	{
		if (drive->forward)
		{
			digitalWrite(drive->pinForward, LOW);
			digitalWrite(drive->pinBackward, HIGH);
			Serial.print(drive->name);
			Serial.print(": ");
			Serial.println("Move Forward");
		}
		else
		{
			digitalWrite(drive->pinForward, HIGH);
			digitalWrite(drive->pinBackward, LOW);
			Serial.print(drive->name);
			Serial.print(": ");
			Serial.println("Move Backward");
		}
	}
	else
	{
		digitalWrite(drive->pinForward, HIGH);
		digitalWrite(drive->pinBackward, HIGH);
		Serial.print(drive->name);
		Serial.print(": ");
		Serial.println("Stop");
	}
}
/******************************************************************************************
*                                       END
******************************************************************************************/
