# bfc3000
##Watch and analyze your bird's eating habits

###Aim 
Writing a database over multi week time frames. Timestamped entries are written by wild bird's setting off an metal
touch sensor.

###Required hardware hacks
Prolonging the metal sensor detection range is essential. I experimented a bit and found out its measurement quality
decreases using long cables.

TODO: directly attach the metal pin of the sensor to a polished part of the brass feeding pot.

###Used hardware
|Hardware|Requirement|Price|My Source|
|--------|-----------|----:|------|
|PIR sensor||<
$1|[Aliexpress](http://www.aliexpress.com/item/1pcs-High-Quality-HC-SR501-Infrared-PIR-Motion-Sensor-Module-For-Arduino-Raspberry-pi/32558562655.html) |
|OpenLog||$15|Sparkfun|
|TinyRTC||$0.6|[Aliexpress](http://www.aliexpress.com/item/Free-shipping-20pcs-lot-The-Tiny-RTC-I2C-modules-24C32-memory-DS1307-clock-RTC-module-for/1876368739.html)|
|Atmel 328P||$2||

###Schematics###
Downloadable in this repository. Rough version as I the wiring myself.
![Schematics](https://github.com/barde/bfc3000/raw/master/bfc3000_bb.png)


###Prototype
![Prototype with ATMEGA2560 boad](https://github.com/barde/bfc3000/raw/master/prototype.jpg)

Using an ATMEGA2560 for prototyping has the big improvement of multiple serial ports. One for debugging and other for
the devices, e.g. the OpenLog which communicates over Serial.

It was somewhat fuzzy to get the OpenLog going with my el-cheapo Chinese MicroSD. Firstly, the sensor was blinking
thrice (card error) with its default FAT32 system. I repartitioned to FAT16 and decreased the partition size. It worked
but the card was not removable during a power cycle.

The solution: a firmware update. You need some TTL converter like FTDI to USB or some 
[cheaper
version](http://www.aliexpress.com/item/Free-Shipping-1pcs-FT232RL-FTDI-USB-3-3V-5-5V-to-TTL-Serial-Adapter-Module/32481520135.htm).
Following [this instruction](https://learn.sparkfun.com/tutorials/openlog-hookup-guide) for reprogramming the on board Atmega328 is straight forward.
After flashing it flawlessly wrote to the full sized FAT32 partitioned card.

###Power consumption during runtime
TBD

###Power consumption during sleep
TBD
