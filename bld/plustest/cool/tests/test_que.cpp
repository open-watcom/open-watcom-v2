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
#include <cool/Queue.h>
#include <cool/Queue.C>
#include <cool/test.h>

typedef char* charP;
Boolean my_compare_charP (const charP& s1, const charP& s2) {
  return (strcmp (s1, s2) ? FALSE : TRUE);
}

void test_int () {
  int foo;
  CoolQueue<int> q0;
  TEST("CoolQueue<int> q0", q0.capacity(), 0);
  CoolQueue<int> q1(5);
  TEST("CoolQueue<int> q1(5)", q1.capacity(), 5);
  CoolQueue<int> q2(10);
  TEST("CoolQueue<int> q2(10)", q2.capacity(), 10);
  TEST("q1.get(foo)", q1.get(foo), FALSE);   // false on an empty Queue
  TEST("q1.unget(4)", q1.unget(4), TRUE);
  TEST ("q1.length()", q1.length(), 1);
  TEST("q1.get()", q1.get(), 4);
  TEST ("q1.length()", q1.length(), 0);
  TEST("q2.put(4)", q2.put(4), TRUE);
  TEST("q2.unput()", q2.unput(), 4);
  TEST("q1.put(5)", q1.put(5), TRUE);
  TEST("q1.get()", q1.get(), 5);
  TEST("q1.put(3)", q1.put(3), TRUE);
  TEST("q1.unget(2)", q1.unget(2), TRUE);
  TEST("q1.put(4)", q1.put(4), TRUE);
  TEST ("q1.length()", q1.length(), 3);
  TEST("q1.unget(1)", q1.unget(1), TRUE);
  TEST("q1.put(5)", q1.put(5), TRUE);
  TEST ("q1.length()", q1.length(), 5);
  TEST("q1.look()", q1.look(), 1);
  TEST("q2.put(1)", q2.put(1), TRUE);
  TEST("q2.put(2)", q2.put(2), TRUE);
  TEST("q2.put(3)", q2.put(3), TRUE);
  TEST("q2.put(4)", q2.put(4), TRUE);
  TEST("q2.put(5)", q2.put(5), TRUE);
  TEST("q1.operator==(q2)", q1.operator==(q2), TRUE);
  TEST("q1.find(5)", q1.find(5), TRUE);
  TEST("q2.find(6)", q2.find(6), FALSE);
  TEST("q1.remove()", q1.remove(), TRUE);
  TEST ("q1.length()", q1.length(), 4);
  TEST("q1.prev()", q1.prev(), TRUE);
  TEST("q2.find(5)", q2.find(5), TRUE);
  TEST("q2.next()", !q2.next(), TRUE);
  TEST("q2.next()", q2.next(), TRUE);
  TEST("q2.value()", q2.value(), 1);
  TEST("q2.remove(3)", q2.remove(3), TRUE);
  TEST("q2.value()", q2.value(), 4);
  TEST("q1.operator=(q2)", q1.operator=(q2), q2);
  TEST("q2.operator!=(q1)", q2.operator!=(q1), FALSE);
  q2.reset();
  TEST("q2.reset()", 1, 1);
  TEST("q2.length()", q2.length(), 4);
  q1.clear();
  TEST("q1.clear()", 1, 1);
  TEST ("q1.length()", q1.length(), 0);
  TEST("q1.is_empty()", q1.is_empty(), TRUE);
  q1.resize(10);
  TEST("q1.resize(10)", 1, 1);
  TEST ("q1.length()", q1.length(), 0);
  TEST("q1.capacity()", q1.capacity(), 10);
  CoolQueue<int> q3 = q2;
  TEST("CoolQueue<int> q3 = q2", q3.operator==(q2), TRUE);
}


void test_double () {
  double foo;
  CoolQueue<double> q0;
  TEST("CoolQueue<double> q0", q0.capacity(), 0);
  CoolQueue<double> q1(5);
  TEST("CoolQueue<double> q1(5)", q1.capacity(), 5);
  CoolQueue<double> q2(10);
  TEST("CoolQueue<double> q2(10)", q2.capacity(), 10);
  TEST("q1.get(foo)", q1.get(foo), FALSE);   // false on an empty Queue
  TEST("q1.unget(4.0)", q1.unget(4.0), TRUE);
  TEST ("q1.length()", q1.length(), 1);
  TEST("q1.get()", q1.get(), 4.0);
  TEST ("q1.length()", q1.length(), 0);
  TEST("q2.put(4.0)", q2.put(4.0), TRUE);
  TEST("q2.unput()", q2.unput(), 4.0);
  TEST("q1.put(5.0)", q1.put(5.0), TRUE);
  TEST("q1.get()", q1.get(), 5.0);
  TEST("q1.put(3.0)", q1.put(3.0), TRUE);
  TEST("q1.unget(2.0)", q1.unget(2.0), TRUE);
  TEST("q1.put(4.0)", q1.put(4.0), TRUE);
  TEST ("q1.length()", q1.length(), 3);
  TEST("q1.unget(1.0)", q1.unget(1.0), TRUE);
  TEST("q1.put(5.0)", q1.put(5.0), TRUE);
  TEST ("q1.length()", q1.length(), 5);
  TEST("q1.look()", q1.look(), 1.0);
  TEST("q2.put(1)", q2.put(1.0), TRUE);
  TEST("q2.put(2)", q2.put(2.0), TRUE);
  TEST("q2.put(3)", q2.put(3.0), TRUE);
  TEST("q2.put(4)", q2.put(4.0), TRUE);
  TEST("q2.put(5)", q2.put(5.0), TRUE);
  TEST("q1.operator==(q2)", q1.operator==(q2), TRUE);
  TEST("q1.find(5.0)", q1.find(5.0), TRUE);
  TEST("q2.find(6.0)", q2.find(6.0), FALSE);
  TEST("q1.remove()", q1.remove(), TRUE);
  TEST ("q1.length()", q1.length(), 4);
  TEST("q1.prev()", q1.prev(), TRUE);
  TEST("q2.find(5.0)", q2.find(5.0), TRUE);
  TEST("q2.next()", !q2.next(), TRUE);
  TEST("q2.next()", q2.next(), TRUE);
  TEST("q2.value()", q2.value(), 1.0);
  TEST("q2.remove(3.0)", q2.remove(3.0), TRUE);
  TEST("q2.value()", q2.value(), 4.0);
  TEST("q1.operator=(q2)", q1.operator=(q2), q2);
  TEST("q2.operator!=(q1)", q2.operator!=(q1), FALSE);
  q2.reset();
  TEST("q2.reset()", 1, 1);
  TEST("q2.length()", q2.length(), 4);
  q1.clear();
  TEST("q1.clear()", 1, 1);
  TEST ("q1.length()", q1.length(), 0);
  TEST("q1.is_empty()", q1.is_empty(), TRUE);
  q1.resize(10);
  TEST("q1.resize(10)", 1, 1);
  TEST ("q1.length()", q1.length(), 0);
  TEST("q1.capacity()", q1.capacity(), 10);
  CoolQueue<double> q3 = q2;
  TEST("CoolQueue<double> q3 = q2", q3.operator==(q2), TRUE);
}


void test_charP () {
  char* foo;
  CoolQueue<char*> q0;
  TEST("CoolQueue<char*> q0", q0.capacity(), 0);
  CoolQueue<char*> q1(5);
  TEST("CoolQueue<char*> q1(5)", q1.capacity(), 5);
  CoolQueue<char*> q2(10);
  TEST("CoolQueue<char*> q2(10)", q2.capacity(), 10);
  TEST("q1.get(foo)", q1.get(foo), FALSE);   // false on empty Queue
  q1.set_compare(&my_compare_charP);
  TEST("q1.unget(\"AAA\")", q1.unget("AAA"), TRUE);
  TEST("q1.get()", strcmp (q1.get(), "AAA"), 0);
  TEST("q2.put(\"AAA\")", q2.put("AAA"), TRUE);
  TEST("q2.unput()", strcmp (q2.unput(), "AAA"), 0);
  TEST("q1.put(\"BBB\")", q1.put("BBB"), TRUE);
  TEST("q1.get()", strcmp (q1.get(), "BBB"), 0);
  TEST("q1.put(\"CCC\")", q1.put("CCC"), TRUE);
  TEST("q1.unget(\"DDD\")", q1.unget("DDD"), TRUE);
  TEST("q1.put(\"BBB\")", q1.put("BBB"), TRUE);
  TEST("q1.unget(\"EEE\")", q1.unget("EEE"), TRUE);
  TEST("q1.put(\"AAA\")", q1.put("AAA"), TRUE);
  TEST("q1.look()", strcmp (q1.look(), "EEE"), 0);
  TEST("q2.put(\"EEE\")", q2.put("EEE"), TRUE);
  TEST("q2.put(\"DDD\")", q2.put("DDD"), TRUE);
  TEST("q2.put(\"CCC\")", q2.put("CCC"), TRUE);
  TEST("q2.put(\"BBB\")", q2.put("BBB"), TRUE);
  TEST("q2.put(\"AAA\")", q2.put("AAA"), TRUE);
  TEST("q1.operator==(q2)", q1.operator==(q2), TRUE);
  TEST("q1.find(\"AAA\")", q1.find("AAA"), TRUE);
  TEST("q2.find(\"FFF\")", q2.find("FFF"), FALSE);
  TEST("q1.remove()", q1.remove(), TRUE);
  TEST("q1.prev()", q1.prev(), TRUE);
  TEST("q2.find(\"AAA\")", q2.find("AAA"), TRUE);
  TEST("q2.next()", !q2.next(), TRUE);
  TEST("q2.next()", q2.next(), TRUE);
  TEST("q2.value()", strcmp (q2.value(), "EEE"), 0);
// The following removed because a cfront 2.0 bug prevents it's compilation
#ifdef BUGGYCFRONT
  TEST("q2.remove(\"DDD\")", q2.remove("DDD"), TRUE);
  TEST("q2.value()", strcmp (q2.value(), "CCC"), 0);
  TEST("q2.length()", q2.length(), 4);
#endif
  TEST("q1.operator=(q2)", (q1.operator=(q2), q1==q2), TRUE);
  TEST("q2.operator!=(q1)", q2.operator!=(q1), FALSE);
  q2.reset();
  TEST("q2.reset()", 1, 1);
  q1.clear();
  TEST("q1.clear()", 1, 1);
  TEST("q1.is_empty()", q1.is_empty(), TRUE);
  q1.resize(10);
  TEST("q1.resize(10)", q1.capacity(), 10);
  TEST("q1.capacity()", q1.capacity(), 10);
  CoolQueue<char*> q3 = q2;
  TEST("CoolQueue<char*> q3 = q2", q3.operator==(q2), TRUE);
}


void test_String () {
  CoolQueue<CoolString> q0;
  TEST("CoolQueue<CoolString> q0", q0.capacity(), 0);
  CoolQueue<CoolString> q1(5);
  TEST("CoolQueue<CoolString> q1(5)", q1.capacity(), 5);
  CoolQueue<CoolString> q2(10);
  TEST("CoolQueue<CoolString> q2(10)", q2.capacity(), 10);
  //TEST("q1.get()", q1.get(), ERROR);   // should error on an empty Queue
  TEST("q1.unget(CoolString(\"AAA\"))", q1.unget(CoolString("AAA")), TRUE);
  TEST("q1.get()", strcmp (q1.get(), CoolString("AAA")), 0);
  TEST("q2.put(CoolString(\"AAA\"))", q2.put(CoolString("AAA")), TRUE);
  TEST("q2.unput()", strcmp (q2.unput(), CoolString("AAA")), 0);
  TEST("q1.put(CoolString(\"BBB\"))", q1.put(CoolString("BBB")), TRUE);
  TEST("q1.get()", strcmp (q1.get(), CoolString("BBB")), 0);
  TEST("q1.put(CoolString(\"CCC\"))", q1.put(CoolString("CCC")), TRUE);
  TEST("q1.unget(CoolString(\"DDD\"))", q1.unget(CoolString("DDD")), TRUE);
  TEST("q1.put(CoolString(\"BBB\"))", q1.put(CoolString("BBB")), TRUE);
  TEST("q1.unget(CoolString(\"EEE\"))", q1.unget(CoolString("EEE")), TRUE);
  TEST("q1.put(CoolString(\"AAA\"))", q1.put(CoolString("AAA")), TRUE);
  TEST("q1.look()", strcmp (q1.look(), CoolString("EEE")), 0);
  TEST("q2.put(CoolString(\"EEE\"))", q2.put(CoolString("EEE")), TRUE);
  TEST("q2.put(CoolString(\"DDD\"))", q2.put(CoolString("DDD")), TRUE);
  TEST("q2.put(CoolString(\"CCC\"))", q2.put(CoolString("CCC")), TRUE);
  TEST("q2.put(CoolString(\"BBB\"))", q2.put(CoolString("BBB")), TRUE);
  TEST("q2.put(CoolString(\"AAA\"))", q2.put(CoolString("AAA")), TRUE);
  TEST("q1.operator==(q2)", q1.operator==(q2), TRUE);
  TEST("q1.find(CoolString(\"AAA\"))", q1.find(CoolString("AAA")), TRUE);
  TEST("q2.find(\"FFF\")", q2.find("FFF"), FALSE);
  TEST("q1.remove()", q1.remove(), TRUE);
  TEST("q1.prev()", q1.prev(), TRUE);
  TEST("q2.find(CoolString(\"AAA\"))", q2.find(CoolString("AAA")), TRUE);
  TEST("q2.next()", !q2.next(), TRUE);
  TEST("q2.next()", q2.next(), TRUE);
  TEST("q2.value()", strcmp (q2.value(), CoolString("EEE")), 0);
  TEST("q2.remove(CoolString(\"DDD\"))", q2.remove(CoolString("DDD")), TRUE);
  TEST("q2.value()", strcmp (q2.value(), CoolString("CCC")), 0);
  TEST("q1.operator=(q2)", (q1.operator=(q2), q1==q2), TRUE);
  TEST("q2.operator!=(q1)", q2.operator!=(q1), FALSE);
  q2.reset();
  TEST("q2.reset()", 1, 1);
  TEST("q2.length()", q2.length(), 4);
  q1.clear();
  TEST("q1.clear()", 1, 1);
  TEST("q1.is_empty()", q1.is_empty(), TRUE);
  q1.resize(10);
  TEST("q1.resize(10)", q1.capacity(), 10);
  TEST("q1.capacity()", q1.capacity(), 10);
  CoolQueue<CoolString> q3 = q2;
  TEST("CoolQueue<CoolString> q3 = q2", q3.operator==(q2), TRUE);
}

void test_leak() {
  for (;;) {
    test_int();
    test_double();
    test_charP();
    test_String();
  }
}

int main (void) {
  START("CoolQueue");
  test_int();
  test_double();
  test_charP();
  test_String();
#if LEAK
  test_leak();
#endif
  SUMMARY();
  return 0;
}

