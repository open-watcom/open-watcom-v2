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
#include <cool/String.h>
#include <cool/N_Node.h>
#include <cool/D_Node.h>

#include <cool/N_Node.C>
#include <cool/D_Node.C>
#include <cool/Vector.C>

void test_NN () {
  CoolN_Node<short,3> n0;
  TEST ("CoolN_Node<short,3> n0", n0.is_leaf(), TRUE);
  TEST ("n0.set(4)", (n0.set(short(4)), n0.get()), 4);
  CoolN_Node<short,3>& n1 = *(new CoolN_Node<short,3>(short(3)));
  TEST ("CoolN_Node<short,3> n1(3)", n1.is_leaf(), TRUE);
  TEST ("n1.get()", n1.get(), 3);
  CoolN_Node<short,3>& n2 = *(new CoolN_Node<short,3>(n1));
  TEST ("CoolN_Node<short,3> n2(n1)", (n1.get() == n2.get()), 1);
  CoolN_Node<short,3>& n3 = *(new CoolN_Node<short,3>(short(5)));
  TEST ("CoolN_Node<short,3> n3(5)", n3.is_leaf(), TRUE);
  TEST ("n3.get()", n3.get(), 5);
  TEST ("n1.set(4)", (n1.set(short(4)), n1.get()), 4);
  TEST ("n0[0] = &n1", (n0[0] = &n1, (n0[0] == &n1)), 1);
  TEST ("n0.is_leaf()", n0.is_leaf(), FALSE);
  TEST ("n0.insert_before(n2,0)", n0.insert_before(n2,0), TRUE);
  TEST ("n0[0] == &n2", (n0[0] == &n2), 1);
  TEST ("n0[1] == &n1", (n0[1] == &n1), 1);
  TEST ("n0.insert_after(n3,0)", n0.insert_after(n3,0), TRUE);
  TEST ("n0[0] == &n2", (n0[0] == &n2), 1);
  TEST ("n0[1] == &n3", (n0[1] == &n3), 1);
  TEST ("n0[2] == &n1", (n0[2] == &n1), 1);

  CoolN_Node<CoolString,3> n8;
  TEST ("CoolN_Node<CoolString,3> n", n8.is_leaf(), TRUE);
  TEST ("n8.set(CoolString(\"AAA\"))", (n8.set(CoolString("AAA")), strcmp (n8.get(), "AAA")), 0);
  CoolN_Node<CoolString,3>& n9 = *(new CoolN_Node<CoolString,3>(CoolString("BBB")));
  TEST ("CoolN_Node<CoolString,3> n9(CoolString(\"BBB\"))", n9.is_leaf(), TRUE);
  TEST ("n9.get()", strcmp (n9.get(), "BBB"),0);
  CoolN_Node<CoolString,3>& n10 = *(new CoolN_Node<CoolString,3>(n9));
  TEST ("CoolN_Node<CoolString,3> n10(n9)", strcmp (n9.get(), n10.get()), 0);
  CoolN_Node<CoolString,3>& n11 = *(new CoolN_Node<CoolString,3>(CoolString("CCC")));
  TEST ("CoolN_Node<CoolString,3> n11(CoolString(\"CCC\"))", n11.is_leaf(), TRUE);
  TEST ("n11.get()", strcmp (n11.get(), "CCC"), 0);
  TEST ("n9.set(CoolString(\"DDD\"))", (n9.set(CoolString("DDD")), strcmp (n9.get(), "DDD")), 0);
  TEST ("n8[0] = &n9", (n8[0] = &n9, (n8[0] == &n9)), 1);
  TEST ("n8.is_leaf()", n8.is_leaf(), FALSE);
  TEST ("n8.insert_before(n10,0)", n8.insert_before(n10,0), TRUE);
  TEST ("n8[0] == &n10", (n8[0] == &n10), 1);
  TEST ("n8[1] == &n9", (n8[1] == &n9), 1);
  TEST ("n8.insert_after(n11,0)", n8.insert_after(n11,0), TRUE);
  TEST ("n8[0] == &n10", (n8[0] == &n10), 1);
  TEST ("n8[1] == &n11", (n8[1] == &n11), 1);
  TEST ("n8[2] == &n9", (n8[2] == &n9), 1);
}

void test_DN () {
  CoolD_Node<short,3> d0;
  TEST ("CoolD_Node<short,3> d0", d0.is_leaf(), TRUE);
  TEST ("d0.set(4)", (d0.set(short(4)), d0.get()), 4);
  CoolD_Node<short,3>& d1 = *(new CoolD_Node<short,3>(short(3)));
  TEST ("CoolD_Node<short,3> d1(3)", d1.is_leaf(), TRUE);
  TEST ("d1.get()", d1.get(), 3);
  CoolD_Node<short,3>& d2 = *(new CoolD_Node<short,3>(d1));
  TEST ("CoolD_Node<short,3> d2(d1)", (d1.get() == d2.get()), 1);
  CoolD_Node<short,3>& d3 = *(new CoolD_Node<short,3>(short(5)));
  TEST ("CoolD_Node<short,3> d3(5)", d3.is_leaf(), TRUE);
  TEST ("d3.get()", d3.get(), 5);
  TEST ("d1.set(4)", (d1.set(short(4)), d1.get()), 4);
  TEST ("d0[0] = &d1", (d0[0] = &d1, (d0[0] == &d1)), 1);
  TEST ("d0.is_leaf()", d0.is_leaf(), FALSE);
  TEST ("d0.insert_before(d2,0)", d0.insert_before(d2,0), TRUE);
  TEST ("d0[0] == &d2", (d0[0] == &d2), 1);
  TEST ("d0[1] == &d1", (d0[1] == &d1), 1);
  TEST ("d0.insert_after(d3,0)", d0.insert_after(d3,0), TRUE);
  TEST ("d0[0] == &d2", (d0[0] == &d2), 1);
  TEST ("d0[1] == &d3", (d0[1] == &d3), 1);
  TEST ("d0[2] == &d1", (d0[2] == &d1), 1);

  CoolD_Node<CoolString,3> d8;
  TEST ("CoolD_Node<CoolString,3> n", d8.is_leaf(), TRUE);
  TEST ("d8.set(CoolString(\"AAA\"))", (d8.set(CoolString("AAA")), strcmp (d8.get(), "AAA")), 0);
  CoolD_Node<CoolString,3>& d9 = *(new CoolD_Node<CoolString,3>(CoolString("BBB")));
  TEST ("CoolD_Node<CoolString,3> d9(CoolString(\"BBB\"))", d9.is_leaf(), TRUE);
  TEST ("d9.get()", strcmp (d9.get(), "BBB"),0);
  CoolD_Node<CoolString,3>& d10 = *(new CoolD_Node<CoolString,3>(d9));
  TEST ("CoolD_Node<CoolString,3> d10(d9)", strcmp (d9.get(), d10.get()), 0);
  CoolD_Node<CoolString,3>& d11 = *(new CoolD_Node<CoolString,3>(CoolString("CCC")));
  TEST ("CoolD_Node<CoolString,3> d11(CoolString(\"CCC\"))", d11.is_leaf(), TRUE);
  TEST ("d11.get()", strcmp (d11.get(), "CCC"), 0);
  TEST ("d9.set(CoolString(\"DDD\"))", (d9.set(CoolString("DDD")), strcmp (d9.get(), "DDD")), 0);
  TEST ("d8[0] = &d9", (d8[0] = &d9, (d8[0] == &d9)), 1);
  TEST ("d8.is_leaf()", d8.is_leaf(), FALSE);
  TEST ("d8.insert_before(d10,0)", d8.insert_before(d10,0), TRUE);
  TEST ("d8[0] == &d10", (d8[0] == &d10), 1);
  TEST ("d8[1] == &d9", (d8[1] == &d9), 1);
  TEST ("d8.insert_after(d11,0)", d8.insert_after(d11,0), TRUE);
  TEST ("d8[0] == &d10", (d8[0] == &d10), 1);
  TEST ("d8[1] == &d11", (d8[1] == &d11), 1);
  TEST ("d8[2] == &d9", (d8[2] == &d9), 1);
}


void test_NN_charP() {
  CoolN_Node<char*,4> n4;
  TEST ("CoolN_Node<char*,4> n", n4.is_leaf(), TRUE);
  TEST ("n4.set(\"AAA\")", (n4.set("AAA"), strcmp (n4.get(), "AAA")), 0);
  CoolN_Node<char*,4>& n5 = *(new CoolN_Node<char*,4>("BBB"));
  TEST ("CoolN_Node<char*,4> n5(\"BBB\")", n5.is_leaf(), TRUE);
  TEST ("n5.get()", strcmp (n5.get(), "BBB"),0);
  CoolN_Node<char*,4>& n6 = *(new CoolN_Node<char*,4>(n5));
  TEST ("CoolN_Node<char*,4> n6(n5)", strcmp (n5.get(), n6.get()), 0);
  CoolN_Node<char*,4>& n7 = *(new CoolN_Node<char*,4>("CCC"));
  TEST ("CoolN_Node<char*,4> n7(\"CCC\")", n7.is_leaf(), TRUE);
  TEST ("n7.get()", strcmp (n7.get(), "CCC"), 0);
  TEST ("n5.set(\"DDD\")", (n5.set("DDD"), strcmp (n5.get(), "DDD")), 0);
  TEST ("n4[0] = &n5", (n4[0] = &n5, (n4[0] == &n5)), 1);
  TEST ("n4.is_leaf()", n4.is_leaf(), FALSE);
  TEST ("n4.insert_before(n6,0)", n4.insert_before(n6,0), TRUE);
  TEST ("n4[0] == &n6", (n4[0] == &n6), 1);
  TEST ("n4[1] == &n5", (n4[1] == &n5), 1);
  TEST ("n4.insert_after(n7,0)", n4.insert_after(n7,0), TRUE);
  TEST ("n4[0] == &n6", (n4[0] == &n6), 1);
  TEST ("n4[1] == &n7", (n4[1] == &n7), 1);
  TEST ("n4[2] == &n5", (n4[2] == &n5), 1);
}

void test_DN_charP() {
  CoolD_Node<char*,4> d4;
  TEST ("CoolD_Node<char*,4> n", d4.is_leaf(), TRUE);
  TEST ("d4.set(\"AAA\")", (d4.set("AAA"), strcmp (d4.get(), "AAA")), 0);
  CoolD_Node<char*,4>& d5 = *(new CoolD_Node<char*,4>("BBB"));
  TEST ("CoolD_Node<char*,4> d5(\"BBB\")", d5.is_leaf(), TRUE);
  TEST ("d5.get()", strcmp (d5.get(), "BBB"),0);
  CoolD_Node<char*,4>& d6 = *(new CoolD_Node<char*,4>(d5));
  TEST ("CoolD_Node<char*,4> d6(d5)", strcmp (d5.get(), d6.get()), 0);
  CoolD_Node<char*,4>& d7 = *(new CoolD_Node<char*,4>("CCC"));
  TEST ("CoolD_Node<char*,4> d7(\"CCC\")", d7.is_leaf(), TRUE);
  TEST ("d7.get()", strcmp (d7.get(), "CCC"), 0);
  TEST ("d5.set(\"DDD\")", (d5.set("DDD"), strcmp (d5.get(), "DDD")), 0);
  TEST ("d4[0] = &d5", (d4[0] = &d5, (d4[0] == &d5)), 1);
  TEST ("d4.is_leaf()", d4.is_leaf(), FALSE);
  TEST ("d4.insert_before(d6,0)", d4.insert_before(d6,0), TRUE);
  TEST ("d4[0] == &d6", (d4[0] == &d6), 1);
  TEST ("d4[1] == &d5", (d4[1] == &d5), 1);
  TEST ("d4.insert_after(d7,0)", d4.insert_after(d7,0), TRUE);
  TEST ("d4[0] == &d6", (d4[0] == &d6), 1);
  TEST ("d4[1] == &d7", (d4[1] == &d7), 1);
  TEST ("d4[2] == &d5", (d4[2] == &d5), 1);
}

void test_leak () {
  for (;;) {
    test_NN ();
    test_DN ();
    test_NN_charP();
    test_DN_charP();
  }
}

int main () {
  START("CoolN_Node and CoolD_Node");
  test_NN ();
  test_DN ();
  test_NN_charP();
  test_DN_charP();
#if LEAK
  test_leak ();
#endif
  SUMMARY();
  return 0;
}
