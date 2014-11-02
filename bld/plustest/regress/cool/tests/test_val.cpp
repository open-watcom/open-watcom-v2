//
// Copyright (C) 1992 General Electric Company.
//
// Permission is granted to any individual or institution to use, copy, modify,
// and distribute this software, provided that this complete copyright and
// permission notice is maintained, intact, in all copies and supporting
// documentation.
//
// General Electric Company provides this software "as is" without
// express or implied warranty.

#include <cool/Value.h>
#include <cool/test.h>
#include <iostream.h>
#include <iomanip.h>

void test_value () {
  TEST ("size>=8+1", sizeof(CoolValue) >=(8+1), TRUE);
  CoolValue v0;
  TEST ("Value v0;", TRUE, TRUE);
  CoolValue v1(v0);
  TEST ("Value v1(v0)", TRUE, TRUE);
  TEST ("v1==v0", v1==v0, TRUE);
  TEST ("v1=v0", (v1 = v0, v1==v0), TRUE);
  CoolValue v2 = v0;
  TEST ("Value v2=v0", v2==v0, TRUE);
  TEST ("(v0='d')==(v1='d')", (v0 = 'd', v1 = 'd', v0==v1), TRUE);
  TEST ("(v0='D')==(v1='d')", (v0 = 'D', v1 = 'd', v0==v1), FALSE);
  CoolValue v3(v0);
  TEST ("Value v3(v0)", v3==v0, TRUE);

  CoolValue i1 = 5;
  TEST ("Value i1=5", i1==v3, FALSE);  
  CoolValue i2(i1);
  TEST ("Value i2(i1)", (i2==i1), TRUE);  
  CoolValue i3;
  TEST ("i3=5", (i3=5, (i3==i2 && i3==i1)), TRUE);  

  CoolValue d1 = double(-5.999999999);
  TEST ("Value d1=-5.999999999", d1==v3, FALSE);  
  CoolValue d2(d1);
  TEST ("Value d2(d1)", (d2==d1), TRUE);  
  CoolValue d3;
  TEST ("d3=-5.999999999", 
        (d3=double(-5.999999999), (d3==d2 && d3==d1)), TRUE);  
  TEST ("d3=-6.0",
        (d3=double(-6.0), (d3=-6.0, d3==d2)), FALSE);  
  TEST ("int!=double", d1==i1, FALSE);
  cout << d1 << "=" << &d1 << endl;
  TEST ("<<", TRUE, TRUE);

  // Conversion from one type to another by promotion,
  // Can loose precision when convert from int to float, or from double to float.
  CoolValue c;
  TEST ("unsigned(char)", unsigned(c='a')==unsigned('a'), TRUE);
  TEST ("int(char)", int(c='A')==int('A'), TRUE);
  CoolValue i;
  TEST ("long(int)", long(i=1000)==long(1000), TRUE);
  TEST ("double(int)", (i=int(-5.5), double(i)==double(-5.0) 
                        || double(i)==double(-6.0)), TRUE);
  {
    float f0 =-5.12345;                         // double(float(-5.12345)) may
    CoolValue f;                                // be optimized by compiler.
    double d1=0, d2=0;                          // and different by more than 1bit.
    TEST ("double(float)", (d1 = double(f=f0), d2 = double(f0), d1==d2), TRUE);
    double d0 = -5.123456789123456789;
    CoolValue d;
    float f1=0, f2=0;
    TEST ("float(double)", (f1 = float(d=d0), f2 = float(d0), f1==f2), TRUE);
  }
}

void test_conversion () {
  char c = 'z', cc = 0;
  CoolValue C(c);
  cc = C;
  TEST ("char", cc==c, TRUE);
  int i = 15, ii = 0;
  CoolValue I(i);
  ii = I;
  TEST ("int", ii==i, TRUE);
  unsigned u = 10, uu = 0;
  CoolValue U(u);
  uu = U;
  TEST ("unsigned", uu==u, TRUE);
  long l = 20000000, ll = 0;
  CoolValue L(l);
  ll = L;
  TEST ("long", ll==l, TRUE);
  float f = 12.3, ff = 0;
  CoolValue F(f);
  ff = F;
  TEST ("float", ff==f, TRUE);
  double d = -12.3e3, dd = 0;
  CoolValue D(d);
  dd = D;
  TEST ("double", dd==d, TRUE);
  void* v = &D; void* vv = NULL;
  CoolValue V(v);
  vv = V;
  TEST ("void*", vv==v, TRUE);
}

void test_leak () {
  for (;;) {
    test_value();
    test_conversion();
  }
}

int main (void) {
  START("CoolValue");
  test_value();
  test_conversion();
  //test_leak();
  SUMMARY();
  return 0;
}

