#include "dump.h"

struct A { int i; };

struct B : public A {};

struct C : public B {};

void f(int B::*p) GOOD;
void f(int C::*p) BAD;

void g(int C::*p) BAD;
void g(int B::*p) GOOD;

int main()
{
   f(&A::i);
   CHECK_GOOD( 9 );
   g(&A::i);
   CHECK_GOOD( 9+13 );
   return( errors != 0 );
}
