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
#include <cool/Association.h>
#include <cool/test.h>

#include <cool/Pair.C>
#include <cool/Vector.C>
#include <cool/Association.C>

Boolean my_compare_charP (char* const& s1, char* const& s2) {
  return (strcmp (s1, s2) ? FALSE : TRUE);
}

void xx(CoolPair<int,char*>); //##

void test_int_charP () {
  int key;
  char* value;

  CoolAssociation<int,char*> a0;
  TEST("CoolAssociation<int,char*> a0", 1, 1);
  TEST ("a0.length()", a0.length(), 0);
  TEST ("a0.capacity()", a0.capacity(), 0);
  CoolAssociation<int,char*> a1(5);
  TEST("CoolAssociation<int,char*> a1(5)", 1, 1);
  TEST ("a1.length()", a1.length(), 0);
  TEST ("a1.capacity()", a1.capacity(), 5);
  a1.set_value_compare(&my_compare_charP);
  TEST ("a1.set_value_compare(&my_compare_charP)",1,1);
  CoolAssociation<int,char*> a2(10);
  TEST ("CoolAssociation<int,char*> a2(10)",1,1);
  TEST ("a2.capacity()",a2.capacity(),10);
  TEST ("a2.put(1,\"AAA\")",a2.put(1,"AAA"), TRUE);
  TEST ("a2.put(2,\"BBB\")",a2.put(2,"BBB"), TRUE);
  TEST ("a2.put(3,\"CCC\")",a2.put(3,"CCC"), TRUE);
  TEST ("a2.put(4,\"DDD\")",a2.put(4,"DDD"), TRUE);
  TEST ("a2.length()", a2.length(), 4);
  TEST ("a2.capacity()", a2.capacity(), 10);
  CoolAssociation<int,char*> a3 = a2;
  TEST("CoolAssociation<int,char*> a3 = a2", 1, 1);
  TEST ("a3.length()", a3.length(), 4);
  TEST ("a3.capacity()", a3.capacity(), 10);
  TEST ("a2 == a3", a2 == a3, TRUE);
  TEST("a3 != a1", (a3 != a1), TRUE);
  TEST("a3 = a1", (a3 = a1), a1);
  TEST("a2.find(3)", a2.find(3), TRUE);
  TEST("a2.value()", (strcmp (a2.value(),"CCC")), 0);
  TEST("a2.prev()", (a2.prev() && !strcmp(a2.value(),"BBB")), TRUE);
  TEST("a2.key()", a2.key(), 2);
  TEST("a2.next()", (a2.next() && !strcmp(a2.value(),"CCC")), TRUE);
  TEST("a2.get(1, value)", (a2.get(1, value) && !strcmp(value,"AAA")), TRUE);
  TEST("a2.get_key(\"DDD\", key)", (a2.get_key("DDD", key) && key==4), TRUE);
  TEST("a2.length()", a2.length(), 4);
  TEST("a2.put(5,\"EEE\")", (a2.put(5,"EEE") && a2.length() == 5), TRUE);
  a2.reset();
  TEST("a2.reset()", a2.prev(), TRUE);
  TEST("a2.find(2)", a2.find(2), TRUE);
  TEST("a2.remove()", strcmp (a2.remove(),"BBB"),0);
  TEST("a2.remove(4)", a2.remove(4), TRUE);
  TEST("a2.remove(4)", a2.remove(4), FALSE);
  a1.resize(10);
  TEST("a1.resize(10)", 1, 1);
  a0.set_length(2);
  TEST("a0.set_length(2)", 1, 1);
  a3.set_growth_ratio(2.0);
  TEST("a3.set_growth_ratio", 1, 1);
}

void xx(CoolPair<int,double>); //##

void test_int_double () {
  double a = 1.0;
  double b = 2.0;
  double c = 3.0;
  double d = 4.0;
  double e = 5.0;
 
  int key;
  double value;

  CoolAssociation<int,double> a0;
  TEST("CoolAssociation<int,double> a0", 1, 1);
  TEST ("a0.length()", a0.length(), 0);
  TEST ("a0.capacity()", a0.capacity(), 0);
  CoolAssociation<int,double> a1(5);
  TEST("CoolAssociation<int,double> a1(5)", 1, 1);
  TEST ("a1.length()", a1.length(), 0);
  TEST ("a1.capacity()", a1.capacity(), 5);
  CoolAssociation<int,double> a2(10);
  TEST ("CoolAssociation<int,double> a2(10)",1,1);
  TEST ("a2.capacity()",a2.capacity(),10);
  TEST ("a2.put(1,1.0)",a2.put(1,1.0), TRUE);
  TEST ("a2.put(2,2.0)",a2.put(2,2.0), TRUE);
  TEST ("a2.put(3,3.0)",a2.put(3,3.0), TRUE);
  TEST ("a2.put(4,4.0)",a2.put(4,4.0), TRUE);
  TEST ("a2.length()", a2.length(), 4);
  TEST ("a2.capacity()", a2.capacity(), 10);
  CoolAssociation<int,double> a3 = a2;
  TEST("CoolAssociation<int,double> a3 = a2", 1, 1);
  TEST ("a3.length()", a3.length(), 4);
  TEST ("a3.capacity()", a3.capacity(), 10);
  TEST ("a2 == a3", a2 == a3, TRUE);
  TEST("a3 != a1", (a3 != a1), TRUE);
  TEST("a3 = a1", (a3 = a1), a1);
  TEST("a2.find(3)", a2.find(3), TRUE);
  TEST("a2.value()", (a2.value() == c), TRUE);
  TEST("a2.prev()", (a2.prev() && a2.value() == b), TRUE);
  TEST("a2.key()", a2.key(), 2);
  TEST("a2.next()", (a2.next() && a2.value() == c), TRUE);
  TEST("a2.get(1, value)", (a2.get(1, value) && value==a), TRUE);
  TEST("a2.get_key(d, key)", (a2.get_key(d, key) && key==4), TRUE);
  TEST("a2.length()", a2.length(), 4);
  TEST("a2.put(5,e)", (a2.put(5,e) && a2.length() == 5), TRUE);
  a2.reset();
  TEST("a2.reset()", a2.prev(), TRUE);
  TEST("a2.find(2)", a2.find(2), TRUE);
  TEST("a2.remove()", a2.remove(), b);
  TEST("a2.remove(4)", a2.remove(4), TRUE);
  a1.resize(10);
  TEST("a1.resize(10)", 1, 1);
  a0.set_length(2);
  TEST("a0.set_length(2)", 1, 1);
  a3.set_growth_ratio(2.0);
  TEST("a3.set_growth_ratio", 1, 1);
}

void xx(CoolPair<int,CoolString>); //##

void test_int_String () {
  CoolString a = "AAA";
  CoolString b = "BBB";
  CoolString c = "CCC";
  CoolString d = "DDD";
  CoolString e = "EEE";
 
  int key;
  CoolString value;

  CoolAssociation<int,CoolString> a0;
  TEST("CoolAssociation<int,CoolString> a0", 1, 1);
  TEST ("a0.length()", a0.length(), 0);
  TEST ("a0.capacity()", a0.capacity(), 0);
  CoolAssociation<int,CoolString> a1(5);
  TEST("CoolAssociation<int,CoolString> a1(5)", 1, 1);
  TEST ("a1.length()", a1.length(), 0);
  TEST ("a1.capacity()", a1.capacity(), 5);
  CoolAssociation<int,CoolString> a2(10);
  TEST ("CoolAssociation<int,CoolString> a2(10)",1,1);
  TEST ("a2.capacity()",a2.capacity(),10);
  TEST ("a2.put(1,CoolString(\"AAA\"))",a2.put(1,CoolString("AAA")), TRUE);
  TEST ("a2.put(2,CoolString(\"BBB\"))",a2.put(2,CoolString("BBB")), TRUE);
  TEST ("a2.put(3,CoolString(\"CCC\"))",a2.put(3,CoolString("CCC")), TRUE);
  TEST ("a2.put(4,CoolString(\"DDD\"))",a2.put(4,CoolString("DDD")), TRUE);
  TEST ("a2.length()", a2.length(), 4);
  TEST ("a2.capacity()", a2.capacity(), 10);
  CoolAssociation<int,CoolString> a3 = a2;
  TEST("CoolAssociation<int,CoolString> a3 = a2", 1, 1);
  TEST ("a3.length()", a3.length(), 4);
  TEST ("a3.capacity()", a3.capacity(), 10);
  TEST ("a2 == a3", a2 == a3, TRUE);
  TEST("a3 != a1", (a3 != a1), TRUE);
  TEST("a3 = a1", (a3 = a1), a1);
  TEST("a2.find(3)", a2.find(3), TRUE);
  TEST("a2.value()", (a2.value() == c), TRUE);
  TEST("a2.prev()", (a2.prev() && a2.value() == b), TRUE);
  TEST("a2.key()", a2.key(), 2);
  TEST("a2.next()", (a2.next() && a2.value() == c), TRUE);
  TEST("a2.get(1, value)", (a2.get(1, value) && value==a), TRUE);
  TEST("a2.get_key(d, key)", (a2.get_key(d, key) && key==4), TRUE);
  TEST("a2.length()", a2.length(), 4);
  TEST("a2.put(5,e)", (a2.put(5,e) && a2.length() == 5), TRUE);
  a2.reset();
  TEST("a2.reset()", a2.prev(), TRUE);
  TEST("a2.find(2)", a2.find(2), TRUE);
  TEST("a2.remove()", a2.remove(), b);
  TEST("a2.remove(4)", a2.remove(4), TRUE);
  a1.resize(10);
  TEST("a1.resize(10)", 1, 1);
  a0.set_length(2);
  TEST("a0.set_length(2)", 1, 1);
  a3.set_growth_ratio(2.0);
  TEST("a3.set_growth_ratio", 1, 1);
  cout << a2 << endl;
}

void test_leak () {
  for (;;) {
    test_int_double ();
    test_int_charP ();
    test_int_String ();
  }
}

int main () {
  START("CoolAssociation");
  test_int_double ();
  test_int_charP ();
  test_int_String ();
#if LEAK
  test_leak ();
#endif
  SUMMARY();
  return 0;
}



