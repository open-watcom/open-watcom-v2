#pragma inline_depth 200
class Point {
    long        x, y;
    public:
    Point(const Point&);
    Point(long, long);
};

inline Point::Point(long a, long b) : x(a), y(b) { }
inline Point::Point(const Point& p) : x(p.x), y(p.y) { }

inline Point id(Point p){
    return p;
}

Point barf(){
    return id(id(id(id(id(Point(0,0))))));
}
