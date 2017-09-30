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
// Updated: JAM 08/18/92 -- modernize template syntax, remove macro hacks

#include <cool/String.h>
#include <cool/Vector.h>
#include <cool/Hash_Table.h>
#include <cool/test.h>

#include <cool/Vector.C>
#include <cool/Hash_Table.C>

// get and put pass values by reference.  Cfront creates stack
// temporaries for these values, which creates too many
// nesting levels for the Microsoft C compiler.

void test_int_double_get_put(CoolHash_Table<int,double>& h0) {
  int i;
  double d;
  static int c1=1, c2=2, c3=3, c4=4, c5=5, c6=6, c7=7, c8=8, c9=9;
  TEST ("h0.reset()",(h0.reset(),c1),c1);
  TEST ("h0.put(1,2.5)", h0.put(c1,2.5),TRUE);
  TEST ("h0.value()", h0.value(), 2.5);
  TEST ("h0.get(1,d)", (h0.get(c1,d) == TRUE && d == 2.5), TRUE);
  TEST ("h0.length()", h0.length(), c1);
  TEST ("h0.value()", h0.value(), 2.5);
  TEST ("h0.put(2,3.5)", h0.put(c2,3.5),TRUE);
  TEST ("h0.get(2,d)", (h0.get(c2,d) == TRUE && d == 3.5), TRUE);
  TEST ("h0.put(3,4.5)", h0.put(c3,4.5),TRUE);
  TEST ("h0.get(3,d)", (h0.get(c3,d) == TRUE && d == 4.5), TRUE);
  TEST ("h0.put(4,5.5)", h0.put(c4,5.5),TRUE); 
  TEST ("h0.value()", h0.value(), 5.5);
  TEST ("h0.get(4,d)", (h0.get(c4,d) == TRUE && d == 5.5), TRUE);
  TEST ("h0.value()", h0.value(), 5.5);
  TEST ("h0.put(5,9.5)", h0.put(c5,6.5),TRUE);
  TEST ("h0.get(5,d)", (h0.get(c5,d) == TRUE && d == 6.5), TRUE);
  TEST ("h0.value()", h0.value(), 6.5);
  TEST ("h0.put(6,7.5)", h0.put(c6,7.5),TRUE);
  TEST ("h0.get(6,d)", (h0.get(c6,d) == TRUE && d == 7.5), TRUE);
  TEST ("h0.get_key(7.5,i)", h0.get_key(7.5,i), TRUE);
  TEST ("i == 6", (i == c6), 1);
  TEST ("h0.put(7,8.5)", h0.put(c7,8.5),TRUE);
  TEST ("h0.get(7,d)", (h0.get(c7,d) == TRUE && d == 8.5), TRUE);
  TEST ("h0.put(8,9.5)", h0.put(c8,9.5),TRUE);
  TEST ("h0.get(8,d)", (h0.get(c8,d) == TRUE && d == 9.5), TRUE);
  TEST ("h0.put(9,10.5)", h0.put(c9,10.5),TRUE);
  TEST ("h0.get(9,d)", (h0.get(c9,d) == TRUE && d == 10.5), TRUE);
  TEST ("h0.get_bucket_count()", h0.get_bucket_count(), 3);
  TEST ("h0.length()", h0.length(), 9);
  TEST ("h0.remove(1)", h0.remove(1), TRUE);
  TEST ("h0.length()", h0.length(), 8);
}

void test_int_double_next(CoolHash_Table<int,double>& h0) {
  TEST ("h0.reset()",(h0.reset(),1),1);
  TEST ("h0.next()",h0.next(),TRUE);
  TEST ("h0.next()",h0.next(),TRUE);
  TEST ("h0.next()",h0.next(),TRUE);
  TEST ("h0.next()",h0.next(),TRUE);
  TEST ("h0.next()",h0.next(),TRUE);
  TEST ("h0.next()",h0.next(),TRUE);
  TEST ("h0.next()",h0.next(),TRUE);
  TEST ("h0.next()",h0.next(),TRUE);
  TEST ("h0.next()",h0.next(),FALSE);
  TEST ("h0.prev()",h0.prev(),TRUE);
  TEST ("h0.prev()",h0.prev(),TRUE);
  TEST ("h0.prev()",h0.prev(),TRUE);
  TEST ("h0.prev()",h0.prev(),TRUE);
  TEST ("h0.prev()",h0.prev(),TRUE);
  TEST ("h0.prev()",h0.prev(),TRUE);
  TEST ("h0.prev()",h0.prev(),TRUE);
  TEST ("h0.prev()",h0.prev(),TRUE);
  TEST ("h0.prev()",h0.prev(),FALSE);
}

void test_int_double () {
  int i;
  double d;
  // get and put pass values by reference.  Cfront creates stack
  // temporaries for these values, which creates too many
  // nesting levels for the Microsoft C compiler.
  static int c2=2, c3=3, c4=4, c6=6, c7=7, c8=8, c9=9;
  CoolHash_Table<int,double> h0;
  TEST ("CoolHash_Table<int,double> h0",h0.get_bucket_count(),3);
  CoolHash_Table<int,double> h1(20);
  TEST ("CoolHash_Table<int,double> h1(20)",h1.get_bucket_count(),3);
  CoolHash_Table<int,double> h2(25);
  TEST ("CoolHash_Table<int,double> h2(25)",h2.get_bucket_count(),7);
  CoolHash_Table<int,double> h3(h2);
  TEST ("CoolHash_Table<int,double> h3(h2)",h3.get_bucket_count(),7);
  TEST ("h2==h3", (h2==h3), TRUE);
  TEST ("h2 == h3", (h2 == h3), TRUE);
  TEST ("h1 == h2", (h1 == h2), TRUE);
  TEST ("h1 != h3", (h1 != h3), FALSE);
  test_int_double_get_put(h0);
  test_int_double_next(h0);
  TEST ("h1=h0", (h1=h0,(h0==h1)), TRUE);
  h1.resize(30); //  TEST ("h1.resize(30)", h1.resize(30), TRUE);
  TEST ("h1.get(7,d)", (h1.get(c7,d) == TRUE && d == 8.5), TRUE);
  TEST ("h1.value()", h1.value(), 8.5);
  TEST ("h1.get(3,d)", (h1.get(c3,d) == TRUE && d == 4.5), TRUE);
  TEST ("h1.value()", h1.value(), 4.5);
  TEST ("h1.get(9,d)", (h1.get(c9,d) == TRUE && d == 10.5), TRUE);
  TEST ("h1.value()", h1.value(), 10.5);
  TEST ("h1.get_bucket_count()", h1.get_bucket_count(), 7);
  TEST ("h1.length()", h1.length(), 8);
  TEST ("h0.set_ratio(1.0)", (h0.set_ratio(1.0),1), 1);
  h0.resize(30); //  TEST ("h0.resize(30)", h0.resize(30), TRUE);
  TEST ("h0.get_bucket_count()", h0.get_bucket_count(), 7);
  TEST ("h0.get(2,d)", (h0.get(c2,d) == TRUE && d == 3.5), TRUE);
  TEST ("h0.get(4,d)", (h0.get(c4,d) == TRUE && d == 5.5), TRUE);
  TEST ("h0.get(6,d)", (h0.get(c6,d) == TRUE && d == 7.5), TRUE);
  TEST ("h0.get(8,d)", (h0.get(c8,d) == TRUE && d == 9.5), TRUE);
  h0.resize(60); // TEST ("h0.resize(60)", h0.resize(60), TRUE);
  TEST ("h0.get_bucket_count()", h0.get_bucket_count(), 19);
  TEST ("h0.get(2,d)", (h0.get(c2,d) == TRUE && d == 3.5), TRUE);
  TEST ("h0.value()", h0.value(), 3.5);
  TEST ("h0.get(4,d)", (h0.get(c4,d) == TRUE && d == 5.5), TRUE);
  TEST ("h0.value()", h0.value(), 5.5);
  TEST ("h0.get(6,d)", (h0.get(c6,d) == TRUE && d == 7.5), TRUE);
  TEST ("h0.value()", h0.value(), 7.5);
  TEST ("h0.get(8,d)", (h0.get(c8,d) == TRUE && d == 9.5), TRUE);
  TEST ("h0.get_key(9.5,i)", h0.get_key(9.5,i), TRUE);
  TEST ("i == 8", (i == 8), 1);
  TEST ("h0.value()", h0.value(), 9.5);
  TEST ("h0.clear()", (h0.clear(), h0.length()), 0);
  TEST ("h1.clear()", (h1.clear(), h1.length()), 0);
  TEST ("h3.clear()", (h3.clear(), h3.length()), 0);
  TEST ("for(i=0;i<100;i++) h3.put(i,double(i))",1,1);
  for(i = 0; i < 100; i++)
     h3.put(i, (double) i);
  TEST ("for(i=0;i<100;i++) h3.find(i)",1,1);
  for(i = 0; i < 100; i++)
    if(h3.find(i) == FALSE) {
      TEST ("for(i=0;i<100;i++) h3.find(i)",h3.find(i),TRUE);
      break;
    }
}


void test_double_String () {
  CoolString s;
  // get and put pass values by reference.  Cfront creates stack
  // temporaries for these values, which creates too many
  // nesting levels for the Microsoft C compiler.
  static double d1=1, d2=2, d3=3, d4=4, d5=5, d6=6, d7=7, d8=8, d9=9;
  CoolHash_Table<double,CoolString> h0;
  TEST ("CoolHash_Table<double,CoolString> h0",h0.get_bucket_count(),3);
  CoolHash_Table<double,CoolString> h1(20);
  TEST ("CoolHash_Table<double,CoolString> h1(20)",h1.get_bucket_count(),3);
  CoolHash_Table<double,CoolString> h2(25);
  TEST ("CoolHash_Table<double,CoolString> h2(25)",h2.get_bucket_count(),7);
  CoolHash_Table<double,CoolString> h3(h2);
  TEST ("CoolHash_Table<double,CoolString> h3(h2)",h3.get_bucket_count(),7);
  TEST ("h2==h3",(h2==h3), TRUE);
  TEST ("h2 == h3", (h2 == h3), TRUE);
  TEST ("h1 == h2", (h1 == h2), TRUE);
  TEST ("h1 != h3", (h1 != h3), FALSE);
  TEST ("h0.put(1.0,CoolString(\"ABCD\"))", h0.put(d1,CoolString("ABCD")),TRUE);
  TEST("h0.get(1.0,s)",(h0.get(d1,s)==TRUE && strcmp(s,"ABCD")==0),TRUE);
  TEST ("h0.length()", h0.length(), 1);
  TEST ("h0.put(2.0,CoolString(\"EFGH\"))", h0.put(d2,CoolString("EFGH")),TRUE);
  TEST("h0.get(2.0,s)",(h0.get(d2,s)==TRUE && strcmp(s,"EFGH")==0),TRUE);
  TEST ("h0.put(3.0,CoolString(\"IJKL\"))", h0.put(d3,CoolString("IJKL")),TRUE);
  TEST("h0.get(3.0,s)",(h0.get(d3,s)==TRUE && strcmp(s,"IJKL")==0),TRUE);
  TEST ("h0.put(4.0,CoolString(\"MNOP\"))", h0.put(d4,CoolString("MNOP")),TRUE);
  TEST("h0.get(4.0,s)",(h0.get(d4,s)==TRUE && strcmp(s,"MNOP")==0),TRUE);
  TEST ("h0.put(5.0,CoolString(\"QRsT\"))", h0.put(d5,CoolString("QRST")),TRUE);
  TEST("h0.get(5.0,s)",(h0.get(d5,s)==TRUE && strcmp(s,"QRST")==0),TRUE);
  TEST ("h0.put(6.0,CoolString(\"UVWX\"))", h0.put(d6,CoolString("UVWX")),TRUE);
  TEST("h0.get(6.0,s)",(h0.get(d6,s)==TRUE && strcmp(s,"UVWX")==0),TRUE);
  // TEST ("h0.get_key(\"QRST\",d)", h0.get_key("QRST",d), TRUE);
  // TEST ("d == 5.0", (d == 5.0), 1);
  TEST ("h0.put(7.0,CoolString(\"YZab\"))", h0.put(d7,CoolString("YZab")),TRUE);
  TEST("h0.get(7.0,s)",(h0.get(d7,s)==TRUE && strcmp(s,"YZab")==0),TRUE);
  TEST ("h0.put(8.0,CoolString(\"cdef\"))", h0.put(d8,CoolString("cdef")),TRUE);
  TEST("h0.get(8.0,s)",(h0.get(d8,s)==TRUE && strcmp(s,"cdef")==0),TRUE);
  TEST ("h0.put(9.0,CoolString(\"ghij\"))", h0.put(d9,CoolString("ghij")),TRUE);
  TEST("h0.get(9.0,s)",(h0.get(d9,s)==TRUE && strcmp(s,"ghij")==0),TRUE);
  TEST ("h0.get_bucket_count()", h0.get_bucket_count(), 3);
  TEST ("h0.length()", h0.length(), 9);
  TEST ("h0.remove(1.0)", h0.remove(1.0), TRUE);
  TEST ("h0.length()", h0.length(), 8);
  TEST ("h1=h0", (h1=h0,(h0==h1)), TRUE);
  h1.resize(30); //  TEST ("h1.resize(30)", h1.resize(30), TRUE);
  TEST("h1.get(7.0,s)",(h1.get(d7,s)==TRUE && strcmp(s,"YZab")==0),TRUE);
  TEST("h1.get(3.0,s)",(h1.get(d3,s)==TRUE && strcmp(s,"IJKL")==0),TRUE);
  TEST("h1.get(9.0,s)",(h1.get(d9,s)==TRUE && strcmp(s,"ghij")==0),TRUE);
  TEST ("h1.get_bucket_count()", h1.get_bucket_count(), 7);
  TEST ("h1.length()", h1.length(), 8);
  TEST ("h0.set_ratio(1.0)", (h0.set_ratio(1.0),1), 1);
  h0.resize(30); //  TEST ("h0.resize(30)", h0.resize(30), TRUE);
  TEST ("h0.get_bucket_count()", h0.get_bucket_count(), 7);
  TEST("h0.get(2.0,s)",(h0.get(d2,s)==TRUE && strcmp(s,"EFGH")==0),TRUE);
  TEST("h0.get(4.0,s)",(h0.get(d4,s)==TRUE && strcmp(s,"MNOP")==0),TRUE);
  TEST("h0.get(6.0,s)",(h0.get(d6,s)==TRUE && strcmp(s,"UVWX")==0),TRUE);
  TEST("h0.get(8.0,s)",(h0.get(d8,s)==TRUE && strcmp(s,"cdef")==0),TRUE);
  h0.resize(60); //  TEST ("h0.resize(60)", h0.resize(60), TRUE);
  TEST ("h0.get_bucket_count()", h0.get_bucket_count(), 19);
  TEST("h0.get(2.0,s)",(h0.get(d2,s)==TRUE && strcmp(s,"EFGH")==0),TRUE);
  TEST("h0.get(4.0,s)",(h0.get(d4,s)==TRUE && strcmp(s,"MNOP")==0),TRUE);
  TEST("h0.get(6.0,s)",(h0.get(d6,s)==TRUE && strcmp(s,"UVWX")==0),TRUE);
  TEST("h0.get(8.0,s)",(h0.get(d8,s)==TRUE && strcmp(s,"cdef")==0),TRUE);
  // TEST ("h0.get_key(\"cdef\",d)", h0.get_key("cdef",d), TRUE);
  // TEST ("d == 8.0", (d == 8.0), 1);
  TEST ("h0.clear()", (h0.clear(), h0.length()), 0);
  TEST ("h1.clear()", (h1.clear(), h1.length()), 0);
}

template<class Type>
unsigned long CoolVector_hash(const CoolVector<Type>& key) {
   return key.length(); //##
}

unsigned long CoolHash_Table_default_hash (const CoolVector<int>& key) {
   return key.length(); //##
}

void test_Vector_int_String () {
  CoolString s;
  CoolString s1("Hello World");
  TEST ("CoolString s1(\"Hello World\")",0,0);
  CoolString s2("Up, up, and away!");
  TEST ("CoolString s2(\"Up, up, and away!\")",0,0);
  CoolVector<int> v1(5, 5, 1, 1, 1, 1, 1);
  TEST ("CoolVector<int> v1(5,1)",(v1[0]==1 && v1[1]==1 && v1[2]==1 && v1[3]==1 && v1[4]==1),1);
  CoolVector<int> v2(8);
  TEST ("CoolVector<int> v2(8)",0,0);
  v2.set_length(8);
  TEST ("v2.set_length(8)", v2.length(), 8);
  for (int i = 0; i < 8; i++)
    v2[i] = i;
  CoolHash_Table<CoolVector<int>,CoolString> h1;
  TEST ("CoolHash_Table<CoolVector<int>,CoolString> h1",h1.get_bucket_count(),3);
  TEST ("h1.put(v1,s1)", h1.put(v1,s1), TRUE);
  TEST ("h1.get(v1,s)", (h1.get(v1,s)==TRUE && strcmp (s,s1)==0), TRUE);
  TEST ("h1.length()", h1.length(), 1);
  TEST ("h1.put(v2,s2)", h1.put(v2,s2), TRUE);
  TEST ("h1.put(v2,s2) again", h1.put(v2,s2), FALSE);
  TEST ("h1.get(v2,s)", (h1.get(v2,s)==TRUE && strcmp (s,s2)==0), TRUE);
  TEST ("h1.length()", h1.length(), 2);
}

void test_leak() {
  for (;;) {
    test_int_double ();
    test_double_String  ();
    test_Vector_int_String ();
  }
}


int main (void) {
  START("CoolHash_Table");
  test_int_double ();
  test_double_String  ();
  test_Vector_int_String ();
#if LEAK
  test_leak ();
#endif
  SUMMARY();
  return 0;
}
