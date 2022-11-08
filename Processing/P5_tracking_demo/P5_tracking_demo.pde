import oscP5.*;

OscP5 oscP5;
Tracker tracker;

void setup() {
  size(640, 360);
  oscP5 = new OscP5(this, 12000);

  // tracker setup
  tracker = new Tracker();
  tracker.setMaxPoints(5);
  tracker.setMaxPointLife(50);
  tracker.setNearDistance(100);
}

synchronized void draw() {
  background(50);
  tracker.update();
  tracker.draw();

  ArrayList<TrackingPoint> points = tracker.get();
  println("----");
  for (TrackingPoint p : points) {
    println("id=" + p.id + " x=" + p.x + " y=" + p.y );
  }
}

synchronized void oscEvent(OscMessage msg) {
  for (int i=0; i<msg.typetag().length(); i+=2) {
    int x = msg.get(i).intValue();
    int y = msg.get(i+1).intValue();    
    tracker.set(x, y);
  }
}
