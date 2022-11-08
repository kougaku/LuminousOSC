class Tracker {
  ArrayList<TrackingPoint> points = new ArrayList<TrackingPoint>();
  int max_points = 5;
  int max_point_life = 50;
  int near_distance = 100;

  Tracker() {
  }

  void setMaxPoints(int max_points) {
    this.max_points = max_points;
  }

  void setMaxPointLife(int max_point_life) {
    this.max_point_life = max_point_life;
  }

  void setNearDistance(int near_distance) {
    this.near_distance = near_distance;
  }

  ArrayList<TrackingPoint> get() {
    return (ArrayList<TrackingPoint>)points.clone();
  }

  synchronized void draw() {
    for (int i = 0; i < points.size(); i++) {
      TrackingPoint p = points.get(i);

      fill(255, 0, 0);
      ellipse( p.x, p.y, 5, 5);

      noFill();
      stroke(255);
      ellipse(p.x, p.y, p.life, p.life);

      fill(255);
      textSize(20);
      text(p.id, p.x + 10, p.y + 5 );
    }
  }

  synchronized void set(int x, int y) {    
    // 最も近い点を探す
    int min_i = -1;
    float min_d = 0;
    for (int i=0; i<points.size(); i++) {
      TrackingPoint tracker = points.get(i);
      float d = dist( x, y, tracker.x, tracker.y );
      if (i==0 || d < min_d ) {
        min_d = d;
        min_i = i;
      }
    }
    // 最も近くて閾値以下であれば座標値を更新＆ライフを回復
    if ( min_i >= 0 &&  min_d <= near_distance ) {
      TrackingPoint p = points.get(min_i);
      p.x = x;
      p.y = y;
      p.life = constrain(p.life + 5, 0, max_point_life);  // ライフの回復
    } else {
      // まだ登録された点がない（-1）か、閾値以下の点がなければ新規の点として登録      
      // 使われていない最小の番号にするためにリストの空き位置を探す
      int i = 0;
      for (i=0; i<points.size(); i++) {
        TrackingPoint p = points.get(i);
        if ( p.id != i ) {
          break;
        }
      }
      // 点が上限数に達していなければ新規登録
      if ( points.size() < max_points ) {
        points.add(i, new TrackingPoint(x, y, i, max_point_life));
      }
    }
  }

  // 点のライフ更新処理
  // ライフが0以下であれば、その点は削除
  synchronized void update() {
    for (int i=0; i<points.size(); i++) {
      TrackingPoint p = points.get(i);
      p.life--;  // ライフの減少
      if ( p.life <= 0 ) {
        points.remove(i);
      }
    }
  }
}
