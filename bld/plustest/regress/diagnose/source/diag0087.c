int a[10];
int foo(int,int);
int foo();
static int sss;
template <class T,int x,int *p, int(f)()> class X;
X<int> *bad1;
X<int,1,0,0,1> *bad2;
X<1,1,a,foo> *bad3;
X<int,1,0,foo> *bad4;
X<int,1,a,0> *bad5;
X<int,int,a,foo> *bad6;
X<int,0,int,foo> *bad7;
X<int,0,a,int> *bad8;
X<double,foo(1,2),a,foo> *bad9;
X<double,1,&sss,foo> *ok1;
X<double,0,a,foo> *ok1;
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
