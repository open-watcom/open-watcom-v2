template <class T,int x,int *p, int(f)()> class X;
X<int> *bad1;
X<int,1,0,0,1> *bad2;
X<1,1,0,0> *bad3;
X<1,int,int,int> *bad4;
int a[10];
int foo(int,int);
int foo();
static int sss;
X<double,foo(1,2),&a[2],foo> *ok1;
X<double,1,&sss,foo> *ok1;
struct A {
    A foo() const;
};
A A::foo() const {
    return( *this );	// OK
}
void foo( int, int, int a, int b )
{
    unrefd:;
    a = a;
}
#pragma off(unreferenced);
void foo( int, int a, int b )
{
    unrefd:;
    a = a;
}
