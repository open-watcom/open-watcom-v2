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
// Updated: JAM 08/14/92 -- changed counter n from int->long

#include <cool/test.h>
#include <cool/List.h>
#include <cool/Vector.h>
#include <cool/Timer.h>

#include <cool/List.C>
#include <cool/Vector.C>

// *****************
// List of int tests
// *****************

int my_compare_int(const int& t1, const int& t2) {
  return ((t1 < t2) ? -1 : 1);
}

void list_int_op_test1() {
  
  CoolList<int> ltemp1;
  CoolList<int> ltemp2;

  // CoolList l0 is ().
  CoolList<int> l0;
  TEST("CoolList<int> l0", 1, 1);

  // CoolList l1(3) is (3).
  CoolList<int> l1(3);
  TEST("CoolList<int> l1(3)", 1, 1);

  // CoolList l2(2,l1) is (2 3).
  CoolList<int> l2(2,l1);
  TEST("CoolList<int> l2(2,l1)", 1, 1);

  // CoolList l3(1,l2) is (1 2 3).
  CoolList<int> l3(1,l2);
  TEST("CoolList<int> l3(1,l2)", 1, 1);

  // CoolList l4(3,1,2,3) is (1 2 3).
  CoolList<int> l4(3,1,2,3);
  TEST("CoolList<int> l4(3,1,2,3)", 1, 1);

  CoolList<int> l41(1, 0);
  TEST("CoolList<int> l41(1, 0)", 
       (l41[0]==0 && l41.length()==1), TRUE);
  CoolList<int> l42(2, 0, 1);
  TEST("CoolList<int> l42(2, 0, 1)", 
       (l42[0]==0 && l42[1]==1 && l42.length()==2), TRUE);
  CoolList<int> l43(3, 0, 1, 2);
  TEST("CoolList<int> l43(3, 0, 1, 2)", 
       (l43[0]==0 && l43[1]==1 && l43[2]==2 && l43.length()==3), TRUE);


  // CoolList l5(l4) is (1 2 3).
  CoolList<int> l5(l4);
  TEST("CoolList<int> l5(l4)", 1, 1);
  TEST("l3 == l4", l3==l4, TRUE);

  TEST("l5[2]", l5[2]==3 && l5.value()==3, TRUE);
//  TEST("l5[-1]", l5[-1], ERROR);
//  TEST("l5[3]", l5[3], ERROR);
  TEST("l5.get(1)", l5.get(1)==2, TRUE);
  TEST_RUN("l5[1]=43", l5[1]=43, l5[1]==43, TRUE);
  TEST_RUN("l5.put(55,1)", l5.put(55,1), l5.get(1)==55, TRUE);
  // l5 is (1 55 3);
  TEST_RUN("l5.tail(ltemp1)", l5.tail(ltemp1),
           (ltemp1.length()==2 && ltemp1[0]==55 && ltemp1[1]==3), TRUE);
  TEST_RUN("l5.tail(ltemp1,0)", l5.tail(ltemp1,0),
           (ltemp1.length()==3 && ltemp1[0]==1 && ltemp1[1]==55 
            && ltemp1[2]==3),
           TRUE);
  TEST_RUN("l5.tail(ltemp1,3)", l5.tail(ltemp1,3), ltemp1.is_empty(), TRUE);
  TEST_RUN("l5.tail(ltemp1,5)", l5.tail(ltemp1,5), ltemp1.is_empty(), TRUE);

  // l5 is (1 55 3);
  TEST_RUN("l5.last(ltemp1, 0)", l5.last(ltemp1, 0), ltemp1.is_empty(), TRUE);
  TEST_RUN("l5.last(ltemp1)", l5.last(ltemp1),
           (ltemp1.length()==1 && ltemp1[0]==3), TRUE);
  TEST_RUN("l5.last(ltemp1,2)", l5.last(ltemp1,2),
           (ltemp1.length()==2 && ltemp1[0]==55 && ltemp1[1]==3),
           TRUE);
  TEST_RUN("l5.last(ltemp1,3)", l5.last(ltemp1,3),
           (ltemp1.length()==3 && ltemp1[0]==1 && ltemp1[1]==55 
            && ltemp1[2]==3),
           TRUE);
  TEST_RUN("l5.last(ltemp1,5)", l5.last(ltemp1,5), ltemp1.is_empty(), TRUE);
  // l2 is (2 3)
  TEST("l2.is_empty()", l2.is_empty(), FALSE);
  // l3 is (1 2 3)
  TEST("l3.length()", l3.length()== 3, TRUE);
}

void list_int_op_test2() {
  
  CoolList<int> ltemp1;
  CoolList<int> ltemp2;

  ltemp1 = CoolList<int>(4,11,22,33,44);
  CoolList<int> l6;

  TEST_RUN("l6.but_last(ltemp2, 0)", l6.copy(ltemp1);  l6.but_last(ltemp2, 0),
           (ltemp2.length()==4 && ltemp2[0]==11 && ltemp2[1]==22 
            && ltemp2[2]==33 && ltemp2[3]==44),
           TRUE);

  // l6 is now (11 22 33 44)
  TEST_RUN("l6.but_last(ltemp2)", l6.but_last(ltemp2),
           (ltemp2.length()==3 && ltemp2[0]==11 && ltemp2[1]==22 
            && ltemp2[2]==33),
           TRUE);
  TEST_RUN("l6.but_last(ltemp2, 2)", l6.but_last(ltemp2, 2),
           (ltemp2.length()==2 && ltemp2[0]==11 && ltemp2[1]==22),
           TRUE);
  TEST_RUN("l6.but_last(ltemp2, 4)", l6.but_last(ltemp2, 4), 
           (ltemp2.length()==0), TRUE);
  TEST_RUN("l6.but_last(ltemp2, 5)", l6.but_last(ltemp2, 5),
           (ltemp2.length()==0), TRUE);

  TEST_RUN("l6.clear()", l6.clear(), (l6.length()==0), TRUE); 
  // l6 is now ()
  TEST("l6.is_empty()", l6.is_empty(), TRUE);
  TEST("l6.length()", l6.length()==0, TRUE);

  // l6 is (11 22 33 44)
  TEST_RUN("l6.position(33)", l6.copy(ltemp1), 
           (l6.position(33)==2 && l6.value()==33), TRUE);
  TEST("l6.position(2)", l6.position(2)==-1, TRUE);
}

void list_int_op_test3() {
  int temp;
  CoolList<int> ltemp1;
  CoolList<int> ltemp2;
  CoolList<int> l0;
  CoolList<int> l1(3);                          // CoolList l1 is (3).
  CoolList<int> l2(2,l1);                               // CoolList l2 is (2 3).
  CoolList<int> l3(1,l2);                               // CoolList l3 is (1 2 3).
  CoolList<int> l4(3,1,55,3);                   // CoolList l4 is (1 55 3).
  CoolList<int> l5(l4);                         // CoolList l5 is (1 2 3).
  CoolList<int> l6(4, 11, 22, 33, 44);          // CoolList l6 is (11 22 33 44)

  CoolList<int> l9;
  TEST_RUN("l9.copy(l4)", l9.copy(l4), (l4==l9), TRUE);
  TEST_RUN("l9 = l4", l9 = l4, (l4==l9), TRUE);

  // l6 is (11 22 33 44)
  ltemp1 = CoolList<int>(4,44,33,22,11);
  TEST_RUN("l6.reverse()", l6.reverse(), (l6==ltemp1), TRUE);

  // l6 is (44 33 22 11)
  TEST_RUN("l6.push_end(66)", l6.push_end(66), 
           (l6.length()==5 && l6[4]==66 && l6.value()==66),
           TRUE);
  TEST_RUN("l6.push(77)", l6.push(77), 
           (l6.length()==6 && l6[0]==77 && l6.value()==77), TRUE);
  TEST_RUN("l6.push_new(88)", l6.push_new(88), 
           (l6.length()==7 && l6[0]==88 && l6.value()==88),
           TRUE);
  TEST_RUN("l6.push_new(22)", l6.push_new(22), 
           (l6.length()==7 && l6[0]==88), TRUE);
  TEST("l6.pop()", (l6.pop(temp), temp), 88);

  // l6 is (77 44 33 22 11 66)
  // l2 is (2 3)
  // l4 is (1 55 3)

  // l6 is now (77 44 33 22 11 66 2 3)
  TEST_RUN("l6.append(l2)", l6.append(l2),
           (l6.value()==2 && l6.length()==8 && l6[6]==2 && l6[7]==3), TRUE);

  // l6 is now (1 55 3 77 44 33 22 11 66 2 3)
  TEST_RUN("l6.prepend(l4)", l6.prepend(l4),
           (l6.value()==1 && l6.length()==11 && l6[0]==1 && l6[1]==55 
            && l6[2]==3),
           TRUE);

  TEST_RUN("l6.set_tail(l2, 14)", ltemp1.copy(l6), l6.set_tail(l2,14), FALSE);
  TEST("l6.set_tail(l2, 14) ==", l6==ltemp1, TRUE);
  ltemp2 = CoolList<int>(6,1,55,3,77,2,3);
  TEST("l6.set_tail(l2,4)", l6.set_tail(l2,4), TRUE);
  TEST("l6.set_tail(l2,4) ==", (l6==ltemp2), TRUE);
  TEST("l6.set_tail(l2,4) value", l6.value(), 2);
  TEST_RUN("l6.set_tail(l2)", l6.set_tail(l2),
           (l6.value()==2 && l6.length()==3 && l6[0]==1 && l6[1]==2 
            && l6[2]==3),
           TRUE);
  TEST_RUN("l6.set_tail(l2,0)", l6.set_tail(l2,0),
           (l6.value()==2 && l6.length()==2 && l6[0]==2 && l6[1]==3), TRUE);

}

void list_int_op_test4() {
  CoolList<int> ltemp1;
  CoolList<int> ltemp2;

  ltemp1 = CoolList<int>(4,44,33,22,11);

  CoolList<int> l8(5,11,22,33,44,55);
  ltemp1.copy(l8);
  ltemp2 = CoolList<int>(4,11,22,33,55);
  TEST_RUN("l8.remove(77)", l8.remove(77), (l8==ltemp1), TRUE);
  TEST_RUN("l8.remove(44)", l8.remove(44), (l8==ltemp2 && l8.value()==55),
           TRUE);

  CoolList<int> l11(7,11,22,33,11,22,33,44,11,11);
  ltemp1 = CoolList<int>(4,11,22,33,44);
  l11.remove_duplicates();
  TEST_RUN("l11.remove_duplicates()", l11.remove_duplicates(), (l11==ltemp1),
           TRUE);

  // l8 is now (11 22 33 55)
  ltemp1.copy(l8);
  ltemp2 = CoolList<int>(4,11,55,33,55);
  TEST_RUN("l8.replace(66,22)", l8.replace(66,22), (l8==ltemp1), TRUE);
  TEST_RUN("l8.replace(22,55)", l8.replace(22,55), 
           (l8==ltemp2 && l8.value()==55), TRUE);

  ltemp1.copy(l8);
  ltemp2 = CoolList<int>(4,11,22,33,22);
  TEST_RUN("l8.replace_all(66,22)", l8.replace_all(66,22), (l8==ltemp1), TRUE);
  TEST_RUN("l8.replace_all(55,22)", l8.replace_all(55,22), (l8==ltemp2), TRUE);


  // l8 is now (11 22 33 22)
  ltemp1.copy(l8);
  ltemp2 = CoolList<int>(5,11,22,111,33,22);
  TEST_RUN("l8.insert_after(111,44)", l8.insert_after(111,44), (l8==ltemp1),
           TRUE);
  TEST_RUN("l8.insert_after(111,22)", l8.insert_after(111,22), 
           (l8==ltemp2 && l8.value()==111), TRUE);

  TEST_RUN("l8.insert_before(222,44)", 
           ltemp1.copy(l8); l8.insert_before(222,44), 
           (l8==ltemp1), TRUE);
  ltemp2 = CoolList<int>(6,11,222,22,111,33,22);
  TEST_RUN("l8.insert_before(222,22)", l8.insert_before(222,22),
           (l8==ltemp2 && l8.value()==222), TRUE);
  ltemp2 = CoolList<int>(7,333,11,222,22,111,33,22);
  TEST_RUN("l8.insert_before(333,11)", l8.insert_before(333,11),
           (l8==ltemp2 && l8.value()==333), TRUE);
}

void list_int_sort_test() {

  CoolList<int> l1(4,6,66,22,222);
  CoolList<int> l2(4,61,12,222,666);
  CoolList<int> l3(4,7,51,77,24);
  CoolList<int> l4(2,22,33);
  CoolList<int> ltemp1;

  ltemp1 = CoolList<int>(8,6,61,12,66,22,222,222,666);
  TEST_RUN("l1.merge(l2,&my_compare_int)", l1.merge(l2,&my_compare_int), 
           (l1==ltemp1), TRUE);

  ltemp1 = CoolList<int>(6,22,33,61,12,222,666);
  TEST_RUN("l4.merge(l2,&my_compare_int)", l4.merge(l2,&my_compare_int), 
           (l4==ltemp1), TRUE);

  ltemp1 = CoolList<int>(4,12,61,222,666);
  TEST_RUN("l2.sort(&my_compare_int)", l2.sort(&my_compare_int), (l2==ltemp1),
           TRUE);

  ltemp1 = CoolList<int>(4,7,24,51,77);
  TEST_RUN("l3.sort(&my_compare_int)", l3.sort(&my_compare_int), (l3==ltemp1),
           TRUE);
}

void list_int_io_test() {

  // ** an automated test would get input from file, rather than cin **
  CoolList<int> input_list;
//  cout << "\nTesting operator>> for CoolList<int>.";
//  cout << "\nExample of CoolList<int> is (1 2 3).";
//  cout << "\nPlease enter a CoolList<int> structure: ";
//  cin >> input_list;
//  cout << "\nInput_list is now set to: " << input_list << ".\n";

}


void list_int_set_test1() {
  CoolList<int> ltemp1;
  CoolList<int> l1;
  CoolList<int> l2(4,11,22,33,44);
  CoolList<int> l3(2,22,33);
  CoolList<int> l4(5,11,22,33,44,55);

  TEST("l2.find(33)", (l2.find(33) && l2.value()==33), TRUE);
  TEST("l2.find(2)", l2.find(2), FALSE);

  TEST("l2.search(l3)", (l2.search(l3) && l2.value()==22), TRUE);
  TEST("l2.search(l1)", l2.search(l1), FALSE);
  TEST("l2.search(l4)", l2.search(l4), FALSE);
  TEST("l4.search(l2)", (l4.search(l2) && l4.value()==11), TRUE);
  TEST("l1.search(l2)", l1.search(l2), FALSE);

  TEST_RUN("l2.member(ltemp1, 22)", l2.member(ltemp1, 22),
           (ltemp1.length()==3, ltemp1[0]==22 && ltemp1[1]==33, ltemp1[2]==44
            && l2.value()==22),
           TRUE);
  TEST_RUN("l2.member(ltemp1, 44)", l2.member(ltemp1, 44),
           (ltemp1.length()==1 && ltemp1[0]==44 && l2.value()==44),
           TRUE);
  TEST_RUN("l2.member(ltemp1, 3)", l2.member(ltemp1, 3), (ltemp1.length()==0),
           TRUE);

  TEST_RUN("l2.sublist(ltemp1, l3)", l2.sublist(ltemp1, l3),
           (ltemp1.length()==3 && ltemp1[0]==22, ltemp1[1]==33, ltemp1[2]==44
            && l2.value()==22),
           TRUE);
  TEST_RUN("l2.sublist(ltemp1, l1)", l2.sublist(ltemp1, l1),
           (ltemp1.length()==0), TRUE);
  TEST_RUN("l2.sublist(ltemp1, l4)", l2.sublist(ltemp1, l4), 
           (ltemp1.length()==0), TRUE);
  TEST_RUN("l4.sublist(ltemp1, l2)", l4.sublist(ltemp1, l2), 
           (ltemp1==l4 && l4.value()==11), TRUE);
  TEST_RUN("l1.sublist(ltemp1, l2)", l1.sublist(ltemp1, l2), 
           (ltemp1.length()==0), TRUE);
}

void list_int_set_test2() {
  CoolList<int> ltemp1;
  CoolList<int> l5(12,1,55,3,88,77,44,33,22,11,66,2,3);
  CoolList<int> l6(5,11,66,111,33,55);

  ltemp1 = CoolList<int>(4,55,33,11,66);
  TEST_RUN("l5.set_intersection(l6)", l5.set_intersection(l6), (l5==ltemp1), TRUE);

  ltemp1 = CoolList<int>(5,111,55,33,11,66);
  TEST_RUN("l5.set_union(l6)", l5.set_union(l6), (l5==ltemp1), TRUE);

  TEST_RUN("l5.set_difference(l6)", ltemp1.clear(); l5.set_difference(l6), 
           (l5==ltemp1), TRUE);

  ltemp1 = CoolList<int>(5,11,66,111,33,55);
  TEST_RUN("l5.set_xor(l6)", l5.set_xor(l6), (l5==ltemp1), TRUE);

}

void list_int_set_test3() {
  CoolList<int> ltemp1;
  CoolList<int> l7(2,2,3);
  CoolList<int> l8(3,1,2,3);
  CoolList<int> l9 = (CoolList<int> &) (l7 + l8);
  CoolList<int> l10(3,4,5,6);
  CoolList<int> l11(12,1,55,3,88,77,44,33,22,11,66,2,3);
  CoolList<int> l12(5,11,66,111,33,55);
  CoolList<int> l13;

  ltemp1 = CoolList<int>(5,2,3,1,2,3);
  TEST("l9 = l7 + l8", (l9==ltemp1), TRUE);

  ltemp1 = CoolList<int>(5,2,3,4,5,6);
  TEST_RUN("l7 += l10", l7 += l10, (l7==ltemp1), TRUE);

  ltemp1 = CoolList<int>(4,55,33,11,66);
  TEST_RUN("l11 &= l12", l11 &= l12, (l11==ltemp1), TRUE);

  ltemp1 = CoolList<int>(5,111,55,33,11,66);
  TEST_RUN("l11 |= l12", l11 |= l12, (l11==ltemp1), TRUE);

  TEST_RUN("l11 -= l12", ltemp1.clear(); l11 -= l12, (l11==ltemp1), TRUE);

  ltemp1 = CoolList<int>(5,11,66,111,33,55);
  TEST_RUN("l11 ^= l12", l11 ^= l12, (l11==ltemp1), TRUE);

  l11 = CoolList<int>(12,1,55,3,88,77,44,33,22,11,66,2,3);
  l12 = CoolList<int>(5,11,66,111,33,55);

  ltemp1 = CoolList<int>(4,55,33,11,66);
  TEST_RUN("l13 = l11 & l12", l13 = l11 & l12, (l13==ltemp1), TRUE);

  ltemp1 = CoolList<int>(13,111,1,55,3,88,77,44,33,22,11,66,2,3);
  TEST_RUN("l13 = l11 | l12", l13 = l11 | l12, (l13==ltemp1), TRUE);

  ltemp1 = CoolList<int>(8,1,3,88,77,44,22,2,3);
  TEST_RUN("l13 = l11 - l12", l13 = l11 - l12, (l13==ltemp1), TRUE);

  ltemp1 = CoolList<int>(9,1,3,88,77,44,22,2,3,111);
  TEST_RUN("l13 = l11 ^ l12", l13 = l11 ^ l12, (l13==ltemp1), TRUE);
}

void list_int_set_test() {

  list_int_set_test1();
  list_int_set_test2();
  list_int_set_test3();
}



void list_int_curpos_test1() {
  CoolList<int> l1(3,11,22,33);
  CoolList<int> ltemp1;

  TEST_RUN("l1.next()",l1.next(), l1.value(), 11);
  
  ltemp1 = CoolList<int>(4,11,55,22,33);
  TEST_RUN("l1.insert_after(55)", l1.insert_after(55),
           (l1.value()==55 && l1==ltemp1), TRUE);

  TEST_RUN("l1.prev()",l1.prev(),l1.value(), 11);

  ltemp1 = CoolList<int>(5,66,11,55,22,33);
  TEST_RUN("l1.insert_before(66)", l1.insert_before(66), 
           (l1.value()==66 && l1==ltemp1), TRUE);

  ltemp1 = CoolList<int>(4,11,55,22,33);
  TEST("l1.remove()", (l1.remove()==66 && l1.value()==11 && l1==ltemp1), TRUE);

  ltemp1 = CoolList<int>(3,11,55,33);
  TEST_RUN("l1.remove()", l1.next();l1.next(), 
           (l1.remove()==22 && l1.value()==33 && l1==ltemp1), TRUE);

  TEST_RUN("l1.position()", l1.prev(), (l1.position()==1 && l1.value()==55),
           TRUE);
}

void list_int_curpos_test2() {
  CoolList<int> l1(3,11,55,33);
  CoolList<int> l2(4,55,66,33,22);
  CoolList<int> ltemp1;

  TEST_RUN("l1.next_union(l2)", l1.reset(),
           (l1.next_union(l2) && l1.value()==11), TRUE);
  TEST_RUN("l1.next_union(l2)", l1.next_union(l2);l1.next_union(l2),
           (l1.next_union(l2) && l1.value()==66), TRUE);
  TEST("l1.next_union(l2)", (l1.next_union(l2) && l1.value()==22),TRUE);
  TEST("l1.next_union(l2)", l1.next_union(l2), FALSE);

  TEST_RUN("l1.next_intersection(l2)", l1.reset(), 
           (l1.next_intersection(l2) && l1.value()==55),  TRUE);
  TEST("l1.next_intersection(l2)", 
       (l1.next_intersection(l2) && l1.value()==33), TRUE);
  TEST("l1.next_intersection(l2)", l1.next_intersection(l2), FALSE);
  TEST_RUN("l1.next_difference(l2)", l1.reset(), 
           (l1.next_difference(l2) && l1.value()==11), TRUE);
  TEST("l1.next_difference(l2)", l1.next_difference(l2), FALSE);
  l1.reset();
  TEST_RUN("l1.next_xor(l2)", l1.reset(), 
           (l1.next_xor(l2) && l1.value()==11), TRUE);
  TEST_RUN("l1.next_xor(l2)", l1.next_xor(l2), 
           (l1.next_xor(l2) && l1.value()==22), TRUE);
  TEST("l1.next_xor(l2)", l1.next_xor(l2), FALSE);
}

void list_int_currentpos_test() {
  list_int_curpos_test1();
  list_int_curpos_test2();
}



void test_list_int() {

  cout << "\n*** Testing CoolList of Int ***\n\n";
  list_int_op_test1();
  list_int_op_test2();
  list_int_op_test3();
  list_int_op_test4();
  list_int_set_test();
  list_int_sort_test();
  list_int_currentpos_test();
  list_int_io_test();
}


// *********************
// List of doubles tests
// *********************

int my_compare_double(const double& t1, const double& t2) {
  return ((t1 < t2) ? -1 : 1);
}

void list_double_op_test1() {

  CoolList<double> ltemp1;
  CoolList<double> ltemp2;

  // CoolList l0 is ().
  CoolList<double> l0;
  TEST("CoolList<double> l0", 1, 1);

  // CoolList l4(3,1.0,2.0,3.0) is (1.0 2.0 3.0).
  CoolList<double> l4(3,1.0,2.0,3.0);
  TEST("CoolList<double> l4(3,1.0,2.0,3.0)", 1, 1);

  // CoolList l5(l4) is (1.0 2.0 3.0).
  CoolList<double> l5(l4);
  TEST("CoolList<double> l5(l4)", 1, 1);

  TEST("l5[2]", l5[2]==3.0 && l5.value()==3.0, TRUE);
//  TEST("l5[-1]", l5[-1], ERROR);
//  TEST("l5[3]", l5[3], ERROR);
  TEST("l5.get(1)", l5.get(1)==2.0, TRUE);
//  l5[1]=43.0;
  TEST_RUN("l5[1]=43.0", l5[1]=43.0, l5[1]==43.0, TRUE);
  TEST_RUN("l5.put(55.0,1)", l5.put(55.0,1), l5.get(1)==55.0, TRUE);
  // l5 is (1 55 3);
  TEST_RUN("l5.tail(ltemp1)", l5.tail(ltemp1),
       (ltemp1.length()==2 && ltemp1[0]==55.0 && ltemp1[1]==3.0), TRUE);
  TEST_RUN("l5.tail(ltemp1,0)", l5.tail(ltemp1,0),
           (ltemp1.length()==3 && ltemp1[0]==1.0 && ltemp1[1]==55.0
            && ltemp1[2]==3.0),
       TRUE);
  TEST_RUN("l5.tail(ltemp1,3)", l5.tail(ltemp1,3), ltemp1.is_empty(), TRUE);
  TEST_RUN("l5.tail(ltemp1,5)", l5.tail(ltemp1,5), ltemp1.is_empty(), TRUE);

  // l5 is (1 55 3);
  TEST_RUN("l5.last(ltemp1,0)", l5.last(ltemp1,0), ltemp1.is_empty(), TRUE);
  TEST_RUN("l5.last(ltemp1)", l5.last(ltemp1), 
           (ltemp1.length()==1 && ltemp1[0]==3.0), TRUE);
  TEST_RUN("l5.last(ltemp1,2)", l5.last(ltemp1,2),
           (ltemp1.length()==2 && ltemp1[0]==55.0 && ltemp1[1]==3.0),
           TRUE);
  TEST_RUN("l5.last(ltemp1,3)", l5.last(ltemp1,3),
           (ltemp1.length()==3 && ltemp1[0]==1.0 && ltemp1[1]==55.0
            && ltemp1[2]==3.0),
           TRUE);
  TEST_RUN("l5.last(ltemp1,5)", l5.last(ltemp1,5), ltemp1.is_empty(), TRUE);

}

void list_double_op_test2() {

  CoolList<double> ltemp1;
  CoolList<double> ltemp2;

  // CoolList l1(3.0) is (3.0).
  CoolList<double> l1(3.0);
  TEST("CoolList<double> l1(3.0)", 1, 1);

  // CoolList l2(2.0,l1) is (2.0 3.0).
  CoolList<double> l2(2.0,l1);
  TEST("CoolList<double> l2(2.0,l1)", 1, 1);

  // CoolList l3(1.0,l2) is (1.0 2.0 3.0).
  CoolList<double> l3(1.0,l2);
  TEST("CoolList<double> l3(1.0,l2)", 1, 1);

  // CoolList l4(3,1.0,2.0,3.0) is (1.0 2.0 3.0).
  CoolList<double> l4(3,1.0,2.0,3.0);
  TEST("CoolList<double> l4(3,1.0,2.0,3.0)", 1, 1);

  TEST("l3 == l4", l3==l4, TRUE);

  ltemp1 = CoolList<double>(4,11.0,22.0,33.0,44.0);
  CoolList<double> l6;
  
  TEST_RUN("l6.but_last(ltemp2, 0)", l6.copy(ltemp1); l6.but_last(ltemp2, 0),
           (ltemp2.length()==4 && ltemp2[0]==11.0 && ltemp2[1]==22.0
            && ltemp2[2]==33.0 && ltemp2[3]==44.0),
           TRUE);
  
  // l6 is (11 22 33 44)
  TEST_RUN("l6.but_last(ltemp2)", l6.but_last(ltemp2),
           (ltemp2.length()==3 && ltemp2[0]==11.0 && ltemp2[1]==22.0
            && ltemp2[2]==33.0),
           TRUE);
  TEST_RUN("l6.but_last(ltemps2, 2)", l6.but_last(ltemp2, 2),
           (ltemp2.length()==2 && ltemp2[0]==11.0 && ltemp2[1]==22.0),
           TRUE);
  TEST_RUN("l6.but_last(ltemp2, 4)", l6.but_last(ltemp2, 4), 
           (ltemp2.length()==0), TRUE);
  TEST_RUN("l6.but_last(ltemp2, 5)", l6.but_last(ltemp2, 5), 
           (ltemp2.length()==0), TRUE);

  // l6 is now ()
  TEST_RUN("l6.clear()", l6.clear(), (l6.length()==0), TRUE);

  TEST("l6.is_empty()", l6.is_empty(), TRUE);  // l6 is ();
  TEST("l2.is_empty()", l2.is_empty(), FALSE); // l2 is (2 3)

  TEST("l6.length()", l6.length()==0, TRUE);   // l6 is ()
  TEST("l3.length()", l3.length()== 3, TRUE);  // l3 is (1 2 3)

  // l6 is (11 22 33 44)
  TEST_RUN("l6.position(33.0)", l6.copy(ltemp1),
           (l6.position(33.0)==2 && l6.value()==33.0), TRUE);
  TEST("l6.position(2.0)", l6.position(2.0)==-1, TRUE);
}

void list_double_op_test3() {

  double temp;
  CoolList<double> ltemp1;
  CoolList<double> ltemp2;
  
  CoolList<double> l0;                    // CoolList l0 is ().
  CoolList<double> l1(3.0);                       // CoolList l1 is (3.0).
  CoolList<double> l2(2.0,l1);            // CoolList l2 is (2.0 3.0).
  CoolList<double> l3(1.0,l2);            // CoolList l3 is (1.0 2.0 3.0).
  CoolList<double> l4(3,1.0,55.0,3.0);    // CoolList l4 is (1.0 55.0 3.0).
  CoolList<double> l5(l4);                        // CoolList l5 is (1.0 2.0 3.0).
  CoolList<double> l6(4,11.0,22.0,33.0,44.0); // CoolList l6 is (11.0 22.0 33.0 44.0).
  CoolList<double> l9;

  TEST_RUN("l9.copy(l4)", l9.copy(l4), (l4==l9), TRUE);
  TEST_RUN("l9 = l4", l9 = l4, (l4==l9), TRUE);

  // l6 is (11 22 33 44)
  ltemp1 = CoolList<double>(4,44.0,33.0,22.0,11.0);
  TEST_RUN("l6.reverse()", l6.reverse(), (l6==ltemp1), TRUE);

  // l6 is (44 33 22 11)
  TEST_RUN("l6.push_end(66.0)", l6.push_end(66.0),
           (l6.length()==5 && l6[4]==66.0 && l6.value()==66.0),
           TRUE);
  TEST_RUN("l6.push(77.0)", l6.push(77.0),
           (l6.length()==6 && l6[0]==77.0 && l6.value()==77.0),
           TRUE);
  TEST_RUN("l6.push_new(88.0)", l6.push_new(88.0),
           (l6.length()==7 && l6[0]==88.0 && l6.value()==88.0),
           TRUE);
  TEST_RUN("l6.push_new(22.0)", l6.push_new(22.0),
           (l6.length()==7 && l6[0]==88.0), TRUE);
  TEST("l6.pop()", (l6.pop(temp), temp), 88);

  // l6 is (77 44 33 22 11 66)
  // l2 is (2 3)
  // l4 is (1 55 3)

  // l6 is now (77 44 33 22 11 66 2 3)
  TEST_RUN("l6.append(l2)", l6.append(l2),
           (l6.value()==2.0 && l6.length()==8 && l6[6]==2.0 && l6[7]==3.0),
           TRUE);

  // l6 is now (1 55 3 77 44 33 22 11 66 2 3)
  TEST_RUN("l6.prepend(l4)", l6.prepend(l4),
           (l6.value()==1.0 && l6.length()==11 && l6[0]==1.0 && l6[1]==55.0
            && l6[2]==3.0),
           TRUE);

  TEST_RUN("l6.set_tail(l2, 14)", ltemp1.copy(l6); l6.set_tail(l2,14),
           l6==ltemp1, TRUE);
  ltemp2 = CoolList<double>(6,1.0,55.0,3.0,77.0,2.0,3.0);
  TEST_RUN("l6.set_tail(l2,4)", l6.set_tail(l2,4), 
           (l6==ltemp2 && l6.value()==2.0), TRUE);
  TEST_RUN("l6.set_tail(l2)", l6.set_tail(l2),
           (l6.value()==2.0 && l6.length()==3 && l6[0]==1.0 && l6[1]==2.0
            && l6[2]==3.0),
           TRUE);
  TEST_RUN("l6.set_tail(l2,0)", l6.set_tail(l2,0),
           (l6.value()==2.0 && l6.length()==2 && l6[0]==2.0 && l6[1]==3.0),
           TRUE);
}

void list_double_op_test4() {

  CoolList<double> ltemp1;
  CoolList<double> ltemp2;
  CoolList<double> l8(5,11.0,22.0,33.0,44.0,55.0);
  CoolList<double> l11(7,11.0,22.0,33.0,11.0,22.0,33.0,44.0);
  
  ltemp1 = CoolList<double>(4,44.0,33.0,22.0,11.0);

  ltemp2 = CoolList<double>(4,11.0,22.0,33.0,55.0);
  TEST_RUN("l8.remove(77.0)", ltemp1.copy(l8); l8.remove(77.0),
           (l8==ltemp1), TRUE);
  TEST_RUN("l8.remove(44.0)", l8.remove(44.0),
           (l8==ltemp2 && l8.value()==55.0), TRUE);

  ltemp1 = CoolList<double>(4,11.0,22.0,33.0,44.0);
  TEST_RUN("l11.remove_duplicates()", l11.remove_duplicates(), (l11==ltemp1),
           TRUE);

  // l8 is now (11 22 33 55)
  ltemp2 = CoolList<double>(4,11.0,55.0,33.0,55.0);
  TEST_RUN("l8.replace(66.0,22.0)", ltemp1.copy(l8); l8.replace(66.0,22.0),
           (l8==ltemp1), TRUE);
  TEST_RUN("l8.replace(22.0,55.0)", ltemp1.copy(l8); l8.replace(22.0,55.0),
           (l8==ltemp2 && l8.value()==55.0), TRUE);

  ltemp2 = CoolList<double>(4,11.0,22.0,33.0,22.0);
  TEST_RUN("l8.replace_all(66.0,22.0)",
           ltemp1.copy(l8); l8.replace_all(66.0,22.0), 
           (l8==ltemp1), TRUE);
  TEST_RUN("l8.replace_all(55.0,22.0)",
           ltemp1.copy(l8); l8.replace_all(55.0,22.0), 
           (l8==ltemp2), TRUE);

  // l8 is now (11 22 33 22)
  ltemp2 = CoolList<double>(5,11.0,22.0,111.0,33.0,22.0);
  TEST_RUN("l8.insert_after(111.0,44.0)",
           ltemp1.copy(l8); l8.insert_after(111.0,44.0), 
           (l8==ltemp1), TRUE);
  TEST_RUN("l8.insert_after(111.0,22.0)",
           ltemp1.copy(l8); l8.insert_after(111.0,22.0), 
           (l8==ltemp2 && l8.value()==111.0), TRUE);

  TEST_RUN("l8.insert_before(222.0,44.0)",
           ltemp1.copy(l8); l8.insert_before(222.0,44.0), 
           (l8==ltemp1), TRUE);
  ltemp2 = CoolList<double>(6,11.0,222.0,22.0,111.0,33.0,22.0);
  TEST_RUN("l8.insert_before(222.0,22.0)", l8.insert_before(222.0,22.0),
           (l8==ltemp2 && l8.value()==222.0), TRUE);
  ltemp2 = CoolList<double>(7,333.0,11.0,222.0,22.0,111.0,33.0,22.0);
  TEST_RUN("l8.insert_before(333.0,11.0)", l8.insert_before(333.0,11.0),
           (l8==ltemp2 && l8.value()==333.0), TRUE);
}

void list_double_set_test1() {

  CoolList<double> ltemp1;
  CoolList<double> l1;
  CoolList<double> l2(4,11.0,22.0,33.0,44.0);
  CoolList<double> l3(2,22.0,33.0);
  CoolList<double> l4(5,11.0,22.0,33.0,44.0,55.0);
  CoolList<double> l5(12,1.0,55.0,3.0,88.0,77.0,44.0,33.0,22.0,11.0,66.0,2.0,3.0);
  CoolList<double> l6(5,11.0,66.0,111.0,33.0,55.0);


  TEST("l2.find(33.0)", (l2.find(33.0) && l2.value()==33.0), TRUE);
  TEST("l2.find(2.0)", l2.find(2.0), FALSE);

  TEST("l2.search(l3)", (l2.search(l3) && l2.value()==22.0), TRUE);
  TEST("l2.search(l1)", l2.search(l1), FALSE);
  TEST("l2.search(l4)", l2.search(l4), FALSE);
  TEST("l4.search(l2)", (l4.search(l2) && l4.value()==11.0), TRUE);
  TEST("l1.search(l2)", l1.search(l2), FALSE);

  TEST_RUN("l2.member(ltemp1, 22.0)", l2.member(ltemp1, 22.0),
           (ltemp1.length()==3, ltemp1[0]==22.0 && ltemp1[1]==33.0,
            ltemp1[2]==44.0 && l2.value()==22.0),
           TRUE);
  TEST_RUN("l2.member(ltemp1, 44.0)", l2.member(ltemp1, 44.0),
           (ltemp1.length()==1 && ltemp1[0]==44.0 && l2.value()==44.0),
           TRUE);
  TEST_RUN("l2.member(ltemp1, 3.0)", l2.member(ltemp1, 3.0), 
           (ltemp1.length()==0), TRUE);
  TEST_RUN("l2.sublist(ltemp, l3)", l2.sublist(ltemp1, l3),
           (ltemp1.length()==3 && ltemp1[0]==22.0, ltemp1[1]==33.0, 
            ltemp1[2]==44.0 && l2.value()==22.0), 
           TRUE);
  TEST_RUN("l2.sublist(ltemp1, l1)", l2.sublist(ltemp1, l1),
           (ltemp1.length()==0), TRUE);
  TEST_RUN("l2.sublist(ltemp1, l4)", l2.sublist(ltemp1, l4), 
           (ltemp1.length()==0), TRUE);
  TEST_RUN("l4.sublist(ltemp1, l2)", l4.sublist(ltemp1, l2), 
           (ltemp1==l4 && l4.value()==11.0), TRUE);
  TEST_RUN("l1.sublist(ltemp1, l2)", l1.sublist(ltemp1, l2), 
           (ltemp1.length()==0), TRUE);

  ltemp1 = CoolList<double>(4,55.0,33.0,11.0,66.0);
  TEST_RUN("l5.set_intersection(l6)", l5.set_intersection(l6), (l5==ltemp1), TRUE);

  ltemp1 = CoolList<double>(5,111.0,55.0,33.0,11.0,66.0);
  TEST_RUN("l5.set_union(l6)", l5.set_union(l6), (l5==ltemp1), TRUE);

  TEST_RUN("l5.set_difference(l6)", ltemp1.clear(); l5.set_difference(l6), 
           (l5==ltemp1), TRUE);

  ltemp1 = CoolList<double>(5,11.0,66.0,111.0,33.0,55.0);
  TEST_RUN("l5.set_xor(l6)", l5.set_xor(l6), (l5==ltemp1), TRUE);

}

void list_double_set_test2() {

  CoolList<double> ltemp1;
  CoolList<double> l7(2,2.0,3.0);
  CoolList<double> l8(3,1.0,2.0,3.0);
  CoolList<double> l9 = (CoolList<double> &) (l7 + l8);
  CoolList<double> l10(3,4.0,5.0,6.0);
  CoolList<double> l11(12,1.0,55.0,3.0,88.0,77.0,44.0,33.0,22.0,11.0,66.0,2.0,3.0);
  CoolList<double> l12(5,11.0,66.0,111.0,33.0,55.0);
  CoolList<double> l13;

  ltemp1 = CoolList<double>(5,2.0,3.0,1.0,2.0,3.0);
  TEST("l9 = l7 + l8", (l9==ltemp1), TRUE);
  ltemp1 = CoolList<double>(5,2.0,3.0,4.0,5.0,6.0);
  TEST_RUN("l7 += l10", l7 += l10, (l7==ltemp1), TRUE);

  ltemp1 = CoolList<double>(4,55.0,33.0,11.0,66.0);
  TEST_RUN("l11 &= l12", l11 &= l12, (l11==ltemp1), TRUE);

  ltemp1 = CoolList<double>(5,111.0,55.0,33.0,11.0,66.0);
  TEST_RUN("l11 |= l12", l11 |= l12, (l11==ltemp1), TRUE);

  TEST_RUN("l11 -= l12", ltemp1.clear(); l11 -= l12, (l11==ltemp1), TRUE);

  ltemp1 = CoolList<double>(5,11.0,66.0,111.0,33.0,55.0);
  TEST_RUN("l11 ^= l12", l11 ^= l12, (l11==ltemp1), TRUE);

  l11 = CoolList<double>(12,1.0,55.0,3.0,88.0,77.0,44.0,33.0,22.0,11.0,66.0,2.0,
                     3.0);
  l12 = CoolList<double>(5,11.0,66.0,111.0,33.0,55.0);

  ltemp1 = CoolList<double>(4,55.0,33.0,11.0,66.0);
  TEST_RUN("l13 = l11 & l12", l13 = l11 & l12, (l13==ltemp1), TRUE);

  ltemp1 = CoolList<double>(13,111.0,1.0,55.0,3.0,88.0,77.0,44.0,33.0,22.0,11.0,
                        66.0,2.0,3.0);
  TEST_RUN("l13 = l11 | l12", l13 = l11 | l12, (l13==ltemp1), TRUE);

  ltemp1 = CoolList<double>(8,1.0,3.0,88.0,77.0,44.0,22.0,2.0,3.0);
  TEST_RUN("l13 = l11 - l12", l13 = l11 - l12, (l13==ltemp1), TRUE);

  ltemp1 = CoolList<double>(9,1.0,3.0,88.0,77.0,44.0,22.0,2.0,3.0,111.0);
  TEST_RUN("l13 = l11 ^ l12", l13 = l11 ^ l12, (l13==ltemp1), TRUE);

}

void list_double_sort_test() {

  CoolList<double> l1(4,6.0,66.0,22.0,222.0);
  CoolList<double> l2(4,61.0,12.0,222.0,666.0);
  CoolList<double> l3(4,7.0,51.0,77.0,24.0);
  CoolList<double> ltemp1;

  ltemp1 = CoolList<double>(8,6.0,61.0,12.0,66.0,22.0,222.0,222.0,666.0);
  TEST_RUN("l1.merge(l2,&my_compare_double)", l1.merge(l2,&my_compare_double), 
           (l1==ltemp1), TRUE);

  ltemp1 = CoolList<double>(4,12.0,61.0,222.0,666.0);
  TEST_RUN("l2.sort(&my_compare_double)", l2.sort(&my_compare_double), 
           (l2==ltemp1), TRUE);

  ltemp1 = CoolList<double>(4,7.0,24.0,51.0,77.0);
  TEST_RUN("l3.sort(&my_compare_double)", l3.sort(&my_compare_double),
           (l3==ltemp1), TRUE);

}

void list_double_currentpos_test() {

  CoolList<double> l1(3,11.0,22.0,33.0);
  CoolList<double> l2;
  CoolList<double> ltemp1;

  TEST_RUN("l1.next()",l1.next(),l1.value(), 11.0);
  
  ltemp1 = CoolList<double>(4,11.0,55.0,22.0,33.0);
  TEST_RUN("l1.insert_after(55.0)", l1.insert_after(55.0), 
           (l1.value()==55.0 && l1==ltemp1), TRUE);

  TEST_RUN("l1.prev()",l1.prev(),l1.value(), 11.0);

  ltemp1 = CoolList<double>(5,66.0,11.0,55.0,22.0,33.0);
  TEST_RUN("l1.insert_before(66.0)", l1.insert_before(66.0), 
           (l1.value()==66.0 && l1==ltemp1), TRUE);

  ltemp1 = CoolList<double>(4,11.0,55.0,22.0,33.0);
  TEST("l1.remove()", (l1.remove()==66.0 && l1.value()==11.0 && l1==ltemp1),
        TRUE);

  ltemp1 = CoolList<double>(3,11.0,55.0,33.0);
  TEST_RUN("l1.remove()", l1.next();l1.next(),
           (l1.remove()==22.0 && l1.value()==33.0 && l1==ltemp1),
           TRUE);

  TEST_RUN("l1.position()", l1.prev(), (l1.position()==1 && l1.value()==55.0),
           TRUE);

  // l1 is (11.0,55.0,33.0)
  l2 = CoolList<double>(4,55.0,66.0,33.0,22.0);

  TEST_RUN("l1.next_union(l2)", l1.reset(),
           (l1.next_union(l2) && l1.value()==11.0), TRUE);
  TEST_RUN("l1.next_union(l2)", l1.next_union(l2); l1.next_union(l2),
           (l1.next_union(l2) && l1.value()==66.0),TRUE);
  TEST("l1.next_union(l2)", (l1.next_union(l2) && l1.value()==22.0),TRUE);
  TEST("l1.next_union(l2)", l1.next_union(l2), FALSE);

  TEST_RUN("l1.next_intersection(l2)", l1.reset(),
           (l1.next_intersection(l2) && l1.value()==55.0),
           TRUE);
  TEST("l1.next_intersection(l2)", 
       (l1.next_intersection(l2) && l1.value()==33.0),
       TRUE);
  TEST("l1.next_intersection(l2)", l1.next_intersection(l2), FALSE);
  TEST_RUN("l1.next_difference(l2)", l1.reset(),
           (l1.next_difference(l2) && l1.value()==11.0), 
           TRUE);
  TEST("l1.next_difference(l2)", l1.next_difference(l2), FALSE);
  TEST_RUN("l1.next_xor(l2)", l1.reset(),
           (l1.next_xor(l2) && l1.value()==11.0),
           TRUE);
  TEST_RUN("l1.next_xor(l2)", l1.next_xor(l2),
           (l1.next_xor(l2) && l1.value()==22.0),
           TRUE);
  TEST("l1.next_xor(l2)", l1.next_xor(l2), FALSE);
}

void list_double_io_test() {

  // ** an automated test would get input from file, rather than cin **
  CoolList<double> input_list;
//  cout << "\nTesting operator>> for CoolList<double>.";
//  cout << "\nExample of CoolList<double> is (1 2 3).";
//  cout << "\nPlease enter a CoolList<double> structure: ";
//  cin >> input_list;
//  cout << "\nInput_list is now set to: " << input_list << ".\n";

}

void test_list_double() {
  cout << "\n*** Testing List of Double ***\n\n";
  list_double_op_test1();
  list_double_op_test2();
  list_double_op_test3();
  list_double_op_test4();
  list_double_set_test1();
  list_double_set_test2();
  list_double_sort_test();
  list_double_currentpos_test();
  list_double_io_test();

}

// *******************
// List of char* tests
// *******************

void list_charp_op_test() {

  char* a = "a";
  char* b = "b";
  char* c = "c";
  char* d = "d";
  char* e = "e";

  CoolList<char*> l0;
  TEST("CoolList<char*> l0", 1, 1);

  CoolList<char*> l1(c);
  TEST("CoolList<char*> l1(c)", 1, 1);

  CoolList<char*> l2(b,l1);
  TEST("CoolList<char*> l2(b,l1)", 1, 1);

  CoolList<char*> l3(a,l2);
  TEST("CoolList<char*> l3(a,l2)", 1, 1);

  CoolList<char*> l4(3,a,b,c);
  TEST("CoolList<char*> l4(3,a,b,c)", 1, 1);

  CoolList<char*> l5(l4);
  TEST("CoolList<char*> l5(l4)", 1, 1);

  TEST("l3==l4", l3==l4, TRUE);
  TEST_RUN("l4[2]=d", l4[2]=d, l4[2]==d, TRUE);

}

// *******************
// List of char tests
// *******************

void list_char_op_test() {

  char a = 'a';
  char b = 'b';
  char c = 'c';
  char d = 'd';
  char e = 'e';

  CoolList<char> l0;
  TEST("CoolList<char> l0", 1, 1);

  CoolList<char> l1(c);
  TEST("CoolList<char> l1(c)", 1, 1);

  CoolList<char> l2(b,l1);
  TEST("CoolList<char> l2(b,l1)", 1, 1);

  CoolList<char> l3(a,l2);
  TEST("CoolList<char> l3(a,l2)", 1, 1);

//  CoolList<char> l4(3,a,b,c);                 // char in ... will cause 
//  TEST("CoolList<char> l4(3,a,b,c)", 1, 1);   // alignment error

  CoolList<char> l5(l3);
  TEST("CoolList<char> l5(l3)", 1, 1);

  TEST("l3==l5", l3==l5, TRUE);
  cout << l3 << &l5 << "\n";

  TEST_RUN("l5[2]=d", l5[2]=d, l5[2]==d, TRUE);

}

void test_list_chars() {
  cout << "\n*** Testing List of Char* ***\n\n";
  list_charp_op_test();
  cout << "\n*** Testing List of Char ***\n\n";
  list_char_op_test();

}

void test_list_vector() {

  cout << "\n*** Testing List of Vector of Int ***\n\n";
  CoolVector<int> v1(3,3,1,2,3);
  CoolVector<int> v2(3,3,1,2,3);

  CoolList<CoolVector<int> > l0;
  TEST("CoolList<CoolVector<int> > l0", l0.length()==0, TRUE);

  CoolList<CoolVector<int> > l1(v1);
  TEST("CoolList<CoolVector<int> > l1(v1)", (l1[0]==v1 && l1.length()==1), TRUE);

  CoolList<CoolVector<int>*> l2(2,&v1,&v2);
  TEST("CoolList<CoolVector<int>*> l2(2,&v1,&v2);", (l2[0]==&v1 && l2[1]==&v2), 1);

//  CoolList<CoolVector<int> > l3(2,v1,v2);   // canot pass vector in ...
//  TEST("CoolList<CoolVector<int> > l3(2,v1,v2);", (l3[0]==v1 && l3[1]==v2), 1);

  CoolList<CoolVector<int> > l3;
  l3.push(v1);
  l3.push_end(v2);
  TEST("CoolList<CoolVector<int> > l3(2,v1,v2);", (l3[0]==v1 && l3[1]==v2), 1);
  l3.push_end(v2);
}

void test_list_list() {
  cout << "\n*** Testing List of List ***\n\n";
  CoolList<int> l1(3,1,2,3);

  CoolList<CoolList<int> > l2(l1);
  TEST("CoolList<CoolList<int> > l2(l1)", (l2[0]==l1), 1);

  CoolList<CoolList<int>*> l4(&l1);
  TEST("CoolList<CoolList<int>*> l4(&l1)", (l4[0]==&l1), 1);

  CoolList<int>* l5 = &l1;
  CoolList<CoolList<int>*> l6(l5);
  TEST("CoolList<CoolList<int>*> l6(l5)", (l6[0]==&l1), 1);

//  CoolList<CoolList<int> > l3(2,l1,l1);  // canot pass list in ...
//  TEST("CoolList<CoolList<int> > l2(2,l1,l1)", (l2[0]==l1 && l2[1]==l1), 1);

  CoolList<CoolList<int> > l3;
  l3.push(l1);
  l3.push_end(l1);
  TEST("CoolList<CoolList<int> > l3(2,l1,l1)", (l3[0]==l1 && l3[1]==l1), 1);
}

void test_speed () {
  long n;
  for (n = 10; n <= 100000; n*=10) {    // length = 10, 100, 1000, 10000
    CoolTimer t;
    t.mark();
    {
      CoolList<int> l;
      for (int i = 0; i < n; i++) {
        l.push(i);
      }
    }
    cout << "Create and delete list of " << n << " elements took ";
    cout << t.real() << "milliseconds.\n";
  }
  for (n = 10000; n > 10; n/=10) {      // length = 10, 100, 1000, 10000
    CoolTimer t;
    t.mark();
    {
      CoolList<int> l;
      for (int i = 0; i < n; i++) {
        l.push(i);
      }
    }
    cout << "Create and delete list of " << n << " elements took ";
    cout << t.real() << "milliseconds.\n";
  }
}

void test_leak() { 
  for (;;) { 
    test_list_int(); 
    test_list_double();   
    test_list_chars() ;
    test_list_vector();
    test_list_list(); 
  } 
}

int main (void) {
  START("CoolList");
  test_list_int();
  test_list_double();
  test_list_chars();
  test_list_vector(); 
  test_list_list();
#if LEAK
  test_leak();
#endif
#if SPEED
  test_speed();
#endif
  SUMMARY();
  return 0;
}

