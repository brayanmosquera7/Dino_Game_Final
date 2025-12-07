#ifndef POINT_H
#define POINT_H

struct Point {
  int x;
  int y;
  
  Point() : x(0), y(0) {}
  Point(int _x, int _y) : x(_x), y(_y) {}
  
  bool equals(const Point& other) const {
    return x == other.x && y == other.y;
  }
};

enum Command {
  CMD_JUMP = 1,
  CMD_NONE = 0
};

#endif
