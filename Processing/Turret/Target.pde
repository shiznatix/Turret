class Target {
  int x = 0;
  int y = 0;
  String type = "";
  
  Target(int x, int y, String type) {
    this.x = x;
    this.y = y;
    this.type = type;
  }
  
  Target(float x, float y, String type) {
    this.x = int(x);
    this.y = int(y);
    this.type = type;
  }
}