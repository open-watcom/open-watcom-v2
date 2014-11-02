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
#include <cool/Stack.h>
#include <cool/Stack.C>
#include <cool/test.h>
 
typedef char* charP;

Boolean my_compare_charP (const charP& s1, const charP& s2) {
  return (strcmp (s1, s2) ? FALSE : TRUE);
}

void test_int () {
  CoolStack<int> s0;
  TEST("CoolStack<int> s0", 1, 1);
  CoolStack<int> s1(4);
  TEST("CoolStack<int> s1", 1, 1);
  CoolStack<int> s2(4);
  s1.pushn(7, 4);
  TEST("s1.push(7, 4)", (s1[3]==7 && s1[2]==7 && s1[1]==7 && s1[0]==7), TRUE);
  TEST("s1.length()", s1.length(), 4);
  s2.push(7);
  TEST("s2.push(7)", (s2[0]==7), TRUE);
  s2.push(7);
  TEST("s2.push(7)", (s2[0]==7), TRUE);
  s2.pushn(7, 2);
  TEST("s2.pushn(7, 2)", 1, 1);
  TEST("s1.operator==(s2)", s1.operator==(s2), TRUE);
  TEST("s2.operator!=(s1)", s2.operator!=(s1), FALSE);
  s2.pop();
  TEST("s2.pop()", (s2.length()==3 && s2[0]==7), TRUE);
  s2.popn(2);
  TEST("s2.popn(2)", (s2.length()==1 && s2[0]==7), TRUE);
  s2.pushn(1, 3);  
  TEST("s2.pushn(1, 3)", 1, 1);
  TEST("s1.operator=(s2)", s1.operator=(s2), s2);
  TEST("s1.top()", (s1.top() == 1), TRUE);
  TEST("s2.position(7)", s2.position(7), 3);
  TEST("s1.find(33)", s1.find(33), FALSE);
  TEST("s2.operator[](3)", s2.operator[](3), 7);
  s2.clear();
  TEST("s2.clear()", 1, 1);
  TEST("s2.is_empty()", s2.is_empty(), TRUE);
  s2.resize(8);
  TEST("s2.resize(8)", 1, 1);
  TEST("s1.pushn(4, 8)", s1.pushn(4, 8), TRUE);
  TEST("s1.popn(13)", s1.popn(13), 7);
  s1.set_length(10);
  TEST("s1.set_length(10)", s1.length(), 10);
  s1.set_growth_ratio(2.0);
  TEST("s1.set_growth_ratio(2.0)", 1, 1);
  s1.set_alloc_size(50);
  TEST("s1.set_alloc_size(50)", 1, 1);
  CoolStack<int> s3(s1);
  TEST("CoolStack<int> s3 = s1", (s3==s1), TRUE);
}

void test_charP() {
  CoolStack<char*> s0;
  TEST("CoolStack<char*> s0", 1, 1);
  TEST("s0.length()", s0.length(), 0);
  CoolStack<char*> s1(4);
  TEST("CoolStack<char*> s1(4)", s1.capacity(), 4);
  CoolStack<char*> s2(4);
  TEST("CoolStack<char*> s2(4)", s1.capacity(), 4);
  s1.set_compare(&my_compare_charP);
  TEST ("s1.set_compare(&my_compare_charP)", 1,1);
  s1.pushn("AAA", 4);
  TEST("s1.pushn(\"AAA\", 4)", ((strcmp (s1[3],"AAA")==0) && (strcmp (s1[2],"AAA")==0) && (strcmp (s1[1],"AAA")==0) && (strcmp (s1[0],"AAA")==0)), 1);
  TEST("s1.length()", s1.length(), 4);
  s2.push("AAA");
  TEST("s2.push(\"AAA\")", (strcmp (s2[0],"AAA")==0), 1);
  TEST("s2.length()", s2.length(), 1);
  s2.push("AAA");
  TEST("s2.push(\"AAA\")", (strcmp (s2[0],"AAA")==0), 1);
  TEST("s2.length()", s2.length(), 2);
  s2.pushn("AAA", 2);
  TEST("s2.pushn(\"AAA\", 2)", 1, 1);
  TEST("s2.length()", s2.length(), 4);
  TEST("s1.operator==(s2)", s1.operator==(s2), TRUE);
  TEST("s2.operator!=(s1)", s2.operator!=(s1), FALSE);
  s2.pop();
  TEST("s2.pop()", (s2.length()==3 && (strcmp (s2[0], "AAA")==0)), 1);
  s2.popn(2);
  TEST("s2.popn(2)", (s2.length()==1 && (strcmp (s2[0], "AAA")==0)), 1);
  s2.pushn("BBB", 3);  
  TEST("s2.pushn(\"BBB\", 3)", 1, 1);
  TEST("s1.operator=(s2)", s1.operator=(s2), s2);
  TEST("s1.top()", (strcmp(s1.top(),"BBB")), 0);
  TEST("s2.position(\"AAA\")", s2.position("AAA"), 3);
  TEST("s1.find(\"ZZZ\")", s1.find("ZZZ"), FALSE);
  TEST("s2.operator[](3)", (strcmp (s2.operator[](3), "AAA")), 0);
  s2.clear();
  TEST("s2.clear()", 1, 1);
  TEST ("s2.length()", s2.length(), 0);
  TEST("s2.is_empty()", s2.is_empty(), TRUE);
  s2.resize(8);
  TEST("s2.resize(8)", 1, 1);
  TEST ("s2.capacity()", s2.capacity(), 8);
  TEST("s1.pushn(\"CCC\", 8)", s1.pushn("CCC", 8), TRUE);
  TEST("s1.popn(13)", strcmp (s1.popn(13), "AAA"), 0);
  s1.set_length(10);
  TEST("s1.set_length(10)", s1.length(), 10);
  s1.set_growth_ratio(2.0);
  TEST("s1.set_growth_ratio(2.0)", 1, 1);
  s1.set_alloc_size(50);
  TEST("s1.set_alloc_size(50)", 1, 1);
  CoolStack<char*> s3(s1);
  TEST("CoolStack<char*> s3 = s1", (s3==s1), TRUE);
}

void test_String () {
  CoolString S1("AAA");
  CoolStack<CoolString> s0;
  TEST("CoolStack<CoolString> s0", 1, 1);
  TEST("s0.length()", s0.length(), 0);
  CoolStack<CoolString> s1(4);
  TEST("CoolStack<CoolString> s1(4)", s1.capacity(), 4);
  CoolStack<CoolString> s2(4);
  TEST("CoolStack<CoolString> s2(4)", s1.capacity(), 4);
  s1.pushn(CoolString("AAA"), 4);
  TEST("s1.pushn(CoolString(\"AAA\"), 4)", (s1[3]==S1 && s1[2]==S1 && s1[1]==S1 && s1[0]==S1), TRUE);
  TEST("s1.length()", s1.length(), 4);
  s2.push(CoolString("AAA"));
  TEST("s2.push(CoolString(\"AAA\"))", s2[0]==S1, TRUE);
  TEST("s2.length()", s2.length(), 1);
  s2.push(CoolString("AAA"));
  TEST("s2.push(CoolString(\"AAA\"))", s2[0]==S1, TRUE);
  TEST("s2.length()", s2.length(), 2);
  s2.pushn(CoolString("AAA"), 2);
  TEST("s2.pushn(CoolString(\"AAA\"), 2)", 1, 1);
  TEST("s2.length()", s2.length(), 4);
  TEST("s1.operator==(s2)", s1.operator==(s2), TRUE);
  TEST("s2.operator!=(s1)", s2.operator!=(s1), FALSE);
  s2.pop();
  TEST("s2.pop()", (s2.length()==3 && s2[0] == S1), 1);
  s2.popn(2);
  TEST("s2.popn(2)", (s2.length()==1 && s2[0] == S1), 1);
  s2.pushn(CoolString("BBB"), 3);  
  TEST("s2.pushn(CoolString(\"BBB\"), 3)", 1, 1);
  TEST("s1.operator=(s2)", s1.operator=(s2), s2);
  TEST("s1.top()", (strcmp(s1.top(),CoolString("BBB"))), 0);
  TEST("s2.position(CoolString(\"AAA\"))", s2.position(CoolString("AAA")), 3);
  TEST("s1.find(CoolString(\"ZZZ\"))", s1.find(CoolString("ZZZ")), FALSE);
  TEST("s2.operator[](3)", (strcmp (s2.operator[](3), CoolString("AAA"))), 0);
  s2.clear();
  TEST("s2.clear()", 1, 1);
  TEST ("s2.length()", s2.length(), 0);
  TEST("s2.is_empty()", s2.is_empty(), TRUE);
  s2.resize(8);
  TEST("s2.resize(8)", 1, 1);
  TEST ("s2.capacity()", s2.capacity(), 8);
  TEST("s1.pushn(CoolString(\"CCC\"), 8)", s1.pushn(CoolString("CCC"), 8), TRUE);
  TEST("s1.popn(13)", strcmp (s1.popn(13), CoolString("AAA")), 0);
  s1.set_length(10);
  TEST("s1.set_length(10)", s1.length(), 10);
  s1.set_growth_ratio(2.0);
  TEST("s1.set_growth_ratio(2.0)", 1, 1);
  s1.set_alloc_size(50);
  TEST("s1.set_alloc_size(50)", 1, 1);
  CoolStack<CoolString> s3(s1);
  TEST("CoolStack<CoolString> s3 = s1", (s3==s1), TRUE);
}

void test_leak () {
  for (;;) {
  test_int();
  test_charP();
  test_String();
  }
}

int main (void) {
  START("CoolStack");
  test_int();
  test_charP();
  test_String();
#if LEAK
  test_leak();
#endif
  SUMMARY();
  return 0;
}

