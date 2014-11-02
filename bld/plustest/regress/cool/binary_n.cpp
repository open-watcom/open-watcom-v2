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

#include <cool/Binary_Node.h>

// CoolBinary_Node -- Simple constructor to initialize a node object
// Input:         None
// Output:        None

template <class Type>
CoolBinary_Node<Type>::CoolBinary_Node() {
}


// CoolBinary_Node -- constructor to initialize a node object and assign a value
//                to the data slot
// Input:         Data slot value
// Output:        None

template <class Type>
CoolBinary_Node<Type>::CoolBinary_Node(const Type& value) {
  this->data = value;                           // Assign data value
}

// CoolBinary_Node -- Copy constructor to copy node and all subnodes
// Input:         Binary node reference
// Output:        Binary node reference

template <class Type>
CoolBinary_Node<Type>::CoolBinary_Node(const CoolBinary_Node<Type>& bn) {
  this->ltree = this->copy_nodes(bn.get_ltree()); // copy left tree 
  this->rtree = this->copy_nodes(bn.get_rtree()); // copy right tree
  this->avl_balance = bn.avl_balance;             // copy avl balance
  this->data = bn.data;                           // copy data at this node
}


// ~CoolBinary_Node -- Destructor must be virtual
// Input:          None
// Output:         None

template <class Type>
CoolBinary_Node<Type>::~CoolBinary_Node() {} // data deleted when is object



// operator= -- Assignment with another binary node, with recursive deep copy.
// Input:       reference to node
// Ouput:       mutated *this

template <class Type>
CoolBinary_Node<Type>& CoolBinary_Node<Type>::operator= (const CoolBinary_Node<Type>& n) {
  delete this->ltree;                           // free old subnodes
  delete this->rtree;
  this->ltree = copy_nodes (n.get_ltree());     // Copy the left tree
  this->rtree = copy_nodes (n.get_rtree());     // Copy the right tree
  this->avl_balance = n.avl_balance;            // Copy avl balance
  this->data = n.data;                          // Copy value at node
  return *this;
}


// copy_nodes -- Copies this node and all its subnodes
// Input:       pointer to node to be copied
// Output:      pointer to new copy of node with all new subnodes.

template <class Type>
CoolBinary_Node<Type>* CoolBinary_Node<Type>::copy_nodes (const CoolBinary_Node<Type>* n) const {
  if (n == NULL)                                
    return NULL;
  CoolBinary_Node<Type>* new_n = new CoolBinary_Node<Type>; // Allocate a new node
  new_n->ltree = copy_nodes (n->get_ltree());               // Copy the left tree
  new_n->rtree = copy_nodes (n->get_rtree());               // Copy the right tree
  new_n->data = n->data;                                    // Copy the value
  new_n->avl_balance = n->avl_balance;                      // Copy avl balance
  return new_n;                                             // Return copied node.
}

