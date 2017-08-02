struct {
    int a;
    double x;
};
int d;
union { int a; double d; };
static union { int i1; double d1; };
union { int i2; double d2; } static;
struct NEST1 {
    typedef union { int i1; double d1; };
    union { int i2; double d2; } static;
};
static union {
protected:
    int protected1;
private:
    int private1;
public:
    void foo( int );
    friend void friendly_foo( int );
    enum { A, B, C };
    typedef int T;
};
struct NEST2 {
    int a;
    union {
	double a;
	float f;
    };
};
void fn1( void )
{
    typedef union { int a; char c; };
    union { int i1; long l1; };
    union { float f1; double d1; } static;
    a = 1;
    f1 = 1;
    l1 = f1;
}
void bad_location( union { int b1; char b2; } )
{
}
