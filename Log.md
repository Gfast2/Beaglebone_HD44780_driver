# **LOG**
---
This is the ***Dairy*** of this Project. Visit [**my website**](http://www.gao-su.com) for more information.

## TODO ##


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
