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
#include <cool/AVL_Tree.h>
#include <cool/test.h>

#include <cool/Binary_Node.C>
#include <cool/Binary_Tree.C>
#include <cool/AVL_Tree.C>

void test_int_remove (CoolAVL_Tree<int>&);

void test_int_insert () {  
  CoolAVL_Tree<int> b0;
  CoolBinary_Node<int>* n0;

  TEST ("b0.put(8)", b0.put(8), TRUE);
  TEST ("b0.count() 1", b0.count(), 1);
  TEST ("b0.tree_depth() 0", b0.tree_depth(), 0);

  TEST ("b0.put(4)", b0.put(4), TRUE);
  TEST ("b0.count()", b0.count(), 2);
  TEST ("b0.tree_depth()", b0.tree_depth(), 1);

  TEST ("b0.put(10)", b0.put(10), TRUE);
  TEST ("b0.count()", b0.count(), 3);
  TEST ("b0.tree_depth()", b0.tree_depth(), 1);

  TEST ("b0.put(2)", b0.put(2), TRUE);
  TEST ("b0.count()", b0.count(), 4);
  TEST ("b0.tree_depth()", b0.tree_depth(), 2);

  TEST ("b0.put(6)", b0.put(6), TRUE);
  TEST ("b0.count()", b0.count(), 5);
  TEST ("b0.tree_depth()", b0.tree_depth(), 2);

  TEST ("b0.put(5)", b0.put(5), TRUE);
  TEST ("b0.count()", b0.count(), 6);
  TEST ("b0.tree_depth()", b0.tree_depth(), 2);
  TEST ("b0.get_root()->get()",b0.get_root()->get(), 6);
  TEST ("b0.find(4)", b0.find(4), TRUE);
  TEST ("(b0.node() == b0.get_root()->get_ltree())",
         (b0.node() == b0.get_root()->get_ltree()), TRUE);
  TEST ("b0.find(8)", b0.find(8), TRUE);
  TEST ("(b0.node() == b0.get_root()->get_rtree())",
         (b0.node() == b0.get_root()->get_rtree()), TRUE);
  TEST ("b0.find(5)",b0.find(5),TRUE);
  n0 = b0.node();
  TEST ("(n0 == (b0.find(4),b0.node()->get_rtree()))",
         (n0 == (b0.find(4),b0.node()->get_rtree())), TRUE);

  TEST ("b0.put(9)", b0.put(9), TRUE);
  TEST ("b0.count()", b0.count(), 7);
  TEST ("b0.tree_depth()", b0.tree_depth(), 2);
  TEST ("b0.find(9)", b0.find(9), TRUE);
  TEST ("(b0.node() == b0.get_root()->get_rtree())",
         (b0.node() == b0.get_root()->get_rtree()), TRUE);
  TEST ("(b0.find(8),b0.node()->is_leaf())",
         (b0.find(8),b0.node()->is_leaf()),TRUE);
  TEST ("(b0.find(10),b0.node()->is_leaf())",
         (b0.find(10),b0.node()->is_leaf()),TRUE);

  TEST ("b0.put(1)", b0.put(1), TRUE);
  TEST ("b0.count()", b0.count(), 8);
  TEST ("b0.tree_depth()", b0.tree_depth(), 3);

  TEST ("b0.put(0)", b0.put(0), TRUE);
  TEST ("b0.count()", b0.count(), 9);
  TEST ("b0.tree_depth()", b0.tree_depth(), 3);
  TEST ("b0.find(1)",b0.find(1),TRUE);
  n0 = b0.node();
  TEST ("(n0 == (b0.find(4),b0.node()->get_ltree()))",
        (n0 == (b0.find(4),b0.node()->get_ltree())), TRUE);
  TEST ("(b0.find(0),b0.node()->is_leaf())",
         (b0.find(0),b0.node()->is_leaf()),TRUE);
  TEST ("(b0.find(2),b0.node()->is_leaf())",
         (b0.find(2),b0.node()->is_leaf()),TRUE);
  

  TEST ("b0.put(11)", b0.put(11), TRUE);
  TEST ("b0.count()", b0.count(), 10);
  TEST ("b0.tree_depth()", b0.tree_depth(), 3);

  TEST ("b0.put(12)", b0.put(12), TRUE);
  TEST ("b0.count()", b0.count(), 11);
  TEST ("b0.tree_depth()", b0.tree_depth(), 3);
  TEST ("b0.find(11)",b0.find(11),TRUE);
  n0 = b0.node();
  TEST ("(n0 == (b0.find(9),b0.node()->get_rtree()))",
         (n0 == (b0.find(9),b0.node()->get_rtree())), TRUE);
  TEST ("(b0.find(8),b0.node()->is_leaf())",
         (b0.find(8),b0.node()->is_leaf()),TRUE);
  TEST ("(b0.find(10),b0.node()->is_leaf())",
         (b0.find(10),b0.node()->is_leaf()),TRUE);
  test_int_remove(b0);
}

void test_int_remove (CoolAVL_Tree<int>& b0) {
  CoolBinary_Node<int>* n0;

  TEST ("b0.remove(12)", b0.remove(12), TRUE);
  TEST ("b0.count()", b0.count(), 10);
  TEST ("b0.tree_depth()", b0.tree_depth(), 3);

  TEST ("b0.remove(8)", b0.remove(8), TRUE);
  TEST ("b0.count()", b0.count(), 9);
  TEST ("b0.tree_depth()", b0.tree_depth(), 3);
  TEST ("b0.find(10)",b0.find(10),TRUE);
  n0 = b0.node();
  TEST ("(n0 == (b0.get_root()->get_rtree()))",
         (n0 == (b0.get_root()->get_rtree())), TRUE);
  TEST ("(b0.find(9),b0.node()->is_leaf())",
         (b0.find(9),b0.node()->is_leaf()),TRUE);
  TEST ("(b0.find(11),b0.node()->is_leaf())",
         (b0.find(11),b0.node()->is_leaf()),TRUE);

  TEST ("b0.remove(1)", b0.remove(1), TRUE);
  TEST ("b0.count()", b0.count(), 8);
  TEST ("b0.tree_depth()", b0.tree_depth(), 3);
  TEST ("b0.find(0)",b0.find(0),TRUE);
  n0 = b0.node();
  TEST ("(n0 == (b0.find(4),b0.node()->get_ltree()))",
         (n0 == (b0.find(4),b0.node()->get_ltree())), TRUE);
  TEST ("b0.find(2)",b0.find(2),TRUE);
  n0 = b0.node();
  TEST ("(n0 == (b0.find(0),b0.node()->get_rtree()))",
         (n0 == (b0.find(0),b0.node()->get_rtree())), TRUE);


  TEST ("b0.remove(5)", b0.remove(5), TRUE);
  TEST ("b0.count()", b0.count(), 7);
  TEST ("b0.tree_depth()", b0.tree_depth(), 2);
  TEST ("b0.find(2)",b0.find(2),TRUE);
  n0 = b0.node();
  TEST ("(n0 == (b0.find(6),b0.node()->get_ltree()))",
         (n0 == (b0.find(6),b0.node()->get_ltree())), TRUE);
  TEST ("b0.find(4)",b0.find(4),TRUE);
  n0 = b0.node();
  TEST ("(n0 == (b0.find(2),b0.node()->get_rtree()))",
         (n0 == (b0.find(2),b0.node()->get_rtree())), TRUE);

  TEST ("b0.remove(9)", b0.remove(9), TRUE);
  TEST ("b0.count()", b0.count(), 6);
  TEST ("b0.tree_depth()", b0.tree_depth(), 2);

  TEST ("b0.remove(6)", b0.remove(6), TRUE);
  TEST ("b0.count()", b0.count(), 5);
  TEST ("b0.tree_depth()", b0.tree_depth(), 2);
  TEST ("b0.get_root()->get()",b0.get_root()->get(), 4);
  TEST ("b0.find(2)",b0.find(2),TRUE);
  n0 = b0.node();
  TEST ("(n0 == (b0.find(4),b0.node()->get_ltree()))",
         (n0 == (b0.find(4),b0.node()->get_ltree())), TRUE);
  TEST ("b0.find(10)",b0.find(10),TRUE);
  n0 = b0.node();
  TEST ("(n0 == (b0.find(4),b0.node()->get_rtree()))",
         (n0 == (b0.find(4),b0.node()->get_rtree())), TRUE);

  TEST ("b0.remove(4)", b0.remove(4), TRUE);
  TEST ("b0.count()", b0.count(), 4);
  TEST ("b0.tree_depth()", b0.tree_depth(), 2);
  TEST ("b0.get_root()->get()",b0.get_root()->get(), 2);
  TEST ("b0.find(0)",b0.find(0),TRUE);
  n0 = b0.node();
  TEST ("(n0 == (b0.find(2),b0.node()->get_ltree()))",
         (n0 == (b0.find(2),b0.node()->get_ltree())), TRUE);

  TEST ("b0.remove(11)", b0.remove(11), TRUE);
  TEST ("b0.count()", b0.count(), 3);
  TEST ("b0.tree_depth()", b0.tree_depth(), 1);

  TEST ("b0.remove(2)", b0.remove(2), TRUE);
  TEST ("b0.count()", b0.count(), 2);
  TEST ("b0.tree_depth()", b0.tree_depth(), 1);
  TEST ("b0.get_root()->get()",b0.get_root()->get(), 0);
  TEST ("b0.find(10)",b0.find(10),TRUE);
  n0 = b0.node();
  TEST ("(n0 == (b0.find(0),b0.node()->get_rtree()))",
         (n0 == (b0.find(0),b0.node()->get_rtree())), TRUE);

  TEST ("b0.remove(0)", b0.remove(0), TRUE);
  TEST ("b0.count() 1", b0.count(), 1);
  TEST ("b0.tree_depth() 0", b0.tree_depth(), 0);
  TEST ("b0.get_root()->get()",b0.get_root()->get(), 10);

  TEST ("b0.remove(10)", b0.remove(10), TRUE);
  TEST ("b0.count() 1", b0.count(), 0);
  TEST ("b0.tree_depth() 0", b0.tree_depth(), 0);
}

void test_int_convert () {
  CoolBinary_Tree<int> bt0;
  bt0.put(1),bt0.put(2),bt0.put(3),bt0.put(4),bt0.put(5),bt0.put(6),bt0.put(7);
  TEST ("bt0.count()",bt0.count(),7);
  TEST ("bt0.tree_depth()",bt0.tree_depth(),6);
  CoolAVL_Tree<int> avl0(bt0);
  TEST ("avl0.count()", avl0.count(), 7);
  TEST ("avl0.tree_depth()", avl0.tree_depth(), 2);
  TEST ("avl0.put(9),avl0.put(8)", (avl0.put(9), avl0.put(8)), TRUE);
  TEST ("avl0.tree_depth()", avl0.tree_depth(), 3);
  TEST ("avl0=bt0,avl.count()", (avl0=bt0, avl0.count()), 7);
  TEST ("avl0.tree_depth()", avl0.tree_depth(), 2);
  TEST ("avl0.put(9),avl0.put(8)", (avl0.put(9), avl0.put(8)), TRUE);
  TEST ("avl0.tree_depth()", avl0.tree_depth(), 3);
  TEST ("avl0.clear(),bo.count()", (avl0.clear(), avl0.count()), 0);
}

void test_int () {
  CoolAVL_Tree<int> b0;
  TEST ("CoolAVL_Tree<int> b0", b0.count(), 0);
  TEST ("b0.put(1)", b0.put(1), TRUE);
  TEST ("b0.count()", b0.count(), 1);
  TEST ("b0.find(1)", b0.find(1), TRUE);
  TEST ("b0.value()", b0.value(), 1);
  TEST ("b0.remove()", b0.remove(), TRUE);
  TEST ("b0.count()", b0.count(), 0); 
  TEST ("b0.put(4)", b0.put(4), TRUE);
  TEST ("b0.count()", b0.count(), 1);
  TEST ("b0.tree_depth()", b0.tree_depth(), 0);
  TEST ("b0.put(8)", b0.put(8), TRUE);
  TEST ("b0.count()", b0.count(), 2);
  TEST ("b0.tree_depth()", b0.tree_depth(), 1);
  TEST ("b0.put(3)", b0.put(3), TRUE);
  TEST ("b0.count()", b0.count(), 3);
  TEST ("b0.tree_depth()", b0.tree_depth(), 1);
  TEST ("b0.put(1)", b0.put(1), TRUE);
  TEST ("b0.count()", b0.count(), 4);
  TEST ("b0.tree_depth()", b0.tree_depth(), 2);
  TEST ("b0.put(2)", b0.put(2), TRUE);
  TEST ("b0.count()", b0.count(), 5);
  TEST ("b0.tree_depth()", b0.tree_depth(), 2);
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
  CoolAVL_Tree<int> b1(b0);
  TEST ("CoolAVL_Tree<int> b1(b0)", b1.count(), 6);
  TEST ("b1.remove(3)", b1.remove(3), TRUE);
  TEST ("b1.count()", b1.count(), 5);
  TEST ("b1.find(3)", b1.find(3), FALSE);
  TEST ("b0.remove(3)", b0.remove(3), TRUE);
  TEST ("b0.count()", b0.count(), 5);
  TEST ("b0.find(3)", b0.find(3), FALSE);
  TEST ("b0.put(-3)", b0.put(-3), TRUE);
  TEST ("b0.tree_depth()", b0.tree_depth(), 2);
  TEST ("b0.count()", b0.count(), 6);
  TEST ("b0.put(18)", b0.put(18), TRUE);
  TEST ("b0.tree_depth()", b0.tree_depth(), 2);
  TEST ("b0.count()", b0.count(), 7);
  TEST ("b0.put(13)", b0.put(13), TRUE);
  TEST ("b0.tree_depth()", b0.tree_depth(), 3);
  TEST ("b0.count()", b0.count(), 8);
  TEST ("b0.put(1)", b0.put(1), FALSE);
  TEST ("b0.tree_depth()", b0.tree_depth(), 3);
  TEST ("b0.count()", b0.count(), 8);
  TEST ("b0.put(5)", b0.put(5), TRUE);
  TEST ("b0.tree_depth()", b0.tree_depth(), 3);
  TEST ("b0.count()", b0.count(), 9);
  TEST ("b0.put(17)", b0.put(17), TRUE);
  TEST ("b0.tree_depth()", b0.tree_depth(), 3);
  TEST ("b0.count()", b0.count(), 10);
  TEST ("b0.put(3)", b0.put(3), TRUE);
  TEST ("b0.tree_depth()", b0.tree_depth(), 3);
  TEST ("b0.count()", b0.count(), 11);
  TEST ("b0.find(3)", b0.find(3), TRUE);
  TEST ("b0.find(8)", b0.find(8), TRUE);
  TEST ("b0.find(9)", b0.find(9), FALSE);
  TEST ("b0.find(-3)", b0.find(-3), TRUE);
  TEST ("b0.find(17)", b0.find(17), TRUE);
  TEST ("b1=b0", (b1=b0, b0.count() == b1.count()), 1);
  TEST ("b1.tree_depth()", b1.tree_depth(), 3);
  TEST ("b1.find(3)", b1.find(3), TRUE);
  TEST ("b1.find(8)", b1.find(8), TRUE);
  TEST ("b1.find(9)", b1.find(9), FALSE);
  TEST ("b1.find(-3)", b1.find(-3), TRUE);
  TEST ("b1.find(17)", b1.find(17), TRUE);
  TEST ("b1.put(-4)", b1.put(-4), TRUE);
  TEST ("b1.tree_depth()", b1.tree_depth(), 3);
  TEST ("b1.count()", b1.count(), 12);
  TEST ("b1.put(-5)", b1.put(-5), TRUE);
  TEST ("b1.tree_depth()", b1.tree_depth(), 3);
  TEST ("b1.count()", b1.count(), 13);
  TEST ("b1.put(-6)", b1.put(-6), TRUE);
  TEST ("b1.tree_depth()", b1.tree_depth(), 4);
  TEST ("b1.count()", b1.count(), 14);
  TEST ("b1.put(27)", b1.put(27), TRUE);
  TEST ("b1.tree_depth()", b1.tree_depth(), 4);
  TEST ("b1.count()", b1.count(), 15);
  TEST ("b1.put(7)", b1.put(7), TRUE);
  TEST ("b1.tree_depth()", b1.tree_depth(), 4);
  TEST ("b1.count()", b1.count(), 16);
}  


typedef char* charP;
int my_comp (const charP& s1, const charP& s2) {
  return strcmp (s1, s2);
}

void test_charP () {  
  CoolAVL_Tree<char*> s0;
  TEST ("CoolAVL_Tree<char*> s0", s0.count(), 0);
  TEST ("s0.set_compare(&my_comp)",(s0.set_compare(&my_comp),1),1);
  TEST ("s0.put(\"aaa\")", s0.put("aaa"), TRUE);
  TEST ("s0.count()", s0.count(), 1);
  TEST ("s0.find(\"aaa\")", s0.find("aaa"), TRUE);
  TEST ("s0.value()", (strcmp (s0.value(), "aaa")), 0);
  TEST ("s0.remove()", s0.remove(), TRUE);
  TEST ("s0.count()", s0.count(), 0);
  TEST ("s0.put(\"ddd\")", s0.put("ddd"), TRUE);
  TEST ("s0.count()", s0.count(), 1);
  TEST ("s0.tree_depth()", s0.tree_depth(), 0);
  TEST ("s0.put(\"hhh\")", s0.put("hhh"), TRUE);
  TEST ("s0.count()", s0.count(), 2);
  TEST ("s0.tree_depth()", s0.tree_depth(), 1);
  TEST ("s0.put(\"ccc\")", s0.put("ccc"), TRUE);
  TEST ("s0.count()", s0.count(), 3);
  TEST ("s0.tree_depth()", s0.tree_depth(), 1);
  TEST ("s0.put(\"aaa\")", s0.put("aaa"), TRUE);
  TEST ("s0.count()", s0.count(), 4);
  TEST ("s0.tree_depth()", s0.tree_depth(), 2);
  TEST ("s0.put(\"bbb\")", s0.put("bbb"), TRUE);
  TEST ("s0.count()", s0.count(), 5);
  TEST ("s0.tree_depth()", s0.tree_depth(), 2);
  TEST ("s0.put(\"fff\")", s0.put("fff"), TRUE);
  TEST ("s0.count()", s0.count(), 6);
  TEST ("s0.find(\"bbb\")", s0.find("bbb"), TRUE);
  TEST ("s0.value()", (strcmp (s0.value(), "bbb")), 0);
  TEST ("s0.reset()", (s0.reset(), 1), 1);
  TEST ("s0.next()", s0.next(), TRUE);
  TEST ("s0.value()", (strcmp (s0.value(), "aaa")), 0);
  TEST ("s0.next()", s0.next(), TRUE);
  TEST ("s0.value()", (strcmp (s0.value(), "bbb")), 0);
  TEST ("s0.next()", s0.next(), TRUE);
  TEST ("s0.value()", (strcmp (s0.value(), "ccc")), 0);
  TEST ("s0.next()", s0.next(), TRUE);
  TEST ("s0.value()", (strcmp (s0.value(), "ddd")), 0);
  TEST ("s0.prev()", s0.prev(), TRUE);
  TEST ("s0.value()", (strcmp (s0.value(), "ccc")), 0);
  TEST ("s0.prev()", s0.prev(), TRUE);
  TEST ("s0.value()", (strcmp (s0.value(), "bbb")), 0);
  TEST ("s0.next()", s0.next(), TRUE);
  TEST ("s0.value()", (strcmp (s0.value(), "ccc")), 0);
  TEST ("s0.next()", s0.next(), TRUE);
  TEST ("s0.value()", (strcmp (s0.value(), "ddd")), 0);
  TEST ("s0.next()", s0.next(), TRUE);
  TEST ("s0.value()", (strcmp (s0.value(), "fff")), 0);
  TEST ("s0.next()", s0.next(), TRUE);
  TEST ("s0.value()", (strcmp (s0.value(), "hhh")), 0);
  TEST ("s0.count()", s0.count(), 6);
  TEST ("s0.find(\"ABCD\")", s0.find("ABCD"), FALSE);
  CoolAVL_Tree<char*> s1(s0);
  TEST ("CoolAVL_Tree<char*> s1(s0)", s1.count(), 6);
  TEST ("s1.remove(\"ccc\")", s1.remove("ccc"), TRUE);
  TEST ("s1.count()", s1.count(), 5);
  TEST ("s1.find(\"ccc\")", s1.find("ccc"), FALSE);
  TEST ("s0.remove(\"ccc\")", s0.remove("ccc"), TRUE);
  TEST ("s0.count()", s0.count(), 5);
  TEST ("s0.find(\"ccc\")", s0.find("ccc"), FALSE);
  TEST ("s0.put(\"XXX\")", s0.put("XXX"), TRUE);
  TEST ("s0.tree_depth()", s0.tree_depth(), 2);
  TEST ("s0.count()", s0.count(), 6);
  TEST ("s0.put(\"rrr\")", s0.put("rrr"), TRUE);
  TEST ("s0.tree_depth()", s0.tree_depth(), 2);
  TEST ("s0.count()", s0.count(), 7);
  TEST ("s0.put(\"mmm\")", s0.put("mmm"), TRUE);
  TEST ("s0.tree_depth()", s0.tree_depth(), 3);
  TEST ("s0.count()", s0.count(), 8);
  TEST ("s0.put(\"aaa\")", s0.put("aaa"), FALSE);
  TEST ("s0.tree_depth()", s0.tree_depth(), 3);
  TEST ("s0.count()", s0.count(), 8);
  TEST ("s0.put(\"eee\")", s0.put("eee"), TRUE);
  TEST ("s0.tree_depth()", s0.tree_depth(), 3);
  TEST ("s0.count()", s0.count(), 9);
  TEST ("s0.put(\"qqq\")", s0.put("qqq"), TRUE);
  TEST ("s0.tree_depth()", s0.tree_depth(), 3);
  TEST ("s0.count()", s0.count(), 10);
  TEST ("s0.put(\"ccc\")", s0.put("ccc"), TRUE);
  TEST ("s0.tree_depth()", s0.tree_depth(), 3);
  TEST ("s0.count()", s0.count(), 11);
  TEST ("s0.find(\"ccc\")", s0.find("ccc"), TRUE);
  TEST ("s0.find(\"hhh\")", s0.find("hhh"), TRUE);
  TEST ("s0.find(\"iii\")", s0.find("iii"), FALSE);
  TEST ("s0.find(\"XXX\")", s0.find("XXX"), TRUE);
  TEST ("s0.find(\"qqq\")", s0.find("qqq"), TRUE);
  TEST ("s1=s0", (s1=s0, s0.count() == s1.count()), 1);
  TEST ("s1.tree_depth()", s1.tree_depth(), 3);
  TEST ("s1.find(\"ccc\")", s1.find("ccc"), TRUE);
  TEST ("s1.find(\"hhh\")", s1.find("hhh"), TRUE);
  TEST ("s1.find(\"iii\")", s1.find("iii"), FALSE);
  TEST ("s1.find(\"XXX\")", s1.find("XXX"), TRUE);
  TEST ("s1.find(\"qqq\")", s1.find("qqq"), TRUE);
  TEST ("s1.put(\"WWW\")", s1.put("WWW"), TRUE);
  TEST ("s1.tree_depth()", s1.tree_depth(), 3);
  TEST ("s1.count()", s1.count(), 12);
  TEST ("s1.put(\"VVV\")", s1.put("VVV"), TRUE);
  TEST ("s1.tree_depth()", s1.tree_depth(), 3);
  TEST ("s1.count()", s1.count(), 13);
  TEST ("s1.put(\"UUU\")", s1.put("UUU"), TRUE);
  TEST ("s1.tree_depth()", s1.tree_depth(), 4);
  TEST ("s1.count()", s1.count(), 14);
  TEST ("s1.put(\"zzz\")", s1.put("zzz"), TRUE);
  TEST ("s1.tree_depth()", s1.tree_depth(), 4);
  TEST ("s1.count()", s1.count(), 15);
  TEST ("s1.put(\"ggg\")", s1.put("ggg"), TRUE);
  TEST ("s1.tree_depth()", s1.tree_depth(), 4);
  TEST ("s1.count()", s1.count(), 16);
}  

void test_String_2 (CoolAVL_Tree<CoolString>& s0) {
  CoolAVL_Tree<CoolString> s1(s0);
  TEST ("CoolAVL_Tree<CoolString> s1(s0)", s1.count(), 6);
  TEST ("s1.remove(CoolString(\"ccc\"))", s1.remove(CoolString("ccc")), TRUE);
  TEST ("s1.count()", s1.count(), 5);
  TEST ("s1.find(CoolString(\"ccc\"))", s1.find(CoolString("ccc")), FALSE);
  TEST ("s0.remove(CoolString(\"ccc\"))", s0.remove(CoolString("ccc")), TRUE);
  TEST ("s0.count()", s0.count(), 5);
  TEST ("s0.find(CoolString(\"ccc\"))", s0.find(CoolString("ccc")), FALSE);
  TEST ("s0.put(CoolString(\"XXX\"))", s0.put(CoolString("XXX")), TRUE);
  TEST ("s0.tree_depth()", s0.tree_depth(), 2);
  TEST ("s0.count()", s0.count(), 6);
  TEST ("s0.put(CoolString(\"rrr\"))", s0.put(CoolString("rrr")), TRUE);
  TEST ("s0.tree_depth()", s0.tree_depth(), 2);
  TEST ("s0.count()", s0.count(), 7);
  TEST ("s0.put(CoolString(\"mmm\"))", s0.put(CoolString("mmm")), TRUE);
  TEST ("s0.tree_depth()", s0.tree_depth(), 3);
  TEST ("s0.count()", s0.count(), 8);
  TEST ("s0.put(CoolString(\"aaa\"))", s0.put(CoolString("aaa")), FALSE);
  TEST ("s0.tree_depth()", s0.tree_depth(), 3);
  TEST ("s0.count()", s0.count(), 8);
  TEST ("s0.put(CoolString(\"eee\"))", s0.put(CoolString("eee")), TRUE);
  TEST ("s0.tree_depth()", s0.tree_depth(), 3);
  TEST ("s0.count()", s0.count(), 9);
  TEST ("s0.put(CoolString(\"qqq\"))", s0.put(CoolString("qqq")), TRUE);
  TEST ("s0.tree_depth()", s0.tree_depth(), 3);
  TEST ("s0.count()", s0.count(), 10);
  TEST ("s0.put(CoolString(\"ccc\"))", s0.put(CoolString("ccc")), TRUE);
  TEST ("s0.tree_depth()", s0.tree_depth(), 3);
  TEST ("s0.count()", s0.count(), 11);
  TEST ("s0.find(CoolString(\"ccc\"))", s0.find(CoolString("ccc")), TRUE);
  TEST ("s0.find(CoolString(\"hhh\"))", s0.find(CoolString("hhh")), TRUE);
  TEST ("s0.find(CoolString(\"iii\"))", s0.find(CoolString("iii")), FALSE);
  TEST ("s0.find(CoolString(\"XXX\"))", s0.find(CoolString("XXX")), TRUE);
  TEST ("s0.find(CoolString(\"qqq\"))", s0.find(CoolString("qqq")), TRUE);
  TEST ("s1=s0", (s1=s0, s0.count() == s1.count()), 1);
  TEST ("s1.tree_depth()", s1.tree_depth(), 3);
  TEST ("s1.find(CoolString(\"ccc\"))", s1.find(CoolString("ccc")), TRUE);
  TEST ("s1.find(CoolString(\"hhh\"))", s1.find(CoolString("hhh")), TRUE);
  TEST ("s1.find(CoolString(\"iii\"))", s1.find(CoolString("iii")), FALSE);
  TEST ("s1.find(CoolString(\"XXX\"))", s1.find(CoolString("XXX")), TRUE);
  TEST ("s1.find(CoolString(\"qqq\"))", s1.find(CoolString("qqq")), TRUE);
  TEST ("s1.put(CoolString(\"WWW\"))", s1.put(CoolString("WWW")), TRUE);
  TEST ("s1.tree_depth()", s1.tree_depth(), 3);
  TEST ("s1.count()", s1.count(), 12);
  TEST ("s1.put(CoolString(\"VVV\"))", s1.put(CoolString("VVV")), TRUE);
  TEST ("s1.tree_depth()", s1.tree_depth(), 3);
  TEST ("s1.count()", s1.count(), 13);
  TEST ("s1.put(CoolString(\"UUU\"))", s1.put(CoolString("UUU")), TRUE);
  TEST ("s1.tree_depth()", s1.tree_depth(), 4);
  TEST ("s1.count()", s1.count(), 14);
  TEST ("s1.put(CoolString(\"zzz\"))", s1.put(CoolString("zzz")), TRUE);
  TEST ("s1.tree_depth()", s1.tree_depth(), 4);
  TEST ("s1.count()", s1.count(), 15);
  TEST ("s1.put(CoolString(\"ggg\"))", s1.put(CoolString("ggg")), TRUE);
  TEST ("s1.tree_depth()", s1.tree_depth(), 4);
  TEST ("s1.count()", s1.count(), 16);
}  

int my_strcmp (const CoolString& s1, const CoolString& s2) {
  return strcmp (s1, s2);
}

void test_String () {
  CoolAVL_Tree<CoolString> s0;
  TEST ("CoolAVL_Tree<CoolString> s0", s0.count(), 0);
  TEST ("s0.set_compare(&my_strcmp)",(s0.set_compare(&my_strcmp),1),1);
  TEST ("s0.put(CoolString(\"aaa\"))", s0.put(CoolString("aaa")), TRUE);
  TEST ("s0.count()", s0.count(), 1);
  TEST ("s0.find(CoolString(\"aaa\"))", s0.find(CoolString("aaa")), TRUE);
  TEST ("s0.value()", (strcmp (s0.value(), "aaa")), 0);
  TEST ("s0.remove()", s0.remove(), TRUE);
  TEST ("s0.count()", s0.count(), 0);
  TEST ("s0.put(CoolString(\"ddd\"))", s0.put(CoolString("ddd")), TRUE);
  TEST ("s0.count()", s0.count(), 1);
  TEST ("s0.tree_depth()", s0.tree_depth(), 0);
  TEST ("s0.put(CoolString(\"hhh\"))", s0.put(CoolString("hhh")), TRUE);
  TEST ("s0.count()", s0.count(), 2);
  TEST ("s0.tree_depth()", s0.tree_depth(), 1);
  TEST ("s0.put(CoolString(\"ccc\"))", s0.put(CoolString("ccc")), TRUE);
  TEST ("s0.count()", s0.count(), 3);
  TEST ("s0.tree_depth()", s0.tree_depth(), 1);
  TEST ("s0.put(CoolString(\"aaa\"))", s0.put(CoolString("aaa")), TRUE);
  TEST ("s0.count()", s0.count(), 4);
  TEST ("s0.tree_depth()", s0.tree_depth(), 2);
  TEST ("s0.put(CoolString(\"bbb\"))", s0.put(CoolString("bbb")), TRUE);
  TEST ("s0.count()", s0.count(), 5);
  TEST ("s0.tree_depth()", s0.tree_depth(), 2);
  TEST ("s0.put(CoolString(\"fff\"))", s0.put(CoolString("fff")), TRUE);
  TEST ("s0.count()", s0.count(), 6);
  TEST ("s0.find(CoolString(\"bbb\"))", s0.find(CoolString("bbb")), TRUE);
  TEST ("s0.value()", (strcmp (s0.value(), "bbb")), 0);
  TEST ("s0.reset()", (s0.reset(), 1), 1);
  TEST ("s0.next()", s0.next(), TRUE);
  TEST ("s0.value()", (strcmp (s0.value(), "aaa")), 0);
  TEST ("s0.next()", s0.next(), TRUE);
  TEST ("s0.value()", (strcmp (s0.value(), "bbb")), 0);
  TEST ("s0.next()", s0.next(), TRUE);
  TEST ("s0.value()", (strcmp (s0.value(), "ccc")), 0);
  TEST ("s0.next()", s0.next(), TRUE);
  TEST ("s0.value()", (strcmp (s0.value(), "ddd")), 0);
  TEST ("s0.prev()", s0.prev(), TRUE);
  TEST ("s0.value()", (strcmp (s0.value(), "ccc")), 0);
  TEST ("s0.prev()", s0.prev(), TRUE);
  TEST ("s0.value()", (strcmp (s0.value(), "bbb")), 0);
  TEST ("s0.next()", s0.next(), TRUE);
  TEST ("s0.value()", (strcmp (s0.value(), "ccc")), 0);
  TEST ("s0.next()", s0.next(), TRUE);
  TEST ("s0.value()", (strcmp (s0.value(), "ddd")), 0);
  TEST ("s0.next()", s0.next(), TRUE);
  TEST ("s0.value()", (strcmp (s0.value(), "fff")), 0);
  TEST ("s0.next()", s0.next(), TRUE);
  TEST ("s0.value()", (strcmp (s0.value(), "hhh")), 0);
  TEST ("s0.count()", s0.count(), 6);
  TEST ("s0.find(CoolString(\"ABCD\"))", s0.find(CoolString("ABCD")), FALSE);
  test_String_2 (s0);
}

void test_equal (void) {
  CoolAVL_Tree<int> b0, b1;
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
    test_int_insert();
    test_int_convert();
    test_int();
    test_charP();
    test_String();
    test_equal();
  }
}


int main () {
  START("CoolAVL_Tree");
  test_int_insert();
  test_int_convert();
  test_int();
  test_charP();
  test_String();
  test_equal();
#if LEAK
  test_leak();
#endif
  SUMMARY();
  return 0;
}
