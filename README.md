# arduinoR4rgb-elk
arduino program to control elk rgb lights using a voltage analog sensor


This program is provides as is, under no circumstances I will be liable for anything that arises from using this! 

This code is to be flashed into an arduino R4 with bluetooth capabilities. it uses the arduinoBLE library and
it relies on an analog sensor I tested with voltage sensor.

Ideally all you need to do at this time is find the mac address of your (ELK-BLEDOM) RGB controller and replace it on the 
cketch file before flashing/uploading to the arduino.



TODO: scan by bluetooth name and if there's only 1 device use that as MAC