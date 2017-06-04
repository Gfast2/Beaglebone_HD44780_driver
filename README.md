## Personal Project about exploring the control of a HD44780 through Beaglebone.

There are many LCD display around us these days. "Character Display" are fading out. I believe that is because the color display are getting cheaper and cheaper. Sure they can do much more things compare to a typical character display. But the character display is something like "7-Segment-Display", which belonge to the part of our memory. Since some days before I saw a youtube view showing around tuning the HD44780 Character Display, to let it show very very stylful stuffs. I decided to do something by myself.  

### Beautiful Demo from others
- [LCD Graphics MegaDemo HD44780](https://www.youtube.com/watch?v=HugwQ4b9iBU&t=79s)
- [HD44780 Graphic Transitions](https://www.youtube.com/watch?v=bN-XRXLx88Y) [_(Forum that talked about these MegaDemo)_](http://www.picbasic.co.uk/forum/showthread.php?t=18416)
- Odometer effect
[Link1](https://www.youtube.com/watch?v=T6DAZphHbjI)
[Link2](https://www.youtube.com/watch?v=eVILMbrI3ok#t=76.901)
- [LCD Audio VU meter as a shield of Arduino](https://www.youtube.com/watch?v=bONO13WHr2Q)
- [ChibiMo: Arduino + KS0108 graphics LCD = USB Monitor](https://www.youtube.com/watch?v=B0uevXj1ARM)
- [Tetris](https://www.youtube.com/watch?v=ZarVHGqWNkk)
- [16x2 LCD 8 bit Mario animation](https://www.youtube.com/watch?v=-fwm0jb9XOw)
- [Arduino animation using easing functions](https://www.youtube.com/watch?v=8jhDvsIcGIY)
[//]: # ( ASCII Demo not display related: https://www.youtube.com/watch?v=6-SmBeeXRBQ )

### Linux proc/daemon
- [Hd44780 LCD 40x4 Raspberry 1 Modell B (kodi clock demo)](http://www.forum-raspberrypi.de/Thread-libreelec-hd44780-lcd-40x4-raspberry-1-modell-b)
- [The lcdproc](http://lcdproc.org)

### WIKIs
- [Mikrokontroller](https://www.mikrocontroller.net/articles/HD44780)
- [wiki page of the built-in chip pru](http://elinux.org/Ti_AM33XX_PRUSSv2)
- [sprut.de](http://sprut.de/electronic/lcd/)

### Reference Link:
- http://www.thebrokendesk.com/post/writing-to-an-lcd-screen-with-the-beaglebone-black/
- https://github.com/Wicker25/Rpi-hw (C++ Library for Raspberry pi)
- https://www.youtube.com/watch?v=31IZaJWMYjU
- [How to Control LCD Displays | Arduino Tutorial.](https://www.youtube.com/watch?v=85LvW1QDLLw)
_(Basic working logic about how to control the display)_
- [github repo of the Beaglebone built-in chip pru](https://github.com/beagleboard/am335x_pru_package)
- [C Library for HD44780 Display control (TI M430)](http://www.instructables.com/id/C-Library-for-HD44780-LCD-Display-Controller/)
- [Text Tutorial about special character](http://www.circuitvalley.com/2012/02/lcd-custom-character-hd44780-16x2.html)
- [Paul's LiquidCrystal Library (LiquidCrystalFast) looks very very good for the working status for me.](https://www.pjrc.com/teensy/td_libs_LiquidCrystal.html)
- [4 bit mode](https://learningmsp430.wordpress.com/2013/11/16/16x2-lcd-interfacing-in-4-bit-mode/)
- [Beaglebone cross-compile-tool-chain for Windows user ](http://gnutoolchains.com/beaglebone/)
- [Dincer's JavaScript LCD Simulator](http://www.dinceraydin.com/djlcdsim/djlcdsim.html)

### Some important note:
- In 4 bit mode, first upper nibble, then lower nibble.
- Because the busy flag (FG) is set to 1 while an instruction is being executed, check it to make sure it is 0 before sending another instruction from the MPU. (HD44780.pdf P.24)

### Other discoveries
- [overo-blinkm](https://github.com/scottellis/overo-blinkm): control BlinkM in Linux Command line
It looks like that BlinkM has really some nice scripts build into their firmware.
TODO: Fork this repo, pulling some requests to it:
/ in tool-tips list, play-script argument "-n num_repeats" is also optional. If it's not specified, the loop go permanently. So it should be written like "[-n num_repeats]
/ Add new settings for beaglebone black in "i2c_functions.c"
- [The Linux Kernel Module Programming Guide](http://www.tldp.org/LDP/lkmpg/2.6/html/lkmpg.html#AEN710)
"/proc File System" are virtual file system that contain system runtime informations & let user modify these parameters
