import processing.serial.*;
color fillColor = color(0, 0, 0); //circle fill color
Serial port;

void setup() {
  size(100, 100);
  //println("Available serial ports:");
  //println(Serial.list());
  port = new Serial(this, Serial.list()[2], 9600);  
}

void draw() {
  background(0, 0, 0);
  if (true == keyPressed) { // if key is pressed
    fill(fillColor);
    ellipse(50, 50, 50, 50); 
  }
  else {
   keyReleased(); 
  }
}
  
void keyPressed() {
  if (CODED == key) {
    if (UP == keyCode) {
      fillColor = color(255, 255, 255); //white
    } else if (DOWN == keyCode) {
      fillColor = color(255, 0, 0); //red
    } else if (LEFT == keyCode){
      fillColor = color(0, 255, 0); //green1
    } else if (RIGHT == keyCode) {
      fillColor = color(0, 255, 150); //green2      
    }
    
    port.write(keyCode);
  } else if ('c' == key) {
    fillColor = color(0, 0, 255); //blue
    
    port.write(key);
  }
    
  if (false == keyPressed) {
    port.write('0');
  }
}
  
void keyReleased() {
  port.write('0'); 
}