# arduinoR4rgb-elk
arduino program to control elk rgb lights using a voltage analog sensor


This program is provides as is, under no circumstances I will be liable for anything that arises from using this! 

This code is to be flashed into an arduino R4 with bluetooth capabilities. it uses the arduinoBLE library and
it relies on an analog sensor I tested with voltage sensor.

In the event you have multiple main controllered (ELK-Bledom) close to where you park or within range of the arduino you will need to find the mac address of (ELK-BLEDOM) RGB controller you want to control and replace it on the 
sketch file before flashing/uploading to the arduino.

If you have no more than one ELK-Bledom device within the reach of the arduino, you should not need to modify anything!
