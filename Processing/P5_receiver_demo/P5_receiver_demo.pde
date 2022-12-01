import oscP5.*;

OscP5 oscP5;
ArrayList<TrackingPoint> points = new ArrayList<TrackingPoint>();

void setup() {
  size(640, 360);
  oscP5 = new OscP5(this, 12000);
}

synchronized void draw() {
  background(50);  
  textSize(20);
  for (int i = 0; i < points.size(); i++) {
    TrackingPoint p = points.get(i);
    ellipse(p.x, p.y, 10, 10);
    text(p.id, p.x + 10, p.y + 5);
  }
}

synchronized void oscEvent(OscMessage msg) {
  msg.print();

  if ( msg.addrPattern().equals("/points")) {
    points.clear();
    for (int i=0; i<msg.typetag().length(); i+=3) {
      int x = msg.get(i).intValue();
      int y = msg.get(i+1).intValue();
      int id = msg.get(i+2).intValue();
      points.add(new TrackingPoint(x, y, id));
    }
  }
}

class TrackingPoint {
  int x;
  int y;
  int id;
  
  TrackingPoint(int _x, int _y, int _id) {
    this.x = _x;
    this.y = _y;
    this.id = _id;
  }
}
