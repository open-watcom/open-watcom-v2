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
// Updated: VDN 02/21/92 -- New lite version
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
#include <cool/Base_Binary_Node.h>              // include header file
#endif  

// CoolBase_Binary_Node -- Simple constructor to initialize a node object
// Input:         None
// Output:        None

CoolBase_Binary_Node::CoolBase_Binary_Node () {
  this->ltree = NULL;                           // Initialize left subtree
  this->rtree = NULL;                           // Initialize right subtree
  this->avl_balance = 0;                        // Initialize avl balance
}

// ~CoolBase_Binary_Node -- Destructor has to be virtual so that 
//                     ~CoolBase_Binary_Node<Type> is called on ltree and rtree.
// Input:          None
// Output:         None

CoolBase_Binary_Node::~CoolBase_Binary_Node () {
  delete this->ltree;                           // Delete left subtree, virtual
  delete this->rtree;                           // Delete right subtree
}


