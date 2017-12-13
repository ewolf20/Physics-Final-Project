#include <DHT.h>
#include <DHT_U.h>
#include <EEPROM.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>

//Number of cycles should never exceed 127, as this will cause EEPROM to be overloaded
//I promise it can't be 128, 'cause I need three extra bytes for stuff
#define numberOfCycles 96
#define cycleSecondsSpace 900
#define stepNumber 1020

#define motorPowerPin 2
#define motorDirectionPin 3
#define motorStepPin 4
#define motorLogicPin 5
#define sensorPowerPin 6
#define dhtPin 7
#define photoSensorPin A0
#define DHTTYPE DHT11

int dataIndex;

DHT dht(dhtPin, DHT11);

ISR(WDT_vect) {
  wdt_disable();
}

void sleep() {
  
  MCUSR = 0;
  WDTCSR = 0b00011000;
  WDTCSR = 0b01000000 | 0b100001;
  wdt_reset();
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  sleep_mode();
  sleep_disable();
  power_all_enable();
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(motorPowerPin, OUTPUT);
  pinMode(motorDirectionPin, OUTPUT);
  pinMode(motorStepPin, OUTPUT);
  pinMode(motorLogicPin, OUTPUT);
  pinMode(sensorPowerPin, OUTPUT);
  pinMode(photoSensorPin, INPUT);
  digitalWrite(motorPowerPin, LOW);
  digitalWrite(motorStepPin, LOW);
  digitalWrite(motorDirectionPin, LOW);
  digitalWrite(motorLogicPin, HIGH);
  digitalWrite(sensorPowerPin, LOW);
  digitalWrite(LED_BUILTIN, LOW);
  dht.begin();
  //Initialize the array where data gets stored.
  dataIndex = 0;
  //test code
//  Serial.begin(9600);
//  while(!Serial);
//  test code
  for(int i = 0; i < 20; i++) {
    flash(1);
    delay(800);
  }
  int measNumber = numberOfCycles * 4;
  byte measurementByte1 = (measNumber & 0x300) >> 8;
  byte measurementByte2 = measNumber & 0xFF;
  byte timeByte = (byte) (cycleSecondsSpace / 60);
  EEPROM.write(0, measurementByte1);
  EEPROM.write(1, measurementByte2);
  EEPROM.write(2, timeByte);
  dataIndex = 3;
  takeMeasurements();
//  cementData();
}

void loop() {
  flash(1);
  delay(800);
}

void flash(int times) {
  for(int i = 0; i < times; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100); 
  }
}

void takeMeasurements() {
  for(int i = 0; i < numberOfCycles; i++) {
    motor_move(false);
    delay(8000);
    digitalWrite(sensorPowerPin, HIGH);
    float totalTemp = 0.0;
    float totalHumidity = 0.0;
    int totalLight = 0;
    for(int i = 0; i < 14; i++) {
      delay(500);
      float temp = dht.readTemperature();
      totalTemp += temp;
      float humidity = dht.readHumidity();
      totalHumidity += humidity;
      int lightValue = analogRead(photoSensorPin);
      totalLight += lightValue;
    }
    float averageTemp = totalTemp / 14;
    float averageHumidity = totalHumidity / 14;
    int averageLight = totalLight / 14;
    //When converting temp to a byte, add 100 so that you'll never get a negative in the world (and high end isn't a problem)
    byte tempByte = (byte) (averageTemp + 100);
    byte humidityByte = (byte) averageHumidity;
    //The data representing light level is 10 bit, so we bitmask it into two values
    //Be sure to test this
    byte lightByte2 = (0xFF)&(averageLight);
    byte lightByte1 = (byte) (((0x300)&(averageLight))>>8);
    byte currentData[4];
    currentData[0] = tempByte;
    currentData[1] = humidityByte;
    currentData[2] = lightByte1;
    currentData[3] = lightByte2;
    recordData(currentData);
    String tempString = String(averageTemp);
    String humidityString = String(averageHumidity);
    String lightString = String(averageLight);
//    Serial.println("Measured Temperature: " + tempString);
//    Serial.println("Measured Humidity: " + humidityString + "%");
//    Serial.println("Measured Light Level: " + lightString);
    motor_move(true);
    digitalWrite(sensorPowerPin, LOW);
    int sleepTimes = cycleSecondsSpace / 8;
    for(int b = 0; b < sleepTimes; b++) {
      sleep();
    }
    //This delay does the last remaining time as a delay rather than a sleep cycle, which may be good for warming things back up.
    delay((cycleSecondsSpace - sleepTimes*8)*1000);
  }
}

void motor_move(boolean motorDirection) {
  if(motorDirection) {
     digitalWrite(motorDirectionPin, LOW);
  } else {
    digitalWrite(motorDirectionPin, HIGH);
  }
  digitalWrite(motorPowerPin, HIGH);
  for(int i = 0; i < stepNumber; i++) {
    digitalWrite(motorStepPin, HIGH);
    delay(1);
    digitalWrite(motorStepPin, LOW);
    delay(1);
  }
  delay(200);
  digitalWrite(motorPowerPin, LOW);
  digitalWrite(motorDirectionPin, LOW);
}

//Writes the data to EEPROM
void recordData(byte inputArray[]) {
  for(int i = 0; i < 4; i++) {
    EEPROM.write(dataIndex + i, inputArray[i]);
  }
  dataIndex += 4;
}

//Cements the data in EEPROM up to the index of the data. The first two bytes in EEPROM 
//are the actual number of data values that have been stored, and the third is the number of 
//minutes between bytes. Could eliminate this, but it seems like good practice to put them in
//void cementData() {
//  byte indexByte1 = (dataIndex & 0x300) >> 8;
//  byte indexByte2 = dataIndex & 0xFF;
//  byte timeByte = (byte) (cycleSecondsSpace / 60);
//  EEPROM.write(0, indexByte1);
//  EEPROM.write(1, indexByte2);
//  EEPROM.write(2, timeByte);
//  for(int i = 0; i < dataIndex; i++) {
//    EEPROM.write(i + 3, dataArray[i]);
//  }
//}

