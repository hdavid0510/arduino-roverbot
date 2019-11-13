/* 
Arduino Roverbot - car part
Code written by David.Hong (devstudiod@gmail.com)

# WIRING
Motor shield use pin 4,5,6,7 to control the motor.
 Simply connect your motors to M1+,M1-,M2+,M2- on your hield.
Serial port pin (0,1) is used for communication;
 communicate with USB serial or devices like bluetooth(HC-05, ...)
	with pin 0, 1 connected. ** DISCONNECT DEVICE DURING UPLOADING SKETCH!
Connect NS-LED-02 (8 LED stick) to pin 10.

# COMMUNICATION
0x00~0x66
	Motor move command: 0x(Left Motor speed)(Right Motor speed)
	Motor speed:
	0 = Stop
	1 = Backward slow
	2 = Backward medium
	3 = Backward fast
	4 = Forward slow
	5 = Forward medium
	6 = Forward fast
0xf1~0xf4
	Extra functions. 0xf(Function #)
	Each function's behavior can be customized by editing function1() ~ function4().
	Default function 1 and 2 is allocated for LED ON/OFF -
	 you can change this by editing sketch of both controller and car.
*/

//Nulsom LED library
#include <NS_Rainbow.h>

//Constants///////////////////////////////////////////////
//Define pin numbers
#define PINDIRL 7
#define PINDIRR 4
#define PINSPDL 6
#define PINSPDR 5
#define PINLED 10

//Define command protocol
#define CMDFN1 0xf1
#define CMDFN2 0xf2
#define CMDFN3 0xf3
#define CMDFN4 0xf4
//const byte CMDMTR[] = {6,5,4,0,1,2,3};

//Define motor speeds
#define SPDFST 240 //Motor speed: fast
#define SPDMID 128 //Motor speed: medium
#define SPDSLW 64  //Motor speed: slow
#define SPDMAX 255 //Motor speed: maximum

//Motor speed and direction when motor runs at...
// {Stop, Backward slow, Backward medium, Backward fast, Forward slow, Forward medium, Forward fast}
const byte MTRSPD[] = {0, SPDSLW, SPDMID, SPDFST, SPDSLW, SPDMID, SPDFST};
const bool MTRREV[] = {false, true, true, true, false, false, false}; //True: Backward; False: Forward.

//Variables////////////////////////////////////////////////
byte now = 0; //Current state. Used for avoiding running same command frequently too much.
NS_Rainbow ledstick = NS_Rainbow(8, PINLED);

//Functions////////////////////////////////////////////////
boolean inrangeof(byte i) { return (0 <= i && i <= 6); }

void motorL(int pwm, boolean rev)
{
	analogWrite(PINSPDL, pwm); //set pwm control, 0 for stop, and 255 for maximum speed
	digitalWrite(PINDIRL, (rev ? HIGH : LOW));
}
void motorR(int pwm, boolean rev)
{
	analogWrite(PINSPDR, pwm);
	digitalWrite(PINDIRR, (rev ? HIGH : LOW));
}

void function1()
{										 // LED ON=
	ledstick.setColor(4, 255, 255, 255); // digitalWrite(PINLED, HIGH);
}
void function2()
{								   // LED OFF=
	ledstick.setColor(4, 0, 0, 0); // digitalWrite(PINLED, LOW);
}
void function3()
{ // Function 3 (extra)
}
void function4()
{ // Function 4 (extra)
}

//Main loops///////////////////////////////////////////////
void setup()
{
	// Serial open
	Serial.begin(115200);

	// Pinmode Setting
	pinMode(PINDIRR, OUTPUT);
	pinMode(PINSPDR, OUTPUT);
	pinMode(PINSPDL, OUTPUT);
	pinMode(PINDIRL, OUTPUT);
	pinMode(PINLED, OUTPUT);

	// Init LED stick
	ledstick.begin();
	ledstick.setBrightness(32);
	for (byte i = 0; i < 8; i++)
		ledstick.setColor(i, 0, 0, 0);
	ledstick.setColor(3, 0, 255, 0);
	ledstick.setColor(2, 0, 0, 0);
	ledstick.show();
}

void loop()
{
	if (!Serial.available())
		return;

	byte val = Serial.read();
	if (val == now)
		return;

	ledstick.setColor(3, 255, 255, 0);
	ledstick.show();
	if (val == CMDFN1)
		function1();
	else if (val == CMDFN2)
		function2();
	else if (val == CMDFN3)
		function3();
	else if (val == CMDFN4)
		function4();
	else if (inrangeof(val / 16) && inrangeof(val % 16))
	{										 //Motor run command
		byte ml = val >> 4, mr = val & 0x0f; //same as: ml=val/16, mr=val%16;
		motorL(MTRSPD[ml], MTRREV[ml]);
		motorR(MTRSPD[mr], MTRREV[mr]);
		ledstick.setColor(7, MTRREV[ml] ? 0 : MTRSPD[ml], MTRREV[ml] ? MTRSPD[ml] : 0, MTRREV[ml] ? MTRSPD[ml] : 0);
		ledstick.setColor(0, MTRREV[mr] ? 0 : MTRSPD[mr], MTRREV[mr] ? MTRSPD[mr] : 0, MTRREV[mr] ? MTRSPD[mr] : 0);
	}
	now = val; //save current command

	ledstick.setColor(3, 0, 255, 0);
	ledstick.show();
}
