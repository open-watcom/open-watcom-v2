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

#include <cool/test.h>
#include <cool/Bit_Set.h>

void test_bit_set1() {
  CoolBit_Set b0;
  TEST ("CoolBit_Set b0", b0.length(), 0);
  TEST ("b0.capacity()", b0.capacity(), 0);
  TEST ("b0.is_empty()", b0.is_empty(), TRUE);
  CoolBit_Set b1(12);
  TEST ("CoolBit_Set b1(12)", b1.length(), 0);
  TEST ("b1.capacity()", b1.capacity(), 16);
  TEST ("b1.is_empty()", b1.is_empty(), TRUE);
  TEST ("b1.find(4)", b1.find(4), FALSE);
  TEST ("b1[4]==0", (b1[4] == 0), TRUE);
  TEST ("b0.put(3)", b0.put(3), TRUE);
  TEST ("b0.length()", b0.length(), 1);
  TEST ("b0.is_empty()", b0.is_empty(), FALSE);
  TEST ("b0.find(3)", b0.find(3), TRUE);
  TEST ("b0.value()", b0.value(), 3);
  TEST ("b0[3]==1", (b0[3]==1), TRUE);
  TEST ("b1.put(4)", b1.put(4), TRUE);
  TEST ("b1.find(4)", b1.find(4), TRUE);
  TEST ("b1[4]==1", (b1[4] == 1), TRUE);
  TEST ("b1.resize(18)", (b1.resize(18), 1),1);
  TEST ("b1.capacity()", b1.capacity(), 24);
  TEST ("b1.find(20)", b1.find(20), FALSE);
  TEST ("b0.put(2,5)", b0.put (2,5), TRUE);
  TEST ("b0.find(2)", b0.find(2), TRUE);
  TEST ("b0.value()", b0.value(), 2);
  TEST ("b0.find(3)", b0.find(3), TRUE);
  TEST ("b0.value()", b0.value(), 3);
  TEST ("b0.find(4)", b0.find(4), TRUE);
  TEST ("b0.value()", b0.value(), 4);
  TEST ("b0.find(5)", b0.find(5), TRUE);
  TEST ("b0.value()", b0.value(), 5);
  TEST ("b0.length()", b0.length(), 4);
  TEST ("b0.remove(3)", b0.remove(3), TRUE);
  TEST ("b0.find(3)", b0.find(3), FALSE);
  TEST ("b0[3] == FALSE", b0[3], 0);
  CoolBit_Set b2(b0);
  TEST ("CoolBit_Set b2(b0)", b2.capacity(), b0.capacity());
  TEST ("b2.length()", b2.length(), 3);
  TEST ("b0 == b2", (b0 == b2), TRUE);
  TEST ("b0 != b2", (b0 != b2), FALSE);
  TEST ("b0 == b1", (b0 == b1), FALSE);
  TEST ("b0 != b1", (b0 != b1), TRUE);
  TEST ("b2.clear()", (b2.clear(),b2.length()), 0);
  TEST ("b2 = b1", (b2 = b1, (b2 == b1)), TRUE);
  b0.reset();
  TEST ("b0.reset()", 1,1);
  TEST ("b0.next()", b0.next(), TRUE);
  TEST ("b0.value()", b0.value(), 2);
  TEST ("b0.next()", b0.next(), TRUE);
  TEST ("b0.value()", b0.value(), 4);
  TEST ("b0.next()", b0.next(), TRUE);
  TEST ("b0.value()", b0.value(), 5);
  TEST ("b0.prev()", b0.prev(), TRUE);
  TEST ("b0.value()", b0.value(), 4);
  TEST ("b0.next()", b0.next(), TRUE);
  TEST ("b0.value()", b0.value(), 5);
  TEST ("b0.next()", b0.next(), FALSE);
  TEST ("b0.put(9)", b0.put(9), TRUE);
  TEST ("b0.capacity()", b0.capacity(), 16);
  TEST ("b0.find(5)", b0.find(5), TRUE);
  TEST ("b0.remove()", b0.remove(), TRUE);
  TEST ("b0.length()", b0.length(), 3);
  TEST ("b0.remove(2,9)", b0.remove(2,9), TRUE);
  TEST ("b0.length()", b0.length(), 0);
  TEST ("b0.find(9)", b0.find(9), FALSE);
  TEST ("b0.value()", b0.value(), 9);
  TEST ("b0.put(3,11)", b0.put (3,11), TRUE);
  TEST ("b1.clear()", (b1.clear(),b1.length()), 0);
  TEST ("b0.length()", b0.length(), 9);
  TEST ("b1.put(4,9)", b1.put (4,9), TRUE);
  TEST ("b0.search(b1)", b0.search(b1), TRUE);
  TEST ("b1.put(19)", b1.put(19), TRUE);
  TEST ("b0.search(b1)", b0.search(b1), FALSE);
  TEST ("b2=~b0", (b2=~b0, b2!=b0), TRUE);
  CoolBit_Set b3(16);
  TEST ("CoolBit_Set b3(16)", b3.capacity(), 16);
  TEST ("b3.put(0,15)", b3.put(0,15), TRUE);
  TEST ("((b0 | b2) == b3)", ((b0 | b2) == b3), TRUE);
  TEST ("((b0 & b2) == -b3)", ((b0 & b2) == -b3), TRUE);
  TEST ("b1 = b0", (b1 = b0, b1 == b0), TRUE);
  TEST ("b0 |= b2", (b0 |= b2, b0 == b3), TRUE);
  TEST ("b0 = b1", (b0 = b1, b0 == b1), TRUE);
  TEST ("(b0.set_union(b2), b0 == b3)", (b0.set_union(b2), b0 == b3), TRUE);
  TEST ("b0 = b1", (b0 = b1, b0 == b1), TRUE);
  TEST ("b0 &= b2", (b0 &= b2, b0 == -b3), TRUE);
  TEST ("b0 = b1", (b0 = b1, b0 == b1), TRUE);
  TEST ("(b0.set_intersection(b2), b0 == -b3)",(b0.set_intersection(b2), b0 == -b3),TRUE);
  TEST ("b0.clear()", (b0.clear(), b0.length()), 0);
  TEST ("b1.clear()", (b1.clear(), b1.length()), 0);
  TEST ("b2.clear()", (b2.clear(), b2.length()), 0);
  TEST ("b3.clear()", (b3.clear(), b3.length()), 0);
  CoolBit_Set b5(16);
  TEST ("CoolBit_Set b5(16)", b5.capacity(), 16);
  TEST ("b5.put(0,1)", b5.put(0,1), TRUE);
  TEST ("b5.put(6,7)", b5.put(6,7), TRUE);
  CoolBit_Set b6(16);
  TEST ("CoolBit_Set b6(16)", b6.capacity(), 16);
  TEST ("b6.put(0,1)", b6.put(0,1), TRUE);
  CoolBit_Set b7(16);
  TEST ("CoolBit_Set b7(16)", b7.capacity(), 16);
  TEST ("b7.put(6,7)", b7.put(6,7), TRUE);
  TEST ("b0.put(0,5)", b0.put(0,5), TRUE);
  TEST ("b2.put(2,7)", b2.put(2,7), TRUE);
  TEST ("b1 = b0, b3 = b2", (b1=b0,b3=b2,(b1==b0 && b3==b2)), TRUE);
  TEST ("((b0 ^ b2) == b5)", ((b0 ^ b2) == b5), TRUE);
  TEST ("((b0 - b2) == b6)", ((b0 - b2) == b6), TRUE);
  TEST ("((b2 - b0) == b7)", ((b2 - b0) == b7), TRUE);
  TEST ("b0 ^= b2", ((b0 ^= b2), b0==b5), TRUE);
  TEST ("b0 = b1",(b0=b1,b0==b1),TRUE);
  TEST ("b0 -= b2", ((b0 -= b2), b0==b6), TRUE);
  TEST ("b0 = b1",(b0=b1,b0==b1),TRUE);
  TEST ("b2 -= b0", ((b2 -= b0), b2==b7), TRUE);
  TEST ("b2 = b3",(b2=b3,b2==b3),TRUE);
  TEST ("b0.set_xor(b2)", ((b0.set_xor(b2)), b0==b5), TRUE);
  TEST ("b0 = b1",(b0=b1,b0==b1),TRUE);
  TEST ("b0.set_difference(b2)", ((b0.set_difference(b2)), b0==b6), TRUE);
  TEST ("b0 = b1",(b0=b1,b0==b1),TRUE);
  TEST ("b2.set_difference(b0)", ((b2.set_difference(b0)), b2==b7), TRUE);
  CoolBit_Set b8;
  TEST ("CoolBit_Set b8", b8.capacity(), 0);
  TEST ("b8.put(2,4)", b8.put(2,4), TRUE);
  CoolBit_Set b9;
  TEST ("CoolBit_Set b9", b9.capacity(), 0);
  TEST ("b9.put(3,5)", b9.put(3,5), TRUE);
  b8.reset(); b9.reset();
  TEST ("b8.reset(), b9.reset()", 1,1);
  TEST ("b8.next_union(b9)", b8.next_union(b9), TRUE);
  TEST ("b8.value()", b8.value(), 2);
  TEST ("b8.next_union(b9)", b8.next_union(b9), TRUE);
  TEST ("b8.value()", b8.value(), 3);
  TEST ("b8.next_union(b9)", b8.next_union(b9), TRUE);
  TEST ("b8.value()", b8.value(), 4);
  TEST ("b8.next_union(b9)", b8.next_union(b9), TRUE);
  TEST ("b8.value()", b8.value(), 5);
  TEST ("b8.next_union(b9)", b8.next_union(b9), FALSE);
  b8.reset(); b9.reset();
  TEST ("b8.reset(), b9.reset()", 1,1);
  TEST ("b8.next_intersection(b9)", b8.next_intersection(b9), TRUE);
  TEST ("b8.value()", b8.value(), 3);
  TEST ("b8.next_intersection(b9)", b8.next_intersection(b9), TRUE);
  TEST ("b8.value()", b8.value(), 4);
  TEST ("b8.next_intersection(b9)", b8.next_intersection(b9), FALSE);
  b8.reset(); b9.reset();
  TEST ("b8.reset(), b9.reset()", 1,1);
  TEST ("b8.next_difference(b9)", b8.next_difference(b9), TRUE);
  TEST ("b8.value()", b8.value(), 2);
  TEST ("b8.next_difference(b9)", b8.next_difference(b9), FALSE);
  b8.reset(); b9.reset();
  TEST ("b8.reset(), b9.reset()", 1,1);
  TEST ("b8.next_xor(b9)", b8.next_xor(b9), TRUE);
  TEST ("b8.value()", b8.value(), 2);
  TEST ("b8.next_xor(b9)", b8.next_xor(b9), TRUE);
  TEST ("b8.value()", b8.value(), 5);
  TEST ("b8.next_xor(b9)", b8.next_xor(b9), FALSE);
}

void test_bit_set2() {
  CoolBit_Set b(10);
  TEST ("CoolBit_Set b(10)", (b.capacity() >= 10), 1);
  for (int i = 0; i < 10; i++) {
    b.put(i);
    cout << b << endl;
    }
  TEST ("for (int i=0;i<10;i++) b.put(i)", b.length(), 10);
}


void test_leak() {
  for (;;) {
    test_bit_set1();
    test_bit_set2();
  }
}

int main () {
  START("CoolBit_Set");
  test_bit_set1();
  test_bit_set2();
#if LEAK
  test_leak();
#endif
  SUMMARY();
  return 0;
}

