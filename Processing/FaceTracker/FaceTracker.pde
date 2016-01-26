import gab.opencv.*;
import processing.video.*;
import processing.serial.*;
import java.awt.*;

Capture cam;
OpenCV opencv;

//The variables correspond to the middle of the screen,
//and will be compared to the midFace values
int midScreenY;
int midScreenX;
int midScreenWindow = 10;  //This is the acceptable 'error' for the center of the screen.
//Servo positions
int servoTilt = 90;
int servoPan = 90;
int stepSize = 1;

//Arduino serial connection
Serial port;

void setup() {
  size(640, 480);
  
  //Setup Arduino serial connection
  String[] serialDevices = Serial.list();
  println("Available serial ports:");
  
  for (int i = 0; i < serialDevices.length; i++) {
    println(i + ": " + serialDevices[i]);
  }
  
  port = new Serial(this, Serial.list()[1], 9600);
  
  //Setup camera and OpenCV
  midScreenY = (height / 2);
  midScreenX = (width / 2);

  String[] cameras = Capture.list();
  
  if (cameras.length == 0) {
    println("There are no cameras available for capture.");
    exit();
  }
  
  println("Available cameras:");
  
  for (int i = 0; i < cameras.length; i++) {
    print(i + ": ");
    println(cameras[i]);
  }
  
  cam = new Capture(this, cameras[0]);
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
      
      println("midFaceX: " + midFaceX + " midScreenX: " + midScreenX);
      println("midFaceY: " + midFaceY + " midScreenY: " + midScreenY);
      
      //Find out if the Y component of the face is below the middle of the screen.
      if (midFaceY < (midScreenY - midScreenWindow)) {
        if (servoTilt >= 5) {
          //If it is below the middle of the screen,
          //update the tilt position variable to lower the tilt servo.
          servoTilt -= stepSize;
        }
      }
      //Find out if the Y component of the face is above the middle of the screen.
      else if (midFaceY > (midScreenY + midScreenWindow)) {
        if (servoTilt <= 175) {
          //Update the tilt position variable to raise the tilt servo.
          servoTilt +=stepSize;
        }
      }
      
      //Find out if the X component of the face is to the left of the middle of the screen.
      if (midFaceX < (midScreenX - midScreenWindow)) {
        if (servoPan >= 5){
          //Update the pan position variable to move the servo to the left.
          servoPan -= stepSize;
        }
      }
      //Find out if the X component of the face is to the right of the middle of the screen.
      else if (midFaceX > midScreenX + midScreenWindow) {
        if (servoPan <= 175) {
          //Update the pan position variable to move the servo to the right.
          servoPan +=stepSize;
        }
      }
      
      println("servoTilt: " + servoTilt);
      println("servoPan: " + servoPan);
    }
  }
}