struct P {
    int (operator int *);		// must be a simple function
};

struct R {
    operator double ( int );	// no arguments allowed
};

struct S {
    int operator short ();	// no return type allowed
    * operator float * ();
};

struct T {
    int *~T;
};

struct U {
    int * ~U();
};

struct V {
    * ~V();
};

struct W {
    int ~W;
};

struct X {
    ~X( int, double );
    ~X( int );
};

struct Y {
    int ((operator +)[10]);
};
