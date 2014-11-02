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
// Updated: JAM 08/12/92 -- modernize template syntax, remove macro hacks

#include <cool/String.h>
#include <cool/Vector.h>
#include <cool/test.h>

#include <cool/Vector.C>

Boolean my_compare_int (const int& t1, const int& t2) {
  return ((t1 < t2) ? -1 : 1);
}

Boolean my_compare_double (const double& t1, const double& t2) {
  return ((t1 < t2) ? -1 : 1);
}

void test_int1 () {  
  CoolVector<int> v0;
  TEST ("CoolVector<int> v0", 1, 1);
  CoolVector<int> v1(3);
  TEST ("CoolVector<int> v1(3)", 1, 1);
  CoolVector<int> v2(4, 4, 1, 1, 1, 1);
  TEST ("CoolVector<int> v2(4,1)",(v2[0]==1 && v2[1]==1 && v2[2]==1 && v2[3]==1),1);
  TEST_RUN ("v1 = v2",v1=v2,(v1[0]==1 && v1[1]==1 && v1[2]==1 && v1[3]==1),1);
  TEST_RUN ("v1==v2", v1=v2, (v1==v2), TRUE);
  TEST_RUN ("v1[0] = 2", v1[0] = 2, (v1[0] == 2), 1);
  TEST_RUN ("v0 = v1",v0=v1,(v0[0]==2 && v0[1]==1 && v0[2]==1 && v0[3]==1),1);
  CoolVector<int> v3 (5,3,2,3,1);
  TEST ("CoolVector<int> v3(5,3,2,3,1)",(v3[0]==2 && v3[1]==3 && v3[2]==1),1);
  CoolVector<int> v4 = v3;
  TEST ("CoolVector<int> v4 = v3", (v4[0]==2 && v4[1]==3 && v4[2]==1),1);
  TEST ("v1!=v2", (v1!=v2), TRUE);
  TEST ("v4==v3", (v4==v3), TRUE);
  TEST ("v1 != v3", ((v1 != v3) ? TRUE : FALSE), TRUE);
  CoolVector<int> v5(v4);
  TEST ("CoolVector<int> v5(v4)",  (v4==v5), TRUE);
  TEST ("v1.length()", v1.length(), 4);
//  v1.fill (5);
  TEST_RUN ("v1.fill(5)",v1.fill (5),(v1[0]==5 && v1[1]==5 && v1[2]==5 && v1[3]==5),1);
  TEST_RUN ("v3.position(3)", v1.fill (5), v3.position(3), 1);
//  v3.copy(v1,1,4);
  TEST_RUN ("v3.copy(v5,1,4)",v3.copy(v1,1,4),(v3[1]==5 && v3[2]==5 && v3[3]==5), 1);

  v1=CoolVector<int>(8,8,0,1,2,3,4,5,6,7);
  TEST ("v1=CoolVector<int>(8,8,0,1,2,3,4,5,6,7)",(v1.length()==8 && v1[5]==5),1);

 v2=CoolVector<int>(3,3,4,5,6);
  TEST ("v2=CoolVector<int>(3,3,4,5,6)",(v2.length()==3 && v2[2]==6),1);
  TEST ("v1.search(v2)",v1.search(v2), TRUE);

//  v0.push (-1);
  TEST_RUN ("v0.push(-1)", v0.push (-1), v0.position(-1), 4);
  TEST ("v0.push_new(1)", (v0.length()==5), 1);
  TEST ("v0.pop()", v0.pop(), -1);

//  v1.reverse();
  TEST_RUN ("v1.reverse()", v1.reverse(), (v1[1] == 6), 1);
//  v1.remove(6);
  TEST_RUN ("v1.remove(6)", v1.remove(6), (v1[1] == 6), 0);
//  v0.remove_duplicates();
  TEST_RUN ("v0.remove_duplicates()",v0.remove_duplicates(),v0.length(),2);
//  v4.push(5); v4.push(5); v4.push(5);
//  v0.replace(1,3);
  TEST_RUN ("v0.replace(7,3)",v4.push(5); v4.push(5); v4.push(5); v0.replace(1,3);, v0[1], 3);

//  v4.replace_all(5,3);
  TEST_RUN ("v4.replace_all(5,3)", v4.replace_all(5,3), (v4[3]==3 && v4[4]==3 && v4[5]==3), 1);
  TEST ("put(4,3)", v4.put(4,3), TRUE);
  TEST ("get(3)", v4.get(3), 4);
  TEST ("put(5,100)", v4.put(5,100), FALSE);
  TEST ("is_empty()", v4.is_empty(), FALSE);

//  v4.clear();
  TEST_RUN ("clear()", v4.clear(), 0, 0);
  TEST_RUN ("is_empty()", v4.clear(), v4.is_empty(), TRUE);
}

void test_int2() {
  CoolVector<int> v0 = CoolVector<int>(5,5,3,5,1,7,2);
  TEST ("v0=CoolVector<int>(5,5,3,5,1,7,2)", v0.length(), 5);
  CoolVector<int> v1 = CoolVector<int>(3,3,9,2,6);
  TEST ("v1=CoolVector<int>(3,3,9,2,6)", v1.length(), 3);
  CoolVector<int> v2 = v0;
  TEST ("v2=v0", (v0.length() == v2.length()), 1);
  CoolVector<int> v3 = v1;
  TEST ("v3=v1", (v1.length() == v3.length()), 1);
//   v1.prepend(v0);
  TEST_RUN ("v1.prepend(v0)", v1.prepend(v0), v1.length(), 8);
//   v0.append (v3);
  TEST_RUN ("v0.append(v3)", v0.append (v3), v0.length(), 8);
  TEST ("v0==v1", (v0==v1), TRUE);
//   v2.insert_before(9,3);
  TEST_RUN ("v2.insert_before(9,3)",  v2.insert_before(9,3), v2[3], 9);
//   v2.insert_before(12);
  TEST_RUN ("v2.insert_before(12)", v2.insert_before(12), v2[3], 12);
//   v2.insert_after(11,2);
  TEST_RUN ("v2.insert_after(11,2)", v2.insert_after(11,2), v2[3], 11);
//   v2.insert_after(13);
  TEST_RUN ("v2.insert_after(13)", v2.insert_after(13), v2[4], 13);
  TEST ("v2.position()", v2.position(), 4);
  TEST ("v2.reset()", (v2.reset(),v2.position()), INVALID);
  TEST ("v2.next()", v2.next(), TRUE);
  TEST ("v2.remove()", v2.remove(), 3);
  TEST ("v2.next()", v2.next(), TRUE);
  TEST ("v2.prev()", v2.prev(), TRUE);
  TEST ("v2.value()", v2.value(), 5);
//   v2.sort (my_compare_int);
  TEST_RUN ("v2.sort (my_compare_int)", v2.sort (my_compare_int), (v2[0] <= v2[1] && v2[1] <= v2[2] && v2[2] <= v2[3] && v2[3] <= v2[4] && v2[4] <= v2[5]), 1);
  v1 = CoolVector<int> (4,4,1,2,3,4);
  v2 = CoolVector<int> (4,4,1,3,5,7);
  v3 = v1;
  v1.merge(v2,my_compare_int);
  Boolean success = v1[0] <= v1[1] && v1[1] <= v1[2];
  success = success && v1[2] <= v1[3] && v1[3] <= v1[4];
  success = success && v1[4] <= v1[5] && v1[5] <= v1[6];
  TEST ("v1.merge(v2,my_compare_int)", success, TRUE);
}
 
void test_double1 () {
  CoolVector<double> d0;
  TEST ("CoolVector<double> d0", 1, 1);
  CoolVector<double> d1(3);
  TEST ("CoolVector<double> d1(3)", 1, 1);
  CoolVector<double> d2(4, 1.0);
  TEST ("CoolVector<double> d2(4)",(d2[0]==1.0 && d2[1]==1.0 && d2[2]==1.0 && d2[3]==1.0),1);
//  d1 = d2;
  TEST_RUN ("d1 = d2",d1=d2,(d1[0]==1.0 && d1[1]==1.0 && d1[2]==1.0 && d1[3]==1.0),1);
  TEST_RUN ("d1==d2", d1 = d2, (d1==d2), TRUE);
//  d1[0] = 2.0;
  TEST_RUN ("d1[0] = 2.0", d1[0]=2.0, (d1[0] == 2.0), 1);
//  d0 = d1;
  TEST_RUN ("d0 = d1",d0=d1,(d0[0]==2.0 && d0[1]==1.0 && d0[2]==1.0 && d0[3]==1.0),1);
  CoolVector<double> d3 (5,3,2.0,3.0,1.0);
  TEST ("CoolVector<double> d3(5,3,2.0,3.0,1.0)",(d3[0]==2.0 && d3[1]==3.0 && d3[2]==1.0),1);
  CoolVector<double> d4 = d3;
  TEST ("CoolVector<double> d4 = d3", (d4[0]==2.0 && d4[1]==3.0 && d4[2]==1.0),1);
  TEST ("d1!=d2", (d1!=d2), TRUE);
  TEST ("d4==d3", (d4==d3), TRUE);
  TEST ("d1 != d3", ((d1 != d3) ? TRUE : FALSE), TRUE);
  CoolVector<double> d5(d4);
  TEST ("CoolVector<double> d5(d4)",  (d4==d5), TRUE);
  TEST ("d1.length()", d1.length(), 4);
//  d1.fill (5.0);
  TEST_RUN ("d1.fill(5.0)",d1.fill (5.0),(d1[0]==5.0 && d1[1]==5.0 && d1[2]==5.0 && d1[3]==5.0),1);
  TEST ("d3.position(3.0)", d3.position(3.0), 1);
//  d3.copy(d1,1,4);
  TEST_RUN ("d3.copy(d5,1,4)",d3.copy(d1,1,4),(d3[1]==5.0 && d3[2]==5.0 && d3[3]==5.0), 1);
  d1=CoolVector<double>(8,8,0.0,1.0,2.0,3.0,4.0,5.0,6.0,7.0);
  TEST ("d1=CoolVector<double>(8,8,0.0,1.0,2.0,3.0,4.0,5.0,6.0,7.0)",(d1.length()==8 && d1[5]==5.0),1);
  d2=CoolVector<double>(3,3,4.0,5.0,6.0);
  TEST ("d2=CoolVector<double>(3,3,4.0,5.0,6.0)",(d2.length()==3 && d2[2]==6.0),1);
  TEST ("d1.search(d2)",d1.search(d2), TRUE);
//  d0.push (-1.0);
  TEST_RUN ("d0.push(-1.0)", d0.push (-1.0), d0.position(-1.0), 4);
  TEST ("d0.push_new(1.0)", (d0.length()==5), 1);
  TEST ("d0.pop()", d0.pop(), -1.0);
//  d1.reverse();
  TEST_RUN ("d1.reverse()", d1.reverse(), (d1[1] == 6.0), 1);
//  d1.remove(6.0);
  TEST_RUN ("d1.remove(6.0)", d1.remove(6.0), (d1[1] == 6.0), 0);
//  d0.remove_duplicates();
  TEST_RUN ("d0.remove_duplicates()",d0.remove_duplicates(),d0.length(),2);
//  d4.push(5.0); d4.push(5.0); d4.push(5.0);
//  d0.replace(1.0,3.0);
  TEST_RUN ("d0.replace(7.0,3.0)",d4.push(5.0); d4.push(5.0); d4.push(5.0);d0.replace(1.0,3.0),d0[1], 3.0);
//  d4.replace_all(5.0,3.0);
  TEST_RUN ("d4.replace_all(5.0,3.0)", d4.replace_all(5.0,3.0),(d4[3]==3.0 && d4[4]==3.0 && d4[5]==3.0), 1);
}

void test_double2 () {
  CoolVector<double> d0 = CoolVector<double>(5,5,3.0,5.0,1.0,7.0,2.0);
  TEST ("d0=CoolVector<double>(5,5,3.0,5.0,1.0,7.0,2.0)", d0.length(), 5);
  CoolVector<double> d1 = CoolVector<double>(3,3,9.0,2.0,6.0);
  TEST ("d1=CoolVector<double>(3,3,9.0,2.0,6.0)", d1.length(), 3);
  CoolVector<double> d2 = d0;
  TEST ("d2=d0", (d0.length() == d2.length()), 1);
  CoolVector<double> d3 = d1;
  TEST ("d3=d1", (d1.length() == d3.length()), 1);
//  d1.prepend(d0);
  TEST_RUN ("d1.prepend(d0)", d1.prepend(d0), d1.length(), 8);
//  d0.append (d3);
  TEST_RUN ("d0.append(d3)", d0.append (d3), d0.length(), 8);
  TEST ("d0==d1", (d0==d1), TRUE);
//  d2.insert_before(9.0,3);
  TEST_RUN ("d2.insert_before(9.0,3)", d2.insert_before(9.0,3), d2[3], 9.0);
//  d2.insert_before(12.0);
  TEST_RUN ("d2.insert_before(12.0)", d2.insert_before(12.0), d2[3], 12.0);
//  d2.insert_after(11.0,2);
  TEST_RUN ("d2.insert_after(11.0,2)", d2.insert_after(11.0,2), d2[3], 11.0);
//  d2.insert_after(13.0);
  TEST_RUN ("d2.insert_after(13.0)", d2.insert_after(13.0), d2[4], 13.0);
  TEST ("d2.position()", d2.position(), 4);
  TEST ("d2.reset()", (d2.reset(),d2.position()), INVALID);
  TEST ("d2.next()", d2.next(), TRUE);
  TEST ("d2.remove()", d2.remove(), 3.0);
  TEST ("d2.next()", d2.next(), TRUE);
  TEST ("d2.prev()", d2.prev(), TRUE);
  TEST ("d2.value()", d2.value(), 5.0);
//  d2.sort (my_compare_double);
  TEST_RUN ("d2.sort (my_compare_double)", d2.sort (my_compare_double), (d2[0] <= d2[1] && d2[1] <= d2[2] && d2[2] <= d2[3] && d2[3] <= d2[4] && d2[4] <= d2[5]), 1);
  d1 = CoolVector<double> (4,4,1.0,2.0,3.0,4.0);
  d2 = CoolVector<double> (4,4,1.0,3.0,5.0,7.0);
  d3 = d1;
  d1.merge(d2,my_compare_double);
  Boolean success = d1[0] <= d1[1] && d1[1] <= d1[2];
  success = success && d1[2] <= d1[3] && d1[3] <= d1[4];
  success = success && d1[4] <= d1[5] && d1[5] <= d1[6];
  TEST ("d1.merge(d2,my_compare_double)", success, TRUE);
}
 
typedef char* charP;
Boolean my_compare_charP (const charP& s1, const charP& s2) {
  return (strcmp (s1, s2) ? FALSE : TRUE);
}

void test_charP () {  
  CoolVector<char*> c0;
  TEST ("CoolVector<char*> c0", 1, 1);
//  c0.set_compare(&my_compare_charP);
  TEST_RUN ("c0.set_compare(&my_compare_charP)", c0.set_compare(&my_compare_charP), 1,1);
  CoolVector<char*> c1(4);
  TEST ("CoolVector<char*> c1(4)", 1, 1);
  CoolVector<char*> c2(4, "BBB");
  TEST ("CoolVector<char*> c2(4,\"BBB\")",((strcmp (c2[0],"BBB") == 0) && (strcmp (c2[1],"BBB")==0) && (strcmp (c2[2],"BBB")==0) && (strcmp(c2[3],"BBB")==0)),1);
//  c1 = c2;
  TEST_RUN ("c1 = c2",c1=c2,((strcmp (c1[0],"BBB") == 0) && (strcmp (c1[1],"BBB")==0) && (strcmp (c1[2],"BBB")==0) && (strcmp(c1[3],"BBB")==0)),1);
  TEST_RUN ("c1==c2", c1=c2, (c1==c2), TRUE);
//  c1[0] = "CCC";
  TEST_RUN ("c1[0] = \"CCC\"", c1[0] = "CCC", strcmp (c1[0],"CCC"), 0);
//  c0 = c1;
  TEST_RUN ("c0 = c1",c0 = c1,((strcmp (c0[0],"CCC") == 0) && (strcmp (c0[1],"BBB")==0) && (strcmp (c0[2],"BBB")==0) && (strcmp(c0[3],"BBB")==0)),1);
  CoolVector<char*> c3 (5,3,"DDD","EEE","FFF");
  TEST ("CoolVector<char*> c3(5,3,\"DDD\",\"EEE\",\"FFF\")",((strcmp (c3[0],"DDD") == 0) && (strcmp (c3[1],"EEE")==0) && (strcmp (c3[2],"FFF")==0)),1);
  CoolVector<char*> c4 = c3;
  TEST ("CoolVector<char*> c4 = c3", ((strcmp (c4[0],"DDD") == 0) && (strcmp (c4[1],"EEE")==0) && (strcmp (c4[2],"FFF")==0)),1);
  TEST ("c1!=c2", (c1!=c2), TRUE);
  TEST ("c4==c3", (c4==c3), TRUE);
  TEST ("c1 != c3", ((c1 != c3) ? TRUE : FALSE), TRUE);
  CoolVector<char*> c5(c4);
  TEST ("CoolVector<char*> c5(c4)",  (c4==c5), TRUE);
  TEST ("c1.length()", c1.length(), 4);
//  c1.fill ("GGG");
  TEST_RUN ("c1.fill(\"GGG\")",c1.fill ("GGG"),((strcmp (c1[0],"GGG") == 0) && (strcmp (c1[1],"GGG")==0) && (strcmp (c1[2],"GGG")==0)),1);
  TEST ("c3.position(\"DDD\")", c3.position("DDD"), 0);
  TEST ("c5.push(\"GGG\")", c5.push("GGG"), TRUE);
  TEST ("c5.position(\"GGG\")",c5.position("GGG"),3);
//  c3.copy(c5,1,4);
  TEST_RUN ("c3.copy(c5,1,4)",c3.copy(c5,1,4),(strcmp (c3[1],c5[1])==0 && strcmp(c3[2],c5[2])==0 && strcmp (c3[3],c5[3]) ==0), 1);
  TEST ("c3.reset()", (c3.reset(),1),1);
  TEST ("c3.next()",c3.next(),TRUE);
  TEST ("c3.value()", strcmp (c3.value(), "DDD"), 0);
  TEST ("c3.next()",c3.next(),TRUE);
  TEST ("c3.value()", strcmp (c3.value(), "EEE"), 0);
  TEST ("c3.prev()",c3.prev(),TRUE);
  TEST ("c3.value()", strcmp (c3.value(), "DDD"), 0);
  TEST ("c3.next()",c3.next(),TRUE);
  TEST ("c3.value()", strcmp (c3.value(), "EEE"), 0);
  TEST ("c3.next()",c3.next(),TRUE);
  TEST ("c3.value()", strcmp (c3.value(), "FFF"), 0);
  TEST ("c3.next()",c3.next(),TRUE);
  TEST ("c3.value()", strcmp (c3.value(), "GGG"), 0);
  TEST ("c3.next()",c3.next(),FALSE);
}

void test_shuffle() {
  CoolVector<int> v0(4,4, 1, 2, 3, 4);
  CoolVector<int> v = v0;
  TEST ("v.find(-1, 0, +1)", v.find(-1, 0, +1), FALSE);
  TEST ("v.find(-2, 3, -1)", v.find(-2, 3, -1), FALSE);
  TEST ("v.find(2, 3, -1)", v.find(2, 3, -1), TRUE);
  TEST ("v.find(1, 0, +1)", v.find(1, 0, +1), TRUE);
  TEST ("v.shuffle_remove()", 
        (v.shuffle_remove()==1 && v[0]==4 && v[1]==2 && v[2]==3 
         && v.length()==3), TRUE);
  TEST ("v.shuffle_remove(2)",
        (v.shuffle_remove(2)==TRUE && v[0]==4 && v[1]==3 
         && v.length()== 2), TRUE);
  TEST ("v.shuffle_remove(2)", v.shuffle_remove(2), FALSE);
  v = v0;
  TEST ("v.shuffle_insert_before(-1, 2)", 
        (v.shuffle_insert_before(-1, 2)==TRUE && v[1]==2 && v[2]==-1 && v[3]==4
         && v[4]==3), TRUE);
  TEST ("v.shuffle_insert_after(-2, 2)", 
        (v.shuffle_insert_after(-2, 2)==TRUE && v[1]==2 && v[2]==-1 && v[3]==-2
         && v[4]==3 && v[5]==4), TRUE);
}


void test_leak() {
  for (;;) {
    test_int1();
    test_int2();
    test_double1();
    test_double2();
    test_charP();
  }
}


int main (void) {
  START("CoolVector");
  test_int1();
  test_int2();
  test_double1();
  test_double2();
  test_charP();
  test_shuffle();
#if LEAK
  test_leak();
#endif
  SUMMARY();
  return 0;
}
