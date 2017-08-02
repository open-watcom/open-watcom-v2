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
// Created: MBN 07/18/89 -- Initial design and implementation
// Updated: MBN 08/11/89 -- Inherit from Generic
// Updated: DKM 11/05/89 -- Added avl_balance slot, num_subtrees member
// Updated: VDN 02/21/92 -- New lite version
// Updated: JAM 08/19/92 -- removed DOS specifics, stdized #includes
// Updated: JAM 08/19/92 -- modernized template syntax, remove macro hacks
//                          non-template classes CoolBinary_Node=>CoolBase_Binary_Node
//
// The CoolBase_Binary_Node class implements  nodes for binary trees  that have  no data
// slot.  This node class contains left and right subtree pointers.   Since the
// CoolBase_Binary_Node   class is  intended   for the  sole   use of  the parameterized
// CoolBinary_Node<Type> class, all constructors and  methods are protected and the
// CoolBinary_Tree class is declared as a friend class.
//
// The private data section contains two  pointers to CoolBase_Binary_Node  objects, one
// for the left subtree and one for the right subtree.  There are two protected
// constructors  for the class.   The first takes  no arguments and initializes
// the two pointers to  NULL.     The  second takes   a  reference  to  another
// CoolBase_Binary_Node object and duplicates its values.
//
// Methods are provided to determine if a node  is a  leaf or  the root of some
// subtree and implement member-wise assignment from one CoolBase_Binary_Node to another
// via the overloaded operator=.
//

#ifndef BASE_BINARY_NODEH                       // If no definition for class
#define BASE_BINARY_NODEH

#ifndef STREAMH                 // If the Stream support not yet defined,
#include <iostream.h>           // include the Stream class header file
#define STREAMH
#endif

#ifndef MISCELANEOUSH           // If we have not included this file,
#include <cool/misc.h>          // include miscelaneous useful definitions.
#endif

class CoolBase_Binary_Tree;                             // Forward declaration of class

class CoolBase_Binary_Node {
  friend class CoolBase_Binary_Tree;                    // CoolBase_Binary_Tree class is friend

public:
  virtual ~CoolBase_Binary_Node ();                     // Destructor is virtual

  inline Boolean is_leaf () const;              // TRUE if node has no children
  inline int get_avl_balance () const;          // Return avl balance
  inline void set_avl_balance (int);            // Set avl balance

protected:
  CoolBase_Binary_Node* ltree;                  // Left subtree pointer
  CoolBase_Binary_Node* rtree;                  // Right subtree pointer
  int avl_balance;                              // AVL balance for this node

  CoolBase_Binary_Node ();                              // Simple constructor

  inline CoolBase_Binary_Node* subtree (int);   // Return nth subtree of node
  inline int num_subtrees () const;             // Number of subtree slots
  /*inline##*/ friend ostream& operator<< (ostream&, CoolBase_Binary_Node*); // Output
};

// is_leaf -- Determine if node has any children
// Input:     None
// Output:    TRUE if no children, else FALSE

inline Boolean CoolBase_Binary_Node::is_leaf () const {
  return ((this->ltree == NULL && this->rtree == NULL) ? TRUE : FALSE);
}

// get_avl_balance -- Return the value of the avl_balance slot for node
// Input: None
// Output: int value of avl balance.  Must be -1, 0, or 1.

inline int CoolBase_Binary_Node::get_avl_balance () const {
  return this->avl_balance;
}

// set_avl_balance -- Set the value of the avl_balance slot for node
// Input: Int of -1 0 or 1
// Output: Void

inline void CoolBase_Binary_Node::set_avl_balance (int val) {
  this->avl_balance = val;
}

// subtree -- Get nth subtree of node (n is 0 based)
// Input:     int
// Output:    CoolBase_Binary_Node or NULL if no subtree

inline CoolBase_Binary_Node* CoolBase_Binary_Node::subtree (int n) {
  if (n == 0)
    return ltree;
  else if (n == 1)
    return rtree;
// should signal an error here ...
  else return NULL;
}


inline int CoolBase_Binary_Node::num_subtrees () const {
  return 2;
}

// operator<< -- Allow printing something for a pointer to a Binary Node
// Input:        Reference to output stream,  CoolBase_Binary_Node*
// Output:       Reference to output stream

inline ostream& operator<< (ostream& os, CoolBase_Binary_Node *b) {
  os << (long)b;                                // cast as a long
  return os;                                    // Return output stream
}

#endif                                          // End BASE_BINARY_NODEH #if
