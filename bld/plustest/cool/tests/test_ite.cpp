#pragma inline_depth 0;
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
#include <cool/Iterator.h>
#include <cool/Vector.h>
#include <cool/Association.h>
#include <cool/Queue.h>
#include <cool/List.h>
#include <cool/Set.h>
#include <cool/Bit_Set.h>
#include <cool/Hash_Table.h>
//#include <cool/Package.h>
#include <cool/test.h>

#include <cool/Vector.C>
#include <cool/Pair.C>
#include <cool/Association.C>
#include <cool/Queue.C>
#include <cool/List.C>
#include <cool/Set.C>
#include <cool/Hash_Table.C>

void test_Iter_Vector() {
  CoolVector<int> v(10);
  TEST ("CoolVector<int> v(10)", v.capacity(), 10);
  CoolIterator<CoolVector<int> > iv;
  TEST ("CoolIterator<CoolVector> iv", 1, 1);
  TEST_RUN ("for (int i=0;i<10;i++) v.push(i)",
            for (int i = 0; i < 10; i++) v.push(i), v.length(), 10);
  TEST ("v.reset()", (v.reset(), 1),1);
  TEST ("iv = v.current_position()", (iv = v.current_position(), int(iv)), INVALID);
  TEST ("v.next; v.value()==0;", (v.next (), v.value()), 0);
  TEST ("v.next; v.value()==1;", (v.next (), v.value()), 1);
  TEST ("v.current_position() = iv", (v.current_position()=iv, int(iv)), INVALID);
  TEST ("v.next(); v.value()==0;", (v.next(),v.value()), 0);
}

void test_Iter_Association() {
  typedef CoolPair<int,char*> KVHack;
  CoolAssociation<int,char*> a(10);
  TEST ("CoolAssociation<int,char*> a(10)", a.capacity(), 10);
  CoolIterator<CoolAssociation<int,char*> > ia;
  TEST ("CoolIterator<CoolAssociation> ia", 1, 1);
  TEST_RUN ("for (int i=0;i<10;i++) a.put(i,\"ABC\")",
            for (int i = 0; i < 10; i++) a.put(i, "ABC"), a.length(), 10);
  TEST ("a.reset()", (a.reset(), 1),1);
  TEST ("ia = a.current_position()", (ia = a.current_position(), int(ia)), INVALID);
  TEST ("a.next; a.value()==(0,\"ABC\");", 
        (a.next (), (a.key()==0 && !strcmp(a.value(),"ABC"))), 1);
  TEST ("a.next; a.value()==(1,\"ABC\");", 
        (a.next (), (a.key()==1 && !strcmp(a.value(),"ABC"))), 1);
  TEST ("a.current_position() = ia", (a.current_position()=ia, int(ia)), INVALID);
  TEST ("a.next; a.value()==(0,\"ABC\");", 
        (a.next (), (a.key()==0 && !strcmp(a.value(),"ABC"))), 1);
}

void test_Iter_List() {
  CoolList<int> l;
  TEST ("CoolList<int> l", 1, 1);
  CoolIterator<CoolList<int> > il;
  TEST ("CoolIterator<CoolList> il", 1, 1);
  TEST_RUN ("for (int i=0;i<10;i++) l.push(i)",
            for (int i = 0; i < 10; i++) l.push(i), l.length(), 10);
  TEST ("l.reset()", (l.reset(), 1),1);
  TEST ("il = l.current_position()", (il = l.current_position(), il), NULL);
  TEST ("l.next; l.value()==9;", (l.next (), l.value()), 9);
  TEST ("l.next; l.value()==8;", (l.next (), l.value()), 8);
  TEST ("l.current_position() = il", (l.current_position()=il, il), NULL);
  TEST ("l.next(); l.value()==9;", (l.next(),l.value()), 9);
}

void test_Iter_Queue() {
  CoolQueue<int> q(10);
  TEST ("CoolQueue<int> q(10)", q.capacity(), 10);
  CoolIterator<CoolQueue<int> > iq;
  TEST ("CoolIterator<CoolQueue> iq", 1, 1);
  TEST_RUN ("for (int i=0;i<10;i++) q.put(i)",
            for (int i = 0; i < 10; i++) q.put(i),q.length(), 10);
  TEST ("q.reset()", (q.reset(), 1),1);
  TEST ("iq = q.current_position()", (iq = q.current_position(), 0), 0);
  TEST ("q.next; q.value()==0;", (q.next (), q.value()), 0);
  TEST ("q.next; q.value()==1;", (q.next (), q.value()), 1);
  TEST ("q.current_position() = iq", (q.current_position()=iq, 0), 0);
  TEST ("q.next(); q.value()==0;", (q.next(),q.value()), 0);
}


void test_Iter_Set() {
  CoolSet<int> s(10);
  TEST ("CoolSet<int> s(10)", (s.capacity() >= 10), 1);
  CoolIterator<CoolSet<int> > is;
  TEST ("CoolIterator<CoolSet> is", 1, 1);
  TEST_RUN ("for (int i=0;i<10;i++) s.put(i)",
            for (int i = 0; i < 10; i++) s.put(i),s.length(), 10);
  TEST ("s.reset()", (s.reset(), 1),1);
  TEST ("is = s.current_position()", (is = s.current_position(), int(is)), INVALID);
  TEST ("s.next; s.value()==0;", (s.next (), s.value()), 0);
  TEST ("s.next; s.value()==1;", (s.next (), s.value()), 1);
  TEST ("s.current_position() = is", (s.current_position()=is, int(is)), INVALID);
  TEST ("s.next(); s.value()==0;", (s.next(),s.value()), 0);
 }

void test_Iter_Bit_Set() {
  CoolBit_Set b(10);
  TEST ("CoolBit_Set b(10)", (b.capacity() >= 10), 1);
  CoolIterator<CoolBit_Set> ib;
  TEST ("CoolIterator<CoolBit_Set> ib", 1, 1);
  TEST_RUN ("for (int i=0;i<10;i++) b.put(i)",
            for (int i = 0; i < 10; i++) b.put(i),b.length(), 10);
  TEST ("b.reset()", (b.reset(), 1),1);
  TEST ("ib = b.current_position()", (ib = b.current_position(), int(ib)), INVALID);
  TEST ("b.next; b.value()==0;", (b.next (), b.value()), 0);
  TEST ("b.next; b.value()==1;", (b.next (), b.value()), 1);
  TEST ("b.current_position() = ib", (b.current_position()=ib, int(ib)), INVALID);
  TEST ("b.next(); b.value()==0;", (b.next(),b.value()), 0);
}

void test_Iter_Hash_Table() {
  CoolHash_Table<int,CoolString> h(10);
  TEST ("CoolHash_Table<int,CoolString> h(10)", (h.capacity() >= 10), 1);
  CoolIterator<CoolHash_Table<int,CoolString> > ih;
  TEST ("CoolIterator<CoolHash_Table> ih", 1, 1);
  TEST_RUN ("for (int i=0;i<10;i++) h.put(i,CoolString(\"ABC\"))",
            for (int i=0; i < 10; i++) h.put(i, CoolString("ABC")),h.length(), 10);
  TEST ("h.reset()", (h.reset(), 1),1);
  TEST ("ih = h.current_position()", (ih = h.current_position(), int(ih)), INVALID);
  TEST ("h.next; h.value()==(0,CoolString(\"ABC\"));", 
        (h.next (), (h.key()==0 && !strcmp(h.value(),"ABC"))), 1);
  TEST ("h.next; h.value()==(1,CoolString(\"ABC\"));", 
        (h.next (), (h.key()==1 && !strcmp(h.value(),"ABC"))), 1);
  TEST ("h.current_position() = ih", (h.current_position()=ih, int(ih)), INVALID);
  TEST ("h.next; h.value()==(0,CoolString(\"ABC\"));", 
        (h.next (), (h.key()==0 && !strcmp(h.value(),"ABC"))), 1);
 }
/*
void test_Iter_Package() {
  Package p(10);
  TEST ("Package p(10)", (p.capacity() >= 10), 1);
  CoolIterator<Package> ip;
  TEST ("CoolIterator<Package> ip", 1, 1);
  Symbol* sym_a = p.intern("A");
  Symbol* sym_b = p.intern("B");
  Symbol* sym_c = p.intern("C");
  Symbol* sym_d = p.intern("D");
  Symbol* sym_e = p.intern("E");
  Symbol* sym_f = p.intern("F");
  Symbol* sym_g = p.intern("G");
  Symbol* sym_h = p.intern("H");
  Symbol* sym_i = p.intern("I");
  Symbol* sym_j = p.intern("J");
  TEST ("p.intern(\"ABCDEFGHIJ\")",p.length(), 10);
  TEST ("p.reset()", (p.reset(), 1),1);
  TEST ("ip = p.current_position()", (ip = p.current_position(), ip), INVALID);
  TEST ("p.next; p.value()", (p.next (), p.value()), sym_b);
  TEST ("p.next; p.value()", (p.next (), p.value()), sym_e);
  TEST ("p.current_position() = ip", (p.current_position()=ip, ip), INVALID);
  TEST ("p.next; p.value()", (p.next (), p.value()), sym_b);
}
*/


void test_leak() {
  for (;;) {
  test_Iter_Vector();
  test_Iter_Association();
  test_Iter_List();
  test_Iter_Queue();
  test_Iter_Set();
  test_Iter_Bit_Set();
  test_Iter_Hash_Table();
//  test_Iter_Package();
  }
}

int main () {
  START("CoolIterator");
  test_Iter_Vector();
  test_Iter_Association();
  test_Iter_List();
  test_Iter_Queue();
  test_Iter_Set();
  test_Iter_Bit_Set();
  test_Iter_Hash_Table();
#if LEAK
  test_leak();
#endif
  SUMMARY();
  return 0;
}
