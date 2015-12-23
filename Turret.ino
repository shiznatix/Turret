#include <Servo.h>

const int SERVO_XY_PIN = A5;
//const int SERVO_Z_PIN = ;

const int XY_MIN = 100;
const int XY_MAX = 200;

Servo servoXY;
//Servo servoZ;

void setup() {
  Serial.begin(9600);
  
  pinMode(SERVO_XY_PIN, OUTPUT);
  //pinMode(SERVO_Z_PIN, OUTPUT);
  
  servoXY.attach(SERVO_XY_PIN);
  //servoZ.attach(SERVO_Z_PIN);

  randomSeed(analogRead(0));
}

void loop() {
  randomPosition();
  delay(2000);
}

void randomPosition() {
  servoXY.write(random(XY_MIN, (XY_MAX + 1)));
}

