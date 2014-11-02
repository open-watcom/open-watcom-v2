//
// Copyright (C) 1991 Texas Instruments Incorporated.
//
// Permission is granted to any individual or institution to use, copy, modify,
// and distribute this software, provided that this complete copyright and
// permission notice is maintained, intact, in all copies and supporting
// documentation.
//
// Texas Instruments Incorporated provides this software "as is" without
// express or implied warranty.
//

#include <cool/String.h>
#include <cool/Range.h>
#include <cool/Range.C>
#include <cool/test.h>

COOL_DefineRange(int,2,8,IntRange);
COOL_DefineRange(double,2.5,8.8,DoubleRange);
COOL_DefineRange(char*,"D","K",StrRange);


void test_int () {
  IntRange r1;
  TEST ("CoolRange<int,2,8> r1", 1, 1);
  TEST ("r1.low()==2", r1.low(), 2);
  TEST ("r1.high()==8", r1.high(), 8);
  TEST ("r1.set(4)", (r1.set(4), int(r1)), 4);
  TEST ("r1()", int(r1), 4);
  int i;
  TEST ("int i = 2+r1", (i=2+r1,i), 6);
  IntRange r2(r1);
  TEST ("CoolRange<int,2,8> r2(r1)", int(r2), 4);
}


void test_double () {
  DoubleRange r1;
  TEST ("CoolRange<double,2.5,8.8> r1", 1, 1);
  TEST ("r1.low()==2.5", r1.low(), 2.5);
  TEST ("r1.high()==8.8", r1.high(), 8.8);
  TEST ("r1.set(4.3)", (r1.set(4.3), double(r1)), 4.3);
  TEST ("r1()", double(r1), 4.3);
  double d;
  TEST ("double d = 2.8+r1", (d=2.8+r1,d), 7.1);
  DoubleRange r2(r1);
  TEST ("CoolRange<double,2.5,8.8> r2(r1)", double(r2), 4.3);
}


void test_charP () {
  StrRange r1;
  TEST ("CoolRange<char*,\"D\",\"K\"> r1", 1, 1);
  TEST ("r1.low()==\"D\"", strcmp (r1.low(),"D"), 0);
  TEST ("r1.high()==\"K\"", strcmp (r1.high(),"K"), 0);
  TEST ("r1.set(\"EFG\")", (r1.set("EFG"), strcmp ((char*)r1, "EFG")), 0);
  TEST ("r1()", strcmp ((char*)r1, "EFG"), 0);
  CoolString s1 (r1);
  TEST ("CoolString s1(r1)", strcmp (s1,"EFG"), 0);
  StrRange r2(r1);
  TEST ("CoolRange<char*,\"D\",\"K\"> r2(r1)", strcmp ((char*)r2, "EFG"), 0);
}

void test_leak () {
  for (;;) {
    test_int();
    test_double();
    test_charP();
  }
}
int main () {
  START("CoolRange");
  test_int();
  test_double();
  test_charP();
#if LEAK
  test_leak();
#endif
  SUMMARY();
  return 0;
}
