import oscP5.*;
import java.awt.Point;

OscP5 oscP5;
ArrayList<Point> points = new ArrayList<Point>();

void setup() {
  size(640, 360);
  oscP5 = new OscP5(this, 12000);
}

synchronized void draw() {
  background(0);  
  textSize(20);
  for (int i=0; i<points.size(); i++) {
    Point p = points.get(i);
    ellipse( p.x, p.y, 10, 10 );
    text( i, p.x + 10, p.y );
  }
}

synchronized void oscEvent(OscMessage msg) {
  points.clear();
  for (int i=0; i<msg.typetag().length(); i+=2) {
    int x = msg.get(i).intValue();
    int y = msg.get(i+1).intValue();
    points.add( new Point(x, y));
  }
}
