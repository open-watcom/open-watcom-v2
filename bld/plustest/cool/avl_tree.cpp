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

#include <cool/AVL_Tree.h>

// CoolAVL_Tree -- Copy constructor

template <class Type>
CoolAVL_Tree<Type>::CoolAVL_Tree(const CoolAVL_Tree<Type>& b)
 : CoolBinary_Tree<Type>(b)
{}                            

// CoolAVL_Tree -- Reference to CoolBinary_Tree (Copies BT, and make AVL out of it)

template <class Type>
CoolAVL_Tree<Type>::CoolAVL_Tree(const CoolBinary_Tree<Type>& b)
 : CoolBinary_Tree<Type>(b)
{
  this->balance();                      // Do an AVL balance on new tree
}                             


// ~CoolAVL_Tree -- Destructor (not inline because it's virtual)
template <class Type>
CoolAVL_Tree<Type>::~CoolAVL_Tree() {}

// balance  -- Rebalance an AVL tree, and then recalculate each node's
//             balance (right subtree depth minus left subtree depth)
// input:      None
// Output:     None

template <class Type>
void CoolAVL_Tree<Type>::balance () {
  CoolBinary_Tree<Type>::balance();
  CoolBase_Binary_Tree::calc_depth (this->get_root(), 0, TRUE);
}


template<class Type>
ostream& operator<< (ostream& os, const CoolAVL_Tree<Type>& av) {
  print_tree((CoolBinary_Node<Type>*)av.get_root(),os);
  return os;
}
