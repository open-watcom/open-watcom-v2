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

#include <cool/Rational.h>
#include <cool/test.h>
 
void test_rational() {
  CoolRational r1;
  TEST ("CoolRational r1", r1.status(), N_OK);
  CoolRational r2(3,5);
  TEST ("CoolRational r2(3,5)", r2.status(), N_OK);
  TEST ("r2.numerator()", r2.numerator(), 3);
  TEST ("r2.denominator()", r2.denominator(), 5);
  TEST ("r1 != r2", r1 != r2, TRUE);
  CoolRational r3(9);
  TEST ("CoolRational r3(9)", r3.status(), N_OK);
  TEST ("r3.numerator()", r3.numerator(), 9);
  TEST ("r3.denominator()", r3.denominator(), 1);
  CoolRational r4(6,10);
  TEST ("CoolRational r4(6,10)", r4.status(), N_OK);
  TEST ("r4.numerator()", r4.numerator(), 3);
  TEST ("r4.denominator()", r4.denominator(), 5);
  TEST ("r2 == r4", r2 == r4, TRUE);
  TEST ("r1 = r2", (r1 = r2, r1 == r2), TRUE);
  TEST ("r1.status()", r1.status(), N_OK);
  TEST ("r1.numerator()", r1.numerator(), 3);
  TEST ("r1.denominator()", r1.denominator(), 5);
  TEST ("r1 == r4", r1 == r4, TRUE);
  TEST ("-r3",(r1 = -r3,(r1.numerator() == -9 && r1.denominator() == 1)),TRUE);
  CoolRational r5(r1);
  TEST ("CoolRational r5(r1)", r5 == r1, TRUE);
  TEST ("!r5", !r5, FALSE);
  TEST ("r3=r1+r2",(r3=r1+r2,r3.numerator()==-42 && r3.denominator()==5),TRUE);
  TEST ("r1+=r2", (r1+=r2, r1 == r3), TRUE);
  TEST ("r3.invert()",(r3.invert(),r3.numerator()==-5&& r3.denominator()==42),TRUE);
  TEST ("r5=r1+r3",(r5=r1+r3, r5.numerator()==-1789 && r5.denominator()==210), TRUE);
  TEST ("r1+=r3",(r1+=r3, r1 == r5), TRUE);
  TEST ("r4 = r1", (r4 = r1, r4 == r1), TRUE);
  TEST ("r5=r2-(r3.invert())", (r5=r2-(r3.invert()), r5.numerator()==9 && r5.denominator()==1),TRUE);
  TEST ("r2-=r3", (r2-=r3, r2 == r5), TRUE);
  TEST ("short (r1)", short(r1), -8);
  TEST ("int (r1)", int(r1), -8);
  TEST ("long (r1)", long(r1), -8);
  TEST ("float (r1)", (((float)(r1)) < -8.51904 && float(r1) > -8.51906), TRUE);
  TEST ("double (r1)", (((double)(r1)) < -8.51904 && double(r1) > -8.51906), TRUE);
  TEST ("r1.floor()", r1.floor(), -9);
  TEST ("r1.ceiling()", r1.ceiling(), -8);
  TEST ("r1.truncate()", r1.truncate(), -8);
  TEST ("r1.round()", r1.round(), -9);
  TEST ("r1 *= -1", (r1 *= CoolRational(-1), r1 == -r4), TRUE);
  TEST ("r1.floor()", r1.floor(), 8);
  TEST ("r1.ceiling()", r1.ceiling(), 9);
  TEST ("r1.truncate()", r1.truncate(), 8);
  TEST ("r1.round()", r1.round(), 9);
  TEST ("r4 < r1", r4 < r1, TRUE);
  TEST ("r4 <= r1", r4 <= r1, TRUE);
  TEST ("r4 <= r4", r4 <= r4, TRUE);
  TEST ("r1 > r4", r1 > r4, TRUE);
  TEST ("r1 >= r4", r1 >= r4, TRUE);
  TEST ("r1 >= r1", r1 <= r1, TRUE);
  TEST ("++r2", (++r2, r2.numerator() == 10 && r2.denominator() == 1), TRUE);
  TEST ("--r2", (--r2, r2 == r5), TRUE);
  TEST ("r1=r2", (r1=r2, r1.numerator() == 9 && r1.denominator() == 1), TRUE);
  TEST ("r4=r2*r3",(r4=r2*r3,r4.numerator()==-378 &&r4.denominator()==5),TRUE);
  TEST ("r2*=r3", (r2*=r3, r2 == r4), TRUE);
  TEST ("r2/r3", r2 / r3, r1);
  TEST ("r2/=r3", (r2/=r3, r2 == r1), TRUE);
  TEST ("r4%(r2/r3)", (r5=r4%(r2/r3), r5.numerator()==-3 && r5.denominator()==5), TRUE);
  TEST ("r4%=(r2/r3)", (r4%=(r2/r3), r4 == r5), TRUE);
}

void test_leak () {
  for (;;) {
    test_rational();
  }
}

int main (void) {
  START("CoolRational");
  test_rational();
#if LEAK
  test_leak();
#endif
  SUMMARY();
  return 0;
}
