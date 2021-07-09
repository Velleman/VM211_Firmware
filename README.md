EARTH LISTENER (VM211 & VM211SE)

Ever wondered how good the air quality is in your living room? Or do you need proof of why you fall asleep in a non-ventilated meeting room after 3 hours? Look no further! This DIY experimental kit has all the answers!
The Earth Listener is an Arduino based sensor device that reads multiple environmental values to indicate the air quality. This kit is assembled in 15 minutes and you don’t need a soldering iron! Just connect the parts and wires, put everything in the case and connect it to a 5V USB socket. The Mega development board is pre-programmed and shows eCO2, TVOC (particles in the air), barometric pressure, temperature and humidity values onto the TFT touchscreen. To top thing off, the Earth Listener also contains a lightning detection sensor. If lightning (or electronic discharge) is detected, the Earth Listener will give an alarm tone and log how far the lightning is located. Since everything is open source, you can add an additional 6 sensors to the Earth Listener via the sensor shield. Logging is possible via the micro SD card (not included).

Buy it here: https://whadda.com/product/earth-listener-wmmi211/
Follow the tutorial here: https://manuals.velleman.eu/category.php?id=378 

Change log v4.2
- added support for VMA354 Tuya IOT interface
- digital pin 35,36,37 are used to indicate the air quality (green, yellow, red) to control external devices

Change log v4.1
- added support for VMA301 Real time clock module

Change log v4.0
- fixed some spelling mistakes
- checks now if Earthlistener is booted for the first time, if true, sets default values
- logging interval can now be set (1 second - 3600 seconds interval, default is now 10 seconds)

Change log v3.8
- modified setup page to icons instead of text
- lightning sensor sensitivity can now be set by icon on setup page (also stored in EEPROM, low by default)
- both IIC & SPI interfaces are available for the lightning sensor via setup page (also stored in EEPORM, SPI by default)
- indoor / outdoor setting is now changeable without reboot
- removed all refs to altitude screen (it wasn't used)
- optimized printing of the graphs, these are now 1 pixel in height instead of 18 pixels. Line is drawn 18x. Huge memory optimization!
- fixed some spelling mistakes

Change log v3.7
- changed lightning sensor library & interrupt function to change

Change log v3.6
- spelling mistakes fixed & extra comments added for better documentation
- changed interrupt function
