#include <stddef.h>
wchar_t x2[] = L"abcd";
wchar_t x3[4] = L"abcd";
char x4[] = "abcd";
char x5[4] = "abcd";
wchar_t x6[] = { L"abcd" };
char x7[] = { "abcd" };

struct T1 {
    T1 operator()(int x) { return T1(x); };
    int operator=(int x) { return x; };
    T1(int) {};
};
struct T2 {
    T2(int) {};
};
int a, (*(*b)(T2))(int), c, d;

struct X1 {
    X1 operator()(int x) { return X1(x); };
    int operator=(int x) { return x; };
    X1(int) {};
};
struct X2 {
    X2(int) {};
};
int e, (*(*f)(T2))(int), g, h;

void x21()
{
    // both lines should be equivalent!
    T1(a) = 3,
    T2(4),
    (*(*b)(T2(c)))(int(d));
    
    auto X1(e) = 3,
    X2(4),
    (*(*f)(X2(g)))(int(h));
}
/*
int x33 = 3;
*/
int x35 = 3;
/*******
************ /******
**** /****
***** /*******
