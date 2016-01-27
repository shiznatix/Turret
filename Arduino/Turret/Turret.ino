#include <Servo.h>
#include <SoftwareSerial.h>

SoftwareSerial btSerial(10, 11); // RX, TX

//Modes
const boolean MANUAL_INPUT = false;

//Servo setup
const int SERVO_TILT_PIN = A2;
const int SERVO_PAN_PIN = A0;

const int TILT_MIN = 130;//limit 105
const int TILT_MAX = 180;//limit 180
const int TILT_MID = 160;

const int PAN_MIN = 40;
const int PAN_MAX = 130;
const int PAN_MID = 75;

const int STEP_MOVEMENT = 3;

int currentTilt = TILT_MID;
int currentPan = PAN_MID;

Servo servoTilt;
Servo servoPan;

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
  pinMode(SERVO_TILT_PIN, OUTPUT);
  pinMode(SERVO_PAN_PIN, OUTPUT);
  
  servoTilt.attach(SERVO_TILT_PIN);
  servoPan.attach(SERVO_PAN_PIN);

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
      int tilt = Serial.parseInt();
      int pan = Serial.parseInt();
  
      currentTilt = tilt;
      currentPan = pan;
  
      moveServos();
    }
  } else {
    int gotMessageFrom = 0;
    boolean shouldMoveServos = false;

    if (Serial.available()) {
      gotMessageFrom = 1;
    } else if (btSerial.available()) {
      gotMessageFrom = 2;
    }
    
    if (gotMessageFrom > 0) {
      char key = (1 == gotMessageFrom ? Serial.read() : btSerial.read());
  
      if ('l' == key) { //left arrow byte(37)
        currentPan += STEP_MOVEMENT;
        shouldMoveServos = true;
      } else if ('r' == key) { //right arrow byte(39)
        currentPan -= STEP_MOVEMENT;
        shouldMoveServos = true;
      } else if ('u' == key) { //up arrow byte(38)
        currentTilt += STEP_MOVEMENT;
        shouldMoveServos = true;
      } else if ('d' == key) { //down arrow byte(40)
        currentTilt -= STEP_MOVEMENT;
        shouldMoveServos = true;
      } else if ('m' == key) { //midde (m) byte(109)
        middlePosition();
        shouldMoveServos = true;
      } else if ('1' == key) { //fire 1 (1) byte(49)
        fire(IGNITER_1_PIN);
        igniter1Timer = currentTime;
      } else if ('2' == key) { //fire 2 (2) byte(50)
        fire(IGNITER_2_PIN);
        igniter2Timer = currentTime;
      } else if ('3' == key) { //fire 3 (3) byte(51)
        fire(IGNITER_3_PIN);
        igniter3Timer = currentTime;
      }
    }

    if (shouldMoveServos) {
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
  currentTilt = TILT_MID;
  currentPan = PAN_MID;
}

void moveServos() {
  //make sure we are constrained
  currentTilt = constrain(currentTilt, TILT_MIN, TILT_MAX);
  currentPan = constrain(currentPan, PAN_MIN, PAN_MAX);
  
  servoTilt.write(currentTilt);
  servoPan.write(currentPan);

  //printing to Serial slows us down a lot
  //so only enable when needed
  //printCurrentPosition();
}

void printCurrentPosition() {
  Serial.print("Current tilt: ");
  Serial.print(currentTilt);
  Serial.print(" z: ");
  Serial.println(currentPan);
}

void fire(int firePin) {
  digitalWrite(firePin, HIGH);
}

