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
#include <cool/N_Node.h>
#include <cool/D_Node.h>
#include <cool/N_Tree.h>

#include <cool/Vector.C>
#include <cool/N_Node.C>
#include <cool/D_Node.C>
#include <cool/N_Tree.C>

void test_NN_int () {
  CoolN_Node<int,3>& node1 = *(new CoolN_Node<int,3>(7));
  TEST ("CoolN_Node<int,3> node1(7)", node1.get(), 7); 
  CoolN_Tree<CoolN_Node<int,3> > n1(node1);
  TEST ("CoolN_Tree<CoolN_Node,int,3> n1(node1)",n1.count(),1);
  TEST ("n1.reset()",(n1.reset(),1),1);
  TEST ("n1.next()",n1.next(),TRUE);
  TEST ("n1.value()",n1.value(),7);
  CoolN_Node<int,3>& node2 = *(new CoolN_Node<int,3>(3));
  TEST ("CoolN_Node<int,3> node2(3)", node2.get(), 3);
  CoolN_Node<int,3>& node3 = *(new CoolN_Node<int,3>(8));
  TEST ("CoolN_Node<int,3> node3(8)", node3.get(), 8);
  CoolN_Node<int,3>& node4 = *(new CoolN_Node<int,3>(9));
  TEST ("CoolN_Node<int,3> node4(9)", node4.get(), 9);
  CoolN_Node<int,3>& node5 = *(new CoolN_Node<int,3>(4));
  TEST ("CoolN_Node<int,3> node5(4)", node5.get(), 4);
  CoolN_Node<int,3>& node6 = *(new CoolN_Node<int,3>(2));
  TEST ("CoolN_Node<int,3> node6(2)", node6.get(), 2);
  TEST ("n1[0]=&node6",(n1[0]=&node6,n1[0]==&node6),1);
  TEST ("n1[1]=&node3",(n1[1]=&node3,n1[1]==&node3),1);
  TEST ("n1[2]=&node4",(n1[2]=&node4,n1[2]==&node4),1);
  TEST ("node6[0]=&node2", (node6[0]=&node2,node6[0]==&node2),1);
  TEST ("node3[0]=&node5", (node3[0]=&node5,node3[0]==&node5),1);
  TEST ("n1.count()", n1.count(), 6);
  TEST ("n1.traversal()=PREORDER",(n1.traversal()=PREORDER,n1.traversal()),PREORDER);
  TEST ("n1.reset()",(n1.reset(),1),1);
  TEST ("n1.next()",n1.next(),TRUE);
  TEST ("n1.value()",n1.value(),7);
  TEST ("n1.next()",n1.next(),TRUE);
  TEST ("n1.value()",n1.value(),2);
  TEST ("n1.next()",n1.next(),TRUE);
  TEST ("n1.value()",n1.value(),3);
  TEST ("n1.next()",n1.next(),TRUE);
  TEST ("n1.value()",n1.value(),8);
  TEST ("n1.prev()",n1.prev(),TRUE);   // test backing up in the middle
  TEST ("n1.value()",n1.value(),3);
  TEST ("n1.next()",n1.next(),TRUE);
  TEST ("n1.value()",n1.value(),8);
  TEST ("n1.next()",n1.next(),TRUE);
  TEST ("n1.value()",n1.value(),4);
  TEST ("n1.next()",n1.next(),TRUE);
  TEST ("n1.value()",n1.value(),9);
  TEST ("n1.next()",n1.next(),FALSE);

  TEST ("n1.reset()",(n1.reset(),1),1); // PREORDER in revrse order
  TEST ("n1.prev()",n1.prev(),TRUE);
  TEST ("n1.value()",n1.value(),9);
  TEST ("n1.prev()",n1.prev(),TRUE);
  TEST ("n1.value()",n1.value(),4);
  TEST ("n1.prev()",n1.prev(),TRUE);
  TEST ("n1.value()",n1.value(),8);
  TEST ("n1.prev()",n1.prev(),TRUE);
  TEST ("n1.value()",n1.value(),3);
  TEST ("n1.prev()",n1.prev(),TRUE);
  TEST ("n1.value()",n1.value(),2);
  TEST ("n1.prev()",n1.prev(),TRUE);
  TEST ("n1.value()",n1.value(),7);
  TEST ("n1.prev()",n1.prev(),FALSE);

  TEST ("n1.traversal()=INORDER",(n1.traversal()=INORDER,n1.traversal()),INORDER);
  TEST ("n1.reset()",(n1.reset(),1),1);
  TEST ("n1.next()",n1.next(),TRUE);
  TEST ("n1.value()",n1.value(),3);
  TEST ("n1.next()",n1.next(),TRUE);
  TEST ("n1.value()",n1.value(),2);
  TEST ("n1.next()",n1.next(),TRUE);
  TEST ("n1.value()",n1.value(),7);
  TEST ("n1.prev()",n1.prev(),TRUE);   // test backing up in the middle
  TEST ("n1.value()",n1.value(),2);
  TEST ("n1.next()",n1.next(),TRUE);
  TEST ("n1.value()",n1.value(),7);
  TEST ("n1.next()",n1.next(),TRUE);
  TEST ("n1.value()",n1.value(),4);
  TEST ("n1.next()",n1.next(),TRUE);
  TEST ("n1.value()",n1.value(),8);
  TEST ("n1.next()",n1.next(),TRUE);
  TEST ("n1.value()",n1.value(),9);
  TEST ("n1.next()",n1.next(),FALSE);

  TEST ("n1.reset()",(n1.reset(),1),1); // INORDER in reverse order
  TEST ("n1.prev()",n1.prev(),TRUE);
  TEST ("n1.value()",n1.value(),9);
  TEST ("n1.prev()",n1.prev(),TRUE);
  TEST ("n1.value()",n1.value(),8);
  TEST ("n1.prev()",n1.prev(),TRUE);
  TEST ("n1.value()",n1.value(),4);
  TEST ("n1.prev()",n1.prev(),TRUE);
  TEST ("n1.value()",n1.value(),7);
  TEST ("n1.prev()",n1.prev(),TRUE);
  TEST ("n1.value()",n1.value(),2);
  TEST ("n1.prev()",n1.prev(),TRUE);
  TEST ("n1.value()",n1.value(),3);
  TEST ("n1.prev()",n1.prev(),FALSE);

  TEST ("n1.traversal()=POSTORDER",(n1.traversal()=POSTORDER,n1.traversal()),POSTORDER);
  TEST ("n1.reset()",(n1.reset(),1),1);
  TEST ("n1.next()",n1.next(),TRUE);
  TEST ("n1.value()",n1.value(),3);
  TEST ("n1.next()",n1.next(),TRUE);
  TEST ("n1.value()",n1.value(),2);
  TEST ("n1.next()",n1.next(),TRUE);
  TEST ("n1.value()",n1.value(),4);
  TEST ("n1.prev()",n1.prev(),TRUE);  // test backing up in the middle
  TEST ("n1.value()",n1.value(),2);
  TEST ("n1.next()",n1.next(),TRUE);
  TEST ("n1.value()",n1.value(),4);
  TEST ("n1.next()",n1.next(),TRUE);
  TEST ("n1.value()",n1.value(),8);
  TEST ("n1.next()",n1.next(),TRUE);
  TEST ("n1.value()",n1.value(),9);
  TEST ("n1.next()",n1.next(),TRUE);
  TEST ("n1.value()",n1.value(),7);
  TEST ("n1.next()",n1.next(),FALSE);

  TEST ("n1.reset()",(n1.reset(),1),1); // POSTORDER in reverse order
  TEST ("n1.prev()",n1.prev(),TRUE);
  TEST ("n1.value()",n1.value(),7);
  TEST ("n1.prev()",n1.prev(),TRUE);
  TEST ("n1.value()",n1.value(),9);
  TEST ("n1.prev()",n1.prev(),TRUE);
  TEST ("n1.value()",n1.value(),8);
  TEST ("n1.prev()",n1.prev(),TRUE);
  TEST ("n1.value()",n1.value(),4);
  TEST ("n1.prev()",n1.prev(),TRUE);
  TEST ("n1.value()",n1.value(),2);
  TEST ("n1.prev()",n1.prev(),TRUE);
  TEST ("n1.value()",n1.value(),3);
  TEST ("n1.prev()",n1.prev(),FALSE);
}

void test_DN_int () {
  CoolD_Node<int,3>& node1 = *(new CoolD_Node<int,3>(7));
  TEST ("CoolD_Node<int,3> node1(7)", node1.get(), 7);
  CoolN_Tree<CoolD_Node<int,3> > d1(node1);
  TEST ("CoolN_Tree<CoolD_Node,int,3> d1(node1)",d1.count(),1);
  TEST ("d1.reset()",(d1.reset(),1),1);
  TEST ("d1.next()",d1.next(),TRUE);
  TEST ("d1.value()",d1.value(),7);
  CoolD_Node<int,3>& node2 = *(new CoolD_Node<int,3>(3));
  TEST ("CoolD_Node<int,3> node2(3)", node2.get(), 3);
  CoolD_Node<int,3>& node3 = *(new CoolD_Node<int,3>(8));
  TEST ("CoolD_Node<int,3> node3(8)", node3.get(), 8);
  CoolD_Node<int,3>& node4 = *(new CoolD_Node<int,3>(9));
  TEST ("CoolD_Node<int,3> node4(9)", node4.get(), 9);
  CoolD_Node<int,3>& node5 = *(new CoolD_Node<int,3>(4));
  TEST ("CoolD_Node<int,3> node5(4)", node5.get(), 4);
  CoolD_Node<int,3>& node6 = *(new CoolD_Node<int,3>(2));
  TEST ("CoolD_Node<int,3> node6(2)", node6.get(), 2);
  TEST ("d1[0]=&node6",(d1[0]=&node6,d1[0]==&node6),1);
  TEST ("d1[1]=&node3",(d1[1]=&node3,d1[1]==&node3),1);
  TEST ("d1[2]=&node4",(d1[2]=&node4,d1[2]==&node4),1);
  TEST ("node6[0]=&node2", (node6[0]=&node2,node6[0]==&node2),1);
  TEST ("node3[0]=&node5", (node3[0]=&node5,node3[0]==&node5),1);
  TEST ("d1.count()", d1.count(), 6);
  TEST ("d1.traversal()=PREORDER",(d1.traversal()=PREORDER,d1.traversal()),PREORDER);
  TEST ("d1.reset()",(d1.reset(),1),1);
  TEST ("d1.next()",d1.next(),TRUE);
  TEST ("d1.value()",d1.value(),7);
  TEST ("d1.next()",d1.next(),TRUE);
  TEST ("d1.value()",d1.value(),2);
  TEST ("d1.next()",d1.next(),TRUE);
  TEST ("d1.value()",d1.value(),3);
  TEST ("d1.next()",d1.next(),TRUE);
  TEST ("d1.value()",d1.value(),8);
  TEST ("d1.next()",d1.next(),TRUE);
  TEST ("d1.value()",d1.value(),4);
  TEST ("d1.next()",d1.next(),TRUE);
  TEST ("d1.value()",d1.value(),9);
  TEST ("d1.next()",d1.next(),FALSE);
  TEST ("d1.traversal()=INORDER",(d1.traversal()=INORDER,d1.traversal()),INORDER);
  TEST ("d1.reset()",(d1.reset(),1),1);
  TEST ("d1.next()",d1.next(),TRUE);
  TEST ("d1.value()",d1.value(),3);
  TEST ("d1.next()",d1.next(),TRUE);
  TEST ("d1.value()",d1.value(),2);
  TEST ("d1.next()",d1.next(),TRUE);
  TEST ("d1.value()",d1.value(),7);
  TEST ("d1.next()",d1.next(),TRUE);
  TEST ("d1.value()",d1.value(),4);
  TEST ("d1.next()",d1.next(),TRUE);
  TEST ("d1.value()",d1.value(),8);
  TEST ("d1.next()",d1.next(),TRUE);
  TEST ("d1.value()",d1.value(),9);
  TEST ("d1.next()",d1.next(),FALSE);
  TEST ("d1.traversal()=POSTORDER",(d1.traversal()=POSTORDER,d1.traversal()),POSTORDER);
  TEST ("d1.reset()",(d1.reset(),1),1);
  TEST ("d1.next()",d1.next(),TRUE);
  TEST ("d1.value()",d1.value(),3);
  TEST ("d1.next()",d1.next(),TRUE);
  TEST ("d1.value()",d1.value(),2);
  TEST ("d1.next()",d1.next(),TRUE);
  TEST ("d1.value()",d1.value(),4);
  TEST ("d1.next()",d1.next(),TRUE);
  TEST ("d1.value()",d1.value(),8);
  TEST ("d1.next()",d1.next(),TRUE);
  TEST ("d1.value()",d1.value(),9);
  TEST ("d1.next()",d1.next(),TRUE);
  TEST ("d1.value()",d1.value(),7);
  TEST ("d1.next()",d1.next(),FALSE);
  TEST ("d1.clear(),d1.count()",(d1.clear(),d1.count()), 0)
}

void test_leak () {
  for (;;) {
    test_NN_int ();
    test_DN_int ();
  }
}

int main () {
  START("CoolN_Tree");
  test_NN_int ();
  test_DN_int ();
#if LEAK
  test_leak ();
#endif
  SUMMARY ();
  return 0;
}
