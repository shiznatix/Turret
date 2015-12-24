#include <Servo.h>

const int SERVO_XY_PIN = A2;
const int SERVO_Z_PIN = A0;

const int XY_MIN = 120;//limit 105
const int XY_MAX = 170;//limit 180
const int XY_MID = 142;

const int Z_MIN = 10;
const int Z_MAX = 160;
const int Z_MID = (Z_MAX + Z_MIN) / 2;

const int STEP_MOVEMENT = 3;

int currentXY = XY_MID;
int currentZ = Z_MID;

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
  middlePosition();
  moveServos();
}

void loop() {
  if (Serial.available()) {
    char key = Serial.read();

    if (byte(37) == key) { //left
      currentZ += STEP_MOVEMENT;
    } else if (byte(39) == key) { //right
      currentZ -= STEP_MOVEMENT;
    } else if (byte(38) == key) { //up
      currentXY += STEP_MOVEMENT;
    } else if (byte(40) == key) { //down
      currentXY -= STEP_MOVEMENT;
    } else if (byte(99) == key) { //center
      middlePosition();
    }

    moveServos();
  }
}

void randomPosition() {
  currentXY = random(XY_MIN, (XY_MAX + 1));
  currentZ = random(Z_MIN, (Z_MAX + 1));
}

void middlePosition() {
  currentXY = XY_MID;
  currentZ = Z_MID;
}

void moveServos() {
  //make sure we are constrained
  currentXY = constrain(currentXY, XY_MIN, XY_MAX);
  currentZ = constrain(currentZ, Z_MIN, Z_MAX);
  
  servoXY.write(currentXY);
  servoZ.write(currentZ);

  //printing to Serial slows us down a lot
  //so only enable when needed
  //printCurrentPosition();
}

void printCurrentPosition() {
  Serial.print("Current xy: ");
  Serial.print(currentXY);
  Serial.print(" z: ");
  Serial.println(currentZ);
}

