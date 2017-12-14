  #include <EEPROM.h>

//For erasing the data stored in EEPROM
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  while(!Serial);
  Serial.println("Please type erase to delete all EEPROM.");
}

void loop() {
  //Wait for user erase command
  if(Serial.available() > 0) {
    String input = Serial.readString();
    if(input.equals("erase")) {
      //Prompt again
      Serial.println("Are you sure you wish to erase all stored data? Type yes to proceed.");
      while(Serial.available() == 0);
      String command = Serial.readString();
      if(command.equals("yes")) {
        //erase EEPROM by setting byte values to 0xFF, but only if they aren't there already, to avoid unnecessary write cycles
        for(int i = 0; i < 512; i++) {
            if(EEPROM.read(i) != 0xFF) {
               EEPROM.write(i, 0xFF);
            }
        }
        Serial.println("All EEPROM data deleted.");
      } else {
        flash(1);   
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

