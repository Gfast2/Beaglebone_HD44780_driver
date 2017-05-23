//============================================================================
// Name        : HD44780_Driver.cpp
// Author      : SuGao
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
// toggle one GPIO on Beaglebone
//============================================================================

#include <iostream>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include <inttypes.h>
#include "./gpio/SimpleGPIO.h"
#include <typeinfo>
using namespace std;

// gpio number on Beaglebone
//                       rs, rw, en, d5, d6, d7, d8
unsigned int GPIO[7] = { 66, 67, 69, 68, 45, 44, 26 };

void initPinMode() {
	for (int i = 0; i < 7; i++) {
		gpio_export(GPIO[i]);
		gpio_set_dir(GPIO[i], OUTPUT_PIN);
	}
}

// This way si far slower then it should (see datasheet)
// TODO: Here I should split it into two parts:
// 		 Put this pin high before I change datas on DB pins
// 		 Put this pin low that is acutally what "send()" means
void send() {
	gpio_set_value(GPIO[2], HIGH);
	usleep(5); // the minimum time is 450 nano second, here I do 5000 nano sec.
	gpio_set_value(GPIO[2], LOW);
}

void setData(uint8_t data) { // Argument is a 4bit instruction set


#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))

//	cout << "data now is : " << data << " vs " << 0b0010 << endl;
	printf("data now is %u\n", data);
	int w = CHECK_BIT(data, 0);
	printf("First bit of data now is %u\n", w );
	w = CHECK_BIT(data, 1);
	printf("Second bit of data now is %u\n", w );
	w = CHECK_BIT(data, 2);
	printf("Third bit of data now is %u\n", w );
	w = CHECK_BIT(data, 3);
	printf("Forth bit of data now is %u\n", w );

	// I literally spend 90% time of the night on figuring out the prority issue here...
	gpio_set_value(GPIO[3], ((data & 0b0001) != 0 ? HIGH : LOW));
	gpio_set_value(GPIO[4], ((data & 0b0010) != 0 ? HIGH : LOW));
	gpio_set_value(GPIO[5], ((data & 0b0100) != 0 ? HIGH : LOW));
	gpio_set_value(GPIO[6], ((data & 0b1000) != 0 ? HIGH : LOW));

}

// Display GPIO read
void setModeRead(){
	gpio_set_value(GPIO[1], HIGH);
}

// Display GPIO write
void setModeWrite(){
	gpio_set_value(GPIO[1], LOW);
}

//void setRegister

int main(int argc, char *argv[]) {

	cout << "Testing the GPIO Pins!" << endl;
	initPinMode();

	cout << "Flashing High/Low of each pin" << endl;
	// Flash the LED pin 5 times
	for (int i = 0; i < 5; i++) {
		cout << "Setting the PIN on" << endl;
		for (int i = 0; i < 7; i++) {
			gpio_set_value(GPIO[i], HIGH);
		}
		usleep(50000); // on for 50ms
		cout << "Setting the PIN off" << endl;
		for (int i = 0; i < 7; i++) {
			gpio_set_value(GPIO[i], LOW);
		}
		usleep(50000); // off for 50ms
	} // After this, all pins are set to low

	cout << " === INIT STEP ONE: TURN TO 4-BIT MODE (TEST DATA PIN WITH SCOPE NOW)===" << endl;
	setData(0b0010);
	if (cin.get() == '\n') cout << "go on." << endl; // This will hack the program till the user press enter key to going on
	send();
	cout << "==== FINISH SENDING ====" << endl;

//	cout << " === INIT STEP ONE: TURN TO 4-BIT MODE (second part of this command (for this command, this 4-bit are dummy))===" << endl;
//	setData(0b0000);
//	if (cin.get() == '\n') cout << "go on." << endl; // This will hack the program till the user press enter key to going on
//	send();
//	cout << "==== FINISH SENDING ====" << endl;

	cout << "==== INIT STEP TWO: FUNCTION SET ====" << endl;
	setData(0b0010);
	if (cin.get() == '\n') cout << "go on." << endl;
	send();
	cout << "==== FINISH SENDING ====" << endl;

	cout << "==== INIT STEP TWO: FUNCTION SET (second part of this command) ====" << endl;
	setData(0b1000);
	if (cin.get() == '\n') cout << "go on." << endl;
	send();
	cout << "==== FINISH SENDING ====" << endl;

	cout << "==== INIT STEP THREE: DISPLAY ON/OFF ====" << endl;
	setData(0b0000);
	if (cin.get() == '\n') cout << "go on." << endl;
	send();
	cout << "==== FINISH SENDING ====" << endl;

	cout << "==== INIT STEP THREE: DISPLAY ON/OFF (second part of this command) ====" << endl;
	setData(0b1111);
	if (cin.get() == '\n') cout << "go on." << endl;
	send();
	cout << "==== FINISH SENDING ====" << endl;

	cout << "==== INIT STEP FOUR: CLEAR DISPLAY ====" << endl;
	setData(0b0000);
	if (cin.get() == '\n') cout << "go on." << endl;
	send();
	cout << "==== FINISH SENDING ====" << endl;

	cout << "==== INIT STEP FOUR: CLEAR DISPLAY (second part of this command) ====" << endl;
	setData(0b0001);
	if (cin.get() == '\n') cout << "go on." << endl;
	send();
	cout << "==== FINISH SENDING ====" << endl;


	cout << "==== INIT STEP FIVE: ENTRY> MODE SET ====" << endl;
	setData(0b0000);
	if (cin.get() == '\n') cout << "go on." << endl;
	send();
	cout << "==== FINISH SENDING ====" << endl;

	cout << "==== INIT STEP FIVE: ENTRY> MODE SET (second part of this command) ====" << endl;
	setData(0b0110);
	if (cin.get() == '\n') cout << "go on." << endl;
	send();
	cout << "==== FINISH SENDING ====" << endl;

	cout << "And now all init steps are finished, press Enter to finish the init process" << endl;
	if (cin.get() == '\n')
		cout << "go on." << endl;

	cout << "==== ALL FINISHED ====" << endl;

	return 0;

}