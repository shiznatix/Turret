import processing.serial.*;

color COLOR_BLACK = color(0, 0, 0);

//control movements
color COLOR_WHITE = color(255, 255, 255);
color COLOR_RED = color(255, 0, 0);
color COLOR_BLUE = color(0, 0, 255);
color COLOR_GREEN = color(0, 255, 0);

//modes
color COLOR_YELLOW = color(255, 255, 0);
color COLOR_TURQUOISE = color(0, 255, 255);
color COLOR_PINK = color(255, 0, 255);

//fire
color COLOR_GRAY = color(125, 125, 125);
color COLOR_PURPLE = color(125, 0, 125);
color COLOR_GOLD = color(125, 125, 0);

color currentColor = COLOR_BLACK;

Serial port;

void setup() {
  size(100, 100);
  
  String[] serialDevices = Serial.list();
  println("Available serial ports:");
  
  for (int i = 0; i < serialDevices.length; i++) {
    println(i + ": " + serialDevices[i]);
  }
  
  port = new Serial(this, Serial.list()[2], 9600);
}

void draw() {
  background(0, 0, 0);
  if (true == keyPressed) { // if key is pressed
    fill(currentColor);
    ellipse(50, 50, 50, 50); 
  }
  else {
   keyReleased(); 
  }
}
  
void keyPressed() {
  if (CODED == key) {
    char sendChar = '.';
    
    if (UP == keyCode) {
      currentColor = COLOR_WHITE;
      sendChar = 'u';
    } else if (DOWN == keyCode) {
      currentColor = COLOR_RED;
      sendChar = 'd';
    } else if (LEFT == keyCode){
      currentColor = COLOR_GREEN;
      sendChar = 'l';
    } else if (RIGHT == keyCode) {
      currentColor = COLOR_BLUE;
      sendChar = 'r';
    }
    
    if ('.' != sendChar) {
      port.write(sendChar);
    }
  } else {
    if ('m' == key) {//move to middle
      currentColor = COLOR_PINK;
    } else if ('1' == key) {//fire 1
      currentColor = COLOR_GRAY;
    } else if ('2' == key) {//fire 2
      currentColor = COLOR_PURPLE;
    } else if ('3' == key) {//fire 3
      currentColor = COLOR_GOLD;
    }
    
    port.write(key);
  }
    
  if (false == keyPressed) {
    port.write('0');
  }
}
  
void keyReleased() {
  port.write('0'); 
}