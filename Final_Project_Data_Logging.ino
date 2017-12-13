#include <EEPROM.h>


int numberOfValues;
int measurementMinutes;
byte dataArray[512];
int index;

void setup() {
  Serial.begin(9600);
  while(!Serial);
  pinMode(LED_BUILTIN, OUTPUT);
  for(int i = 0; i < 10; i++) {
    flash(1);
    delay(800);
  }
  dataArray[0] = 0;
  
  numberOfValues = ((int) EEPROM.read(0))*256 + ((int) EEPROM.read(1));
  measurementMinutes = (int) EEPROM.read(2);
  for(int i = 0; i < numberOfValues; i++) {
    dataArray[i] = EEPROM.read(i + 3);
  }
  flash(5);
  index = 0;
  //If this gives issue, just manually insert the number of measurements
//  boolean initialSent = false;
//  while(!initialSent) {
//    if(Serial.available() > 0) {
//      String input = Serial.readString();
//      if(input.equals("send")) {
//        for(int i = 0; i < 3; i++) {
//          byte setupByte = EEPROM.read(i);
//          Serial.print(setupByte);
//          boolean infoReceived = false;
//          while(!infoReceived) {
//            String receivedString = Serial.readString();
//            infoReceived = receivedString.equals("received");
//          }
//        }
//      }
//    }
//  }
}


void loop() {
  if(Serial.available() > 0) {
    String input = Serial.readString();

    
    if(input.equals("send")) {
      boolean dataReceived = false;
      Serial.write(dataArray, numberOfValues);
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

