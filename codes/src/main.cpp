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

// For "better sleep":
// https://stackoverflow.com/questions/158585/how-do-you-add-a-timed-delay-to-a-c-program
//#include <chrono> // For sleep type
//#include <thread> // For sleep type
//using namespace std::this_thread;
//using namespace std::chrono;

// gpio number on Beaglebone
// pin name:             rs, rw, en, d4, d5, d6, d7
unsigned int GPIO[7] = { 66, 67, 69, 45, 68, 44, 26 }; // On Prototype Board

void initPinMode() {
	for (int i = 0; i < 7; i++) {
		gpio_export(GPIO[i]);
		gpio_set_dir(GPIO[i], OUTPUT_PIN);
		gpio_set_value(GPIO[i], LOW);
	}
	cout << "==== FINISH INIT PIN MODE ====" << endl;
}

// This way is far slower then it should (see datasheet)
void send() {
	gpio_set_value(GPIO[2], HIGH);
	usleep(500); // the minimum time is 450 nano second, here I do 500 micro second
	gpio_set_value(GPIO[2], LOW);
}

void setData(uint8_t data) { // Argument is a 4bit instruction set
#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))
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
	usleep(1000);
	setModeRead();
	gpio_set_value(GPIO[0], LOW);
	gpio_set_dir(GPIO[6], INPUT_PIN);
	unsigned int bf = 1000;
	// Do this checking in while loop till BF is get LOW -> 0 again
	while(bf != 0){
		gpio_set_value(GPIO[2], HIGH);
		usleep(500); // the minimum time is 450 nano second, here I do 5000 nano sec.
//		sleep_for( nanoseconds(1000) );
		gpio_get_value(GPIO[6], &bf);
		gpio_set_value(GPIO[2], LOW);
		usleep(100);
		gpio_set_value(GPIO[2], HIGH);
		usleep(500); // the minimum time is 450 nano second, here I do 5000 nano sec.
//		sleep_for( nanoseconds(1000) );
		gpio_set_value(GPIO[2], LOW);
		usleep(100);
		cout << "++++ BF: " << bf << " ++++"<< endl;
	}
	gpio_set_dir(GPIO[6], OUTPUT_PIN);
	return true;
}

// Human understandable position (0 -> characterPerLine x Line - 1)
int positionTrans(int p){
	const int h=16, l=2; // TODO: These settings should be placed somewhere else
	if(p < h){
		return p - 1;
	} else if(p > h*l){
		return 41 + h - 1;
	}
	return 41 + p - 16 - 1;
}

void sendData(int data) {
	setModeWrite();
	setData( (data&0xF0)>>4 ); send();
	setData(  data&0x0F     ); send();
	usleep(5);
	printf("In com data in sendData: %d\n", data);
	getBF();
}

void clear(){
	setModeWrite();
	gpio_set_value(GPIO[0], LOW);
	cout << "#### clear display: stage 1 ####" << endl;
	sendData(0b00000001);
	cout << "FINISH clear()" << endl;
}

void displayChar(char c){
	cout << "==== DISPLAYCHAR() ====" << endl;
	setRS_HIGH(); // For writing
	setModeWrite();
	sendData( (int)c );
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

// Set the position of the cursor, "Set CGRAM addresss"
void moveCursor(unsigned int p){
	setRS_LOW();
	setModeWrite();
	//	int finalPosi = 0b10000000 | hdPosition;

	// Hard coded position: -> 5 ( start from 0)
	setData(0b1000); send();
	setData(0b0101); send();

	if (cin.get() == '\n') cout << "." << endl;

	// The first character on the second line -> 41
	setData(0b1010); send();
	setData(0b1001); send();

	if (cin.get() == '\n') cout << "." << endl;
}

int main(int argc, char *argv[]) {
	cout << "######################" << endl;
	cout << "### PROGRAMM START ###" << endl;
	cout << "######################" << endl;
	cout << "------------------------------------------------------------------" << endl;

	cout << "=== INIT PIN STATE ===" << endl;
	initPinMode();

	cout << "=== INIT STEP ONE: TURN TO 4-BIT MODE ===" << endl;

	setModeWrite(); // Make sure rw pin low
	setRS_LOW(); // Make sure rs pin low

	setData(0b0010);
	if (cin.get() == '\n') cout << "Press Enter to send 4-bit mode command (very beginning)" << endl;
	send();

//	setData(0b1000); send();
//	sendData(0b00101000);
//	getBF();

	cout << "==== INIT STEP TWO: FUNCTION SET ====" << endl;
	sendData(0b00101000);

	cout << "==== INIT STEP THREE: DISPLAY ON/OFF ====" << endl;
	sendData(0b00001111);

	cout << "==== INIT STEP FOUR: CLEAR DISPLAY ====" << endl;
	sendData(0b00000001);

	cout << "==== INIT STEP FIVE: ENTRY> MODE SET ====" << endl;
	sendData(0b00000110);

	cout << "And now all init steps are finished, press Enter to finish the init process" << endl;
	cout << "==== ALL FINISHED ====" << endl;
	if (cin.get() == '\n') cout << "." << endl;

	setModeWrite();
	print("123456789 123456789 ");

	if (cin.get() == '\n') cout << "." << endl;
	clear();

	moveCursor(5);
	print("Hallo Yan :D !!");

	if (cin.get() == '\n') cout << "." << endl;
	moveCursor(5);
	print("Hallo Qiao!");

	return 0;
}
