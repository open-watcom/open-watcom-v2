// wpp seems to generate very poor code for inlined functions, esp. when
// parameters are passed by value.  For example, there is a significant
// difference in the quality of code generated for foo and bar.  Changing
// operator+ and operator/ to take references as parameters improves
// things.

class Point {
  public:
    long x,y;
  public:
    Point(const Point&);
    Point(const long, const long);
};

inline Point::Point(const Point& p)
    : x(p.x), y(p.y) {
    ;
}

inline Point::Point(const long a, const long b)
    : x(a), y(b) {
    ;
}

inline Point operator+(const Point a, const Point b){
    return Point(a.x+b.x, a.y+b.y);
}

inline Point operator/(const Point a, const long b){
    return Point(a.x/b, a.y/b);
}

Point foo(Point a, Point b){
    return (a+b)/2;
}

Point bar(Point a, Point b){
    return Point((a.x+b.x)/2, (a.y+b.y)/2);
}
