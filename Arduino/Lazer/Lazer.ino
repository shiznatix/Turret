#include <Servo.h>

//Servo setup
const int SERVO_XY_PIN = A2;
const int SERVO_Z_PIN = A0;

const int XY_MIN = 120;//limit 105
const int XY_MAX = 170;//limit 180
const int XY_MID = 142;

const int Z_MIN = 50;
const int Z_MAX = 120;
const int Z_MID = 73;

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

  moveServos(XY_MIN, Z_MID);
}

void loop() {
  int xy, z;

  //spiral from center
  for (double i = 0; i < 25; i += 0.1) {
    xy = XY_MID + (i * cos(i));
    z = Z_MID + (i * sin(i));

    moveServos(xy, z);
    delay(70);
  }

  //sprial fom end
  for (double i = 25; i > 0; i -= 0.1) {
    xy = XY_MID + (i * cos(i));
    z = Z_MID + (i * sin(i));

    moveServos(xy, z);
    delay(70);
  }
}

void moveServos(int xy, int z) {
  //make sure we are constrained
  xy = constrain(xy, XY_MIN, XY_MAX);
  z = constrain(z, Z_MIN, Z_MAX);
  
  servoXY.write(xy);
  servoZ.write(z);

  //printing to Serial slows us down a lot
  //so only enable when needed
  printPosition(xy, z);
}

void printPosition(int xy, int z) {
  Serial.print("xy: ");
  Serial.print(xy);
  Serial.print(" z: ");
  Serial.println(z);
}

