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
// pin name:			 RS  RW  EN  D4  D5  D6  D7
unsigned int GPIO[7] = { 66, 67, 69, 68, 45, 44, 26 };

void initPinMode() {
	for (int i = 0; i < 7; i++) {
		gpio_export(GPIO[i]);
		gpio_set_dir(GPIO[i], OUTPUT_PIN);
		gpio_set_value(GPIO[i], LOW);
	}
	cout << "==== FINISH INIT PIN MODE ====" << endl;
}

// This way is far slower then it should (see datasheet)
// TODO: Here I should split it into two parts:
// 		 Put this pin high before I change datas on DB pins
// 		 Put this pin low that is acutally what "send()" means
void send() {
	usleep(1); // the minimum time is 450 nano second, here I do 5000 nano sec.
	gpio_set_value(GPIO[2], HIGH);
	usleep(5); // the minimum time is 450 nano second, here I do 5000 nano sec.
	gpio_set_value(GPIO[2], LOW);
}

void setData(uint8_t data) { // Argument is a 4bit instruction set

#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))
	const int w = CHECK_BIT(data, 0);
	const int a = CHECK_BIT(data, 1);
	const int b = CHECK_BIT(data, 2);
	const int c = CHECK_BIT(data, 3);
	printf("bits: 1 2 3 4\n");
	printf("      %u %u %u %u\n", w, a, b, c );

	// I literally spend 90% time of the night on figuring out the priority issue here...
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

void setRS_HIGH(){
	gpio_set_value(GPIO[0], HIGH);
}

void setRS_LOW(){
	gpio_set_value(GPIO[0], LOW);
}

// get busy flag state in 4bit mode
bool getBF(){
	usleep(1);
	setModeRead();
	gpio_set_value(GPIO[0], LOW);
	gpio_set_dir(GPIO[6], INPUT_PIN);
	unsigned int bf = 1000;
	// Do this checking in while loop till BF is get LOW -> 0 again
	while(bf != 0){
		gpio_set_value(GPIO[2], HIGH);
		usleep(5); // the minimum time is 450 nano second, here I do 5000 nano sec.
		gpio_get_value(GPIO[6], &bf);
		gpio_set_value(GPIO[2], LOW);
		usleep(1);
		gpio_set_value(GPIO[2], HIGH);
		usleep(5); // the minimum time is 450 nano second, here I do 5000 nano sec.
		gpio_set_value(GPIO[2], LOW);
		//	usleep(1);
		cout << "++++ BF: " << bf << " ++++"<< endl;
	}
	gpio_set_dir(GPIO[6], OUTPUT_PIN);
	return true;
}

// Set the position of the cursor, "Set CGRAM addresss"
void moveCursor(unsigned int p){
	setRS_LOW();
	setModeWrite();

	// Hard coded position: -> 5 ( start from 0)
	setData(0b1000); send();
	setData(0b0101); send();

	if (cin.get() == '\n') cout << "." << endl;

	setData(0b1010); send();
	setData(0b1001); send(); // The second character on the second line

//	if (cin.get() == '\n') cout << "." << endl;
//
//	setData(0b1101); send();
//	setData(0b0001); send(); // The second character on the second line

	getBF(); // TODO: I believe I have some interesting problem: For each new command, I check the BF but not after each executed function
}

void displayChar(char c){
	cout << "==== DISPLAYCHAR() ====" << endl;
	setRS_HIGH(); // For writing
	setModeWrite();
	setData(( ((int)c) & 0xF0) >> 4);
	send();
	setData(( ((int)c) & 0x0F));
	send();
	getBF();
}

int print(string str){
	cout << "RPINT PASSED IN STRING: " << str << endl;
	unsigned int _rs = 1000;
	gpio_get_value(GPIO[0], &_rs); // TODO: should I set this pin as input when asking its state?
	setRS_HIGH();
	for(unsigned int i=0; i<str.length(); i++){
		displayChar(str.at(i));
	}
	return 0;
}

void clear(){
	setModeWrite();
	gpio_set_value(GPIO[0], LOW);
	cout << "#### clear display: stage 1 ####" << endl;
	setData(0b0000);
	send();
	setData(0b0001);
	send();
	getBF();
	cout << "FINISH clear()" << endl;
}

int main(int argc, char *argv[]) {

	cout << "=== INIT PIN STATE ===" << endl;
	initPinMode();

	cout << "=== INIT STEP ONE: TURN TO 4-BIT MODE ===" << endl;
	setModeWrite();
	setData(0b0010); send();
	setData(0b1000); send();
	getBF();

	cout << "==== INIT STEP TWO: FUNCTION SET ====" << endl;
	setModeWrite();
	setData(0b0010); send();
	setData(0b1000); send();
	getBF();

	cout << "==== INIT STEP THREE: DISPLAY ON/OFF ====" << endl;
	setModeWrite();
	setData(0b0000); send();
	setData(0b1111); send();
	getBF();

	cout << "==== INIT STEP FOUR: CLEAR DISPLAY ====" << endl;
	setModeWrite();
	setData(0b0000); send();
	setData(0b0001); send();
	getBF();

	cout << "==== INIT STEP FIVE: ENTRY> MODE SET ====" << endl;
	setModeWrite();
	setData(0b0000); send();
	setData(0b0110); send();
	getBF();

	cout << "And now all init steps are finished, press Enter to finish the init process" << endl;
	cout << "==== ALL FINISHED ====" << endl;
	if (cin.get() == '\n') cout << "." << endl;

	setModeWrite();
	print("123456789 123456789 ");
	getBF();

	// Clear display
	clear();

	moveCursor(5);

	print("Hallo Qiao!");

	return 0;
}

//	if (cin.get() == '\n') cout << "." << endl; // This will hack the program till the user press enter key to going on
