#include "dump.h"

void foo(int i1, int i2 = 2);	 	// (1)
void foo(int i1, int i2, int i3 = 6);	// (2)
void foo(int i1, int i2);	// (3) redeclares (1) ambig with (2)
void (*f)(int, int) = &foo; // OK (head: OK   branch: OK)
void foo(int,int){}

void goo(int i1 );
void goo(int i1, int i2, int i3 = 6);

void goo(int,int,int) GOOD;

struct D
{
    void operator() (int i1, int i2 = 3);
    void operator() (int i1, int i2, int i3 = 6);
};
void D::operator() (int i1, int i2);
void (D::*pmfb)(int, int) = &D::operator(); // no ambiguity
void (D::*pmfb2)(int, int,int) = &D::operator(); // no ambiguity
void D::operator() (int i1, int i2) GOOD
void D::operator() (int,int,int) GOOD

int main()
{
    D d;

    (d.*pmfb)(3,2);
    (d.*pmfb2)(3,2,1);
    goo(2,2);	
    CHECK_GOOD(12+22+23);
    return errors != 0;
}
