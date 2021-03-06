#include "LiquidCrystalFast.h"
#include <stdio.h>
#include <string.h>

typedef unsigned char BYTE; // replace datatype "byte" from Arudino ecosystem. // TODO: Try to replace this specified datatype with "unit8_t"

/******************************************/
/**  hardware initialization             **/
/******************************************/

// When the display powers up, it is configured as follows:
//
// 1. Display clear
// 2. Function set: 
//    DL = 1; 8-bit interface data 
//    N = 0; 1-line display 
//    F = 0; 5x8 dot character font 
// 3. Display on/off control: 
//    D = 0; Display off 
//    C = 0; Cursor off 
//    B = 0; Blinking off 
// 4. Entry mode set: 
//    I/D = 1; Increment by 1 
//    S = 0; No shift 
//
// Note, however, that resetting the Arduino doesn't reset the LCD, so we
// can't assume that its in that state when a sketch starts (and the
// LiquidCrystalFast constructor is called).

void LiquidCrystalFast::init(
	uint8_t rs, uint8_t rw, uint8_t enable, uint8_t en2,
	uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3
){
	_rs_pin = rs;
	_rw_pin = rw;
	_enable_pin = enable;
	_en2 = en2;
	_chip = 0;
	_scroll_count = 0;  //to fix the bug if we scroll and then setCursor w/o home() or clear()
	_x = 0;
	_y = 0;
	
	_setCursFlag = 0;
	_direction = LCD_Right;

	_data_pins[0] = d0;
	_data_pins[1] = d1;
	_data_pins[2] = d2;
	_data_pins[3] = d3; 

	gpio_set_dir(d0, OUTPUT_PIN); //set data pin modes, // TODO: Why _data_pins[n] aren't used? Paul, is that from you?
	gpio_set_dir(d1, OUTPUT_PIN);
	gpio_set_dir(d2, OUTPUT_PIN);
	gpio_set_dir(d3, OUTPUT_PIN);

	row_offsets[0] = 00;   // DDRAM addresses inside the HD44780 are strange: 0-nColumns-1 on line 0
	row_offsets[1] = 0x40; // 64-(63+nColumns) for line 1
	row_offsets[2] = 0x14; // 20- (19+nColumns) for line 2 --- NOTHING FROM 40-63 !
	row_offsets[3] = 0x54; // 84 - (83+nColumns) for line 3  -- so 80 characters tops out at #103 !
	
	gpio_set_dir(_rs_pin, OUTPUT_PIN);
	// we can save 1 pin by not using RW. Indicate by passing 255 instead of pin#
	if (rw != 255) { 
		gpio_set_dir(rw, OUTPUT_PIN);  //once in init does it
		gpio_set_value(rw,LOW); //write data to LCD mode: LOW->Write to Display, HIGH->Read from Display
	}
	gpio_set_dir(_enable_pin, OUTPUT_PIN);
	if( en2 != 255) gpio_set_dir(en2,OUTPUT_PIN);  //4X40 LCD

	begin(20, 1); // TODO: Figure out which "begin()" is called here, why call a 20x1 one but not like 16x2 one?
	// _rw_pin = rw;         //the game to initialize the 40x4 is over, // There is still some dirty tweak here! @Paul
}

void LiquidCrystalFast::begin(uint8_t cols, uint8_t lines, uint8_t dotsize)
{
	numcols=_numcols=cols;    //there is an implied lack of trust; the private version can't be munged up by the user.
	numlines=_numlines=lines;
	row_offsets[2] = cols + row_offsets[0];  //should autoadjust for 16/20 or whatever columns now
	row_offsets[3] = cols + row_offsets[1];
	begin2( cols,  lines,  dotsize, _enable_pin);
	
	if (_en2 != 255) {                      //if we were called with a 2nd enable line i.e. 4x40 LCD
		row_offsets[2] = 0;
		row_offsets[3] = 0x40;              //each line gets its own little 40 char section of DDRAM--would be fine if there were a 4x32, I suppose
		_chip = 2;
		begin2( cols,  lines,  dotsize,_en2);//initialize the second HD44780 chip
	}
}

void LiquidCrystalFast::begin2(uint8_t cols, uint8_t lines, uint8_t dotsize, uint8_t enable)
{  
	uint8_t	displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;

	if (lines > 1) {
		displayfunction |= LCD_2LINE;
	}
	
	// for some 1 line displays you can select a 10 pixel high font
	if ((dotsize != 0) && (lines == 1)) {
		displayfunction |= LCD_5x10DOTS;
	}
	
	// SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
	// according to datasheet, we need at least 40ms after power rises above 2.7V
	// before sending commands. Arduino can turn on way before 4.5V.
	// is this delay long enough for all cases??
	for (uint8_t i=0;i<18;i++) {
		// http://stackoverflow.com/questions/1157209/is-there-an-alternative-sleep-function-in-c-to-milliseconds
		struct timespec ts = {0, 7500000 };
		nanosleep (&ts, NULL);
		// usleep(7500);
		// delayMicroseconds(7500);
	}

	// Now we pull both RS and R/W low to begin commands
	gpio_set_value(_rs_pin, LOW);
	gpio_set_value(enable, LOW);
	
	//put the LCD into 4 bit mode
	// this is according to the hitachi HD44780 datasheet
	// figure 24, pg 46
		
	// we start in 8bit mode, try to set 4 bit mode
	// at this point we are in 8 bit mode but of course in this
	// interface 4 pins are dangling unconnected and the values
	// on them don't matter for these instructions.
	gpio_set_value(_rs_pin, LOW);
	write4bits(0x03);
	// delayMicroseconds(5000); // I have one LCD for which 4500 here was not long enough.
	// usleep(5000);
	struct timespec ts = {0, 5000000L };
	nanosleep (&ts, NULL);
	// second try
	write4bits(0x03);      
	// delayMicroseconds(150); // wait 
	// usleep(150);
	ts = {0, 150000L };
	nanosleep (&ts, NULL);
	// third go!
	write4bits(0x03); 
	// delayMicroseconds(150);
	// usleep(150);
	nanosleep (&ts, NULL);
	// finally, set to 4-bit interface
	write4bits(0x02); 
	// delayMicroseconds(150);
	//usleep(150);
	nanosleep (&ts, NULL);
	
	// finally, set # lines, font size, etc.
	command(LCD_FUNCTIONSET | displayfunction);  
	
	// turn the display on with no cursor or blinking default
	_displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;  display();
	
	// clear it off
	clear();
	
	// Initialize to default text direction (for romance languages)
	_displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
	// set the entry mode
	command(LCD_ENTRYMODESET | _displaymode);	
}


/******************************************/
/**  high level commands, for the user!   */
/******************************************/

void LiquidCrystalFast::clear()
{
	if (_en2 != 255) {
		_chip=2;
		command(LCD_CLEARDISPLAY); 
		_chip=0;
		command(LCD_CLEARDISPLAY);
		delayPerHome();
		setCursor(0,0);
	} else {
		command(LCD_CLEARDISPLAY);  // clear display, set cursor position to zero
		delayPerHome();
	}
	_scroll_count = 0;
}

void LiquidCrystalFast::home()
{
	commandBoth(LCD_RETURNHOME);  // set cursor position to zero both chips.
	delayPerHome();
	_scroll_count = 0;
	if (_en2 != 255) setCursor(0,0); 
}


// Turn the display on/off (quickly)
void LiquidCrystalFast::noDisplay() {
	_displaycontrol &= ~LCD_DISPLAYON;
	commandBoth(LCD_DISPLAYCONTROL | _displaycontrol);  //both chips
}
void LiquidCrystalFast::display() {
	_displaycontrol |= LCD_DISPLAYON;
	commandBoth(LCD_DISPLAYCONTROL | _displaycontrol);   //both chips
}

// Turns the underline cursor on/off
void LiquidCrystalFast::noCursor() {
	_displaycontrol &= ~LCD_CURSORON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void LiquidCrystalFast::cursor() {
	_displaycontrol |= LCD_CURSORON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turn on and off the blinking cursor
void LiquidCrystalFast::noBlink() {
	_displaycontrol &= ~LCD_BLINKON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void LiquidCrystalFast::blink() {
	_displaycontrol |= LCD_BLINKON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// These commands scroll the display without changing the RAM
void LiquidCrystalFast::scrollDisplayLeft(void) {
	commandBoth(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);  //both chips
	_scroll_count++;
	if (_scroll_count >= 40) _scroll_count -= 40;   //  -39< scroll_count<39
}
void LiquidCrystalFast::scrollDisplayRight(void) {
	commandBoth(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);  //both chips
	_scroll_count--;
	if (_scroll_count <= -40) _scroll_count += 40;
}

// This is for text that flows Left to Right
void LiquidCrystalFast::leftToRight(void) {
	_displaymode |= LCD_ENTRYLEFT;
	_direction = LCD_Right;
	commandBoth(LCD_ENTRYMODESET | _displaymode);     //both chips
}

// This is for text that flows Right to Left
void LiquidCrystalFast::rightToLeft(void) {
	_displaymode &= ~LCD_ENTRYLEFT;
	_direction = LCD_Left;
	commandBoth(LCD_ENTRYMODESET | _displaymode);    //both chips
}


// Allows us to fill the first 8 CGRAM locations
// with custom characters
void LiquidCrystalFast::createChar(uint8_t location, uint8_t charmap[]) {    
	location &= 0x7; // we only have 8 locations 0-7
	if (_en2 == 255) {
		command(LCD_SETCGRAMADDR | (location << 3));
		for (int i=0; i<8; i++) {
			send(charmap[i],HIGH);
		}
	}
	else {
		uint8_t chipSave = _chip;
		_chip = 0;
		command(LCD_SETCGRAMADDR | (location << 3));
		for (uint8_t i=0; i<8; i++) {
			send(charmap[i],HIGH);
		}
		_chip = 2;
		command(LCD_SETCGRAMADDR | (location << 3));
		for (uint8_t i=0; i<8; i++) {
			send(charmap[i],HIGH);
		}
		_chip = chipSave;
	}
}

void LiquidCrystalFast::setCursor(uint8_t col, uint8_t row)         // this can be called by the user but is also called before writing some characters.
{
	if ( row > _numlines ) {
		row = _numlines-1;    // we count rows starting w/0
	}
	_y = row;
	_x = col;
	_setCursFlag = 0;                                                 //user did a setCursor--clear the flag that may have been set in write()
	int8_t high_bit = row_offsets[row] & 0x40;                        // this keeps coordinates pegged to a spot on the LCD screen even if the user scrolls right or
	int8_t  offset = col + (row_offsets[row] &0x3f)  + _scroll_count; //left under program control. Previously setCursor was pegged to a location in DDRAM
	//the 3 quantities we add are each <40
	if (offset > 39) offset -= 40;                                    // if the display is autoscrolled this method does not work, however.
	if (offset < 0) offset += 40;
	offset |= high_bit;
	if (_chip != (row & 0b10)) noCursor();  //turn off cursor on chip we are leaving
	_chip = row & 0b10;                     //if it is row 0 or 1 this is 0; if it is row 2 or 3 this is 2
	command(LCD_SETDDRAMADDR | (BYTE) offset );
}

// This will 'right justify' text from the cursor 
void LiquidCrystalFast::autoscroll(void) {           //to count the number of times we scrolled; here we'd need to keep track of microseconds and divide. I'm not going there.
	_displaymode |= LCD_ENTRYSHIFTINCREMENT;
	commandBoth(LCD_ENTRYMODESET | _displaymode);   //both chips
}

// This will 'left justify' text from the cursor
void LiquidCrystalFast::noAutoscroll(void) {
	_displaymode &= ~LCD_ENTRYSHIFTINCREMENT;   //both chips
	commandBoth(LCD_ENTRYMODESET | _displaymode);
}


/*************************************************/
/**  mid level commands, for sending data/cmds   */
/*************************************************/

inline void LiquidCrystalFast::command(uint8_t value) {
	send(value, LOW);
}

//for many of the commands that need to be sent twice if 2 controller chips
void LiquidCrystalFast::commandBoth(uint8_t value)
{
	if (_en2 == 255) {
		send(value, LOW);  // normal LCD, single controller chip
	} else {
		uint8_t chipSave = _chip;
		_chip = 0;
		send(value,LOW);   //send command to first HD44780
		_chip = 2;
		send(value,LOW);   //send to 2nd HD44780
		_chip=chipSave;
	}
}

// It was write method (Depending on what version of arduino are used.)
//print calls  this to send characters to the LCD
/*
#if defined(ARDUINO) && ARDUINO >= 100
size_t LiquidCrystalFast::write(uint8_t value) {
#else
void LiquidCrystalFast::write(uint8_t value) {
#endif
*/
void LiquidCrystalFast::write(uint8_t value) { // TODO: I'm not clear how does this Arduino write do indeed. should check it out later

	if ((_scroll_count != 0) || (_setCursFlag != 0) ) setCursor(_x,_y);   //first we call setCursor and send the character
	if ((value != '\r') && (value != '\n') ) send(value, HIGH);

	_setCursFlag = 0;
	if (_direction == LCD_Right) {                    // then we update the x & y location for the NEXT character
		_x++;
		if ((value == '\r') ||(_x >= _numcols) ) {      //romance languages go left to right
			
			_x = 0;
			_y++;
			_setCursFlag = 1;          //we'll need a setCursor() before the next char to move to begin of next line
		}
	} else {
		_x--;
		if ( (value == '\n') || (_x < 0)) {   //emulate right to left text mode which is built in but would be defeated by my code above
			_x = _numcols-1;
			_y++;
			_setCursFlag = 1;
		}
	}
	if (_y >= _numlines) _y = 0;   //wrap last line up to line 0
// #if defined(ARDUINO) && ARDUINO >= 100
// 	return 1;
// #endif
// }
}


/****************************************/
/**  low level data pushing commands   **/
/****************************************/

// write either command or data, with automatic 4/8-bit selection
void LiquidCrystalFast::send(uint8_t value, uint8_t mode) {
	uint8_t en = _enable_pin;
	if ((_en2 != 255) && (_chip)) en = _en2;
	if (_rw_pin == 255) {
		// delayMicroseconds(DELAYPERCHAR);
//		usleep(DELAYPERCHAR);
		struct timespec ts = {0, DELAYPERCHAR*1000 };
		nanosleep (&ts, NULL);
	} else {
		gpio_set_dir(_data_pins[0], INPUT_PIN);
		gpio_set_dir(_data_pins[1], INPUT_PIN);
		gpio_set_dir(_data_pins[2], INPUT_PIN);
		gpio_set_dir(_data_pins[3], INPUT_PIN);
		gpio_set_value(_rw_pin, HIGH);
		gpio_set_value(_rs_pin, LOW);
		uint8_t busy;
		do {
			gpio_set_value(en, HIGH);
			unsigned int *bz; // tmp variable for passing read value
			gpio_get_value(_data_pins[3], bz); // TODO: figure out if this is equal to the old one.
//			busy = digitalRead(_data_pins[3]);
			busy = *bz;
			gpio_set_value(en, LOW);
			gpio_set_value(en, HIGH);
			gpio_set_value(en, LOW);
		} while (busy == HIGH);
		gpio_set_dir(_data_pins[0], OUTPUT_PIN);
		gpio_set_dir(_data_pins[1], OUTPUT_PIN);
		gpio_set_dir(_data_pins[2], OUTPUT_PIN);
		gpio_set_dir(_data_pins[3], OUTPUT_PIN);
		gpio_set_value(_rw_pin, LOW);
	}
	gpio_set_value(_rs_pin, mode == 1 ? HIGH : LOW);

	gpio_set_value(_data_pins[0], (value & 0x10) == 1 ? HIGH : LOW);
	gpio_set_value(_data_pins[1], (value & 0x20) == 1 ? HIGH : LOW);
	gpio_set_value(_data_pins[2], (value & 0x40) == 1 ? HIGH : LOW);
	gpio_set_value(_data_pins[3], (value & 0x80) == 1 ? HIGH : LOW);
	gpio_set_value(en, HIGH);   // enable pulse must be >450ns // TODO: Use scope to make sure Beaglebone won't do it "too fast" for this
	gpio_set_value(en, LOW);

	gpio_set_value(_data_pins[0], (value & 0x01) == 1 ? HIGH : LOW);
	gpio_set_value(_data_pins[1], (value & 0x02) == 1 ? HIGH : LOW);
	gpio_set_value(_data_pins[2], (value & 0x04) == 1 ? HIGH : LOW);
	gpio_set_value(_data_pins[3], (value & 0x08) == 1 ? HIGH : LOW);
	gpio_set_value(en, HIGH);   // enable pulse must be >450ns
	gpio_set_value(en, LOW);
}

// used during init
void LiquidCrystalFast::write4bits(uint8_t value)
{
	uint8_t v=value;
	uint8_t *pinptr = _data_pins;
	gpio_set_value(*pinptr++, (v & 1) == 1 ? HIGH : LOW );
	gpio_set_value(*pinptr++, (( v >>= 1) & 1) == 1 ? HIGH : LOW );
	gpio_set_value(*pinptr++, (( v >>= 1) & 1) == 1 ? HIGH : LOW );
	gpio_set_value(*pinptr++, (( v >>= 1) & 1) == 1 ? HIGH : LOW );
//	byte en = _enable_pin;
	BYTE en = _enable_pin;
	if ((_en2 != 255) && (_chip)) en = _en2;    // 4x40 LCD with 2 controller chips with separate enable lines if we called w 2 enable pins and are on lines 2 or 3 enable chip 2  
	gpio_set_value(en, HIGH);   // enable pulse must be >450ns
	gpio_set_value(en, LOW);
}
 
