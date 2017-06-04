# **LOG**
---
This is the **Dairy** of this Project. Visit [**my website**](http://www.gao-su.com) for more information.

## TODO ##

Add the function that print "print"

## FINISHED ##

### ####### 13.05.2017 ####### ###

##### Test Level Converter with Arduino Mega, Osciliscope
- build circuit with one Level-Converter with 3.3V & 5V volt from arduino
- hook Scopte on one paired channel
- Test functionalities with a jumper wire one both side, figure out what does Level Converter do
> - Figure out this Level Converter has built in pull-up resister.
> - If `SCL` and `SDA` of BlinkM is floating, `SDA` is pulled up, `SCL` is pull low.

##### Test with BlinkM, See what does Beaglebone do & If he can find out BlinkM (Default Adresse 0x09)
The following are test notes:
When without connect commen ground, only test the I2C Port
-> There is no problem with command "i2cdetect -r 2" at all.
When without connect 3.3V, 5V and GND on Level Converter and its Breadboard (I2C only)
-> this command crashed / doesn't probing at all.

When with GND, 3.3V (from Beaglebone) on Level Converter (LC in following texts),
but with out BlinkM connected to other side of LC
-> I2C signal get a even better shape.

When add 5V (From Arduino) on the breadboard, besides 3.3V, GND.
And the GNDs from both sides of LC are separated from each other
-> The scope said, there is probing signal on SCL, SDA is pulled down permanently
-> I'm thinking, that there is a short circuit on SDA of BlinkM
-> (After a long probing around with scope) The BlinkM pulled down SDA permanently as reaction. Likely wanna say something to Beaglebone, but without success.
> - After all these test, I think the command that i2cdetect are using for probing diffrent ports are for BlinkM unknow.

find overo-blinkm lbrary on github that let me control BlinkM without problem anymore

### ####### 15.05.2017 ####### ###
Build-Up cross-compile toolchain for deploying software from Eclipse to Beaglebone

### ####### 16.05.2017 ####### ###
- Test Some of the free GPIO pins with scope

- Figure out even I've run as `root` for controlling LCD control pins (like P8_46) I still just can't toggle its value in real (Software aspect permitted though)

### ####### 17.05.2017 ####### ###
- Read [this](http://www.cplusplus.com/forum/articles/10627/) about how c/c++ is compiled
- Solve Problem: import a header file from other directory -> just use relative path "../targetDirectory/neededHeader.h"

### ####### 18.05.2017 ####### ###
- resolve "gpio_get_value()" / "gpio_set_value()" in LiquidCrystalFast.cpp
- **Marion** think
  - Its bad practice to define header in source code. All user defined included header should be saved in a folder, and this directory (folder) should be defined in makefiles
  - I should build the proof of concept prototype that without any "_libx_" (third party library) that control the display
- Figured out this say: `LiquidCrystalFast` class **extend** class `Print` (from Arduino) (In source code of `LiquidCrystalFast.h`)
```
class LiquidCrystalFast : public Print { ... }
```
### ####### 19.05.2017 ####### ###
Tip of the day: (From _Marion_) 
- build a dummy init() that configure the display correctely (into 4-bit mode, possibly after this there would be a blinking cursor)
- build a dummy begin() that do somehow the same thing as init().
- build the circuit
- build a dummy that display somehow some character
- **ps**: All of above shouold happen in main.cpp firstly 
/ **Alex**: In Eclipse: On Project's name -Right Mouse-> Index -> Rebuild
/ On _HD44780.pdf_ Page:42 is a example instruction on how to use 4 bit mode control this display
/ On _HD44780.pdf_ Page:46 _Figure 24_ 4-Bit Interface initialization
/ Build up the "Hello World!" Sketch that control HD44780 directly
- sketch this code
- compile it without error
- **LOG**: LiquidCrystalFast.cpp are using function from SimpleGPIO.cpp, It just can't.
- Early Morning of this day, figure out how to setup debugger session (cross-compile) on Eclipse

### ####### 20.05.2017 ####### ###

try to init the display
- figure out how to get "cin" and get read the input
- display the pin status on each step (step by step do the diagnose)
- set it into 4 bit mode & turn on the blinking cursor

### ####### 21.05.2017 ####### ###
today I write a big scaffold to walk through the initiation of the display.
- ps: It's with out Busy-Flag checking
- ps: Display will not initiated correctly on the second time of the same instruction set. But the third time everything goes well again. (every two times)
- ps: in pdf "4_16.pdf" on page 4 state: Im 4-Bit Mode ist die Busy-Abfrage vor jedem Bytezugriff notwendig.

### ####### 22.05.2017 ####### ###
Add the Busy-Flag read function in 4bit mode
try to figure out how to parse input as char[] and display them respectively

### ####### 25.05.2017 ####### ###
Make routine to display string & move cursor correctly

### ####### 04.04.2017 ####### ###
After a very long frastration of "Why this prototype won't work anymore" I correct init the display again
- ps: I belive, last time I try something new alway with a properly inited display.
- Finish a physical adapter for this project, and configure the software correctly for it.
