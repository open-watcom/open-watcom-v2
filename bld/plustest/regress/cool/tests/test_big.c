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
// Updated: JAM 08/12/92 -- anach. form() replaced with iomanips

#include <iomanip.h>    // for hex
#include <cool/Bignum.h>
#include <cool/test.h>

int close_enough( double x, double y ) {
    double mx, mn;
    if( x < y ) {
        mn = x;
        mx = y;
    } else {
        mn = y;
        mx = x;
    }
    if( mn == mx ) return 1;
    if( mx == 0 ) return 0;
    if((( mn / mx ) - 1.0 ) < 1e-15 ) {
        return 1;
    }
    return 0;
}

void run_constructor_tests() {
  cout << "\nCoolBignum constructor tests:\n";

  cout << "long constructor:\n";
  {CoolBignum b2(0L);
  TEST ("CoolBignum b2(long(0));", (long)b2 == (long)0, TRUE);}

  {CoolBignum b3(1L);
  TEST ("CoolBignum b3(long(1));", (long)b3 == (long)1, TRUE);}

  {CoolBignum b4(-1L);
  TEST ("CoolBignum b4(long(-1));", (long)b4 == (long)-1, TRUE);}

  {CoolBignum b5((long)(MAXSHORT));
  TEST ("CoolBignum b5(long(MAXSHORT));", (long)b5 == (long)MAXSHORT, TRUE);}
  
  {CoolBignum b6((long)(-MAXSHORT));
  TEST ("CoolBignum b6(long(-MAXSHORT));", (long)b6 == (long)-MAXSHORT, TRUE);}

  {CoolBignum b7((long)(MAXINT));
  TEST ("CoolBignum b7(long(MAXINT));", (long)b7 == (long)MAXINT, TRUE);}

  {CoolBignum b8((long)(-MAXINT));
  TEST ("CoolBignum b8(long(-MAXINT));", (long)b8 == (long)-MAXINT, TRUE);}

  {CoolBignum b9((long)(MAXLONG));
  TEST ("CoolBignum b9(long(MAXLONG));", (long)b9 == (long)MAXLONG, TRUE);}

  {CoolBignum b10((long)(-MAXLONG));
  TEST ("CoolBignum b10(long(-MAXLONG));", (long)b10 == (long)-MAXLONG, TRUE);}

  {CoolBignum b10l((long)(0xF00000l));
  TEST ("CoolBignum b10l(long(0xF00000l));", (long)b10l == 0xF00000, TRUE);}

  cout << "double constructor:\n";
  {CoolBignum b11(0.0);
  TEST ("CoolBignum b11(0.0);", (double)b11 == 0.0, TRUE);}

  {CoolBignum b12(1.0);
  TEST ("CoolBignum b12(1.0);", (double)b12 == 1.0, TRUE);}

  {CoolBignum b13(-1.0);
  TEST ("CoolBignum b13(-1.0);", (double)b13 == -1.0, TRUE);}

  {CoolBignum b14(MAXDOUBLE);
  TEST ("CoolBignum b14(MAXDOUBLE);", (double)b14 == MAXDOUBLE, TRUE);}

  {CoolBignum b15(-MAXDOUBLE);
  TEST ("CoolBignum b15(-MAXDOUBLE);", (double)b15 == -MAXDOUBLE, TRUE);}

  {CoolBignum b16(1234567.0);
  TEST ("CoolBignum b16(1234567.0);", (double)b16 == 1234567.0, TRUE);}

  {CoolBignum b17(-1234567.0);
  TEST ("CoolBignum b17(-1234567.0);", (double)b17 == -1234567.0, TRUE);}

  {CoolBignum b18(1234567000.0);
  TEST ("CoolBignum b18(1234567000.0);", (double)b18 == 1234567000.0, TRUE);}

  {CoolBignum b19(-1234567000.0);
  TEST ("CoolBignum b19(-1234567000.0);", (double)b19 == -1234567000.0, TRUE);}

  {CoolBignum b20((double)(0xF00000)); TEST ("CoolBignum b20(double(0xF00000));",
                                      (double)b20 == (double)0xF00000, TRUE);}

  cout << "char* constructor:\n";
  {CoolBignum b21("-1");
  TEST ("CoolBignum b21(\"-1\");", (long)b21 == -1, TRUE);}

  {CoolBignum b22("+1");
  TEST ("CoolBignum b22(\"+1\");", (long)b22 == 1, TRUE);}

  {CoolBignum b23("1");
  TEST ("CoolBignum b23(\"1\");", (long)b23 == 1, TRUE);}

  {CoolBignum b24("123");
  TEST ("CoolBignum b24(\"123\");", (long)b24 == 123, TRUE);}

  {CoolBignum b25("123e12");
  TEST ("CoolBignum b25(\"123e12\");", (double)b25 == 123e12, TRUE);}

  {CoolBignum b26("123e120");
  TEST ("CoolBignum b26(\"123e120\");", close_enough( (double)b26, 123e120), TRUE);}

  {CoolBignum b27("0x0");
  TEST ("CoolBignum b27(\"0x0\");", (long)b27 == 0x0, TRUE);}

  {CoolBignum b28("0x9");
  TEST ("CoolBignum b28(\"0x9\");", (long)b28 == 0x9, TRUE);}

  {CoolBignum b29("0xa");
  TEST ("CoolBignum b29(\"0xa\");", (long)b29 == 0xa, TRUE);}

  {CoolBignum b30("0xf");
  TEST ("CoolBignum b30(\"0xf\");", (long)b30 == 0xf, TRUE);}

  {CoolBignum b31("0xA");
  TEST ("CoolBignum b31(\"0xA\");", (long)b31 == 0xA, TRUE);}

  {CoolBignum b32("0xF");
  TEST ("CoolBignum b32(\"0xF\");", (long)b32 == 0xF, TRUE);}

  {CoolBignum b33("0x1aF");
  TEST ("CoolBignum b33(\"0x1aF\");", (long)b33 == 0x1aF, TRUE);}

  {CoolBignum b34("0");
  TEST ("CoolBignum b34(\"0\");", (long)b34 == 0, TRUE);}

  {CoolBignum b35("00");
  TEST ("CoolBignum b35(\"00\");", (long)b35 == 0, TRUE);}

  {CoolBignum b36("012334567");
  TEST ("CoolBignum b36(\"012334567\");", (long)b36 == 012334567, TRUE);}

  {CoolBignum b37("9");
  TEST ("CoolBignum b37(\"9\");", (long)b37 == 9, TRUE);}

  {CoolBignum b38("011");
  TEST ("CoolBignum b38(\"011\");", (long)b38 == 9, TRUE);}

  // leading blanks not supported yet
  //{CoolBignum b39(" 9");
  //TEST ("CoolBignum b39(\" 9\");", (long)b39 == 9, TRUE);}

  {CoolBignum b40("0XF");
  TEST ("CoolBignum b40(\"0XF\");", (long)b40 == 0XF, TRUE);}

  cout << "CoolBignum& constructor:\n";
  {CoolBignum b50(CoolBignum(0l));
  TEST ("CoolBignum b50(CoolBignum(0l));", (long)b50 == 0l, TRUE);}

  {CoolBignum b51(CoolBignum(100l));
  TEST ("CoolBignum b51(CoolBignum(100l));", (long)b51 == 100l, TRUE);}
}

void run_conversion_operator_tests() {
  cout << "\nConversion operator tests:\n";

  cout << "short conversion operator:\n";
  TEST("short(CoolBignum(0l)) == 0", short(CoolBignum(0l)) == 0, TRUE);
  TEST("short(CoolBignum(long(MAXSHORT))) == MAXSHORT",
       short(CoolBignum(long(MAXSHORT))) == MAXSHORT, TRUE);
  TEST("short(CoolBignum(long(-MAXSHORT))) == -MAXSHORT",
       short(CoolBignum(long(-MAXSHORT))) == -MAXSHORT, TRUE);

  TEST("short(CoolBignum(long(-MAXSHORT+1))) == short(-MAXSHORT+1)",
       short(CoolBignum(long(-MAXSHORT+1))) == short(-MAXSHORT+1), TRUE);

    // conversion to short fails for MAXSHORT+1
  //TEST("short(CoolBignum(long(MAXSHORT+1))) == short(MAXSHORT+1)",
       //short(CoolBignum(long(MAXSHORT + 1))) == short(MAXSHORT + 1), TRUE);

  cout << "int conversion operator:\n";
  TEST("int(CoolBignum(0l)) == 0", int(CoolBignum(0l)) == 0, TRUE);
  TEST("int(CoolBignum(long(MAXINT))) == MAXINT",
      int(CoolBignum(long(MAXINT))) == MAXINT, TRUE);
  TEST("int(CoolBignum(long(-MAXINT))) == -MAXINT",
       int(CoolBignum(long(-MAXINT))) == -MAXINT, TRUE);

  {CoolBignum b((long)(MAXINT));
  ++b;
  // conversion of bignum MAXINT+1 cannot fit in an int
  //TEST("CoolBignum b(long(MAXINT); b++; (int)b == int(MAXINT+1)",
       //(int)b == int(MAXINT+1), TRUE);

  b = -b;
  ++b;
  TEST("CoolBignum b(long(MAXINT); b++; b=-b; ++b; (int)b == int(-MAXINT)",
       (int)b == int(-MAXINT), TRUE);

  cout << "long conversion operator:\n";
  TEST("long(CoolBignum(0l)) == 0", long(CoolBignum(0l)) == 0, TRUE);
  TEST("long(CoolBignum(MAXLONG)) == MAXLONG",
       long(CoolBignum(MAXLONG)) == MAXLONG, TRUE);
  TEST("long(CoolBignum(-MAXLONG)) == -MAXLONG",
       long(CoolBignum(-MAXLONG)) == -MAXLONG, TRUE);

  b = long(MAXLONG);
  --b;
  TEST("CoolBignum b; b =(long(MAXLONG); b--; (long)b == long(MAXLONG-1)",
       (long)b == long(MAXLONG-1), TRUE);

  b = -b;
  TEST("CoolBignum b = (long(MAXLONG); b--; b=-b; (long)b == long(-(MAXLONG-1))",
       (long)b == long(-(MAXLONG-1)), TRUE);

  cout << "float conversion operator:\n";
  float f;
  TEST("float(CoolBignum(0.0)) == 0.0", ((float) CoolBignum(0.0)) == 0.0, TRUE);
  TEST("float(CoolBignum(99999.0)) == 99999.0",
       ((float) CoolBignum(99999.0)) == 99999.0, TRUE);
  TEST("float(CoolBignum(MAXFLOAT)) == MAXFLOAT",
       ((f = MAXFLOAT) == (float) CoolBignum(MAXFLOAT)), TRUE);
  TEST("float(CoolBignum(-MAXFLOAT)) == -MAXFLOAT",
       ((f = -MAXFLOAT) == float(CoolBignum(-MAXFLOAT))), TRUE);
#if 0
  // these should produce failures because MAXDOUBLE cannot fit in a float
  b = MAXDOUBLE;
  --b;
  f = (float) b;
  TEST("CoolBignum b = MAXDOUBLE; b--; (float)b == float(MAXDOUBLE-1)",
       f == float(MAXDOUBLE-1), TRUE);

  f = (float) (-b);
  TEST("CoolBignum b = MAXDOUBLE; b--, b=-b; (float)b == float(-(MAXDOUBLE-1))",
       f = float(-(MAXDOUBLE-1)), TRUE);
#endif


  cout << "double conversion operator:\n";
  double d;
  TEST("double(CoolBignum(0.0)) == 0.0", ((double) CoolBignum(0.0)) == 0.0, TRUE);
  TEST("double(CoolBignum(99999.0)) == 99999.0",
       (double) CoolBignum(99999.0) == 99999.0, TRUE);
  TEST("double(CoolBignum(1e300)) == 1e300",
       double(CoolBignum(1e300)) == 1e300, TRUE);
  TEST("double(CoolBignum(-1e300)) == -1e300",
       double(CoolBignum(-1e300)) == -1e300, TRUE);
  TEST("double(CoolBignum(MAXFLOAT)) == MAXFLOAT",
       ((d = MAXFLOAT) == (double) CoolBignum(MAXFLOAT)), TRUE);
  TEST("double(CoolBignum(-MAXFLOAT)) == -MAXFLOAT",
       ((d = -MAXFLOAT) == double(CoolBignum(-MAXFLOAT))), TRUE);
  TEST("double(CoolBignum(MAXDOUBLE)) == MAXDOUBLE",
       (double) CoolBignum(MAXDOUBLE) == MAXDOUBLE, TRUE);
  TEST("double(CoolBignum(-MAXDOUBLE)) == -MAXDOUBLE",
       (double) CoolBignum(-MAXDOUBLE) == -MAXDOUBLE, TRUE);
  b = MAXDOUBLE;
  --b;
  ++b;
  b = b / CoolBignum(2L);
  d = (double) b;
  TEST("CoolBignum b = MAXDOUBLE; b--; ++b; b/=2; (double)b == double(MAXDOUBLE/2)",
       close_enough( d, double(MAXDOUBLE/2) ), TRUE);

  d = (double) (-b);
  TEST("CoolBignum b =MAXDOUBLE; b--, ++b; b/=2; b=-b; (double)b == double(-(MAXDOUBLE/2))",
       close_enough( d, double(-(MAXDOUBLE/2)) ), TRUE);
}}

void run_assignment_tests() {
  cout << "\nStarting assignment tests:\n";
  CoolBignum b1;

  TEST_RUN ("CoolBignum b1; b1 = 0xffff;", b1 = 0xffffl,
            long(b1) == 0xffffl, TRUE);

  // double assignment operator
  TEST_RUN ("double(b1) == -1.23e6", b1 = -1.23e6,
            double(b1) == -1.23e6, TRUE);

  // long assignment operator
  TEST_RUN ("long(b1) = -MAXINT", b1 = long(-MAXINT),
            long(b1) == -MAXINT, TRUE);

  // char * assignment operator
  TEST_RUN ("long(b1) = 0x1FFFFFl", b1 = "0x1FFFFF",
            long(b1) == 0x1fffffl, TRUE);


  // CoolBignum& assignment operator
  b1 = "0x1FFFFF";
  CoolBignum b5(0x1FFFFl);
  TEST_RUN ("b1 = b5", b1 = b5, b1 == b5, TRUE);
}

void run_logical_comparison_tests() {
  cout << "\nStarting logical comparison tests:\n";
  CoolBignum b0(0L);
  CoolBignum b1(1L);
  CoolBignum b2((long)(MAXSHORT));
  CoolBignum b3((long)(-MAXSHORT));

  TEST("b0 == b0", b0 == b0, TRUE);
  TEST("b0 == b1", b0 == b1, FALSE);
  TEST("b0 == b2", b0 == b2, FALSE);
  TEST("b0 == b3", b0 == b3, FALSE);
  TEST("b1 == b2", b1 == b2, FALSE);
  TEST("b1 == b3", b1 == b3, FALSE);
  TEST("b2 == b3", b2 == b3, FALSE);

  TEST("b0 != b0", b0 != b0, FALSE);
  TEST("b0 != b1", b0 != b1, TRUE);
  TEST("b0 != b2", b0 != b2, TRUE);
  TEST("b0 != b3", b0 != b3, TRUE);
  TEST("b1 != b2", b1 != b2, TRUE);
  TEST("b1 != b3", b1 != b3, TRUE);
  TEST("b2 != b3", b2 != b3, TRUE);

  TEST("b0 < b0", b0 < b0, FALSE);
  TEST("b0 < b1", b0 < b1, TRUE);
  TEST("b0 < b2", b0 < b2, TRUE);
  TEST("b0 < b3", b0 < b3, FALSE);
  TEST("b1 < b2", b1 < b2, TRUE);
  TEST("b1 < b3", b1 < b3, FALSE);
  TEST("b2 < b3", b2 < b3, FALSE);

  TEST("b0 > b0", b0 > b0, FALSE);
  TEST("b0 > b1", b0 > b1, FALSE);
  TEST("b0 > b2", b0 > b2, FALSE);
  TEST("b0 > b3", b0 > b3, TRUE);
  TEST("b1 > b2", b1 > b2, FALSE);
  TEST("b1 > b3", b1 > b3, TRUE);
  TEST("b2 > b3", b2 > b3, TRUE);

  TEST ("b3 != b2", b3 != b2, TRUE);
  TEST ("b3 != b3", b3 != b3, FALSE);
  TEST ("b3 < b2", b3 < b2, TRUE);
  TEST ("b3 <= b2", b3 <= b2, TRUE);
  TEST ("b3 <= b3", b3 <= b3, TRUE);
  TEST ("b3 > b3", b3 > b3, FALSE);
  TEST ("b3 > b2", b3 > b2, FALSE);
  TEST ("b3 >= b2", b3 >= b2, FALSE);
  TEST ("b3 >= b3", b3 >= b3, TRUE);
  TEST ("b2 >= b2", b2 >= b2, TRUE);
}

void run_division_tests() {
  cout << "\nStarting division tests:\n";

  TEST("long(CoolBignum(0l)/CoolBignum(1l))", long(CoolBignum(0l)/CoolBignum(1l)), 0);
  TEST("long(CoolBignum(-1l)/CoolBignum(1l))", long(CoolBignum(-1l)/CoolBignum(1l)), -1);

  CoolBignum b1,b2,b3;
  long i,j,k,l;
  long div_errors = 0;
  long mod_errors = 0;

  cout << " for (i = 0xFFFFFF; i > 0; i /= 0x10) \n";
  cout << "   for (j = 0x7FFFFFF; j > 0; j /= 0x10) \n";
  cout << "     for (k = 1; k < 17; k++) \n";
  cout << "       for (l = 1; l < 17; l++) \n         ";
  {for (i = 0xFFFFFF; i > 0; i /= 0x10) {
    cout.put('.');
    cout.flush();
    for (j = 0x7FFFFFF; j > 0; j /= 0x10) {
      for (k = 1; k < 17; k++) {
        for (l = 1; l < 17; l++) {
          b1 = CoolBignum(i+k);
          b2 = CoolBignum(j+l);
          b3 = CoolBignum(long((i+k)/(j+l)));
          if (b1/b2 != b3) {
            TEST("(CoolBignum(i+k)/CoolBignum(j+l)) == CoolBignum(long((i+k)/(j+l)))",
                 FALSE, TRUE);
            cout << "i=" << hex << i << ", j=" << hex << j << ", k=" << hex << k << ", l=" << hex << l << "\n" << dec;
            div_errors++;
          }
          b3 = CoolBignum(long((i+k)%(j+l)));
          if (b1%b2 != b3) {
            TEST("(CoolBignum(i+k)%CoolBignum(j+l)) == CoolBignum(long((i+k)%(j+l)))",
                 FALSE, TRUE);
            cout << "i=" << hex << i << ", j=" << hex << j << ", k=" << hex << k << ", l=" << hex << l << "\n" << dec;
            mod_errors++;
          }
        }
      }
    }
  }}

  cout << "\n";
  TEST("(CoolBignum(i+k)/CoolBignum(j+l)) == CoolBignum(long((i+k)/(j+l)))",
       div_errors == 0, TRUE);
  TEST("(CoolBignum(i+k)%CoolBignum(j+l)) == CoolBignum(long((i+k)%(j+l)))",
       mod_errors == 0, TRUE);

/*
  char num[130], den[130];
  CoolBignum b,r;
  
  while (1) {
    cout << "Enter next numerator:  ";
    cin >> num;
    cout << "Enter next denominator:  ";
    cin >> den;

    b = CoolBignum(num)/CoolBignum(den);
    r = CoolBignum(num) % CoolBignum(den);
    cout << "\nquotient  = " << b << "\n";
    cout <<   "            "; b.dump(); cout << "\n";
    cout << "\nremainder = " << r << "\n";
    cout <<   "            "; r.dump(); cout << "\n";

  }
*/
}

void run_multiplication_division_tests() {      
  cout << "\nCheck example in book:\n";

  CoolBignum b2 = "0xFFFFFFFF";                 // Create CoolBignum object
  CoolBignum b3 = "12345e30";                   // Create CoolBignum object

  TEST ("(b2*b3) / b3 = b2", ((b2*b3) / b3 == b2), 1);
  TEST ("(b2*b3) / b2 = b3", ((b2*b3) / b2 == b3), 1);
  TEST ("((b3/b2) * b2) + (b3%b2) = b3", (((b3/b2) * b2) + (b3%b2) == b3), 1);
}

void run_addition_subtraction_tests() {
  cout << "\nStarting addition, subtraction tests:\n";

  long i,j;
  long add_errors = 0;
  long sub_errors = 0;
  CoolBignum bi,bj,bij;

  cout << " for (i = 1; i < 0xFFFFFFF;  i *= 3) \n";
  cout << "   for (j = 1; j < 0xFFFFFFF; j *= 3) \n      ";

  {for (i = 1; i < 0xFFFFFFF;  i *= 3) {
    cout.put('.');
    cout.flush();
    for (j = 1; j < 0xFFFFFFF; j *= 3) {
      bi = i;
      bj = j;
      bij = CoolBignum(i+j);
      if (bi + bj != bij) {
        TEST("bi + bj == CoolBignum(i + j)", FALSE, TRUE);
   cout << "i = " << hex << i << ", j = " << hex << j << "\n" << dec;
        add_errors++;
      }
      bij = CoolBignum(i-j);
      if (bi - bj != bij) {
        TEST("bi - bj == CoolBignum(i - j)", FALSE, TRUE);
   cout << "i = " << hex << i << ", j = " << hex << j << "\n" << dec;
        sub_errors++;
      }
    }
  }}
  cout << "\n";
  TEST("bi + bj == CoolBignum(i + j)", add_errors==0, TRUE);
  TEST("bi - bj == CoolBignum(i - j)", sub_errors==0, TRUE);

  CoolBignum b0(0l);
  CoolBignum zillion("1000000000000000000");
  CoolBignum b1000(1000l), b1000000(1000000l);

  TEST("-b0 == b0", (-b0 == b0), TRUE);
  TEST("-b1000 == CoolBignum(-1l)*b1000", (-b1000 == CoolBignum(-1l)*b1000), TRUE);
  TEST("-(-b1000000) == b1000000", (-(-b1000000) == b1000000), TRUE);
  TEST("b0 + b1000 == b1000", (b0 + b1000 == b1000), TRUE);
  TEST("b0 + b1000000 == b1000000", (b0 + b1000000 == b1000000), TRUE);
  TEST("b1000 + b0 == b1000", (b1000 + b0 == b1000), TRUE);
  TEST("b1000000 + b0 == b1000000", (b1000000 + b0 == b1000000), TRUE);
  TEST("b0 + (-b1000) == -b1000", (b0 + (-b1000) == -b1000), TRUE);
  TEST("-b1000 + b0 == -b1000", (-b1000 + b0 == -b1000), TRUE);
  TEST("-b1000 + (-b1000) == (CoolBignum(2l)*(-b1000))",
       (-b1000 + (-b1000) == (CoolBignum(2l)*(-b1000))), TRUE);
  TEST("-b1000000 + (-b1000000) == (CoolBignum(2l)*(-b1000000))",
       (-b1000000 + (-b1000000) == (CoolBignum(2l)*(-b1000000))), TRUE);
  TEST("b1000000 + (-b1000000) == b0", (b1000000 + (-b1000000) == b0), TRUE);
  TEST("zillion + (-zillion) == b0", (zillion + (-zillion) == b0), TRUE);
  TEST("zillion + b1000 == b1000000*b1000000*b1000000 + b1000",
       (zillion + b1000 == b1000000*b1000000*b1000000 + b1000), TRUE);

  TEST("-b0 == b0", (-b0 == b0), TRUE);
  TEST("-b1000 == CoolBignum(-1l)*b1000", (-b1000 == CoolBignum(-1l)*b1000), TRUE);
  TEST("-(-b1000000) == b1000000", (-(-b1000000) == b1000000), TRUE);

  TEST("b0 - b1000 == -b1000", (b0 - b1000 == -b1000), TRUE);
  TEST("b0 - b1000000 == -b1000000", (b0 -b1000000 == -b1000000), TRUE);
  TEST("b1000 - b0 == b1000", (b1000 - b0 == b1000), TRUE);
  TEST("b1000000 - b0 == b1000000", (b1000000 - b0 == b1000000), TRUE);
  TEST("b0 - (-b1000) == b1000", (b0 - (-b1000) == b1000), TRUE);
  TEST("-b1000 - b0 == -b1000", (-b1000 - b0 == -b1000), TRUE);
  TEST("-b1000 - (-b1000) == b0", (-b1000 - (-b1000) == b0), TRUE);
  TEST("-b1000 - (-zillion) == zillion - b1000",
       (-b1000 - (-zillion) == zillion - b1000), TRUE);
  TEST("-b1000000 - (-b1000000) == b0", (-b1000000 - (-b1000000) == b0), TRUE);
  TEST("-b1000000 - (b1000000) == -CoolBignum(2l)*b1000000",
       (-b1000000 - (b1000000) == -CoolBignum(2l)*b1000000), TRUE);
  TEST("b1000000 - (-b1000000) == CoolBignum(2l)*b1000000",
       (b1000000 - (-b1000000) == CoolBignum(2l)*b1000000), TRUE);
  TEST("zillion - (-zillion) == CoolBignum(2l)*zillion",
       (zillion - (-zillion) == CoolBignum(2l)*zillion), TRUE);
  TEST("zillion - b1000 == b1000000*b1000000*b1000000 - b1000",
       (zillion - b1000 == b1000000*b1000000*b1000000 - b1000), TRUE);
  TEST("-zillion - b1000 == -b1000000*b1000000*b1000000 - b1000",
       (-zillion - b1000 == -b1000000*b1000000*b1000000 - b1000), TRUE);

  // example in book
  CoolBignum b2 = "0xFFFFFFFF";                 // Create CoolBignum object
  CoolBignum b3 = "12345e30";                   // Create CoolBignum object
  TEST ("(b2+b3) - b2 = b3", (b2+b3) - b2 == b3, 1);
  TEST ("(b2+b3) - b3 = b2", (b2+b3) - b3 == b2, 1);
}

void run_multiplication_tests() {
  cout << "\nStarting multiplication tests:\n";

  CoolBignum b0(0l), b1000(1000l), b1000000(1000000l),
  zillion("1000000000000000000");

  
  TEST("b0*b0 == b0", (b0*b0 == b0), TRUE);
  TEST("b0*b1000 == b0", (b0*b1000 == b0), TRUE);
  TEST("b1000*b0 == b0", (b1000*b0 == b0), TRUE);
  TEST("b1000*b1000 == b1000000", (b1000*b1000 == b1000000), TRUE);
  TEST("b1000*b1000000 == b1000000*b1000",
       (b1000*b1000000 == b1000000*b1000), TRUE);
  TEST("-b1000000*b1000000*b1000000 == -zillion",
       (-b1000000*b1000000*b1000000 == -zillion), TRUE);
  TEST("zillion*-b1000 == b1000*-zillion",
       (zillion*-b1000 == b1000*-zillion), TRUE);
}

void run_left_shift_tests() {

  CoolBignum b1(1l);

  // left shift
  TEST("int(b1) == 1",int(b1) == 1,TRUE);
  TEST("long(b1 << 1) == 2",long(operator<<(b1,1)) == 2,TRUE);
  TEST("long(b1 << 2) == 4",long(operator<<(b1,2)) == 4,TRUE);
  TEST("long(b1 << 3) == 8",long(operator<<(b1,3)) == 8,TRUE);
  TEST("long(b1 << 4) == 16",long(operator<<(b1,4)) == 16,TRUE);
  TEST("long(b1 << 5) == 32",long(operator<<(b1,5)) == 32,TRUE);
  TEST("long(b1 << 6) == 64",long(operator<<(b1,6)) == 64,TRUE);
  TEST("long(b1 << 7) == 128",long(operator<<(b1,7)) == 128,TRUE);
  TEST("long(b1 << 8) == 256",long(operator<<(b1,8)) == 256,TRUE);
  TEST("long(b1 << 9) == 512",long(operator<<(b1,9)) == 512,TRUE);
  TEST("long(b1 << 10) == 1024",long(operator<<(b1,10)) == 1024,TRUE);
  TEST("long(b1 << 11) == 2048",long(operator<<(b1,11)) == 2048,TRUE);
  TEST("long(b1 << 12) == 4096",long(operator<<(b1,12)) == 4096,TRUE);
  TEST("long(b1 << 13) == 8192",long(operator<<(b1,13)) == 8192,TRUE);
  TEST("long(b1 << 14) == 16384",long(operator<<(b1,14)) == 16384,TRUE);
  TEST("long(b1 << 15) == 32768",long(operator<<(b1,15)) == 32768,TRUE);
  TEST("long(b1 << 16) == 65536",long(operator<<(b1,16)) == 65536,TRUE);
  TEST("(b1 << 32) == CoolBignum(\"4294967296\")",
       (operator<<(b1,32)) == CoolBignum("4294967296"),TRUE);
  TEST("long(-b1 << 1) == -2",long(-operator<<(b1,1)) == -2,TRUE);
  TEST("long(-b1 << 2) == -4",long(-operator<<(b1,2)) == -4,TRUE);
  TEST("long(-b1 << 3) == -8",long(-operator<<(b1,3)) == -8,TRUE);
  TEST("long(-b1 << 4) == -16",long(-operator<<(b1,4)) == -16,TRUE);
  TEST("long(-b1 << 5) == -32",long(-operator<<(b1,5)) == -32,TRUE);
  TEST("long(-b1 << 6) == -64",long(-operator<<(b1,6)) == -64,TRUE);
  TEST("long(-b1 << 7) == -128",long(-operator<<(b1,7)) == -128,TRUE);
  TEST("long(-b1 << 8) == -256",long(-operator<<(b1,8)) == -256,TRUE);
  TEST("long(-b1 << 9) == -512",long(-operator<<(b1,9)) == -512,TRUE);
  TEST("long(-b1 << 10) == -1024",long(-operator<<(b1,10)) == -1024,TRUE);
  TEST("long(-b1 << 11) == -2048",long(-operator<<(b1,11)) == -2048,TRUE);
  TEST("long(-b1 << 12) == -4096",long(-operator<<(b1,12)) == -4096,TRUE);
  TEST("long(-b1 << 13) == -8192",long(-operator<<(b1,13)) == -8192,TRUE);
  TEST("long(-b1 << 14) == -16384",long(-operator<<(b1,14)) == -16384,TRUE);
  TEST("long(-b1 << 15) == -32768",long(-operator<<(b1,15)) == -32768,TRUE);
  TEST("long(-b1 << 16) == -65536",long(-operator<<(b1,16)) == -65536,TRUE);
  TEST("(-b1 << 32) == CoolBignum(\"-4294967296\")",
       (-operator<<(b1,32)) == CoolBignum("-4294967296"),TRUE);
  TEST("long(b1 << -16) == 0",long(operator<<(b1,-16)) == 0,TRUE);
  TEST("long(-b1 << -16) == 0",long(-operator<<(b1,-16)) == 0,TRUE);
}

void run_right_shift_tests() {
  // right shift
  CoolBignum b2("4294967296");
  TEST("b2 == CoolBignum(\"4294967296\")",b2 == CoolBignum("4294967296"), TRUE);
  TEST("(b2 >> 1) == CoolBignum(\"2147483648\")",
       (operator>>(b2,1)) == CoolBignum("2147483648"),TRUE);
  TEST("long(b2 >> 2) == 1073741824",long(operator>>(b2,2)) == 1073741824l,TRUE);
  TEST("long(b2 >> 3) == 536870912",long(operator>>(b2,3)) == 536870912l,TRUE);
  TEST("long(b2 >> 4) == 268435456",long(operator>>(b2,4)) == 268435456l,TRUE);
  TEST("long(b2 >> 5) == 134217728",long(operator>>(b2,5)) == 134217728l,TRUE);
  TEST("long(b2 >> 6) == 67108864",long(operator>>(b2,6)) == 67108864l,TRUE);
  TEST("long(b2 >> 7) == 33554432",long(operator>>(b2,7)) == 33554432l,TRUE);
  TEST("long(b2 >> 8) == 16777216",long(operator>>(b2,8)) == 16777216l,TRUE);
  TEST("long(b2 >> 9) == 8388608",long(operator>>(b2,9)) == 8388608l,TRUE);
  TEST("long(b2 >> 10) == 4194304",long(operator>>(b2,10)) == 4194304l,TRUE);
  TEST("long(b2 >> 11) == 2097152",long(operator>>(b2,11)) == 2097152l,TRUE);
  TEST("long(b2 >> 12) == 1048576",long(operator>>(b2,12)) == 1048576l,TRUE);
  TEST("long(b2 >> 13) == 524288",long(operator>>(b2,13)) == 524288l,TRUE);
  TEST("long(b2 >> 14) == 262144",long(operator>>(b2,14)) == 262144l,TRUE);
  TEST("long(b2 >> 15) == 131072",long(operator>>(b2,15)) == 131072l,TRUE);
  TEST("long(b2 >> 16) == 65536",long(operator>>(b2,16)) == 65536l,TRUE);
  TEST("long(b2 >> 32) == 1",long(operator>>(b2,32)) == 1l,TRUE);
  TEST("long(b2 >> 33) == 0",long(operator>>(b2,33)) == 0l,TRUE);
  TEST("long(b2 >> 67) == 0",long(operator>>(b2,67)) == 0l,TRUE);
  TEST("(-b2 >> 1) == CoolBignum(\"-2147483648\")",
       (operator>>(-b2,1)) == CoolBignum("-2147483648"),TRUE);
  TEST("long(-b2 >> 2) == -1073741824",long(operator>>(-b2,2)) == -1073741824l,TRUE);
  TEST("long(-b2 >> 3) == -536870912",long(operator>>(-b2,3)) == -536870912l,TRUE);
  TEST("long(-b2 >> 4) == -268435456",long(operator>>(-b2,4)) == -268435456l,TRUE);
  TEST("long(-b2 >> 5) == -134217728",long(operator>>(-b2,5)) == -134217728l,TRUE);
  TEST("long(-b2 >> 6) == -67108864",long(operator>>(-b2,6)) == -67108864l,TRUE);
  TEST("long(-b2 >> 7) == -33554432",long(operator>>(-b2,7)) == -33554432l,TRUE);
  TEST("long(-b2 >> 8) == -16777216",long(operator>>(-b2,8)) == -16777216l,TRUE);
  TEST("long(-b2 >> 9) == -8388608",long(operator>>(-b2,9)) == -8388608l,TRUE);
  TEST("long(-b2 >> 10) == -4194304",long(operator>>(-b2,10)) == -4194304l,TRUE);
  TEST("long(-b2 >> 11) == -2097152",long(operator>>(-b2,11)) == -2097152l,TRUE);
  TEST("long(-b2 >> 12) == -1048576",long(operator>>(-b2,12)) == -1048576l,TRUE);
  TEST("long(-b2 >> 13) == -524288",long(operator>>(-b2,13)) == -524288l,TRUE);
  TEST("long(-b2 >> 14) == -262144",long(operator>>(-b2,14)) == -262144l,TRUE);
  TEST("long(-b2 >> 15) == -131072",long(operator>>(-b2,15)) == -131072l,TRUE);
  TEST("long(-b2 >> 16) == -65536",long(operator>>(-b2,16)) == -65536l,TRUE);
  TEST("long(-b2 >> 32) == -1",long(operator>>(-b2,32)) == -1,TRUE);
  TEST("long(-b2 >> 33) == -0",long(operator>>(-b2,33)) == 0,TRUE);
  TEST("long(-b2 >> 67) == -0",long(operator>>(-b2,67)) == 0,TRUE);
}
void run_shift_tests() {
  cout << "\nStarting shift tests:\n";

  run_left_shift_tests();
  run_right_shift_tests();
/*
  CoolBignum b;
  char s[100];
  int sh;

  while (1) {
    cout << "Enter next CoolBignum:  ";
    cin >> s;
    b = s;
    cout << "Enter shift amount: ";
    cin >> sh;
    cout << "Shift == " << sh << "\n";

    b = b << sh;
  }
*/    
}

void test_leak() {
  for (;;) {
    run_constructor_tests();
    run_conversion_operator_tests();
    run_assignment_tests();
    run_addition_subtraction_tests();
    run_multiplication_tests();
    run_division_tests();
    run_multiplication_division_tests();
    run_shift_tests();
    run_logical_comparison_tests();
  }
}

int main (void) {
  START("CoolBignum");
  run_constructor_tests();
  run_conversion_operator_tests();
  run_assignment_tests();
  run_addition_subtraction_tests();
  run_multiplication_tests();
  run_division_tests();
  run_multiplication_division_tests();
  run_shift_tests();
  run_logical_comparison_tests();
#if LEAK
  test_leak();
#endif
  SUMMARY();
  return 0;
}
