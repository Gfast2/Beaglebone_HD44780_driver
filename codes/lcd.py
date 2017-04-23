import Adafruit_BBIO.GPIO as GPIO
import time

#==============================================================================#
#   Beaglebone Black script to interface with an HD4478OU Protocol LCD screen
#==============================================================================#
#
#   Authors: Ben Hammel, Erik McKee
#   Date: 2014-12-1

#----------------------------------------------------------------------#
#   Pins used to transfer data to the LCD display
#----------------------------------------------------------------------#
PINS = {'E':'P9_24',    # pin 6 on LCD
        'RS':'P9_23',   # pin 4 on LCD
        'DB7':'P9_22',  # pin 14 on LCD
        'DB6':'P9_21',  # pin 13 on LCD
        'DB5':'P9_16',  # pin 12 on LCD
        'DB4':'P9_15',  # pin 11 on LCD
        'DB3':'P9_14',  # pin 10 on LCD
        'DB2':'P9_13',  # pin 9 on LCD
        'DB1':'P9_12',  # pin 8 on LCD
        'DB0':'P9_11'   # pin 7 on LCD
        }

#----------------------------------------------------------------------#
#   Font Table
#----------------------------------------------------------------------#
#
#   Corresponding 8bit definition for each character
#
CHARS = {'a':'01100001', 'b':'01100010', 'c':'01100011', 'd':'01100100', 
         'e':'01100101', 'f':'01100110', 'g':'01100111', 'h':'01101000', 
         'i':'01101001', 'j':'01101010', 'k':'01101011', 'l':'01101100', 
         'm':'01101101', 'n':'01101110', 'o':'01101111', 'p':'01110000', 
         'q':'01110001', 'r':'01110010', 's':'01110011', 't':'01110100',
         'u':'01110101', 'v':'01110110', 'w':'01110111', 'x':'01111000', 
         'y':'01111001', 'z':'01111010', 'A':'01000001', 'B':'01000010', 
         'C':'01000011', 'D':'01000100', 'E':'01000101', 'F':'01000110', 
         'G':'01000111', 'H':'01001000', 'I':'01001001', 'J':'01001010', 
         'K':'01001011', 'L':'01001100', 'M':'01001101', 'N':'01001110', 
         'O':'01001111', 'P':'01010000', 'Q':'01010001', 'R':'01010010', 
         'S':'01010011', 'T':'01010100', 'U':'01010101', 'V':'01010110', 
         'W':'01010111', 'Y':'01011001', 'Z':'01011010', '1':'00110001',
         '2':'00110010', '3':'00110011', '4':'00110100', '5':'00110101',
         '6':'00110110', '7':'00110111', '8':'00111000', '9':'00111001',
         '0':'00110000', ':':'00111010', ';':'00111011', '<':'00111100',
         '=':'00111101', '>':'00111110', '?':'00111111', '!':'00100001',
         '#':'00100011', '$':'00100100', '%':'00100101', '&':'00100110',
         '(':'00101000', ')':'00101001', '*':'00101010', '+':'00101011',
         ',':'00101100', '-':'00101101', '.':'00101110', '/':'00101111',
         ' ':'11111110' 
         }

#----------------------------------------------------------------------#
#   Helper functions 
#----------------------------------------------------------------------#

# Turn all GPIO pins to outputs and set their values to low
def initalizePins(pins):
    for pin in pins.itervalues():
        GPIO.setup(pin, GPIO.OUT)

    set_all_low(pins)

# Pause the script for 2 ms
def wait():
    time.sleep(.002)

# Set the GPIO pin to a state that's passed to it 
def set_to_state(pin, state):
    GPIO.output(pin, int(state))

# Turn a pin on 
def set_high(pin):
    set_to_state(pin, GPIO.HIGH)

# Turn a pin off
def set_low(pin):
    set_to_state(pin, GPIO.LOW)

# Turn all pins off
def set_all_low(pins):
    for pin in pins.itervalues():
        set_low(pin)

#----------------------------------------------------------------------#
#   LCD control 
#----------------------------------------------------------------------#
class Screen:

    # Necessary steps to turn on the LCD display
    def __init__(self, bit_mode=8, cursor_status='blinking'):

        if bit_mode not in [4,8]:
            bit_mode = 8

        self.bit_mode = bit_mode
        self.pins = dict(PINS)

        # remove pins not used in 4 bit mode
        if self.bit_mode == 4:
            for key in ['DB0', 'DB1', 'DB2', 'DB3']:
                self.pins.pop(key)

        # Initialize screen
        initalizePins(self.pins)
        self.functionSet()
        self.on(cursor_status)
        self.entryModeSet()
        self.clear()

    # Tell the LCD to read in the command 
    def transfer(self):
        set_high(PINS['E'])
        set_all_low(self.pins)

    # Turn the GPIO pins on so the LCD can read them in 
    def sendCommand(self, command, initializing=False):
        set_to_state(PINS['RS'],command[0])
        # Pin R/W is held to ground in the circuit - W only, ignore this bit
        set_to_state(PINS['DB7'], command[2])
        set_to_state(PINS['DB6'], command[3])
        set_to_state(PINS['DB5'], command[4])
        set_to_state(PINS['DB4'], command[5])

        # functionSet is always transfered in parallel - if initializing in 4 bit 
        # mode, don't bother executing the second half of the command
        if self.bit_mode == 8:
            set_to_state(PINS['DB3'], command[6])
            set_to_state(PINS['DB2'], command[7])
            set_to_state(PINS['DB1'], command[8])
            set_to_state(PINS['DB0'], command[9])
        elif self.bit_mode ==4 and not initializing:
            self.transfer()
            set_to_state(PINS['RS'],command[0])
            set_to_state(PINS['DB7'], command[6])
            set_to_state(PINS['DB6'], command[7])
            set_to_state(PINS['DB5'], command[8])
            set_to_state(PINS['DB4'], command[9])

        self.transfer()


    # Clear Display
    def clear(self):
        self.sendCommand('0000000001')

    # Set charecteristics of the LCD screen
    def functionSet(self):
        if self.bit_mode == 4:
            self.sendCommand('0000101100', initializing = True)
        else:
            self.sendCommand('0000111100')

    # Turn the LCD on   
    # Turn the cursor on or off and set it's blinking or on or off
    # If nothing is set, or invalid option, set cursor to blinking
    def on(self, cursor_status='blinking'):
        status_command = {'blinking':'11', 'on':'10', 'off':'00'}
        try:
            command = '00000011' + status_command[cursor_status]
        except:
            command = '00000011' + status_command['blinking']
        self.sendCommand(command)

    # Turn the LCD off
    def off(self):
        self.sendCommand('0000001000')

    # Sets cursor move direction, write left to right  
    def entryModeSet(self):
        self.sendCommand('0000000110')

    # Move cursor to the start of desired line
    def moveCursor(self, line_number):
        line_command = {1:'0000000', 2:'1000000', 3:'0010100', 4:'1010100'}
        try:
            command='001'+ line_command[line_number]
        except:
            command='001'+ line_command[1]
        self.sendCommand(command)

    # Print a 20 char line to the LCD
    def printLine(self, line, line_number=1):
        # limit the length of the string, turn the RS pin to high to start
        # data transfer.
        self.moveCursor(line_number)
        if len(line)>20:
            line = line[:20]

        #  for each character in the line, execute the correct command 
        for char in line:
            try:
                command = '10' + CHARS[char]
            except:
                command = '10' + CHARS['?']
            self.sendCommand(command) 







