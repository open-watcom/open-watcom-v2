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
// Updated: JAM 08/14/92 -- modernized template syntax, remove macro hacks

#include <cool/String.h>
#include <cool/Pair.h>
#include <cool/Pair.C>
#include <cool/test.h>


Boolean compare_double_String (const CoolPair<double,CoolString>& s1,
                               const CoolPair<double,CoolString>& s2) {
  if (s1.get_first() == s2.get_first() && s1.get_second() == s2.get_second())
    return TRUE;
  return FALSE;
}


void test_int_char () {
  CoolPair<int,char> p0;
  TEST("CoolPair<int,char> p0", 1, 1);
  CoolPair<int,char> p1(1,'a');
  TEST("CoolPair<int,char> p1(1,'a')", 1, 1);
  CoolPair<int,char> p2(p1);
  TEST("CoolPair<int,char> p2(p1)", 1, 1);
  TEST("p1.get_first()", p1.get_first(), 1);
  TEST("p2.get_second()", p2.get_second(), 'a');
  p1.set_first(2);
  TEST("p1.set_first(2)", p1.get_first(), 2);
  p1.set_second('b');
  TEST("p1.set_second('b')", p1.get_second(), 'b');
  TEST("p2 = p1", (p2 = p1), p1);
  TEST("p1 == p2", (p1 == p2), TRUE);
  TEST("p1 != p2", (p1 != p2), FALSE);
}


Boolean compare_int_charP(const CoolPair<int,char*>& s1,const CoolPair<int,char*>& s2){
  if (s1.get_first() == s2.get_first() &&
      (strcmp (s1.get_second(), s2.get_second()) == 0))
    return TRUE;
  return FALSE;
}


void test_int_charP () {
  CoolPair<int,char*> p0;
  TEST("CoolPair<int,char*> p0", 1, 1);
  CoolPair<int,char*> p1(1,"AAA");
  TEST("CoolPair<int,char*> p1(1,\"AAA\")", 1, 1);
  p1.set_compare(&compare_int_charP);
  TEST ("p1.set_compare(&compare_int_charP)", 1,1);
  CoolPair<int,char*> p2(p1);
  TEST("CoolPair<int,char*> p2(p1)", 1, 1);
  TEST("p1.get_first()", p1.get_first(), 1);
  TEST("p1.get_second()", strcmp (p1.get_second(), "AAA"), 0);
  TEST("p2.get_second()", strcmp (p2.get_second(), "AAA"), 0);
  p1.set_first(2);
  TEST("p1.set_first(2)", p1.get_first(), 2);
  p1.set_second("BBB");
  TEST("p1.set_second(\"BBB\")", strcmp (p1.get_second(), "BBB"), 0);
  TEST("p2 = p1", (p2 = p1), p1);
  TEST("p1 == p2", (p1 == p2), TRUE);
  TEST("p1 != p2", (p1 != p2), FALSE);
}


void test_double_String () {
  CoolPair<double,CoolString> p0;
  TEST("CoolPair<double,CoolString> p0", 1, 1);
  CoolPair<double,CoolString> p1(1.0,CoolString("AAA"));
  TEST("CoolPair<double,CoolString> p1(1.0,CoolString(\"AAA\"))", 1, 1);
  CoolPair<double,CoolString> p2(p1);
  TEST("CoolPair<double,CoolString> p2(p1)", 1, 1);
  TEST("p1.get_first()", p1.get_first(), 1);
  TEST("p1.get_second()", p1.get_second(), CoolString("AAA"));
  TEST("p2.get_second()", p2.get_second(), CoolString("AAA"));
  p1.set_first(2.0);
  TEST("p1.set_first(2.0)", p1.get_first(), 2.0);
  p1.set_second(CoolString("BBB"));
  TEST("p1.set_second(CoolString(\"BBB\"))", p1.get_second(), "BBB");
  TEST("p2 = p1", (p2 = p1), p1);
  TEST("p1 == p2", (p1 == p2), TRUE);
  TEST("p1 != p2", (p1 != p2), FALSE);
}

void test_leak () {
  for (;;) {
    test_int_char ();
    test_int_charP ();
    test_double_String ();
  }
}

int main (void) {
  START("CoolPair");
  test_int_char ();
  test_int_charP ();
  test_double_String ();
#if LEAK
  test_leak ();
#endif
  SUMMARY();
  return 0;
}  
