#include <Servo.h>

//Modes
const int MODE_CONTROLLED = 1;
const int MODE_RANDOM = 2;
const int MODE_MANUAL_INPUT = 3;

int mode = MODE_CONTROLLED;

//Servo setup
const int SERVO_XY_PIN = A2;
const int SERVO_Z_PIN = A0;

const int XY_MIN = 120;//limit 105
const int XY_MAX = 170;//limit 180
const int XY_MID = 142;

const int CONTROLLED_Z_MIN = 10;
const int CONTROLLED_Z_MAX = 160;
const int RANDOM_Z_MIN = 50;
const int RANDOM_Z_MAX = 140;
const int Z_MID = 85;

const int STEP_MOVEMENT = 3;

int currentXY = XY_MID;
int currentZ = Z_MID;

Servo servoXY;
Servo servoZ;

//Ignighter setup
const int IGNITER_1_PIN = 7;
const int IGNITER_2_PIN = 5;
const int IGNITER_3_PIN = 3;

unsigned long igniter1Timer = 0;
unsigned long igniter2Timer = 0;
unsigned long igniter3Timer = 0;

const int IGNITER_AUTO_SHUTOFF = 3000;

//Random movement timer
unsigned long randomTimer = 0;

const int RANDOM_INTERVAL = 1000;

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(50);

  //setup servos
  pinMode(SERVO_XY_PIN, OUTPUT);
  pinMode(SERVO_Z_PIN, OUTPUT);
  
  servoXY.attach(SERVO_XY_PIN);
  servoZ.attach(SERVO_Z_PIN);

  randomSeed(analogRead(0));
  middlePosition();
  moveServos();

  //setup igniters
  pinMode(IGNITER_1_PIN, OUTPUT);
  pinMode(IGNITER_2_PIN, OUTPUT);
  pinMode(IGNITER_3_PIN, OUTPUT);

  digitalWrite(IGNITER_1_PIN, LOW);
  digitalWrite(IGNITER_2_PIN, LOW);
  digitalWrite(IGNITER_3_PIN, LOW);
}

void loop() {
  unsigned long currentTime = millis();
  
  if (mode == MODE_MANUAL_INPUT) {//only used to debug
    while (Serial.available()) {
      int xy = Serial.parseInt();
      int z = Serial.parseInt();
  
      currentXY = xy;
      currentZ = z;
  
      moveServos();
    }
  } else {
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
      } else if (byte(99) == key) { //controlled (c)
        mode = MODE_CONTROLLED;
        middlePosition();
      } else if (byte(114) == key) { //random (r)
        mode = MODE_RANDOM;
        //no need to change anything yet, moveServos()
        //will keep it in its position
      } else if (byte(109) == key) { //midde (m)
        middlePosition();
      }

      moveServos();

      //dont allow to fire if in random mode
      if (mode == MODE_CONTROLLED) {
        if (byte(49) == key) {
          fire(IGNITER_1_PIN);
          igniter1Timer = currentTime;
        } else if (byte(50) == key) {
          fire(IGNITER_2_PIN);
          igniter2Timer = currentTime;
        } else if (byte(51) == key) {
          fire(IGNITER_3_PIN);
          igniter3Timer = currentTime;
        }
      }
    }
    
    if (mode == MODE_RANDOM) {
      if (currentTime - randomTimer >= RANDOM_INTERVAL) {
        randomTimer = currentTime;
        
        randomPosition();
        moveServos();
      }
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

  /*
  digitalWrite(IGNITER_1_PIN, HIGH);
  delay(2000);
  digitalWrite(IGNITER_1_PIN, LOW);
  delay(2000);
  */
}

void randomPosition() {
  currentXY = random(XY_MIN, (XY_MAX + 1));
  currentZ = random(RANDOM_Z_MIN, (RANDOM_Z_MAX + 1));
}

void middlePosition() {
  currentXY = XY_MID;
  currentZ = Z_MID;
}

void moveServos() {
  //make sure we are constrained
  currentXY = constrain(currentXY, XY_MIN, XY_MAX);
  currentZ = constrainZ();
  
  servoXY.write(currentXY);
  servoZ.write(currentZ);

  //printing to Serial slows us down a lot
  //so only enable when needed
  //printCurrentPosition();
}

int constrainZ() {
  if (mode == MODE_RANDOM) {
    return constrain(currentZ, RANDOM_Z_MIN, (RANDOM_Z_MAX + 1));
  } else {
    return constrain(currentZ, CONTROLLED_Z_MIN, (CONTROLLED_Z_MAX + 1));
  }
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

