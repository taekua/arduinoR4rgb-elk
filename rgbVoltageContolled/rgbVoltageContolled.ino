#include <ArduinoBLE.h>

char * mainControllerName = "ELK-BLEDOM"; //This is the name
char * mainControllerMac = ""; //If you have multiple ELK-Bledoms this is needed use a bluetooth scanner to get this from the controller you want to bind this to.
                              //"BE:16:75:00:33:1C"; //This is the address which we will connect into to turn on/off the lights
char * targetCharacteristicUuid = "fff3"; //This is hardcoded as the characteristic we need to write.

float voltageThreshold = 1.0; //Because there might be some noise, we dont want to use 0, this means when this numer of volts is
                             // surpassed on the sensor, we will issue a command
int STATE = 0; //1=ON 0=OFF we will use this to know if we should again issue a command, if its 1 we should notissue a power on again, and
               //if its off, we should not try to power off again as we would prevent the user from overriding manually with their phone

//These are the data packets we need to send to the device we could do a lot more like control modes and colors but thats outside of the scope
const uint8_t powerOnData[] = {0x7E, 0x00, 0x04, 0x00, 0x00, 0x01, 0xFF, 0x00, 0xEF}; // Hexadecimal value to write
const uint8_t powerOffData[] = {0x7E, 0x00, 0x04, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xEF}; // Hexadecimal value to write

const int analogInPin = A0; //this is the pin where the analog voltage sensor should go in
/* These variables are to store/handle sensor data*/
float vIn;
float oldvIn;
float vOut;
float voltageSensorVal;
const float factor = 5.128;
const float vCC = 5.00;
/* */

void setup() {
  // the code in this function runs only once every time the arduino is powered on
  Serial.begin(9600); 
  pinMode(A0,INPUT); //We specify that this pin will be used for reading data  
  Serial.begin(9600);  
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy module failed!");
    while (1);
  } 
  //We start looking for the rgb controller
  BLEScan();
}

void BLEScan() {
  if (strcmp(mainControllerMac, "" ) != 0 ) {
    Serial.println("Scanning by mac");
    BLE.scanForAddress(mainControllerName);
  } else if (strcmp(mainControllerName, "" ) == 0 ) {
    Serial.println("bad config");    
  } else {
    BLE.scan();  
  }
}

bool startsWith(const char *pre, const char *str)
{
    size_t lenpre = strlen(pre),
           lenstr = strlen(str);
    return lenstr < lenpre ? false : memcmp(pre, str, lenpre) == 0;
}

void loop() {
  // put your main code here, to run repeatedly:
  BLEDevice peripheral = BLE.available();

  if (peripheral) {
    
    if (!peripheral.localName().startsWith(mainControllerName)){
      return;
    }

    // stop scanning
    BLE.stopScan();
    readVoltageLevel();

    if (vIn > voltageThreshold) {
      powerOn(peripheral);
    } else {
      powerOff(peripheral);
    }
    // peripheral disconnected, start scanning again
    BLEScan();
  }
  delay(1);
}

void powerOn(BLEDevice peripheral) {
  //we dont care if its ON, since the user could have decided to override we will regardless
  while(STATE == 0){
    if(power(peripheral)) {
      STATE = 1;
    }
  }
}

void powerOff(BLEDevice peripheral) {
  //we dont care if its OFF, since the user could have decided to override we will regardless
  while(STATE == 1){    
    if(power(peripheral)) {      
      STATE = 0;
    }
  }
}


BLECharacteristic discoveryProcess(BLEDevice peripheral) {  
  Serial.println("Discovering attributes ...");
  if (peripheral.discoverAttributes()) {
    Serial.println("Attributes discovered");
  } else {
    Serial.println("Attribute discovery failed!");
    peripheral.disconnect();
    return BLECharacteristic();
  }

  for(int i=peripheral.serviceCount() -1; i>= 0; i--) {    
    for(int j=peripheral.service(i).characteristicCount(); j >= 0; j--) {
      //Serial.print("Char UUID =");
      //Serial.println(peripheral.service(i).characteristic(j).uuid());
      BLECharacteristic tempChar = peripheral.service(i).characteristic(j);
      if (strcmp(tempChar.uuid(), targetCharacteristicUuid) == 0  && tempChar.canWrite()) {
        //Serial.println("Found target char and its writtable");
        return tempChar;
      }
    }
  }
  
  Serial.println("Char discovery failed!");
  return BLECharacteristic();
}

void readVoltageLevel() {
  /* Read the current voltage level on the A0 analog input pin.
     This is used here to simulate the charge level of a battery.
  */
  voltageSensorVal = analogRead(analogInPin);
  vOut = (voltageSensorVal / 1024) * vCC;
  vIn = vOut * factor;  
}

bool power(BLEDevice peripheral) {
  // connect to the peripheral
  Serial.println("Connecting ...");

  if (peripheral.connect()) {
    Serial.println("Connected");
  } else {
    Serial.println("Failed to connect!");
    return false;
  }

  // retrieve the LED characteristic
  BLECharacteristic ledCharacteristic = discoveryProcess(peripheral);
  bool written = false;
  if (peripheral.connected()) {
    // while the peripheral is connected
    if(STATE == 0) {      
      Serial.println("Powering on");
      ledCharacteristic.writeValue(powerOnData,  sizeof(powerOnData));
    } else {      
      Serial.println("Powering off");
      ledCharacteristic.writeValue(powerOffData,  sizeof(powerOffData));
    }
    written = true;
  }
  peripheral.disconnect();

  Serial.println("Peripheral disconnected");
  return written;
}


