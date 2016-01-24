#include <SoftwareSerial.h>

SoftwareSerial mySerial(10, 11); // RX, TX

const int SERIAL_TYPE_ARDUINO = 1;
const int SERIAL_TYPE_BT = 2;

bool writeStrings = true;

void setup() {
  Serial.begin(9600);
  
  Serial.println("Enter AT commands:");
  mySerial.begin(9600);
}

void loop() {
  if (mySerial.available()) {
    if (writeStrings) {
      String out = mySerial.readString();
  
      Serial.print("Serial.write(): ");
      Serial.println(out);
  
      writeString(out, SERIAL_TYPE_ARDUINO);
    } else {
      Serial.write(mySerial.read());
    }
  }
  
  if (Serial.available()) {
    if (writeStrings) {
      String in = Serial.readString();
  
      Serial.println("");
      Serial.print("mySerial.write(): ");
      Serial.println(in);
      
      writeString(in, SERIAL_TYPE_BT);
    } else {
      mySerial.write(Serial.read());
    }
  }
}

void writeString(String stringData, int serialType) {
  for (int i = 0; i < stringData.length(); i++) {
    if (SERIAL_TYPE_ARDUINO == serialType) {
      Serial.write(stringData[i]);
    } else if (SERIAL_TYPE_BT == serialType) {
      mySerial.write(stringData[i]);
    }
  }
}
