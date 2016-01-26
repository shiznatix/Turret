#include <Servo.h>
#include <SoftwareSerial.h>

SoftwareSerial btSerial(10, 11); // RX, TX

//Modes
const boolean MANUAL_INPUT = false;

//Servo setup
const int SERVO_XY_PIN = A2;
const int SERVO_Z_PIN = A0;

const int XY_MIN = 130;//limit 105
const int XY_MAX = 180;//limit 180
const int XY_MID = 160;

const int Z_MIN = 40;
const int Z_MAX = 130;
const int Z_MID = 75;

const int STEP_MOVEMENT = 3;

int currentXY = XY_MID;
int currentZ = Z_MID;

Servo servoXY;
Servo servoZ;

//Ignighter setup
const int IGNITER_1_PIN = 3;
const int IGNITER_2_PIN = 5;
const int IGNITER_3_PIN = 7;

unsigned long igniter1Timer = 0;
unsigned long igniter2Timer = 0;
unsigned long igniter3Timer = 0;

const int IGNITER_AUTO_SHUTOFF = 1500;

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(50);
  btSerial.begin(9600);

  //setup servos
  pinMode(SERVO_XY_PIN, OUTPUT);
  pinMode(SERVO_Z_PIN, OUTPUT);
  
  servoXY.attach(SERVO_XY_PIN);
  servoZ.attach(SERVO_Z_PIN);

  middlePosition();
  moveServos();

  //setup igniters
  pinMode(IGNITER_1_PIN, OUTPUT);
  pinMode(IGNITER_2_PIN, OUTPUT);
  pinMode(IGNITER_3_PIN, OUTPUT);

  //ensure our igniters are cold...
  digitalWrite(IGNITER_1_PIN, LOW);
  digitalWrite(IGNITER_2_PIN, LOW);
  digitalWrite(IGNITER_3_PIN, LOW);
}

void loop() {
  unsigned long currentTime = millis();
  
  if (MANUAL_INPUT) {//only used to debug
    while (Serial.available()) {
      int xy = Serial.parseInt();
      int z = Serial.parseInt();
  
      currentXY = xy;
      currentZ = z;
  
      moveServos();
    }
  } else {
    bool gotMessage = false;
    
    if (Serial.available()) {
      gotMessage = true;
      
      char key = Serial.read();
  
      if (byte(37) == key) { //left
        currentZ += STEP_MOVEMENT;
      } else if (byte(39) == key) { //right
        currentZ -= STEP_MOVEMENT;
      } else if (byte(38) == key) { //up
        currentXY += STEP_MOVEMENT;
      } else if (byte(40) == key) { //down
        currentXY -= STEP_MOVEMENT;
      } else if (byte(109) == key) { //midde (m)
        middlePosition();
      } else if (byte(49) == key) { //fire 1 (1)
        fire(IGNITER_1_PIN);
        igniter1Timer = currentTime;
      } else if (byte(50) == key) { //fire 2 (2)
        fire(IGNITER_2_PIN);
        igniter2Timer = currentTime;
      } else if (byte(51) == key) { //fire 3 (3)
        fire(IGNITER_3_PIN);
        igniter3Timer = currentTime;
      }
    } else if (btSerial.available()) {
      gotMessage = true;

      char key = btSerial.read();

      if ('l' == key) { //left
        currentZ += STEP_MOVEMENT;
      } else if ('r' == key) { //right
        currentZ -= STEP_MOVEMENT;
      } else if ('u' == key) { //up
        currentXY += STEP_MOVEMENT;
      } else if ('d' == key) { //down
        currentXY -= STEP_MOVEMENT;
      } else if ('m' == key) { //midde (m)
        middlePosition();
      } else if ('1' == key) { //fire 1 (1)
        fire(IGNITER_1_PIN);
        igniter1Timer = currentTime;
      } else if ('2' == key) { //fire 2 (2)
        fire(IGNITER_2_PIN);
        igniter2Timer = currentTime;
      } else if ('3' == key) { //fire 3 (3)
        fire(IGNITER_3_PIN);
        igniter3Timer = currentTime;
      }
    }

    if (gotMessage) {
      moveServos();
    }
  }

  //to be safe, have the safety timers to shutoff the igniters outside all if statements
  if (currentTime - igniter1Timer >= IGNITER_AUTO_SHUTOFF) {
    digitalWrite(IGNITER_1_PIN, LOW);
  }
  if (currentTime - igniter2Timer >= IGNITER_AUTO_SHUTOFF) {
    digitalWrite(IGNITER_2_PIN, LOW);
  }
  if (currentTime - igniter3Timer >= IGNITER_AUTO_SHUTOFF) {
    digitalWrite(IGNITER_3_PIN, LOW);
  }
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

void fire(int firePin) {
  digitalWrite(firePin, HIGH);
}

