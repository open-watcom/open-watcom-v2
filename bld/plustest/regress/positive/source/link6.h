struct Base {
    int a;
    Base( int x ) : a(x) {}
};
struct Exception : Base {
    Exception( int x ) : Base(x) {}
};
