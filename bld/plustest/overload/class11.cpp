#include "dump.h"

struct A {};

struct B: public A {};

struct bottom_middle1: public B {};

struct middle2: public A {};

struct C: public middle2, public bottom_middle1 {};

void f(int, B) GOOD;
void f(int, A) BAD;

void g(int, B*) GOOD;
void g(int, A*) BAD;

void h(int, B&) GOOD;
void h(int, A&) BAD;


int main()
{
    C c;
    C *pc;

    f(13, c);

    g(13, pc);

    h(13, c);

    CHECK_GOOD( 13+16+19 );
    return errors != 0;

}
