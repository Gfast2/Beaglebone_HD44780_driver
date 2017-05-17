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
#include <unistd.h>
#include "./gpio/SimpleGPIO.h"
#include "./LiquidCrystalBBB/LiquidCrystalFast.h"
using namespace std;

unsigned int LEDGPIO = 45; // P8_11 on Beaglebone
unsigned int TESTGPIO = 45; // 71; // P8_46, GPIO_71

unsigned int HD44780[7] = {
	66, 67, 69, 68, 45, 44, 26
};

int main(int argc, char *argv[]) {
	cout << "!!!Start Testing.!!!" << endl; // prints !!!Start Testing.!!!

	cout << "Testing the GPIO Pins!" << endl;

	for(int i=0; i<7; i++){
		gpio_export(HD44780[i]);
		gpio_set_dir(HD44780[i], OUTPUT_PIN);
	}

	// Flash the LED pin 10 times
	for(int i=0; i<5; i++){
		cout << "Setting the PIN on" << endl;
		for(int i=0; i<7; i++){
			gpio_set_value(HD44780[i], HIGH);
		}
		usleep(200000); // on for 200ms
		cout << "Setting the PIN off" << endl;
		for(int i=0; i<7; i++){
			gpio_set_value(HD44780[i], LOW);
		}
		usleep(200000); // off for 200ms
	}

	return 0;
}
