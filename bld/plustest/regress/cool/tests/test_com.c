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

#include <cool/Complex.h>
#include <cool/test.h>

#include <iomanip.h>

inline int almost( double val, double should_be )
{
     return( fabs(val-should_be)<1.0e-12 );
}
 
void test_complex() {
  CoolComplex c1;
  TEST ("Complex c1", c1.status(), N_OK);
  TEST ("c1.modulus", c1.modulus()==0, TRUE);
  CoolComplex c2(3.0,5.0);
  TEST ("Complex c2(3.0,5.0)", c2.status(), N_OK);
  TEST ("c2.real()", c2.real(), 3.0);
  TEST ("c2.imaginary()", c2.imaginary(), 5.0);
  TEST ("c1 != c2", c1 != c2, TRUE);
  TEST ("c2.modulus", almost(c2.modulus(),sqrt(34.0)), TRUE);
  TEST ("c2.argument", almost(c2.argument(),atan2(5.0,3.0)), TRUE);
  CoolComplex c3(9.0);
  TEST ("Complex c3(9.0)", c3.status(), N_OK);
  TEST ("c3.real()", c3.real(), 9.0);
  TEST ("c3.imaginary()", c3.imaginary(), 0.0);
  TEST ("c3.modulus", c3.modulus()==9.0, TRUE);
  TEST ("c3.argument", c3.argument()==0.0, TRUE);
  CoolComplex c4(6,10);
  TEST ("Complex c4(6,10)", c4.status(), N_OK);
  TEST ("c4.real()", c4.real(), 6.0);
  TEST ("c4.imaginary()", c4.imaginary(), 10.0);
  TEST ("c2 == c4", c2 == c4, FALSE);
  TEST ("c1 = c2", c1.operator=(c2), c2);
  TEST ("c1.status()", c1.status(), N_OK);
  TEST ("c1.real()", c1.real(), 3.0);
  TEST ("c1.imaginary()", c1.imaginary(), 5.0);
  TEST ("c1 == c4", c1 == c4, FALSE);
  TEST ("-c3",(c1 = -c3,(c1.real() == -9.0 && c1.imaginary() == 0.0)),TRUE);
  TEST ("c1 = -c3", (c1.real() == -9.0 && c1.imaginary() == 0.0), TRUE);
  CoolComplex c5(c4);
  TEST ("Complex c5(c4)", c4 == c5, TRUE);
  TEST ("!c5", !c5, FALSE);
  TEST ("c5 = c2", (c5 = c2, c5 == c2), TRUE);
  TEST ("--c5", (--c5, c5.real() == 2.0 && c5.imaginary() == 5.0), TRUE);
  TEST ("++c5", (++c5, c5.real() == 3.0 && c5.imaginary() == 5.0), TRUE);
  c5 = c5.invert ();
  TEST ("c5 = c5.invert ()", (c5.real()<=0.089 && c5.real() >= 0.088 &&
           c5.imaginary()<=-0.14 && c5.imaginary()>=-0.15), TRUE);
  TEST ("c1=c2+c4",(c1=c2+c4,c1.real()==9.0 && c1.imaginary()==15.0),TRUE);
  TEST ("c1=c2-c4",(c1=c2-c4,c1.real()==-3.0 && c1.imaginary()==-5.0),TRUE);
  TEST ("c1=c2*c4",(c1=c2*c4,c1.real()==-32.0 && c1.imaginary()==60.0),TRUE);
  TEST ("c1=c2/c4",(c1=c2/c4,c1.real()==0.5 && c1.imaginary()==0.0),TRUE);
  TEST ("c2+=c4",(c2+=c4,c2.real()==9.0 && c2.imaginary()==15.0),TRUE);
  TEST ("c2-=c4",(c2-=c4,c2.real()==3.0 && c2.imaginary()==5.0),TRUE);
  TEST ("c2*=c4",(c2*=c4,c2.real()==-32.0 && c2.imaginary()==-290.0),TRUE);
  TEST ("c2/=c4",(c2/=c4, (c2.real()<=-22.73 && c2.real() >= -22.74 &&
           c2.imaginary()<=-10.44 && c2.imaginary()>=-10.45)),TRUE);
}


void test_linear () {
  CoolComplex r1, c1;
  TEST ("0*x+0", 
        (c1=0, CoolComplex::roots_of_linear(0, 0, r1)==1 && 
         r1==c1), TRUE);
  TEST ("0*x+1", 
        (CoolComplex::roots_of_linear(0, 1, r1)==0), TRUE);
  TEST ("3*x+0", 
        (c1=0, CoolComplex::roots_of_linear(3, 0, r1)==1 && 
         r1==c1), TRUE);
  TEST ("3*x+2", 
        (c1=(-2.0/3.0), CoolComplex::roots_of_linear(3, 2, r1)==1 && 
         r1==c1), TRUE);
}

void test_quadratic () {
  CoolComplex r1, r2, c1, c2;
  TEST ("0*x^2+0*x+0", 
        (c1=0, CoolComplex::roots_of_quadratic(0, 0, 0, r1, r2)==1 && 
         r1==c1), TRUE);
  TEST ("0*x^2+0*x+1", 
        (CoolComplex::roots_of_quadratic(0, 0, 1, r1, r2)==0), TRUE);
  TEST ("3*x^2+2*x+0", 
        (c1=(-2.0/3.0), c2=0,
         CoolComplex::roots_of_quadratic(3, 2, 0, r1, r2)==2 &&
         r1==c1 && 
         r2==c2), TRUE);
  TEST ("x^2+2*x+1", 
        (c1=-1, c2=-1, CoolComplex::roots_of_quadratic(1, 2, 1, r1, r2)==2 &&
         r1==c1 && 
         r2==c2), TRUE);
  TEST ("x^2+3*x+2", 
        (c1=-2, c2=-1, CoolComplex::roots_of_quadratic(1, 3, 2, r1, r2)==2 &&
         r1==c1 && 
         r2==c2), TRUE);
  TEST ("x^2-2*x+2", 
        (c1=CoolComplex(1,1), c2=CoolComplex(1,-1),
         CoolComplex::roots_of_quadratic(1, -2, 2, r1, r2)==2 &&
         r1==c1 && 
         r2==c2), TRUE);
  TEST ("x^2+1", 
        (c1=CoolComplex(0,1), c2=CoolComplex(0,-1),
         CoolComplex::roots_of_quadratic(1, 0, 1, r1, r2)==2 &&
         r1==c1 && 
         r2==c2), TRUE);
}

void test_cubic () {
  CoolComplex r1, r2, r3, c1, c2, c3;
  TEST ("0*x^3+0*x^2+0*x+0", 
        (c1=0, CoolComplex::roots_of_cubic(0, 0, 0, 0, r1, r2, r3)==1 && 
         r1==c1), TRUE);
  TEST ("0*x^3+0*x^2+0*x+1", 
        (CoolComplex::roots_of_cubic(0, 0, 0, 1, r1, r2, r3)==0), TRUE);
  TEST ("x^3+3*x^2+2*x", 
        (c1=-2, c2=-1, c3=0,
         CoolComplex::roots_of_cubic(1, 3, 2, 0, r1, r2, r3)==3 &&
         r1==c1 && 
         r2==c2 &&
         r3==c3), TRUE);
  TEST ("x^3+x", 
        (c1=CoolComplex(0,1), c2=CoolComplex(0,-1), c3=0,
         CoolComplex::roots_of_cubic(1, 0, 1, 0, r1, r2, r3)==3 &&
         r1==c1 && 
         r2==c2 &&
         r3==c3), TRUE);
  TEST ("x^3-3*x^2+3*x-1", 
        (c1=1, c2=1, c3=1,
         CoolComplex::roots_of_cubic(1, -3, 3, -1, r1, r2, r3)==3 &&
         r1==c1 && 
         r2==c2 &&
         r3==c3), TRUE);
  TEST ("x^3-4*x^2+5*x-2", 
        (c1=2, c2=1, c3=1,
         CoolComplex::roots_of_cubic(1, -4, 5, -2, r1, r2, r3)==3 &&
         r1==c1 && 
         r2==c2 && 
         r3==c3), TRUE);
  TEST ("x^3-2*x^2-x+2", 
        (c1=2, c2=1, c3=-1,
         CoolComplex::roots_of_cubic(1, -2, -1, 2, r1, r2, r3)==3 &&
         almost(r1.real(),c1.real()) && almost(r1.imaginary(),c1.imaginary()) &&
         almost(r2.real(),c2.real()) && almost(r2.imaginary(),c2.imaginary()) &&
         almost(r3.real(),c3.real()) && almost(r3.imaginary(),c3.imaginary())
         ), TRUE);
  TEST ("x^3-x^2+x-1", 
        (c1=1, c2=CoolComplex(0,1), c3=CoolComplex(0,-1),
         CoolComplex::roots_of_cubic(1, -1, 1, -1, r1, r2, r3)==3 &&
         almost(r1.real(),c1.real()) && almost(r1.imaginary(),c1.imaginary()) &&
         almost(r2.real(),c2.real()) && almost(r2.imaginary(),c2.imaginary()) &&
         almost(r3.real(),c3.real()) && almost(r3.imaginary(),c3.imaginary())
         ), TRUE);
  TEST ("x^3-x^2+2", 
        (c1=-1, c2=CoolComplex(1,1), c3=CoolComplex(1,-1),
         CoolComplex::roots_of_cubic(1, -1, 0, 2, r1, r2, r3)==3 &&
         almost(r1.real(),c1.real()) && almost(r1.imaginary(),c1.imaginary()) &&
         almost(r2.real(),c2.real()) && almost(r2.imaginary(),c2.imaginary()) &&
         almost(r3.real(),c3.real()) && almost(r3.imaginary(),c3.imaginary())
         ), TRUE);
}

void test_quartic () {
  CoolComplex r1, r2, r3, r4, c1, c2, c3, c4;
  TEST ("0*x^4+0*x^3+0*x^2+0*x+0", 
        (c1=0, CoolComplex::roots_of_quartic(0, 0, 0, 0, 0, r1, r2, r3, r4)==1 && 
         r1==c1), TRUE);
  TEST ("0*x^4+0*x^3+0*x^2+0*x+1", 
        (CoolComplex::roots_of_quartic(0, 0, 0, 0, 1, r1, r2, r3, r4)==0), TRUE);
  TEST ("x^4-2*x^3-x^2+2*x", 
        (c1=2, c2=1, c3=-1, c4=0,
         CoolComplex::roots_of_quartic(1, -2, -1, 2, 0, r1, r2, r3, r4)==4 &&
         almost(r1.real(),c1.real()) && almost(r1.imaginary(),c1.imaginary()) &&
         almost(r2.real(),c2.real()) && almost(r2.imaginary(),c2.imaginary()) &&
         almost(r3.real(),c3.real()) && almost(r3.imaginary(),c3.imaginary()) &&
         almost(r4.real(),c4.real()) && almost(r4.imaginary(),c4.imaginary())
         ), TRUE);
  TEST ("x^4+x^2", 
        (c1=CoolComplex(0,1), c2=CoolComplex(0,-1), c3=0, c4=0,
         CoolComplex::roots_of_quartic(1, 0, 1, 0, 0, r1, r2, r3, r4)==4 &&
         r1==c1 && 
         r2==c2 &&
         r3==c3 &&
         r4==c4), TRUE);
  TEST ("x^4+4*x^3+6*x^2+4*x+1",
        (c1=-1, c2=-1, c3=-1, c4=-1,
         CoolComplex::roots_of_quartic(1, 4, 6, 4, 1, r1, r2, r3, r4)==4 &&
         r1==c1 && 
         r2==c2 &&
         r3==c3 &&
         r4==c4), TRUE);
  TEST ("x^4-4*x^3+6*x^2-4*x+1",
        (c1=+1, c2=+1, c3=+1, c4=+1,
         CoolComplex::roots_of_quartic(1, -4, 6, -4, 1, r1, r2, r3, r4)==4 &&
         r1==c1 && 
         r2==c2 &&
         r3==c3 &&
         r4==c4), TRUE);
  TEST ("x^4-2*x^3+2*x^2-2*x+1",
        (c1=+1, c2=+1, c3=CoolComplex(0,1), c4=CoolComplex(0,-1),
         CoolComplex::roots_of_quartic(1, -2, 2, -2, 1, r1, r2, r3, r4)==4 &&
         r1==c1 && 
         r2==c2 &&
         r3==c3 &&
         r4==c4), TRUE);
  TEST ("8x^4+2x^3-4*x^2+4*x+8",
        ( c1=CoolComplex(0.731504651578053632,0.73922130051136512),
          c2=CoolComplex(0.731504651578053632,-0.73922130051136512),
          c3=CoolComplex(-0.856504651578053632,0.437036774568752704),
          c4=CoolComplex(-0.856504651578053632,-0.437036774568752704),
         CoolComplex::roots_of_quartic(8, 2, -4, 4, 8, r1, r2, r3, r4)==4 &&
         almost(r1.real(),c1.real()) && almost(r1.imaginary(),c1.imaginary()) &&
         almost(r2.real(),c2.real()) && almost(r2.imaginary(),c2.imaginary()) &&
         almost(r3.real(),c3.real()) && almost(r3.imaginary(),c3.imaginary()) &&
         almost(r4.real(),c4.real()) && almost(r4.imaginary(),c4.imaginary())
         ), TRUE);
}

void test_leak () {
  for (;;) {
    test_complex();
    test_linear();
    test_quadratic();
    test_cubic();
    test_quartic();
  }
}

int main (void) {
  START("CoolComplex");
  test_complex();
  test_linear();
  test_quadratic();
  test_cubic();
  test_quartic();
#if LEAK
  test_leak();
#endif
  SUMMARY();
  return 0;
}
