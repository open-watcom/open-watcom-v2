void x1()
{
    extern int i = 0;
}

struct S {
    const int a;
    int &r;
    static const int b;
    static int &q;
    static const int c;
    static int &s;
    S() {}
};

S x;

const int S::b;
int &S::q;
const int S::c = 1;
int &S::s = q;

typedef int x23 = 3;
int x24( int, int ) = 4;

const int a;
int &r;
struct X38 {
    X38(int);
};
X38 const x41;
struct X42 {
    X42();
};
X42 const x45;
struct X46 {
};
const X46 x48;
struct X49 {
    int x;
};
const X49 x51;

typedef const int T44;
T44 volatile x17;

struct A47 {
    virtual void foo() = 0;
    int a;
};

A47 x52;
A47 x53 = { 1 };
struct X54 {
    A47 q;
    static A47 z;
};
