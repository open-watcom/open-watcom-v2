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
// Created: MBN 06/28/89 -- Initial design
// Updated: MBN 07/19/89 -- Derived from Base_Binary_Node class
// Updated: DKM 11/05/89 -- Added accessors to avl_balance
// Updated: MJF 06/30/90 -- Added base class name to constructor initializer
// Updated: VDN 02/21/92 -- New lite version and fix memory leaks
// Updated: JAM 08/19/92 -- modernized template syntax, remove macro hacks
//
// The Binary_Node<Type> class implements parameterized nodes for binary trees.
// This class  is privately  derived from the  Base_Binary_Node class  that contains
// left and right subtree  pointers.   The Binary_Node<Type> class adds  a data
// slot of  the required type  in the private section.    The Binary_Node<Type>
// class is  intended for the sole  use of  the  Binary_Tree<Type>  class.  All
// constructors and protected and the Binary_Tree<Type> class is  declared as a
// friend class.
//
// There are three protected constructors  for the Binary_Node<Type>class.  The
// first takes no arguments  and initializes the two subtree  pointers  to NULL
// via the Base_Binary_Node constructor.  The second takes an  argument of type Type
// and initializes the data slot to that value.  The third takes a reference to
// another Binary_Node<Type> object and duplicates its values.
//
// Methods are provided to set and get the node data value, determine if a node
// is a leaf or the root of some subtree,  and implement member-wise assignment
// from one Binary_Node<Type> to another via the overloaded operator=.
//

#ifndef BINARY_NODEH                            // If no definition for class
#define BINARY_NODEH

#ifndef BASE_BINARY_NODEH                       // If no definition for class
#include <cool/Base_Binary_Node.h>              // Include useful defintions
#endif  

template <class Type>
class CoolBinary_Tree;                  // Forward reference class

template <class Type>
class CoolBinary_Node : public CoolBase_Binary_Node {
  friend class CoolBinary_Tree<Type>;           // CoolBinary_Tree class is friend
public:  
  CoolBinary_Node();                    // Simple constructor
  CoolBinary_Node(const Type&);         // Constructor with data value
  CoolBinary_Node(const CoolBinary_Node<Type>&);        // Copy constructor
  virtual ~CoolBinary_Node();           // Destructor is virtual

  inline void set (Type&);                      // Set node data to value
  inline Type& get () const;                    // Get node data value
  inline void set_ltree (CoolBinary_Node<Type>*);  // Accessor to set ltree pointer
  inline CoolBinary_Node<Type>* get_ltree () const;// Accessor to get ltree pointer
  inline void set_rtree (CoolBinary_Node<Type>*);  // Accessor to set rtree pointer
  inline CoolBinary_Node<Type>* get_rtree () const;// Accessor to get rtree pointer

  CoolBinary_Node<Type>& operator= (const CoolBinary_Node<Type>&); // Overload assignment

  Type data;                                    // Slot to hold data value
  CoolBinary_Node<Type>* copy_nodes (const CoolBinary_Node<Type>*) const; // Copy of subnodes
protected:
};


// set -- Set value of data slot in node
// Input: Reference to data slot value
// Output: None

template <class Type>
inline void CoolBinary_Node<Type>::set (Type& value) {
  this->data = value;                           // Set data slot value
}


// get -- Get value of data slot in node
// Input: None
// Output: Reference to data slot value

template <class Type>
inline Type& CoolBinary_Node<Type>::get () const {
  return ((CoolBinary_Node<Type>*) this)->data; // avoid warning & const
}                                               // Return data slot value


// set_ltree -- Accessor to set left subtree pointer in base class
// Input:       CoolBinary_Node pointer
// Output:      None

template <class Type>
inline void CoolBinary_Node<Type>::set_ltree (CoolBinary_Node<Type>* bn) {
  this->ltree = bn;
}


// set_rtree -- Accessor to set right subtree pointer in base class
// Input:       CoolBinary_Node pointer
// Output:      None

template <class Type>
inline void CoolBinary_Node<Type>::set_rtree (CoolBinary_Node<Type>* bn) {
  this->rtree = bn;
}


// get_ltree -- Accessor to get left subtree pointer in base class
// Input:       None
// Output:      CoolBinary_Node pointer

template <class Type>
inline CoolBinary_Node<Type>* CoolBinary_Node<Type>::get_ltree () const {
  return (CoolBinary_Node<Type>*)this->ltree;
}


// get_rtree -- Accessor to get right subtree pointer in base class
// Input:       None
// Output:      CoolBinary_Node pointer

template <class Type>
inline CoolBinary_Node<Type>* CoolBinary_Node<Type>::get_rtree () const {
  return (CoolBinary_Node<Type>*)this->rtree;
}


#endif                                          // End BINARY_NODEH #if
