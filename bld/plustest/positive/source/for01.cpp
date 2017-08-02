#include "fail.h"
#include <stdio.h>
#include <stdlib.h>

int Ac = 0, Ad = 0;
int Bc = 0, Bd = 0;

class A {
public:
   A( ) { Ac++; }
  ~A( ) { Ad++; }
};

class B {
public:
   B( ) { Bc++; }
  ~B( ) { Bd++; }
};

int main()
{
  int i = 0;
  for (A a; i < 2; i++) {
    B b;
  }
  if ( !( Ac == 1 && Ad == 1 && Bc == 2 && Bd == 2 ) ) {
    fail( __LINE__ );
  }
  _PASS;
}
