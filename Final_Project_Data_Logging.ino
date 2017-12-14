#include <EEPROM.h>

//The number of values to be sent and the spacing between them
int numberOfValues;
int measurementMinutes;
//The buffer for the byte array that gets sent to LabVIEW
byte dataArray[512];
int index;

void setup() {
  Serial.begin(9600);
  while(!Serial);
  pinMode(LED_BUILTIN, OUTPUT);
  //Flash for a while so that I have time to re-flash the arduino if necessary
  for(int i = 0; i < 10; i++) {
    flash(1);
    delay(800);
  }
  //Initialize the buffer explicitly
  dataArray[0] = 0;
  //Read in the (bitmasked) number of values and time between measurements from EEPROM
  numberOfValues = ((int) EEPROM.read(0))*256 + ((int) EEPROM.read(1));
  measurementMinutes = (int) EEPROM.read(2);
  for(int i = 0; i < numberOfValues; i++) {
    dataArray[i] = EEPROM.read(i + 3);
  }
  flash(5);
  index = 0;
}


void loop() {
  //Scan for serial input
  if(Serial.available() > 0) {
    //Check if the input is the send signal
    String input = Serial.readString();
    if(input.equals("send")) {
      boolean dataReceived = false;
      //write the byte data to LabVIEW
      Serial.write(dataArray, numberOfValues);
      //code for telling arduino when the data is received; not used in current iteration of LabVIEW program
      while(!dataReceived) {
        if(Serial.available() > 0) {
          String command = Serial.readString();
          dataReceived = command.equals("received");
          if(!dataReceived) {
            flash(1);
          }
        }
      }
    } else {
      flash(1);
    }
  }
}

void flash(int times) {
  for(int i = 0; i < times; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
  }
}

