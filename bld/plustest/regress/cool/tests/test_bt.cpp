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
// Updated: JAM 08/19/92 -- modernized template syntax, remove macro hacks

#include <cool/String.h>
#include <cool/Binary_Node.h>
#include <cool/Binary_Node.C>
#include <cool/Binary_Tree.h>
#include <cool/Binary_Tree.C>
#include <cool/test.h>

void test_int_remove(CoolBinary_Tree<int>&);

void test_int_put () {  
  CoolBinary_Tree<int> b0;

  TEST ("b0.put(10)", b0.put(10), TRUE);
  TEST ("b0.tree_depth()", b0.tree_depth(), 0);
  TEST ("b0.put(7)", b0.put(7), TRUE);
  TEST ("b0.tree_depth()", b0.tree_depth(), 1);
  TEST ("b0.put(15)", b0.put(15), TRUE);
  TEST ("b0.tree_depth()", b0.tree_depth(), 1);
  TEST ("b0.put(8)", b0.put(8), TRUE);
  TEST ("b0.tree_depth()", b0.tree_depth(), 2);
  TEST ("b0.put(2)", b0.put(2), TRUE);
  TEST ("b0.tree_depth()", b0.tree_depth(), 2);
  TEST ("b0.put(12)", b0.put(12), TRUE);
  TEST ("b0.tree_depth()", b0.tree_depth(), 2);
  TEST ("b0.put(4)", b0.put(4), TRUE);
  TEST ("b0.tree_depth()", b0.tree_depth(), 3);
  TEST ("b0.put(6)", b0.put(6), TRUE);
  TEST ("b0.tree_depth()", b0.tree_depth(), 4);
  TEST ("b0.put(5)", b0.put(5), TRUE);
  TEST ("b0.tree_depth()", b0.tree_depth(), 5);
  TEST ("b0.put(14)", b0.put(14), TRUE);
  TEST ("b0.tree_depth()", b0.tree_depth(), 5);
  TEST ("b0.put(11)", b0.put(11), TRUE);
  TEST ("b0.tree_depth()", b0.tree_depth(), 5);
  TEST ("b0.count()", b0.count(), 11);

  TEST ("b0.find(5),b0.node()->is_leaf()", 
        (b0.find(5),b0.node()->is_leaf()), TRUE);
  TEST ("b0.find(8),b0.node()->is_leaf()",
        (b0.find(8),b0.node()->is_leaf()), TRUE);
  TEST ("b0.find(11),b0.node()->is_leaf()",
        (b0.find(11),b0.node()->is_leaf()), TRUE);
  TEST ("b0.find(14),b0.node()->is_leaf()",
        (b0.find(14),b0.node()->is_leaf()), TRUE);
  TEST ("b0.find(10),b0.node() == b0.get_root()",
        (b0.find(10),b0.node() == b0.get_root()), TRUE);
  test_int_remove (b0);
}

void test_int_remove (CoolBinary_Tree<int>& b0) {
  CoolBinary_Node<int>* n0;
    
  TEST ("b0.remove(7)", b0.remove(7), TRUE);
  TEST ("b0.find(7)", b0.find(7), FALSE);
  TEST ("b0.count()", b0.count(), 10);
  TEST ("b0.tree_depth()", b0.tree_depth(), 4);
  TEST ("b0.find(6)", b0.find(6), TRUE);
  TEST ("(b0.node() == b0.get_root()->get_ltree())",
         (b0.node() == b0.get_root()->get_ltree()), TRUE);
  TEST ("b0.find(5)",b0.find(5),TRUE);
  n0 = b0.node();
  TEST ("(n0 == (b0.find(4),b0.node()->get_rtree()))",
         (n0 == (b0.find(4),b0.node()->get_rtree())), TRUE);
  TEST ("b0.find(8)",b0.find(8),TRUE);
  n0 = b0.node();
  TEST ("(n0 == (b0.find(6),b0.node()->get_rtree()))",
         (n0 == (b0.find(6),b0.node()->get_rtree())), TRUE);

  TEST ("b0.remove(12)", b0.remove(12), TRUE);
  TEST ("b0.find(12)", b0.find(12), FALSE);
  TEST ("b0.count()", b0.count(), 9);
  TEST ("b0.tree_depth()", b0.tree_depth(), 4);
  TEST ("b0.find(11)",b0.find(11), TRUE);
  n0 = b0.node();
  TEST ("(n0 == (b0.find(15),b0.node()->get_ltree()))",
         (n0 == (b0.find(15),b0.node()->get_ltree())), TRUE);
  TEST ("b0.find(14)",b0.find(14), TRUE);
  n0 = b0.node();
  TEST ("(n0 == (b0.find(11),b0.node()->get_rtree()))",
         (n0 == (b0.find(11),b0.node()->get_rtree())), TRUE);


  TEST ("b0.remove(10)", b0.remove(10), TRUE);
  TEST ("b0.count()", b0.count(), 8);
  TEST ("b0.tree_depth()", b0.tree_depth(), 4);
  TEST ("b0.find(10)", b0.find(10), FALSE);
  TEST ("b0.get_root() == (b0.find(8),b0.node())",
        b0.get_root() == (b0.find(8),b0.node()), TRUE)
  TEST ("b0.find(6)",b0.find(6), TRUE);
  n0 = b0.node();
  TEST ("(n0 == (b0.find(8),b0.node()->get_ltree()))",
         (n0 == (b0.find(8),b0.node()->get_ltree())), TRUE);

  TEST ("b0.remove(5)", b0.remove(5), TRUE);
  TEST ("b0.count()", b0.count(), 7);
  TEST ("b0.tree_depth()", b0.tree_depth(), 3);

  TEST ("b0.remove(2)", b0.remove(2), TRUE);
  TEST ("b0.count()", b0.count(), 6);
  TEST ("b0.tree_depth()", b0.tree_depth(), 3);
  TEST ("b0.find(4)",b0.find(4), TRUE);
  n0 = b0.node();
  TEST ("(n0 == (b0.find(6),b0.node()->get_ltree()))",
         (n0 == (b0.find(6),b0.node()->get_ltree())), TRUE);

  TEST ("b0.remove(15)", b0.remove(15), TRUE);
  TEST ("b0.count()", b0.count(), 5);
  TEST ("b0.find(11)",b0.find(11), TRUE);
  TEST ("((b0.node())==(b0.get_root()->get_rtree()))",
         ((b0.node())==(b0.get_root()->get_rtree())), TRUE);

  TEST ("b0.remove(14)", b0.remove(14), TRUE);
  TEST ("b0.count()", b0.count(), 4);
  TEST ("b0.tree_depth()", b0.tree_depth(), 2);
  TEST ("b0.find(11),b0.node()->is_leaf()",
        (b0.find(11),b0.node()->is_leaf()), TRUE);

  TEST ("b0.remove(8)", b0.remove(8), TRUE);
  TEST ("b0.count()", b0.count(), 3);
  TEST ("b0.tree_depth()", b0.tree_depth(), 1);
  TEST ("b0.get_root()==(b0.find(6),b0.node())",
        (b0.get_root()==(b0.find(6),b0.node())), TRUE);
  TEST ("b0.find(4)",b0.find(4), TRUE);
  n0 = b0.node();
  TEST ("(n0==(b0.find(6),b0.node()->get_ltree()))",
         (n0==(b0.find(6),b0.node()->get_ltree())), TRUE);

  TEST ("b0.remove(11)", b0.remove(11), TRUE);
  TEST ("b0.count()", b0.count(), 2);
  TEST ("b0.tree_depth()", b0.tree_depth(), 1);

  TEST ("b0.remove(6)", b0.remove(6), TRUE);
  TEST ("b0.count()", b0.count(), 1);
  TEST ("b0.tree_depth()", b0.tree_depth(), 0);
  TEST ("b0.get_root()==(b0.find(4),b0.node())",
        (b0.get_root()==(b0.find(4),b0.node())), TRUE);
  TEST ("b0.get_root()->is_leaf()",
        (b0.get_root()->is_leaf()), TRUE);

  TEST ("b0.remove(4)", b0.remove(4), TRUE);
  TEST ("b0.count()", b0.count(), 0);
  TEST ("b0.tree_depth()", b0.tree_depth(), 0);
  TEST ("b0.get_root()==NULL", (b0.get_root()==NULL), TRUE);

}

void test_int () {  
  CoolBinary_Tree<int> b0;

  TEST ("CoolBinary_Tree<int> b0", b0.count(), 0);
  TEST ("b0.put(1)", b0.put(1), TRUE);
  TEST ("b0.count()", b0.count(), 1);
  TEST ("b0.find(1)", b0.find(1), TRUE);
  TEST ("b0.value()", b0.value(), 1);
  TEST ("b0.remove()", b0.remove(), TRUE);
  TEST ("b0.count()", b0.count(), 0);
  TEST ("b0.put(4)", b0.put(4), TRUE);
  TEST ("b0.count()", b0.count(), 1);
  TEST ("b0.put(8)", b0.put(8), TRUE);
  TEST ("b0.count()", b0.count(), 2);
  TEST ("b0.put(3)", b0.put(3), TRUE);
  TEST ("b0.count()", b0.count(), 3);
  TEST ("b0.put(1)", b0.put(1), TRUE);
  TEST ("b0.count()", b0.count(), 4);
  TEST ("b0.put(2)", b0.put(2), TRUE);
  TEST ("b0.count()", b0.count(), 5);
  TEST ("b0.put(6)", b0.put(6), TRUE);
  TEST ("b0.count()", b0.count(), 6);
  TEST ("b0.find(2)", b0.find(2), TRUE);
  TEST ("b0.value()", b0.value(), 2);
  TEST ("b0.reset()", (b0.reset(), 1), 1);
  TEST ("b0.next()", b0.next(), TRUE);
  TEST ("b0.value()", b0.value(), 1);
  TEST ("b0.next()", b0.next(), TRUE);
  TEST ("b0.value()", b0.value(), 2);
  TEST ("b0.next()", b0.next(), TRUE);
  TEST ("b0.value()", b0.value(), 3);
  TEST ("b0.next()", b0.next(), TRUE);
  TEST ("b0.value()", b0.value(), 4);
  TEST ("b0.prev()", b0.prev(), TRUE);
  TEST ("b0.value()", b0.value(), 3);
  TEST ("b0.prev()", b0.prev(), TRUE);
  TEST ("b0.value()", b0.value(), 2);
  TEST ("b0.next()", b0.next(), TRUE);
  TEST ("b0.value()", b0.value(), 3);
  TEST ("b0.next()", b0.next(), TRUE);
  TEST ("b0.value()", b0.value(), 4);
  TEST ("b0.next()", b0.next(), TRUE);
  TEST ("b0.value()", b0.value(), 6);
  TEST ("b0.next()", b0.next(), TRUE);
  TEST ("b0.value()", b0.value(), 8);
  TEST ("b0.count()", b0.count(), 6);
  TEST ("b0.find(99)", b0.find(99), FALSE);
  CoolBinary_Tree<int> b1(b0);
  TEST ("CoolBinary_Tree<int> b1(b0)", b1.count(), 6);
  TEST ("b1.remove(3)", b1.remove(3), TRUE);
  TEST ("b1.count()", b1.count(), 5);
  TEST ("b1.find(3)", b1.find(3), FALSE);
  TEST ("b0.balance()", (b0.balance(), b0.count()), 6);
  TEST ("b0.remove(3)", b0.remove(3), TRUE);
  TEST ("b0.count()", b0.count(), 5);
  TEST ("b0.find(3)", b0.find(3), FALSE);
  TEST ("b0.put(-3)", b0.put(-3), TRUE);
  TEST ("b0.count()", b0.count(), 6);
  TEST ("b0.put(18)", b0.put(18), TRUE);
  TEST ("b0.count()", b0.count(), 7);
  TEST ("b0.put(13)", b0.put(13), TRUE);
  TEST ("b0.count()", b0.count(), 8);
  TEST ("b0.put(1)", b0.put(1), FALSE);
  TEST ("b0.count()", b0.count(), 8);
  TEST ("b0.put(5)", b0.put(5), TRUE);
  TEST ("b0.count()", b0.count(), 9);
  TEST ("b0.put(17)", b0.put(17), TRUE);
  TEST ("b0.count()", b0.count(), 10);
  TEST ("b0.put(3)", b0.put(3), TRUE);
  TEST ("b0.count()", b0.count(), 11);
  TEST ("b0.balance()", (b0.balance(), b0.count()), 11);
  TEST ("b0.find(3)", b0.find(3), TRUE);
  TEST ("b0.find(8)", b0.find(8), TRUE);
  TEST ("b0.find(9)", b0.find(9), FALSE);
  TEST ("b0.find(-3)", b0.find(-3), TRUE);
  TEST ("b0.find(17)", b0.find(17), TRUE);
  TEST ("b1=b0", (b1=b0, b0.count() == b1.count()), 1);
  TEST ("b1.find(3)", b1.find(3), TRUE);
  TEST ("b1.find(8)", b1.find(8), TRUE);
  TEST ("b1.find(9)", b1.find(9), FALSE);
  TEST ("b1.find(-3)", b1.find(-3), TRUE);
  TEST ("b1.find(17)", b1.find(17), TRUE);
}  

int my_comp (char* const& s1, char* const& s2) {
  return strcmp (s1, s2);
}

void test_charP () {  
  CoolBinary_Tree<char*> s0;
  TEST ("CoolBinary_Tree<char*> s0", s0.count(), 0);
  TEST ("s0.set_compare(&my_comp)",(s0.set_compare(&my_comp),1),1);
  TEST ("s0.put(\"LLL\")", s0.put("LLL"), TRUE);
  TEST ("s0.count()", s0.count(), 1);
  TEST ("s0.find(\"LLL\")", s0.find("LLL"), TRUE);
  TEST ("s0.value()", (strcmp (s0.value(),"LLL")), 0);
  TEST ("s0.remove()", s0.remove(), TRUE);
  TEST ("s0.count()", s0.count(), 0);
  TEST ("s0.put(\"DDD\")", s0.put("DDD"), TRUE);
  TEST ("s0.count()", s0.count(), 1);
  TEST ("s0.put(\"HHH\")", s0.put("HHH"), TRUE);
  TEST ("s0.count()", s0.count(), 2);
  TEST ("s0.put(\"CCC\")", s0.put("CCC"), TRUE);
  TEST ("s0.count()", s0.count(), 3);
  TEST ("s0.put(\"LLL\")", s0.put("LLL"), TRUE);
  TEST ("s0.count()", s0.count(), 4);
  TEST ("s0.put(\"BBB\")", s0.put("BBB"), TRUE);
  TEST ("s0.count()", s0.count(), 5);
  TEST ("s0.put(\"FFF\")", s0.put("FFF"), TRUE);
  TEST ("s0.count()", s0.count(), 6);
  TEST ("s0.find(\"BBB\")", s0.find("BBB"), TRUE);
  TEST ("s0.value()", (strcmp (s0.value(),"BBB")), 0);
  TEST ("s0.reset()", (s0.reset(), 1), 1);
  TEST ("s0.next()", s0.next(), TRUE);
  TEST ("s0.value()", (strcmp (s0.value(),"BBB")), 0);
  TEST ("s0.next()", s0.next(), TRUE);
  TEST ("s0.value()", (strcmp (s0.value(),"CCC")), 0);
  TEST ("s0.next()", s0.next(), TRUE);
  TEST ("s0.value()", (strcmp (s0.value(),"DDD")), 0);
  TEST ("s0.next()", s0.next(), TRUE);
  TEST ("s0.value()", (strcmp (s0.value(),"FFF")), 0);
  TEST ("s0.prev()", s0.prev(), TRUE);
  TEST ("s0.value()", (strcmp (s0.value(),"DDD")), 0);
  TEST ("s0.prev()", s0.prev(), TRUE);
  TEST ("s0.value()", (strcmp (s0.value(),"CCC")), 0);
  TEST ("s0.next()", s0.next(), TRUE);
  TEST ("s0.value()", (strcmp (s0.value(),"DDD")), 0);
  TEST ("s0.next()", s0.next(), TRUE);
  TEST ("s0.value()", (strcmp (s0.value(),"FFF")), 0);
  TEST ("s0.next()", s0.next(), TRUE);
  TEST ("s0.value()", (strcmp (s0.value(),"HHH")), 0);
  TEST ("s0.next()", s0.next(), TRUE);
  TEST ("s0.value()", (strcmp (s0.value(),"LLL")), 0);
  TEST ("s0.count()", s0.count(), 6);
  TEST ("s0.find(\"JJJ\")", s0.find("JJJ"), FALSE);
  CoolBinary_Tree<char*> s1(s0);
  TEST ("CoolBinary_Tree<char*> s1(s0)", s1.count(), 6);
  TEST ("s0.balance()", (s0.balance(), s0.count()), 6);
  TEST ("s1.remove(\"CCC\")", s1.remove("CCC"), TRUE);
  TEST ("s1.count()", s1.count(), 5);
  TEST ("s1.find(\"CCC\")", s1.find("CCC"), FALSE);
  TEST ("s0.remove(\"CCC\")", s0.remove("CCC"), TRUE);
  TEST ("s0.count()", s0.count(), 5);
  TEST ("s0.find(\"CCC\")", s0.find("CCC"), FALSE);
  TEST ("s0.put(\"KKK\")", s0.put("KKK"), TRUE);
  TEST ("s0.count()", s0.count(), 6);
  TEST ("s0.put(\"MMM\")", s0.put("MMM"), TRUE);
  TEST ("s0.count()", s0.count(), 7);
  TEST ("s0.put(\"NNN\")", s0.put("NNN"), TRUE);
  TEST ("s0.count()", s0.count(), 8);
  TEST ("s0.put(\"LLL\")", s0.put("LLL"), FALSE);
  TEST ("s0.count()", s0.count(), 8);
  TEST ("s0.put(\"EEE\")", s0.put("EEE"), TRUE);
  TEST ("s0.count()", s0.count(), 9);
  TEST ("s0.put(\"GGG\")", s0.put("GGG"), TRUE);
  TEST ("s0.count()", s0.count(), 10);
  TEST ("s0.put(\"CCC\")", s0.put("CCC"), TRUE);
  TEST ("s0.count()", s0.count(), 11);
  TEST ("s0.balance()", (s0.balance(), s0.count()), 11);
  TEST ("s0.find(\"CCC\")", s0.find("CCC"), TRUE);
  TEST ("s0.find(\"HHH\")", s0.find("HHH"), TRUE);
  TEST ("s0.find(\"III\")", s0.find("III"), FALSE);
  TEST ("s0.find(\"KKK\")", s0.find("KKK"), TRUE);
  TEST ("s0.find(\"GGG\")", s0.find("GGG"), TRUE);
  TEST ("s1=s0", (s1=s0, s0.count() == s1.count()), 1);
  TEST ("s1.find(\"CCC\")", s1.find("CCC"), TRUE);
  TEST ("s1.find(\"HHH\")", s1.find("HHH"), TRUE);
  TEST ("s1.find(\"III\")", s1.find("III"), FALSE);
  TEST ("s1.find(\"KKK\")", s1.find("KKK"), TRUE);
  TEST ("s1.find(\"GGG\")", s1.find("GGG"), TRUE);
}  

int my_strcmp (const CoolString& s1, const CoolString& s2) {
  return strcmp(s1, s2);
}

void test_String() {
  CoolBinary_Tree<CoolString> s0;
  CoolString bbb("BBB"), ccc("CCC"), ddd("DDD"), eee("EEE");
  CoolString fff("FFF"), ggg("GGG"), hhh("HHH"), iii("III");
  CoolString jjj("JJJ"), kkk("KKK"), lll("LLL"), mmm("MMM"), nnn("NNN");
  
  TEST ("CoolBinary_Tree<CoolString> s0", s0.count(), 0);
  TEST ("s0.set_compare(&my_strcmp)",(s0.set_compare(&my_strcmp),1),1);
  TEST ("s0.put(CoolString(\"LLL\"))", s0.put(lll), TRUE);
  TEST ("s0.count()", s0.count(), 1);
  TEST ("s0.find(CoolString(\"LLL\"))", s0.find(lll), TRUE);
  TEST ("s0.value()", (strcmp (s0.value(),lll)), 0);
  TEST ("s0.remove()", s0.remove(), TRUE);
  TEST ("s0.count()", s0.count(), 0);
  TEST ("s0.put(CoolString(\"DDD\"))", s0.put(ddd), TRUE);
  TEST ("s0.count()", s0.count(), 1);
  TEST ("s0.put(CoolString(\"HHH\"))", s0.put(hhh), TRUE);
  TEST ("s0.count()", s0.count(), 2);

  TEST ("s0.put(CoolString(\"CCC\"))", s0.put(ccc), TRUE);
  TEST ("s0.count()", s0.count(), 3);
  TEST ("s0.put(CoolString(\"LLL\"))", s0.put(lll), TRUE);
  TEST ("s0.count()", s0.count(), 4);
  TEST ("s0.put(CoolString(\"BBB\"))", s0.put(bbb), TRUE);
  TEST ("s0.count()", s0.count(), 5);
  TEST ("s0.put(CoolString(\"FFF\"))", s0.put(fff), TRUE);
  TEST ("s0.count()", s0.count(), 6);

  TEST ("s0.find(CoolString(\"BBB\"))", s0.find(bbb), TRUE);
  TEST ("s0.value()", (strcmp (s0.value(),bbb)), 0);
  TEST ("s0.reset()", (s0.reset(), 1), 1);
  TEST ("s0.next()", s0.next(), TRUE);
  TEST ("s0.value()", (strcmp (s0.value(),bbb)), 0);
  TEST ("s0.next()", s0.next(), TRUE);
  TEST ("s0.value()", (strcmp (s0.value(),ccc)), 0);
  TEST ("s0.next()", s0.next(), TRUE);
  TEST ("s0.value()", (strcmp (s0.value(),ddd)), 0);
  TEST ("s0.next()", s0.next(), TRUE);
  TEST ("s0.value()", (strcmp (s0.value(),fff)), 0);
  TEST ("s0.prev()", s0.prev(), TRUE);
  TEST ("s0.value()", (strcmp (s0.value(),ddd)), 0);
  TEST ("s0.prev()", s0.prev(), TRUE);
  TEST ("s0.value()", (strcmp (s0.value(),ccc)), 0);
  TEST ("s0.next()", s0.next(), TRUE);
  TEST ("s0.value()", (strcmp (s0.value(),ddd)), 0);
  TEST ("s0.next()", s0.next(), TRUE);
  TEST ("s0.value()", (strcmp (s0.value(),fff)), 0);
  TEST ("s0.next()", s0.next(), TRUE);
  TEST ("s0.value()", (strcmp (s0.value(),hhh)), 0);
  TEST ("s0.next()", s0.next(), TRUE);
  TEST ("s0.value()", (strcmp (s0.value(),lll)), 0);
  TEST ("s0.count()", s0.count(), 6);
  TEST ("s0.find(CoolString(\"JJJ\"))", s0.find(jjj), FALSE);


  CoolBinary_Tree<CoolString> s1(s0);
  TEST ("CoolBinary_Tree<CoolString> s1(s0)", s1.count(), 6);
  TEST ("s0.balance()", (s0.balance(), s0.count()), 6);
  TEST ("s1.remove(CoolString(\"CCC\"))", s1.remove(ccc), TRUE);
  TEST ("s1.count()", s1.count(), 5);
  TEST ("s1.find(CoolString(\"CCC\"))", s1.find(ccc), FALSE);
  TEST ("s0.remove(CoolString(\"CCC\"))", s0.remove(ccc), TRUE);
  TEST ("s0.count()", s0.count(), 5);
  TEST ("s0.find(CoolString(\"CCC\"))", s0.find(ccc), FALSE);
  TEST ("s0.put(CoolString(\"KKK\"))", s0.put(kkk), TRUE);
  TEST ("s0.count()", s0.count(), 6);
  TEST ("s0.put(CoolString(\"MMM\"))", s0.put(mmm), TRUE);
  TEST ("s0.count()", s0.count(), 7);
  TEST ("s0.put(CoolString(\"NNN\"))", s0.put(nnn), TRUE);
  TEST ("s0.count()", s0.count(), 8);
  TEST ("s0.put(CoolString(\"LLL\"))", s0.put(lll), FALSE);
  TEST ("s0.count()", s0.count(), 8);
  TEST ("s0.put(CoolString(\"EEE\"))", s0.put(eee), TRUE);
  TEST ("s0.count()", s0.count(), 9);
  TEST ("s0.put(CoolString(\"GGG\"))", s0.put(ggg), TRUE);
  TEST ("s0.count()", s0.count(), 10);
  TEST ("s0.put(CoolString(\"CCC\"))", s0.put(ccc), TRUE);
  TEST ("s0.count()", s0.count(), 11);
  TEST ("s0.balance()", (s0.balance(), s0.count()), 11);
  TEST ("s0.find(CoolString(\"CCC\"))", s0.find(ccc), TRUE);
  TEST ("s0.find(CoolString(\"HHH\"))", s0.find(hhh), TRUE);
  TEST ("s0.find(CoolString(\"III\"))", s0.find(iii), FALSE);
  TEST ("s0.find(CoolString(\"KKK\"))", s0.find(kkk), TRUE);
  TEST ("s0.find(CoolString(\"GGG\"))", s0.find(ggg), TRUE);
  TEST ("s1=s0", (s1=s0, s0.count() == s1.count()), 1);
  TEST ("s1.find(CoolString(\"CCC\"))", s1.find(ccc), TRUE);
  TEST ("s1.find(CoolString(\"HHH\"))", s1.find(hhh), TRUE);
  TEST ("s1.find(CoolString(\"III\"))", s1.find(iii), FALSE);
  TEST ("s1.find(CoolString(\"KKK\"))", s1.find(kkk), TRUE);
  TEST ("s1.find(CoolString(\"GGG\"))", s1.find(ggg), TRUE);
}  


void test_equal (void) {
  CoolBinary_Tree<int> b0, b1;
  for (int i = 0; i < 10; i++) {
    b0.put (i);
    b1.put (i);
  }
  TEST ("b0 == b1", b0 == b1, TRUE);
  b0.put (11);
  TEST ("b0 != b1", b0 != b1, TRUE);
}

void test_leak () {
  for (;;) {
    test_int_put ();
    test_int ();
    test_charP ();
    test_String ();
    test_equal ();
  }
}

int main () {
  START("CoolBinary_Tree");
  test_int_put ();
  test_int ();
  test_charP ();
  test_String ();
  test_equal ();
#if LEAK
  test_leak ();
#endif
  SUMMARY ();
  return 0;
}
