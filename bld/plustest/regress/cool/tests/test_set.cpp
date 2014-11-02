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

#include <cool/char.h>          // for strcmp
#include <cool/Set.h>
#include <cool/Set.C>
#include <cool/test.h>

void test_int () {
  CoolSet<int> s0;
  TEST ("CoolSet<int> s0",s0.get_bucket_count(),3);
  TEST ("s0.capacity()", s0.capacity(), 24);
  CoolSet<int> s1(20);
  TEST ("CoolSet<int> s1(20)",s1.get_bucket_count(),3);
  TEST ("s1.capacity()", s1.capacity(), 24);
  CoolSet<int> s2(25);
  TEST ("CoolSet<int> s2(25)",s2.get_bucket_count(),7);
  TEST ("s2.capacity()", s2.capacity(), 56);
  CoolSet<int> s3(s2);
  TEST ("CoolSet<int> s3(s2)",s3.get_bucket_count(),7);
  TEST ("s2==s3", (s2==s3), TRUE);
  TEST ("s2 == s3", (s2 == s3), TRUE);
  TEST ("s1 == s2", (s1 == s2), TRUE);
  TEST ("s1 != s3", (s1 != s3), FALSE);
  TEST ("s0.reset()",(s0.reset(),1),1);
  TEST ("s0.put(1)", s0.put(1),TRUE);
  TEST ("s0.find(1)", s0.find(1), TRUE);
  TEST ("s0.value()", s0.value(), 1);
  TEST ("s0.length()", s0.length(), 1);
  TEST ("s0.put(2)", s0.put(2),TRUE);
  TEST ("s0.find(2)", s0.find(2), TRUE);
  TEST ("s0.put(3)", s0.put(3),TRUE);
  TEST ("s0.find(3)", s0.find(3), TRUE);
  TEST ("s0.put(4)", s0.put(4), TRUE);
  TEST ("s0.value()", s0.value(), 4);
  TEST ("s0.find(4)", s0.find(4), TRUE);
  TEST ("s0.value()", s0.value(), 4);
  TEST ("s0.put(5)", s0.put(5),TRUE);
  TEST ("s0.find(5)", s0.find(5), TRUE);
  TEST ("s0.put(6)", s0.put(6),TRUE);
  TEST ("s0.find(6)", s0.find(6), TRUE);
  TEST ("s0.put(7)", s0.put(7),TRUE);
  TEST ("s0.find(7)", s0.find(7), TRUE);
  TEST ("s0.put(8)", s0.put(8),TRUE);
  TEST ("s0.find(8)", s0.find(8), TRUE);
  TEST ("s0.put(9)", s0.put(9),TRUE);
  TEST ("s0.find(9)", s0.find(9), TRUE);
  TEST ("s0.get_bucket_count()", s0.get_bucket_count(), 3);
  TEST ("s0.length()", s0.length(), 9);
  TEST ("s0.remove(1)", s0.remove(1), TRUE);
  TEST ("s0.length()", s0.length(), 8);
  TEST ("s0.reset()",(s0.reset(),1),1);
  TEST ("s0.next()",s0.next(),TRUE);
  TEST ("s0.next()",s0.next(),TRUE);
  TEST ("s0.next()",s0.next(),TRUE);
  TEST ("s0.next()",s0.next(),TRUE);
  TEST ("s0.next()",s0.next(),TRUE);
  TEST ("s0.next()",s0.next(),TRUE);
  TEST ("s0.next()",s0.next(),TRUE);
  TEST ("s0.next()",s0.next(),TRUE);
  TEST ("s0.next()",s0.next(),FALSE);
  TEST ("s0.prev()",s0.prev(),TRUE);
  TEST ("s0.prev()",s0.prev(),TRUE);
  TEST ("s0.prev()",s0.prev(),TRUE);
  TEST ("s0.prev()",s0.prev(),TRUE);
  TEST ("s0.prev()",s0.prev(),TRUE);
  TEST ("s0.prev()",s0.prev(),TRUE);
  TEST ("s0.prev()",s0.prev(),TRUE);
  TEST ("s0.prev()",s0.prev(),TRUE);
  TEST ("s0.prev()",s0.prev(),FALSE);
  TEST ("s1=s0", (s1=s0,(s0==s1)), TRUE);
  TEST ("s1.resize(30)", s1.resize(30), TRUE);
  TEST ("s1.find(7)", s1.find(7), TRUE);
  TEST ("s1.value()", s1.value(), 7);
  TEST ("s1.find(3)", s1.find(3), TRUE);
  TEST ("s1.value()", s1.value(), 3);
  TEST ("s1.find(9)", s1.find(9), TRUE);
  TEST ("s1.value()", s1.value(), 9);
  TEST ("s1.get_bucket_count()", s1.get_bucket_count(), 7);
  TEST ("s1.length()", s1.length(), 8);
  TEST ("s0.set_ratio(1.0)", (s0.set_ratio(1.0),1), 1);
  TEST ("s0.resize(30)", s0.resize(30), TRUE);
  TEST ("s0.get_bucket_count()", s0.get_bucket_count(), 7);
  TEST ("s0.find(2)", s0.find(2), TRUE);
  TEST ("s0.find(4)", s0.find(4), TRUE);
  TEST ("s0.find(6)", s0.find(6), TRUE);
  TEST ("s0.find(8)", s0.find(8), TRUE);
  TEST ("s0.resize(60)", s0.resize(60), TRUE);
  TEST ("s0.get_bucket_count()", s0.get_bucket_count(), 19);
  TEST ("s0.find(2)", s0.find(2), TRUE);
  TEST ("s0.value()", s0.value(), 2);
  TEST ("s0.find(4)", s0.find(4), TRUE);
  TEST ("s0.value()", s0.value(), 4);
  TEST ("s0.find(6)", s0.find(6), TRUE);
  TEST ("s0.value()", s0.value(), 6);
  TEST ("s0.find(8)", s0.find(8), TRUE);
  TEST ("s0.value()", s0.value(), 8);
  TEST ("s0.remove()", s0.remove(), TRUE);
  TEST ("s0.find(8)", s0.find(8), FALSE);
  CoolSet<int> s4(s1);
  TEST ("CoolSet<int> S4(s1)", (s4==s1), TRUE);
  TEST ("s4.remove(6)", s4.remove(6), TRUE);
  TEST ("s1.search(s4)", s1.search(s4), TRUE);
  TEST ("s0.clear()", (s0.clear(), s0.length()), 0);
  TEST ("s1.clear()", (s1.clear(), s1.length()), 0);
}

void test_int_2 () {
  CoolSet<int> s0;
  CoolSet<int> s1;
  CoolSet<int> s2;
  CoolSet<int> s3;
  CoolSet<int> s4;
  CoolSet<int> s5; 
  CoolSet<int> s6;
  CoolSet<int> s7;
  int i;
  for (i = 0; i < 10; i++) 
    s0.put(i);
  s0.remove(8);
  for (i = 2; i < 10; i++)
    s1.put(i);
  s1.remove(4);
  s2.put(0);
  s2.put(1);
  s2.put(4);
  s3.put(8);
  s4 = s0;
  s4.put(8);
  s5 = s1;
  s5.remove(8);
  s6.put(0);
  s6.put(1);
  s6.put(4);
  s6.put(8);
//   cout << s0 << "-" << s1 << "=" << (s0-s1) << endl;
//   cout << s0 << "|" << s1 << "=" << (s0|s1) << endl;
//   cout << s0 << "&" << s1 << "=" << (s0&s1) << endl;
//   cout << s0 << "^" << s1 << "=" << (s0^s1) << endl;

  TEST ("(s0 - s1) == s2", ((s0-s1) == s2), TRUE);
  TEST ("(s1 - s0) == s3", ((s1-s0) == s3), TRUE);
  TEST ("(s0 | s1) == s4", ((s0|s1) == s4), TRUE);
  TEST ("(s1 | s0) == s4", ((s1|s0) == s4), TRUE);
  TEST ("(s0 & s1) == s5", ((s0&s1) == s5), TRUE);
  TEST ("(s1 & s0) == s5", ((s1&s0) == s5), TRUE);
  
  TEST ("(s0 ^ s1) == s6", ((s0^s1) == s6), TRUE);
  TEST ("(s1 ^ s0) == s6", ((s1^s0) == s6), TRUE);
  s7 = s0;
  TEST ("s0 -= s1", (s0 -= s1, s0 == s2), TRUE);
  s0 = s7; s7=s1;
  TEST ("s1 -= s0", (s1 -= s0, s1 == s3), TRUE);
  s1 = s7; s7=s0;
  TEST ("s0 |= s1", (s0 |= s1, s0 == s4), TRUE);
  s0 = s7; s7=s1;
  TEST ("s1 |= s0", (s1 |= s0, s1 == s4), TRUE);
  s1 = s7; s7=s0;
  TEST ("s0 &= s1", (s0 &= s1, s0 == s5), TRUE);
  s0 = s7; s7=s1;
  TEST ("s1 &= s0", (s1 &= s0, s1 == s5), TRUE);
  s1 = s7; s7=s0;
  TEST ("s0 ^= s1", (s0 ^= s1, s0 == s6), TRUE);
  s0 = s7; s7=s1;
  TEST ("s1 ^= s0", (s1 ^= s0, s1 == s6), TRUE);
  s1 = s7; s7 = s0;
  TEST ("s0.set_difference(s1)", (s0.set_difference(s1), s0 == s2), TRUE);
  s0 = s7;
  TEST ("s0.set_union(s1)", (s0.set_union(s1), s0 == s4), TRUE);
  s0 = s7;
  TEST ("s0.set_intersection(s1)", (s0.set_intersection(s1), s0 == s5), TRUE);
  s0 = s7;
  TEST ("s0.set_xor(s1)", (s0.set_xor(s1), s0 == s6), TRUE);
  s0 = s7;
  s0.reset(), s1.reset();
  // Create sets for intersection, union, difference, and xor
  CoolSet<int> sint;
  CoolSet<int> sunion;
  CoolSet<int> sdif;
  CoolSet<int> sxor;
  {sint.put(2);}
  {sint.put(7);}
  {sint.put(9);}
  {sint.put(6);}
  {sint.put(5);}
  {sint.put(3);}
  {sdif.put(1);}
  {sdif.put(4);}
  {sdif.put(0);}
  {sunion.put(6);}
  {sunion.put(1);}
  {sunion.put(3);}
  {sunion.put(2);}
  {sunion.put(8);}
  {sunion.put(0);}
  {sunion.put(5);}
  {sunion.put(7);}
  {sunion.put(4);}
  {sunion.put(9);}
  {sxor.put(1);}
  {sxor.put(8);}
  {sxor.put(4);}
  {sxor.put(0);}

  CoolSet<int> ltemp1;
  CoolSet<int> ltemp2;
  CoolSet<int> ltemp3;
  CoolSet<int> ltemp4;

  s0.reset(), s1.reset();
  TEST_RUN ("s0.next_intersection(s1)", 
            while (s0.next_intersection(s1)) ltemp1.put(s0.value()),
            ltemp1 == sint, TRUE);
  s0.reset(), s1.reset();
  TEST_RUN ("s0.next_difference(s1)", 
            while (s0.next_difference(s1)) ltemp2.put(s0.value()),
            ltemp2 == sdif, TRUE);
  s0.reset(), s1.reset();
  TEST_RUN ("s0.next_union(s1)", 
            while (s0.next_union(s1)) ltemp3.put(s0.value()),
            ltemp3 == sunion, TRUE);
  s0.reset(), s1.reset();
  TEST_RUN ("s0.next_xor(s1)", 
            while (s0.next_xor(s1)) ltemp4.put(s0.value()),
            ltemp4 == sxor, TRUE);
}

void test_double () {
  CoolSet<double> d0;
  TEST ("CoolSet<double> d0",d0.get_bucket_count(),3);
  TEST ("d0.capacity()", d0.capacity(), 24);
  CoolSet<double> d1(20);
  TEST ("CoolSet<double> d1(20)",d1.get_bucket_count(),3);
  TEST ("d1.capacity()", d1.capacity(), 24);
  CoolSet<double> d2(25);
  TEST ("CoolSet<double> d2(25)",d2.get_bucket_count(),7);
  TEST ("d2.capacity()", d2.capacity(), 56);
  CoolSet<double> d3(d2);
  TEST ("CoolSet<double> d3(d2)",d3.get_bucket_count(),7);
  TEST ("d2==d3", (d2==d3), TRUE);
  TEST ("d2 == d3", (d2 == d3), TRUE);
  TEST ("d1 == d2", (d1 == d2), TRUE);
  TEST ("d1 != d3", (d1 != d3), FALSE);
  TEST ("d0.reset()",(d0.reset(),1.0),1.0);
  TEST ("d0.put(1.0)", d0.put(1.0),TRUE);
  TEST ("d0.find(1.0)", d0.find(1.0), TRUE);
  TEST ("d0.value()", d0.value(), 1.0);
  TEST ("d0.length()", d0.length(), 1.0);
  TEST ("d0.put(2.0)", d0.put(2.0),TRUE);
  TEST ("d0.find(2.0)", d0.find(2.0), TRUE);
  TEST ("d0.put(3.0)", d0.put(3.0),TRUE);
  TEST ("d0.find(3.0)", d0.find(3.0), TRUE);
  TEST ("d0.put(4.0)", d0.put(4.0), TRUE);
  TEST ("d0.value()", d0.value(), 4.0);
  TEST ("d0.find(4.0)", d0.find(4.0), TRUE);
  TEST ("d0.value()", d0.value(), 4.0);
  TEST ("d0.put(5.0)", d0.put(5.0),TRUE);
  TEST ("d0.find(5.0)", d0.find(5.0), TRUE);
  TEST ("d0.put(6.0)", d0.put(6.0),TRUE);
  TEST ("d0.find(6.0)", d0.find(6.0), TRUE);
  TEST ("d0.put(7.0)", d0.put(7.0),TRUE);
  TEST ("d0.find(7.0)", d0.find(7.0), TRUE);
  TEST ("d0.put(8.0)", d0.put(8.0),TRUE);
  TEST ("d0.find(8.0)", d0.find(8.0), TRUE);
  TEST ("d0.put(9.0)", d0.put(9.0),TRUE);
  TEST ("d0.find(9.0)", d0.find(9.0), TRUE);
  TEST ("d0.get_bucket_count()", d0.get_bucket_count(), 3.0);
  TEST ("d0.length()", d0.length(), 9);
  TEST ("d0.remove(1.0)", d0.remove(1.0), TRUE);
  TEST ("d0.length()", d0.length(), 8);
  TEST ("d0.reset()",(d0.reset(),1.0),1.0);
  TEST ("d0.next()",d0.next(),TRUE);
  TEST ("d0.next()",d0.next(),TRUE);
  TEST ("d0.next()",d0.next(),TRUE);
  TEST ("d0.next()",d0.next(),TRUE);
  TEST ("d0.next()",d0.next(),TRUE);
  TEST ("d0.next()",d0.next(),TRUE);
  TEST ("d0.next()",d0.next(),TRUE);
  TEST ("d0.next()",d0.next(),TRUE);
  TEST ("d0.next()",d0.next(),FALSE);
  TEST ("d0.prev()",d0.prev(),TRUE);
  TEST ("d0.prev()",d0.prev(),TRUE);
  TEST ("d0.prev()",d0.prev(),TRUE);
  TEST ("d0.prev()",d0.prev(),TRUE);
  TEST ("d0.prev()",d0.prev(),TRUE);
  TEST ("d0.prev()",d0.prev(),TRUE);
  TEST ("d0.prev()",d0.prev(),TRUE);
  TEST ("d0.prev()",d0.prev(),TRUE);
  TEST ("d0.prev()",d0.prev(),FALSE);
  TEST ("d1=d0", (d1=d0,(d0==d1)), TRUE);
  TEST ("d1.resize(30)", d1.resize(30), TRUE);
  TEST ("d1.find(7.0)", d1.find(7.0), TRUE);
  TEST ("d1.value()", d1.value(), 7.0);
  TEST ("d1.find(3.0)", d1.find(3.0), TRUE);
  TEST ("d1.value()", d1.value(), 3.0);
  TEST ("d1.find(9.0)", d1.find(9.0), TRUE);
  TEST ("d1.value()", d1.value(), 9.0);
  TEST ("d1.get_bucket_count()", d1.get_bucket_count(), 7);
  TEST ("d1.length()", d1.length(), 8);
  TEST ("d0.set_ratio(1.0)", (d0.set_ratio(1.0),1.0), 1.0);
  TEST ("d0.resize(30)", d0.resize(30), TRUE);
  TEST ("d0.get_bucket_count()", d0.get_bucket_count(), 7);
  TEST ("d0.find(2.0)", d0.find(2.0), TRUE);
  TEST ("d0.find(4.0)", d0.find(4.0), TRUE);
  TEST ("d0.find(6.0)", d0.find(6.0), TRUE);
  TEST ("d0.find(8.0)", d0.find(8.0), TRUE);
  TEST ("d0.resize(60)", d0.resize(60), TRUE);
  TEST ("d0.get_bucket_count()", d0.get_bucket_count(), 19);
  TEST ("d0.find(2.0)", d0.find(2.0), TRUE);
  TEST ("d0.value()", d0.value(), 2.0);
  TEST ("d0.find(4.0)", d0.find(4.0), TRUE);
  TEST ("d0.value()", d0.value(), 4.0);
  TEST ("d0.find(6.0)", d0.find(6.0), TRUE);
  TEST ("d0.value()", d0.value(), 6.0);
  TEST ("d0.find(8.0)", d0.find(8.0), TRUE);
  TEST ("d0.value()", d0.value(), 8.0);
  TEST ("d0.remove()", d0.remove(), TRUE);
  TEST ("d0.find(8.0)", d0.find(8.0), FALSE);
  CoolSet<double> d4(d1);
  TEST ("CoolSet<double> D4(d1)", (d4==d1), TRUE);
  TEST ("d4.remove(6.0)", d4.remove(6.0), TRUE);
  TEST ("d1.search(d4)", d1.search(d4), TRUE);
  TEST ("d0.clear()", (d0.clear(), d0.length()), 0);
  TEST ("d1.clear()", (d1.clear(), d1.length()), 0);
}

void test_double_2 () {
  CoolSet<double> d0;
  CoolSet<double> d1;
  CoolSet<double> d2;
  CoolSet<double> d3;
  CoolSet<double> d4;
  CoolSet<double> d5; 
  CoolSet<double> d6;
  CoolSet<double> d7;
  double i;
  for (i = 0.0; i < 10.0; i++) 
    d0.put(i);
  d0.remove(8.0);
  for (i = 2.0; i < 10.0; i++)
    d1.put(i);
  d1.remove(4.0);
  d2.put(0.0);
  d2.put(1.0);
  d2.put(4.0);
  d3.put(8.0);
  d4 = d0;
  d4.put(8.0);
  d5 = d1;
  d5.remove(8.0);
  d6.put(0.0);
  d6.put(1.0);
  d6.put(4.0);
  d6.put(8.0);
  TEST ("(d0 - d1) == d2", ((d0-d1) == d2), TRUE);
  TEST ("(d1 - d0) == d3", ((d1-d0) == d3), TRUE);
  TEST ("(d0 | d1) == d4", ((d0|d1) == d4), TRUE);
  TEST ("(d1 | d0) == d4", ((d1|d0) == d4), TRUE);
  TEST ("(d0 & d1) == d5", ((d0&d1) == d5), TRUE);
  TEST ("(d1 & d0) == d5", ((d1&d0) == d5), TRUE);
  TEST ("(d0 ^ d1) == d6", ((d0^d1) == d6), TRUE);
  TEST ("(d1 ^ d0) == d6", ((d1^d0) == d6), TRUE);
  d7 = d0;
  TEST ("d0 -= d1", (d0 -= d1, d0 == d2), TRUE);
  d0 = d7; d7=d1;
  TEST ("d1 -= d0", (d1 -= d0, d1 == d3), TRUE);
  d1 = d7; d7=d0;
  TEST ("d0 |= d1", (d0 |= d1, d0 == d4), TRUE);
  d0 = d7; d7=d1;
  TEST ("d1 |= d0", (d1 |= d0, d1 == d4), TRUE);
  d1 = d7; d7=d0;
  TEST ("d0 &= d1", (d0 &= d1, d0 == d5), TRUE);
  d0 = d7; d7=d1;
  TEST ("d1 &= d0", (d1 &= d0, d1 == d5), TRUE);
  d1 = d7; d7=d0;
  TEST ("d0 ^= d1", (d0 ^= d1, d0 == d6), TRUE);
  d0 = d7; d7=d1;
  TEST ("d1 ^= d0", (d1 ^= d0, d1 == d6), TRUE);
  d1 = d7; d7 = d0;
  TEST ("d0.set_difference(d1)", (d0.set_difference(d1), d0 == d2), TRUE);
  d0 = d7;
  TEST ("d0.set_union(d1)", (d0.set_union(d1), d0 == d4), TRUE);
  d0 = d7;
  TEST ("d0.set_intersection(d1)", (d0.set_intersection(d1), d0 == d5), TRUE);
  d0 = d7;
  TEST ("d0.set_xor(d1)", (d0.set_xor(d1), d0 == d6), TRUE);
  d0 = d7;
  d0.reset(), d1.reset();
  // Create sets containing the expected intersection, union, difference, and xor
  CoolSet<double> dint;
  CoolSet<double> dunion;
  CoolSet<double> ddif;
  CoolSet<double> dxor;

  {dint.put(9);}
  {dint.put(7);}
  {dint.put(5);}
  {dint.put(3);}
  {dint.put(6);}
  {dint.put(2);}
  {ddif.put(1);}
  {ddif.put(0);}
  {ddif.put(4);}
  {dunion.put(1);}
  {dunion.put(7);}
  {dunion.put(2);}
  {dunion.put(3);}
  {dunion.put(0);}
  {dunion.put(5);}
  {dunion.put(8);}
  {dunion.put(6);}
  {dunion.put(4);}
  {dunion.put(9);}
  {dxor.put(1);}
  {dxor.put(8);}
  {dxor.put(4);}
  {dxor.put(0);}

  CoolSet<double> ltemp1;
  CoolSet<double> ltemp2;
  CoolSet<double> ltemp3;
  CoolSet<double> ltemp4;

  d0.reset(), d1.reset();
  TEST_RUN ("d0.next_intersection(d1)", 
            while (d0.next_intersection(d1)) ltemp1.put(d0.value()),
            ltemp1 == dint, TRUE);
  d0.reset(), d1.reset();
  TEST_RUN ("d0.next_difference(d1)", 
            while (d0.next_difference(d1)) ltemp2.put(d0.value()),
            ltemp2 == ddif, TRUE);
  d0.reset(), d1.reset();
  TEST_RUN ("d0.next_union(d1)", 
            while (d0.next_union(d1)) ltemp3.put(d0.value()),
            ltemp3 == dunion, TRUE);
  d0.reset(), d1.reset();
  TEST_RUN ("d0.next_xor(d1)", 
            while (d0.next_xor(d1)) ltemp4.put(d0.value()),
            ltemp4 == dxor, TRUE);
}

void test_charP () {
  CoolSet<char*> color1;
  TEST ("CoolSet<char*> color1",color1.get_bucket_count(),3);
  TEST ("color1.capacity()", color1.capacity(), 24);
  CoolSet<char*> color2(20);
  TEST ("CoolSet<char*> color2(20)",color2.get_bucket_count(),3);
  TEST ("color2.capacity()", color2.capacity(), 24);
  CoolSet<char*> color3(25);
  TEST ("CoolSet<char*> color3(25)",color3.get_bucket_count(),7);
  TEST ("color3.capacity()", color3.capacity(), 56);
  CoolSet<char*> color4(color3);
  TEST ("CoolSet<char*> color4(color3)",color4.get_bucket_count(),7);
  TEST ("color3==color4", (color3==color4), TRUE);
  TEST ("color3 == color4", (color3 == color4), TRUE);
  TEST ("color2 == color3", (color2 == color3), TRUE);
  TEST ("color2 != color4", (color2 != color4), FALSE);
  TEST ("color1.reset()",(color1.reset(),1.0),1.0);
  TEST ("color1.put(\"RED\")", color1.put("RED"),TRUE);
  TEST ("color1.find(\"RED\")", color1.find("RED"), TRUE);
  TEST ("color1.value()", strcmp (color1.value(), "RED"), 0);
  TEST ("color1.length()", color1.length(), 1.0);
  TEST ("color1.put(\"YELLOW\")", color1.put("YELLOW"),TRUE);
  TEST ("color1.find(\"YELLOW\")", color1.find("YELLOW"), TRUE);
  TEST ("color1.put(\"MAGENTA\")", color1.put("MAGENTA"),TRUE);
  TEST ("color1.find(\"MAGENTA\")", color1.find("MAGENTA"), TRUE);
  TEST ("color1.put(\"BLUE\")", color1.put("BLUE"), TRUE);
  TEST ("color1.value()", strcmp (color1.value(), "BLUE"), 0);
  TEST ("color1.find(\"BLUE\")", color1.find("BLUE"), TRUE);
  TEST ("color1.value()", strcmp (color1.value(), "BLUE"), 0);
  TEST ("color1.put(\"ORANGE\")", color1.put("ORANGE"),TRUE);
  TEST ("color1.find(\"ORANGE\")", color1.find("ORANGE"), TRUE);
  TEST ("color1.put(\"WHITE\")", color1.put("WHITE"),TRUE);
  TEST ("color1.find(\"WHITE\")", color1.find("WHITE"), TRUE);
  TEST ("color1.put(\"BROWN\")", color1.put("BROWN"),TRUE);
  TEST ("color1.find(\"BROWN\")", color1.find("BROWN"), TRUE);
  TEST ("color1.put(\"BLACK\")", color1.put("BLACK"),TRUE);
  TEST ("color1.find(\"BLACK\")", color1.find("BLACK"), TRUE);
  TEST ("color1.put(\"GREY\")", color1.put("GREY"),TRUE);
  TEST ("color1.find(\"GREY\")", color1.find("GREY"), TRUE);
  TEST ("color1.get_bucket_count()", color1.get_bucket_count(), 3);
  TEST ("color1.length()", color1.length(), 9);
  TEST ("color1.remove(\"RED\")", color1.remove("RED"), TRUE);
  TEST ("color1.length()", color1.length(), 8);
  TEST ("color1.reset()",(color1.reset(),1.0),1.0);
  TEST ("color1.next()",color1.next(),TRUE);
  TEST ("color1.next()",color1.next(),TRUE);
  TEST ("color1.next()",color1.next(),TRUE);
  TEST ("color1.next()",color1.next(),TRUE);
  TEST ("color1.next()",color1.next(),TRUE);
  TEST ("color1.next()",color1.next(),TRUE);
  TEST ("color1.next()",color1.next(),TRUE);
  TEST ("color1.next()",color1.next(),TRUE);
  TEST ("color1.next()",color1.next(),FALSE);
  TEST ("color1.prev()",color1.prev(),TRUE);
  TEST ("color1.prev()",color1.prev(),TRUE);
  TEST ("color1.prev()",color1.prev(),TRUE);
  TEST ("color1.prev()",color1.prev(),TRUE);
  TEST ("color1.prev()",color1.prev(),TRUE);
  TEST ("color1.prev()",color1.prev(),TRUE);
  TEST ("color1.prev()",color1.prev(),TRUE);
  TEST ("color1.prev()",color1.prev(),TRUE);
  TEST ("color1.prev()",color1.prev(),FALSE);
  TEST ("color2=color1", (color2=color1,(color1==color2)), TRUE);
  TEST ("color2.resize(30)", color2.resize(30), TRUE);
  TEST ("color2.find(\"BROWN\")", color2.find("BROWN"), TRUE);
  TEST ("color2.value()", strcmp (color2.value(), "BROWN"), 0);
  TEST ("color2.find(\"MAGENTA\")", color2.find("MAGENTA"), TRUE);
  TEST ("color2.value()", strcmp (color2.value(), "MAGENTA"), 0);
  TEST ("color2.find(\"GREY\")", color2.find("GREY"), TRUE);
  TEST ("color2.value()", strcmp (color2.value(), "GREY"), 0);
  TEST ("color2.get_bucket_count()", color2.get_bucket_count(), 7);
  TEST ("color2.length()", color2.length(), 8);
  TEST ("color1.set_ratio(1.0)", (color1.set_ratio(1.0),1.0), 1.0);
  TEST ("color1.resize(30)", color1.resize(30), TRUE);
  TEST ("color1.get_bucket_count()", color1.get_bucket_count(), 7);
  TEST ("color1.find(\"YELLOW\")", color1.find("YELLOW"), TRUE);
  TEST ("color1.find(\"BLUE\")", color1.find("BLUE"), TRUE);
  TEST ("color1.find(\"WHITE\")", color1.find("WHITE"), TRUE);
  TEST ("color1.find(\"BLACK\")", color1.find("BLACK"), TRUE);
  TEST ("color1.resize(60)", color1.resize(60), TRUE);
  TEST ("color1.get_bucket_count()", color1.get_bucket_count(), 19);
  TEST ("color1.find(\"YELLOW\")", color1.find("YELLOW"), TRUE);
  TEST ("color1.value()", strcmp (color1.value(), "YELLOW"), 0);
  TEST ("color1.find(\"BLUE\")", color1.find("BLUE"), TRUE);
  TEST ("color1.value()", strcmp (color1.value(), "BLUE"), 0);
  TEST ("color1.find(\"WHITE\")", color1.find("WHITE"), TRUE);
  TEST ("color1.value()", strcmp (color1.value(), "WHITE"), 0);
  TEST ("color1.find(\"BLACK\")", color1.find("BLACK"), TRUE);
  TEST ("color1.value()", strcmp (color1.value(), "BLACK"), 0);
  TEST ("color1.remove()", color1.remove(), TRUE);
  TEST ("color1.find(\"BLACK\")", color1.find("BLACK"), FALSE);
  CoolSet<char*> color5(color2);
  TEST ("CoolSet<char*> Color5(color2)", (color5==color2), TRUE);
  TEST ("color5.remove(\"WHITE\")", color5.remove("WHITE"), TRUE);
  TEST ("color2.search(color5)", color2.search(color5), TRUE);
  TEST ("color1.clear()", (color1.clear(), color1.length()), 0);
  TEST ("color2.clear()", (color2.clear(), color2.length()), 0);
}

void test_charP_2 () {
  CoolSet<char*> color0;
  CoolSet<char*> color1;
  CoolSet<char*> color2;
  CoolSet<char*> color3;
  CoolSet<char*> color4;
  CoolSet<char*> color5; 
  CoolSet<char*> color6;
  CoolSet<char*> color7;
  color0.put("RED");
  color0.put("BLUE");
  color0.put("GREEN");
  color0.put("BLACK");
  color0.put("PURPLE");
  color0.put("GREY");
  color0.put("WHITE");
  color0.put("YELLOW");
  color0.put("ORANGE");
  color1.put("GREEN");
  color1.put("BLACK");
  color1.put("GREY");
  color1.put("WHITE");
  color1.put("ORANGE");
  color1.put("YELLOW");
  color1.put("MAGENTA");
  color1.put("PURPLE");
  color2.put("RED");
  color2.put("BLUE");
  color3.put("MAGENTA");
  color4 = color0;
  color4.put("MAGENTA");
  color5 = color1;
  color5.remove("MAGENTA");
  color6.put("RED");
  color6.put("BLUE");
  color6.put("MAGENTA");
  TEST ("(color0 - color1) == color2", ((color0-color1) == color2), TRUE);
  TEST ("(color1 - color0) == color3", ((color1-color0) == color3), TRUE);
  TEST ("(color0 | color1) == color4", ((color0|color1) == color4), TRUE);
  TEST ("(color1 | color0) == color4", ((color1|color0) == color4), TRUE);
  TEST ("(color0 & color1) == color5", ((color0&color1) == color5), TRUE);
  TEST ("(color1 & color0) == color5", ((color1&color0) == color5), TRUE);
  TEST ("(color0 ^ color1) == color6", ((color0^color1) == color6), TRUE);
  TEST ("(color1 ^ color0) == color6", ((color1^color0) == color6), TRUE);
  color7 = color0;
  TEST ("color0 -= color1", (color0 -= color1, color0 == color2), TRUE);
  color0 = color7; color7=color1;
  TEST ("color1 -= color0", (color1 -= color0, color1 == color3), TRUE);
  color1 = color7; color7=color0;
  TEST ("color0 |= color1", (color0 |= color1, color0 == color4), TRUE);
  color0 = color7; color7=color1;
  TEST ("color1 |= color0", (color1 |= color0, color1 == color4), TRUE);
  color1 = color7; color7=color0;
  TEST ("color0 &= color1", (color0 &= color1, color0 == color5), TRUE);
  color0 = color7; color7=color1;
  TEST ("color1 &= color0", (color1 &= color0, color1 == color5), TRUE);
  color1 = color7; color7=color0;
  TEST ("color0 ^= color1", (color0 ^= color1, color0 == color6), TRUE);
  color0 = color7; color7=color1;
  TEST ("color1 ^= color0", (color1 ^= color0, color1 == color6), TRUE);
  color1 = color7; color7 = color0;
  TEST ("color0.set_difference(color1)", (color0.set_difference(color1), color0 == color2), TRUE);
  color0 = color7;
  TEST ("color0.set_union(color1)", (color0.set_union(color1), color0 == color4), TRUE);
  color0 = color7;
  TEST ("color0.set_intersection(color1)", (color0.set_intersection(color1), color0 == color5), TRUE);
  color0 = color7;
  TEST ("color0.set_xor(color1)", (color0.set_xor(color1), color0 == color6), TRUE);
  color0 = color7;
  color0.reset(), color1.reset();
  TEST ("color0.next_intersection(color1)", color0.next_intersection(color1), TRUE);
  TEST ("color0.value()", strcmp(color0.value(),"BLACK"),0);
  TEST ("color0.next_intersection(color1)", color0.next_intersection(color1), TRUE);
  TEST ("color0.value()", strcmp(color0.value(),"PURPLE"),0);
  TEST ("color0.next_intersection(color1)", color0.next_intersection(color1), TRUE);
  TEST ("color0.value()", strcmp(color0.value(),"WHITE"),0);
  TEST ("color0.next_intersection(color1)", color0.next_intersection(color1), TRUE);
  TEST ("color0.value()", strcmp(color0.value(),"YELLOW"),0);
  TEST ("color0.next_intersection(color1)", color0.next_intersection(color1), TRUE);
  TEST ("color0.value()", strcmp(color0.value(),"GREY"),0);
  TEST ("color0.next_intersection(color1)", color0.next_intersection(color1), TRUE);
  TEST ("color0.value()", strcmp(color0.value(),"ORANGE"),0);
  TEST ("color0.next_intersection(color1)", color0.next_intersection(color1), TRUE);
  TEST ("color0.value()", strcmp(color0.value(),"GREEN"),0);
  TEST ("color0.next_intersection(color1)", color0.next_intersection(color1), FALSE);
  color0.reset(), color1.reset();
  TEST ("color0.next_difference(color1)", color0.next_difference(color1), TRUE);
  TEST ("color0.value()", strcmp(color0.value(),"RED"),0);
  TEST ("color0.next_difference(color1)", color0.next_difference(color1), TRUE);
  TEST ("color0.value()", strcmp(color0.value(),"BLUE"),0);
  TEST ("color0.next_difference(color1)", color0.next_difference(color1), FALSE);
  color0.reset(), color1.reset();
  TEST ("color0.next_union(color1)", color0.next_union(color1), TRUE);
  TEST ("color0.value()", strcmp(color0.value(),"RED"),0);
  TEST ("color0.next_union(color1)", color0.next_union(color1), TRUE);
  TEST ("color0.value()", strcmp(color0.value(),"BLUE"),0);
  TEST ("color0.next_union(color1)", color0.next_union(color1), TRUE);
  TEST ("color0.value()", strcmp(color0.value(),"BLACK"),0);
  TEST ("color0.next_union(color1)", color0.next_union(color1), TRUE);
  TEST ("color0.value()", strcmp(color0.value(),"PURPLE"),0);
  TEST ("color0.next_union(color1)", color0.next_union(color1), TRUE);
  TEST ("color0.value()", strcmp(color0.value(),"WHITE"),0);
  TEST ("color0.next_union(color1)", color0.next_union(color1), TRUE);
  TEST ("color0.value()", strcmp(color0.value(),"YELLOW"),0);
  TEST ("color0.next_union(color1)", color0.next_union(color1), TRUE);
  TEST ("color0.value()", strcmp(color0.value(),"GREY"),0);
  TEST ("color0.next_union(color1)", color0.next_union(color1), TRUE);
  TEST ("color0.value()", strcmp(color0.value(),"ORANGE"),0);
  TEST ("color0.next_union(color1)", color0.next_union(color1), TRUE);
  TEST ("color0.value()", strcmp(color0.value(),"GREEN"),0);
  TEST ("color0.next_union(color1)", color0.next_union(color1), TRUE);
  TEST ("color0.value()", strcmp(color0.value(),"MAGENTA"),0);
  TEST ("color0.next_union(color1)", color0.next_union(color1), FALSE);
  color0.reset(), color1.reset();
  TEST ("color0.next_xor(color1)", color0.next_xor(color1), TRUE);
  TEST ("color0.value()", strcmp(color0.value(),"RED"),0);
  TEST ("color0.next_xor(color1)", color0.next_xor(color1), TRUE);
  TEST ("color0.value()", strcmp(color0.value(),"BLUE"),0);
  TEST ("color0.next_xor(color1)", color0.next_xor(color1), TRUE);
  TEST ("color0.value()", strcmp(color0.value(),"MAGENTA"),0);
  TEST ("color0.next_xor(color1)", color0.next_xor(color1), FALSE);
}


void test_leak () {
  for (;;) {
    test_int ();
    test_int_2 ();
    test_double ();
    test_double_2 ();
    test_charP ();
    test_charP_2 ();
  }
}

int main (void) {
  START("CoolSet");
  test_int ();
  test_int_2 ();
  test_double ();
  test_double_2 ();
  test_charP ();
  test_charP_2 ();
#if LEAK
  test_leak();
#endif
  SUMMARY();
  return 0;
}

