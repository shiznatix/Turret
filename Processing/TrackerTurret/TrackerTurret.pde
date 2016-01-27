import gab.opencv.*;
import processing.video.*;
import processing.serial.*;
import java.awt.*;

//Camera connection
Capture cam;
OpenCV opencv;
final String cameraName = "name=USB2.0 Camera #2,size=640x480,fps=30";

//The variables correspond to the middle of the screen,
//and will be compared to the midFace values
int midScreenY;
int midScreenX;
int midScreenWindow = 50;  //This is the acceptable 'error' for the center of the screen.

//Arduino serial connection
Serial port;
final String arduinoPortName = "/dev/cu.usbmodem1411";

void setup() {
  size(640, 480);
  
  //Setup Arduino serial connection
  String[] serialDevices = Serial.list();
  int serialDevicePort = -1;
  println("Available serial ports:");
  
  for (int i = 0; i < serialDevices.length; i++) {
    if (serialDevicePort < 0 && serialDevices[i].indexOf(arduinoPortName) > -1) {
      serialDevicePort = i;
      println("Found " + i + ": '" + serialDevices[i] + "' looking for: '" + arduinoPortName + "'");
    } else {
      println(i + ": " + serialDevices[i] + "' looking for: " + arduinoPortName + "'");
    }
  }
  
  if (serialDevicePort < 0) {
    println("Could not find arduino");
    exit();
  }
  
  port = new Serial(this, Serial.list()[serialDevicePort], 9600);
  
  //Setup camera and OpenCV
  midScreenY = (height / 2);
  midScreenX = (width / 2);

  String[] cameras = Capture.list();
  int cameraPort = -1;
  
  if (cameras.length == 0) {
    println("There are no cameras available for capture.");
    exit();
  }
  
  println("Available cameras:");
  
  for (int i = 0; i < cameras.length; i++) {
    if (cameraPort < 0 && cameras[i].indexOf(cameraName) > -1) {
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
  
  cam = new Capture(this, cameras[cameraPort]);
  cam.start();
  opencv = new OpenCV(this, width, height);
  opencv.loadCascade(OpenCV.CASCADE_FRONTALFACE);
}

void draw() {
  if (cam.available()) {
    cam.read();
    
    opencv.loadImage(cam);
    
    image(cam, 0, 0);
    
    // Faces detected in this frame
    Rectangle[] faces = opencv.detect();
    
    // Draw all the faces
    if (faces.length > 0) {
      println("--" + frameCount + "--");
      
      Rectangle face = faces[0];
      
      noFill();
      strokeWeight(3);
      stroke(255,0,0);
      
      rect(face.x, face.y, face.width, face.height);
      
      int midFaceX = face.x + (face.width / 2);
      int midFaceY = face.y + (face.height / 2);
      char[] sendChars = {'.', '.', '.', '.'};
      
      println("midFaceX: " + midFaceX + " midScreenX: " + midScreenX);
      println("midFaceY: " + midFaceY + " midScreenY: " + midScreenY);
      
      //Find out if the Y component of the face is below the middle of the screen.
      if (midFaceY < (midScreenY - midScreenWindow)) {
        //sendChar = 'd';
        sendChars[0] = 'd';
      }
      //Find out if the Y component of the face is above the middle of the screen.
      else if (midFaceY > (midScreenY + midScreenWindow)) {
        //sendChar = 'u';
        sendChars[1] = 'u';
      }
      
      //Find out if the X component of the face is to the left of the middle of the screen.
      if (midFaceX < (midScreenX - midScreenWindow)) {
        sendChars[2] = 'l';
      }
      //Find out if the X component of the face is to the right of the middle of the screen.
      else if (midFaceX > (midScreenX + midScreenWindow)) {
        sendChars[3] = 'r';
      }
      
      boolean sentChars = false;
      
      for (int i = 0; i < 4; i++) {
        if ('.' != sendChars[i]) {
          println("sendChar: " + sendChars[i]);
          port.write(sendChars[i]);
          sentChars = true;
        }
      }
      
      if (sentChars) {
        delay(100);
      }
    }
  }
}

void keyPressed() {
  if (false != keyPressed) {
    char sendChar = '.';
    
    if ('1' == key) {//fire 1
      sendChar = '1';
    } else if ('2' == key) {//fire 2
      sendChar = '2';
    } else if ('3' == key) {//fire 3
      sendChar = '3';
    }
    
    if ('.' != sendChar) {
      println("sendChar: " + sendChar);
      port.write(sendChar);
    }
  }
}