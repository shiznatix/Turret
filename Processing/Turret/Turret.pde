import gab.opencv.*;
import processing.video.*;
import processing.serial.*;
import java.awt.*;

//Middle screen vars
float midScreenX;
float midScreenY;
float midScreenWindow = 30; //This is the acceptable 'error' for the center of the screen.

//Input vars
final char INPUT_MODE_BRIGHTEST_POINT = 'q';
final char INPUT_MODE_FACE = 'w';
final char INPUT_MODE_MANUAL = 'e';
final char INPUT_CENTER_CAMERA = 'm';
final char INPUT_INCREASE_CROSSHAIRS = '=';
final char INPUT_DECREASE_CROSSHAIRS = '-';
final char INPUT_FIRE_1 = '1';
final char INPUT_FIRE_2 = '2';
final char INPUT_FIRE_3 = '3';

//Target vars
final int TARGET_MODE_BRIGHTEST_POINT = 1;
final int TARGET_MODE_FACE = 2;
final int TARGET_MODE_MANUAL = 3;
int targetMode = TARGET_MODE_BRIGHTEST_POINT;
PShape targetCircle;

//Camera
Capture camera;
OpenCV opencv;
final String CAMERA_NAME = "name=USB2.0 Camera,size=640x480,fps=30";

//Arduino
Serial arduino;
final String ARDUINO_PORT_NAME = "/dev/cu.usbmodem1411";

final char MOVE_UP = 'u';
final char MOVE_DOWN = 'd';
final char MOVE_LEFT = 'l';
final char MOVE_RIGHT = 'r';
final char MOVE_MIDDLE = 'm';
final char FIRE_1 = '1';
final char FIRE_2 = '2';
final char FIRE_3 = '3';

//Tracking vars
final int CHECK_MOVE_UP_DOWN = 1;
final int CHECK_MOVE_LEFT_RIGHT = 2;
int checkMove = CHECK_MOVE_UP_DOWN;

void setup() {
  size(640, 480);
  
  midScreenX = (width / 2);
  midScreenY = (height / 2);
  
  setupTargetCircle();
  
  arduino = new Serial(this, getArduino(), 9600);
  
  camera = new Capture(this, getCamera());
  camera.start();
  
  opencv = new OpenCV(this, width, height);
  opencv.loadCascade(OpenCV.CASCADE_FRONTALFACE);
}

void draw() {
  if (camera.available()) {
    camera.read();
    
    opencv.loadImage(camera);
    
    image(camera, 0, 0);
    
    Target target;
    boolean moveServosIfNeeded = true;
    
    if (targetMode == TARGET_MODE_FACE) {
      target = targetFromFace();
      
      if (null == target) {
        target = new Target(-1, -1, "Face");
        moveServosIfNeeded = false;
      }
    } else if (targetMode == TARGET_MODE_BRIGHTEST_POINT) {
      target = targetFromBrightestPoint();//Default
    } else {
      target = new Target(midScreenX, midScreenY, "Manual");
    }
    
    char[] sendChars = {'.', '.', '.', '.'};
    boolean pointInTarget = pointInTarget(target.x, target.y);
    
    /*
    println("--" + frameCount + "--");
    println("targetX: " + target.x + " midScreenX: " + midScreenX);
    println("targetY: " + target.y + " midScreenY: " + midScreenY);
    */
    
    if (moveServosIfNeeded && !pointInTarget && targetMode != TARGET_MODE_MANUAL) {
      if (checkMove == CHECK_MOVE_UP_DOWN) {
        //Find out if the Y component of the face is below the middle of the screen.
        if (target.y < midScreenY) {
          sendChars[0] = MOVE_DOWN;
        }
        //Find out if the Y component of the face is above the middle of the screen.
        else if (target.y > midScreenY) {
          sendChars[1] = MOVE_UP;
        }
        
        checkMove = CHECK_MOVE_LEFT_RIGHT;
      } else if (checkMove == CHECK_MOVE_LEFT_RIGHT) {
        //Find out if the X component of the face is to the left of the middle of the screen.
        if (target.x < midScreenX) {
          sendChars[2] = MOVE_LEFT;
        }
        //Find out if the X component of the face is to the right of the middle of the screen.
        else if (target.x > midScreenX) {
          sendChars[3] = MOVE_RIGHT;
        }
        
        checkMove = CHECK_MOVE_UP_DOWN;
      }
    }
    
    writeCurrentStateText(target, pointInTarget, sendChars);
    drawCrosshairs(pointInTarget);
    
    boolean sentChars = false;
    
    for (int i = 0; i < 4; i++) {
      if ('.' != sendChars[i]) {
        println("sendChar: " + sendChars[i]);
        arduino.write(sendChars[i]);
        sentChars = true;
      }
    }
    
    if (sentChars) {
      delay(200);
    }
  }
}

String getArduino() {
  //Setup Arduino serial connection
  String[] serialDevices = Serial.list();
  int serialDevicePort = -1;
  println("Available serial ports:");
  
  for (int i = 0; i < serialDevices.length; i++) {
    if (serialDevicePort < 0 && serialDevices[i].indexOf(ARDUINO_PORT_NAME) > -1) {
      serialDevicePort = i;
      println("Found " + i + ": '" + serialDevices[i]);
    } else {
      println(i + ": " + serialDevices[i]);
    }
  }
  
  if (serialDevicePort < 0) {
    println("Could not find arduino");
    exit();
  }
  
  return serialDevices[serialDevicePort];
}

String getCamera() {
  String[] cameras = Capture.list();
  int cameraPort = -1;
  
  if (cameras.length == 0) {
    println("There are no cameras available for capture.");
    exit();
  }
  
  println("Available cameras:");
  
  for (int i = 0; i < cameras.length; i++) {
    if (cameraPort < 0 && cameras[i].indexOf(CAMERA_NAME) > -1) {
      cameraPort = i;
      println("Found " + i + ": " + cameras[i]);
    } else {
      println(i + ": " + cameras[i]);
    }
  }
  
  if (cameraPort < 0) {
    println("Could not find camera");
    exit();
  }
  
  return cameras[cameraPort];
}

void setupTargetCircle() {
  noFill();
  targetCircle = createShape(ELLIPSE, midScreenX, midScreenY, (midScreenWindow * 2), (midScreenWindow * 2));
  targetCircle.setStroke(color(0, 255, 0));
  targetCircle.setStrokeWeight(2);
}

void writeCurrentStateText(Target target, boolean inTarget, char[] sendChars) {
  int textStep = 15;
  int textPosition = textStep;
  int textSize = 12;
  int textXPosition = 10;
  
  textSize(textSize);
  fill(255, 165, 0);
  
  textAlign(LEFT);
  text("height: " + height, textXPosition, textPosition);
  textPosition += textStep;
  text("width: " + width, textXPosition, textPosition);
  textPosition += textStep;
  text("midScreenWindow: " + midScreenWindow, textXPosition, textPosition);
  textPosition += textStep;
  text("midScreenX: " + midScreenX, textXPosition, textPosition);
  textPosition += textStep;
  text("midScreenY: " + midScreenY, textXPosition, textPosition);
  textPosition += textStep;
  text("target.x: " + target.x, textXPosition, textPosition);
  textPosition += textStep;
  text("target.y: " + target.y, textXPosition, textPosition);
  textPosition += textStep;
  text("target distance x: " + (target.x - midScreenX), textXPosition, textPosition);
  textPosition += textStep;
  text("target distance y: " + (midScreenY - target.y), textXPosition, textPosition);
  textPosition += textStep;
  text("target.type: " + target.type, textXPosition, textPosition);
  textPosition += textStep;
  text("sendChars: d:" + sendChars[0] + " u:" + sendChars[1] + " l:" + sendChars[2] + " r:" + sendChars[3], textXPosition, textPosition);
  textPosition += textStep;
  text("In target: " + (inTarget ? "Yes" : "No"), textXPosition, textPosition);
  textPosition += textStep;
  
  textAlign(RIGHT);
  textPosition = textStep;
  textXPosition = (width - 10);
  text("Command fire 1: 1", textXPosition, textPosition);
  textPosition += textStep;
  text("Command fire 2: 2", textXPosition, textPosition);
  textPosition += textStep;
  text("Command fire 4: 3", textXPosition, textPosition);
  textPosition += textStep;
  text("Command increase crosshairs: " + INPUT_INCREASE_CROSSHAIRS, textXPosition, textPosition);
  textPosition += textStep;
  text("Command decrease crosshairs: " + INPUT_DECREASE_CROSSHAIRS, textXPosition, textPosition);
  textPosition += textStep;
  text("Command target brightness: " + INPUT_MODE_BRIGHTEST_POINT, textXPosition, textPosition);
  textPosition += textStep;
  text("Command target face: " + INPUT_MODE_FACE, textXPosition, textPosition);
  textPosition += textStep;
  text("Command target manual: " + INPUT_MODE_MANUAL, textXPosition, textPosition);
  textPosition += textStep;
  text("Command center camera: " + INPUT_CENTER_CAMERA, textXPosition, textPosition);
  
  textAlign(CENTER, CENTER);
  textPosition = (height - 30);
  text("arduinoPortName: " + ARDUINO_PORT_NAME, (width / 2), textPosition);
  textPosition += textStep;
  text("cameraName: " + CAMERA_NAME, (width / 2), textPosition);
}

void drawCrosshairs(boolean inTarget) {
  noFill();
  strokeWeight(2);
  
  if (inTarget) {
    stroke(0, 255, 0);
    targetCircle.setStroke(color(0, 255, 0));
  } else {
    stroke(255, 0, 0);
    targetCircle.setStroke(color(255, 0, 0));
  }
  
  float[] point;
  float radius = midScreenWindow + 5;
  
  point = getPointOnCircle(midScreenX, midScreenY, radius, 315);
  line(midScreenX, midScreenY, point[0], point[1]);
  point = getPointOnCircle(midScreenX, midScreenY, radius, 45);
  line(midScreenX, midScreenY, point[0], point[1]);
  point = getPointOnCircle(midScreenX, midScreenY, radius, 135);
  line(midScreenX, midScreenY, point[0], point[1]);
  point = getPointOnCircle(midScreenX, midScreenY, radius, 225);
  line(midScreenX, midScreenY, point[0], point[1]);
  
  shape(targetCircle);
}

float[] getPointOnCircle(float centerX, float centerY, float radius, float angle) {
  float radian = (angle * PI / 180);
  float x = centerX + (radius * cos(radian));
  float y = centerY + (radius * sin(radian));
  
  float[] point = {x, y};
  return point;
}

boolean pointInTarget(float x, float y) {
  float squareDistance = sq((midScreenX - x)) + sq((midScreenY - y));
  
  return (squareDistance <= sq(midScreenWindow));
}

Target targetFromBrightestPoint() {
  // Use the brightest spot on screen as the target
  PVector target = opencv.max();
  fill(0, 0, 255);
  strokeWeight(0);
  stroke(0, 0, 255);
  
  ellipse(target.x, target.y, 10, 10);
  
  return new Target(target.x, target.y, "Brightest point");
}

Target targetFromFace() {
  // Faces detected in this frame
  Rectangle[] faces = opencv.detect();
  
  // Draw all the faces
  if (faces.length > 0) {
    Rectangle face = faces[0];
    
    noFill();
    strokeWeight(2);
    stroke(0, 0, 255);
    
    rect(face.x, face.y, face.width, face.height);

    int faceX = face.x + (face.width / 2);
    int faceY = face.y + (face.height / 2);
    
    return new Target(faceX, faceY, "Face");
  }
  
  //No faces found, make the center of the screen the target
  return null;
}

void keyPressed() {
  if (false != keyPressed) {
    char sendChar = '.';
    
    if (INPUT_FIRE_1 == key) {
      sendChar = FIRE_1;
    } else if (INPUT_FIRE_2 == key) {
      sendChar = FIRE_2;
    } else if (INPUT_FIRE_3 == key) {
      sendChar = FIRE_3;
    }
    
    if (targetMode == TARGET_MODE_MANUAL) {
      if (CODED == key) {
        if (UP == keyCode) {
          sendChar = MOVE_UP;
        } else if (DOWN == keyCode) {
          sendChar = MOVE_DOWN;
        } else if (LEFT == keyCode){
          sendChar = MOVE_LEFT;
        } else if (RIGHT == keyCode) {
          sendChar = MOVE_RIGHT;
        }
      } else if (INPUT_CENTER_CAMERA == key) {
        sendChar = MOVE_MIDDLE;
      }
    }
    
    //If it was a key press to send to the Arduino...
    if ('.' != sendChar) {
      println("sendChar: " + sendChar);
      arduino.write(sendChar);
    } else {
      //It wasn't a message for the Arduino, lets check if we can understand it
      if (INPUT_INCREASE_CROSSHAIRS == key) {
        midScreenWindow += 10;
        setupTargetCircle();
      } else if (INPUT_DECREASE_CROSSHAIRS == key) {
        midScreenWindow -= 10;
        setupTargetCircle();
      } else if (INPUT_MODE_BRIGHTEST_POINT == key) {
        targetMode = TARGET_MODE_BRIGHTEST_POINT;
      } else if (INPUT_MODE_FACE == key) {
        targetMode = TARGET_MODE_FACE;
      } else if (INPUT_MODE_MANUAL == key) {
        targetMode = TARGET_MODE_MANUAL;
      }
    }
  }
}