#pragma warning 895 1
#pragma warning 896 1

#define XXXX int \              		    
x;

XXXX

class X {;};
struct Q {;};
class X1 {};
struct Q1 {};

struct Z {
    void *operator new( unsigned ) = 0;
    void operator delete( void * ) = 0;
    virtual void *operator new[]( unsigned ) = 0;
    virtual void operator delete[]( void * ) = 0;
    void foo() = 0;
    Z();
    ~Z() = 0;
};
