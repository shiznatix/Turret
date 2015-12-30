#include <Servo.h>

//Servo setup
const int SERVO_XY_PIN = A2;
const int SERVO_Z_PIN = A0;

const int XY_MIN = 120;//limit 105
const int XY_MAX = 170;//limit 180
const int XY_MID = 142;

const int Z_MIN = 50;
const int Z_MAX = 140;
const int Z_MID = 73;

int currentXY = XY_MID;
int currentZ = Z_MID;

Servo servoXY;
Servo servoZ;

//Random movement timer
unsigned long randomTimer = 0;

const int RANDOM_INTERVAL = 1000;

void setup() {
  Serial.begin(9600);

  //setup servos
  pinMode(SERVO_XY_PIN, OUTPUT);
  pinMode(SERVO_Z_PIN, OUTPUT);
  
  servoXY.attach(SERVO_XY_PIN);
  servoZ.attach(SERVO_Z_PIN);

  randomSeed(analogRead(0));
}

void loop() {
  unsigned long currentTime = millis();
  
  if (currentTime - randomTimer >= RANDOM_INTERVAL) {
    randomTimer = currentTime;
    
    randomPosition();
    moveServos();
  }
}

void randomPosition() {
  currentXY = random(XY_MIN, (XY_MAX + 1));
  currentZ = random(Z_MIN, (Z_MAX + 1));
}

void moveServos() {
  //make sure we are constrained
  currentXY = constrain(currentXY, XY_MIN, XY_MAX);
  currentZ = constrain(currentZ, Z_MIN, Z_MAX);
  
  servoXY.write(currentXY);
  servoZ.write(currentZ);

  //printing to Serial slows us down a lot
  //so only enable when needed
  printCurrentPosition();
}

void printCurrentPosition() {
  Serial.print("Current xy: ");
  Serial.print(currentXY);
  Serial.print(" z: ");
  Serial.println(currentZ);
}

