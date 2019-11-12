/* 
Arduino Roverbot - arduino controller part
Code written by David.Hong (devstudiod@gmail.com)

# WIRING
Detailed wiring is described in wiring.png file!

# HOW TO USE
Use 3 buttons to select roverbot moving direction.
Adjust speed with potentiometer:
 Backward Fast -- Slow -- Stop(Center) -- Slow -- Forward Fast
*/
#include <SoftwareSerial.h>


//Constants///////////////////////////////////////////////
//Direction constant
#define DIRCTR 0
#define DIRRHT 1
#define DIRLFT -1

//Potentiometer constant
#define VAFAST 256	// Potentiometer speed(fast) threshold
#define VASLOW 64   // Potentiometer speed(slow) threshold
#define VFFAST (512 + VAFAST)
#define VFSLOW (512 + VASLOW)
#define VBSLOW (512 - VASLOW)
#define VBFAST (512 - VAFAST)

//Pin number constant
#define LEDLFT 6
#define LEDCTR 5
#define LEDRHT 4
#define BTNLFT 9
#define BTNCTR 8
#define BTNRHT 7
#define VSLCTR A1
#define SWSRRX 2
#define SWSRTX 3

//loop() delay interval (millisec)
#define REFINT 80


//Variables///////////////////////////////////////////////
int direction = DIRCTR;
byte lastsent = 0;

SoftwareSerial subSerial(SWSRRX, SWSRTX);

//Functions///////////////////////////////////////////////
void led(bool l, bool c, bool r) {
	digitalWrite(LEDLFT, l ? HIGH : LOW);
	digitalWrite(LEDCTR, c ? HIGH : LOW);
	digitalWrite(LEDRHT, r ? HIGH : LOW);
}

void out(byte d, String s = "") {
	if (d != lastsent) {
		subSerial.write(d);
		lastsent = d;
		Serial.print("SND 0x");
		Serial.print(d, HEX);
		Serial.println(s.length() > 0 ? " " + s : "");
	} else {
		// Serial.print("IGN ");
	}
}

void setup(){
	// Open serial communications
	Serial.begin(115200);
	subSerial.begin(115200);
	subSerial.write((byte)0x00);

	// Prepare pins
	pinMode(LEDRHT, OUTPUT); digitalWrite(LEDRHT, LOW);
	pinMode(LEDCTR, OUTPUT); digitalWrite(LEDCTR, LOW);
	pinMode(LEDRHT, OUTPUT); digitalWrite(LEDRHT, LOW);
	pinMode(BTNLFT, INPUT);
	pinMode(BTNCTR, INPUT);
	pinMode(BTNRHT, INPUT);
	pinMode(VSLCTR, INPUT);
}

void loop() {

	// Potentiometer -> Speed
	int a = analogRead(VSLCTR);
	if (a < VBFAST) { //Back FAST
		switch (direction) {
		case DIRCTR:
			out(0x66);
			break;
		case DIRRHT:
			out(0x64);
			break;
		case DIRLFT:
			out(0x46);
			break;
		};
	} else if (a < VBSLOW) { //Back SLOW
		switch (direction) {
		case DIRCTR:
			out(0x44);
			break;
		case DIRRHT:
			out(0x40);
			break;
		case DIRLFT:
			out(0x04);
			break;
		};
	} else if (a > VFFAST) { //Forw FAST
		switch (direction) {
		case DIRCTR:
			out(0x33);
			break;
		case DIRRHT:
			out(0x31);
			break;
		case DIRLFT:
			out(0x13);
			break;
		};
	} else if (a > VFSLOW) { //Forward SLOW
		switch (direction) {
		case DIRCTR:
			out(0x11);
			break;
		case DIRRHT:
			out(0x10);
			break;
		case DIRLFT:
			out(0x01);
			break;
		};
	} else {
		out(0x00);
	}

	// Buttons -> Direction
	if (digitalRead(BTNLFT) == HIGH){
		direction = DIRLFT;
		led(true, false, false);
	}
	if (digitalRead(BTNRHT) == HIGH){
		direction = DIRRHT;
		led(false, false, true);
	}
	if (digitalRead(BTNCTR) == HIGH){
		direction = DIRCTR;
		led(false, true, false);
	}

	// Extra function input
	if (Serial.available()) {
		byte i = Serial.read();
		if (i == 'z')
			out(0xf1, "LED ON");
		else if (i == 'x')
			out(0xf2, "LED OFF");
		else if (i == 'c')
			out(0xf3, "F3");
		else if (i == 'v')
			out(0xf4, "F4");
		else if (i == 10 || i == 13)
			return;
	}

	// Relay serial output from roverbot
	if (subSerial.available()) {
		Serial.print("RCV ");
		Serial.write(subSerial.read());
		Serial.println("");
	}

	// Cool down arduino; no need to run loop too fast!
	delay(REFINT);
}