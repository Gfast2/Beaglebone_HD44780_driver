## Personal Project about exploring the control of a HD44780 through Beaglebone.

There are many LCD display around us these days. "Character Display" are fading out. I believe that is because the color display are getting cheaper and cheaper. Sure they can do much more things compare to a typical character display. But the character display is something like "7-Segment-Display", which belonge to the part of our memory. Since some days before I saw a youtube view showing around tuning the HD44780 Character Display, to let it show very very stylful stuffs. I decided to do something by myself.  

**Reference Link:**
http://www.thebrokendesk.com/post/writing-to-an-lcd-screen-with-the-beaglebone-black/
https://github.com/Wicker25/Rpi-hw (C++ Library for Raspberry pi)
https://www.youtube.com/watch?v=31IZaJWMYjU

[How to Control LCD Displays | Arduino Tutorial.](https://www.youtube.com/watch?v=85LvW1QDLLw)
(Basic working logic about how to control the display)


[LCD Graphics MegaDemo HD44780](https://www.youtube.com/watch?v=HugwQ4b9iBU&t=79s)

[HD44780 Graphic Transitions](https://www.youtube.com/watch?v=bN-XRXLx88Y)

[The forum that talked about these MegaDemo](http://www.picbasic.co.uk/forum/showthread.php?t=18416)

Another old example: (Odometer effect)
[Link1](https://www.youtube.com/watch?v=T6DAZphHbjI)
[Link2](https://www.youtube.com/watch?v=eVILMbrI3ok#t=76.901)

[ASCII Demo](https://www.youtube.com/watch?v=6-SmBeeXRBQ)

[github repo of the in chip pru](https://github.com/beagleboard/am335x_pru_package)

[wiki page of the in chip pru](http://elinux.org/Ti_AM33XX_PRUSSv2)

[How to control 40x4 HD44780 display( LCDd daemon )](
http://images.google.de/imgres?imgurl=http%3A%2F%2Fwww.forum-raspberrypi.de%2Fattachment.php%3Fthumbnail%3D8365&imgrefurl=http%3A%2F%2Fwww.forum-raspberrypi.de%2FThread-libreelec-hd44780-lcd-40x4-raspberry-1-modell-b&h=204&w=370&tbnid=5EbBMcydcYqZYM%3A&docid=YRSrBI69p3Ti2M&ei=dboOWN3YD8SHgAbt1JnoDw&tbm=isch&client=safari&iact=rc&uact=3&dur=636&page=0&start=0&ndsp=21&ved=0ahUKEwid5_7k6_TPAhXEA8AKHW1qBv0QMwgmKAgwCA&safe=off&bih=816&biw=1264)

[The lcdproc](http://lcdproc.org)

[Mikrokontroller](https://www.mikrocontroller.net/articles/HD44780)

[Text Tutorial about special character](http://www.circuitvalley.com/2012/02/lcd-custom-character-hd44780-16x2.html)

[C Library for Beaglebone HD44780 Display control](http://www.instructables.com/id/C-Library-for-HD44780-LCD-Display-Controller/)

[A interesting Web site talk about these character display](http://sprut.de/electronic/lcd/index.html)

[Paul's LiquidCrystal Library (LiquidCrystalFast) looks very very good for the working status for me.](https://www.pjrc.com/teensy/td_libs_LiquidCrystal.html)

[4 bit mode](https://learningmsp430.wordpress.com/2013/11/16/16x2-lcd-interfacing-in-4-bit-mode/)
In 4 bit mode, first upper nibble, then lower nibble.

Because the busy flag (FG) is set to 1 while an instruction is being executed, check it to make sure it is 0 before sending another instruction from the MPU. (HD44780.pdf P.24)


