#include <Servo.h>

const int SERVO_XY_PIN = A2;
const int SERVO_Z_PIN = A0;

const int XY_MIN = 105;
const int XY_MAX = 180;
const int XY_MID = (XY_MAX + XY_MIN) / 2;

const int Z_MIN = 10;
const int Z_MAX = 180;
const int Z_MID = (Z_MAX + Z_MIN) / 2;

Servo servoXY;
Servo servoZ;

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(50);
  
  pinMode(SERVO_XY_PIN, OUTPUT);
  pinMode(SERVO_Z_PIN, OUTPUT);
  
  servoXY.attach(SERVO_XY_PIN);
  servoZ.attach(SERVO_Z_PIN);

  randomSeed(analogRead(0));
  middle();
}

void loop() {
  while (Serial.available() > 0) {
    int xy = Serial.parseInt();
    int z = Serial.parseInt();

    Serial.print("Received xy: ");
    Serial.print(xy);
    Serial.print(" z: ");
    Serial.println(z);

    setFromInput(xy, z);
  }
  
  /*
  randomPosition();
  delay(2000);
  middle();
  delay(2000);
  */
}

void randomPosition() {
  int xy = random(XY_MIN, (XY_MAX + 1));
  int z = random(Z_MIN, (Z_MAX + 1));
  
  servoXY.write(xy);
  servoZ.write(z);

  printCurrentPosition(xy, z);
}

void middle() {
  servoXY.write(XY_MID);
  servoZ.write(Z_MID);

  printCurrentPosition(XY_MID, Z_MID);
}

void setFromInput(int xy, int z) {
  xy = constrain(xy, XY_MIN, XY_MAX);
  z = constrain(z, Z_MIN, Z_MAX);
  
  servoXY.write(xy);
  servoZ.write(z);

  printCurrentPosition(xy, z);
}

void printCurrentPosition(int xy, int z) {
  Serial.print("Current xy: ");
  Serial.print(xy);
  Serial.print(" z: ");
  Serial.println(z);
}

