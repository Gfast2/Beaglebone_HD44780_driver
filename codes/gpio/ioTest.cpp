/*
 * Tester toggle one GPIO on Beaglebone
 *
 */

#include <iostream>
#include <string>
#include <unistd.h> 
#include "SimpleGPIO.h"
using namespace std;

unsigned int LEDGPIO = 45; // P8_11 on Beaglebone

int main(int argc, char *argv[]){
    
	cout << "Testing the GPIO Pin P8_11." << endl;

	gpio_export(LEDGPIO); // It's exported automatically in Linux Kernel 4.4.36, (debian 8.7)

	// TODO: Setup the very 7 different GIPOs that belonges to Display control
	//       If I'd need some extra efforts to workaround these perhapse "zugeordnet" pins

	gpio_set_dir(LEDGPIO, OUTPUT_PIN);
	
	// Flash the LED pin 10 times
	for(int i=0; i<5; i++){
		cout << "Setting the LED on" << endl;
		gpio_set_value(LEDGPIO, HIGH);
		usleep(200000); // on for 200ms
		cout << "Setting the LED off" << endl;
		gpio_set_value(LEDGPIO, LOW);
		usleep(200000); // off for 200ms
	}
	

}
