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
// Created: MBN 06/28/89 -- Initial design and implementation
// Updated: MBN 08/20/89 -- Updated template usage to reflect new syntax
// Updated: MBN 09/19/89 -- Added conditional exception handling
// Updated: DKM 11/05/89 -- Replaced re-balance after exceeding goal height
//                          with true AVL rotation alorythm.
// Updated: LGO 12/04/89 -- operator<< not inline
// Updated: LGO 12/04/89 -- balance not inline
// Updated: MJF 06/30/90 -- Added base class name to constructor initializer
// Updated: VDN 02/21/92 -- New lite version
// Updated: JAM 08/19/92 -- modernized template syntax, remove macro hacks
// Updated: JAM 08/19/92 -- made *_state typedef a nested typedef "IterState"
//                          as per new Iterator convention
//
// The AVL_Tree<Type> class implements height-balanced, dynamic,  binary trees.
// The  AVL_Tree<Type> class is  publicly derived   from  the Binary_Tree<Type>
// class and  both  are parameterized  over some type   Type. An AVL  tree is a
// compromise  between the  expense  of a  fully balanced binary  tree and  the
// desire for efficient search times for both average and worst case scenarios.
// As a result, an AVL tree  maintains a  binary tree that  is height-balanced,
// insuring that the minimum and maximum depth  of any path  through the binary
// tree is within some specified range.
//
// The  AVL_Tree<Type>  class has  no private  data slots   and only two public
// constructors.  The first constructor  takes an optional argument that allows
// the user  to  specify  the height-balance limit  (the   default is  two).  A
// height-balance  value   of  zero indicates    that  the  tree should  remain
// completely balanced.   The second  constructor  takes a reference to another
// AVL_Tree<Type> and duplicates its size and values.
//
// The  AVL_Tree<Type>  class   inherits  all its   methods  publicly from  the
// Binary_Tree<Type> class. The only methods that are overloaded are those that
// affect  the structure of  the tree, thus  potentially requiring  one or more
// subtrees to be shaken and restructured.
//

#ifndef AVL_TREEH                               // If no definition for class
#define AVL_TREEH

#ifndef BINARY_TREEH                            // If no definition for class
#include <cool/Binary_Tree.h>                   // include definition file
#endif

template <class Type>
class CoolAVL_Tree : public CoolBinary_Tree<Type> {
  
public:
  typedef CoolBT_State IterState;

  /*inline##*/ CoolAVL_Tree() {};                       // Simple constructor
  CoolAVL_Tree(const CoolAVL_Tree<Type>&);      // Copy constructor
  CoolAVL_Tree(const CoolBinary_Tree<Type>&);   // Convert BT into AVL
  ~CoolAVL_Tree();                              // Destructor

  inline Boolean put (const Type&);             // Add an item to tree
  inline Boolean remove (const Type&);          // Remove item from tree
  inline Boolean remove ();                     // Remove item current position
  void balance ();                              // Special balance for AVL
  inline CoolAVL_Tree<Type>& operator= (CoolAVL_Tree<Type>&);  // Assignment overloaded
  inline CoolAVL_Tree<Type>& operator= (CoolBinary_Tree<Type>&);//Assignment overloaded
  template< class U >
  friend ostream& operator<< (ostream&, const CoolAVL_Tree<U>&);
  template< class U >
  inline friend ostream& operator<< (ostream&, const CoolAVL_Tree<U>*);
};


// put -- Add a value to the AVL tree if it is not already there and balance
//        tree if necessary
// Input: Reference to value to add to tree
// Output: TRUE if item added, FALSE otherwise

template <class Type> 
inline Boolean CoolAVL_Tree<Type>::put (const Type& value) {
  return (CoolBinary_Tree<Type>::put_internal (value, TRUE)); 
}


// remove -- Remove a value from the AVL tree. Deletion of a node and balance
//           tree if necessary
// Input:    Reference to value to remove
// Output:   TRUE if item removed, FALSE otherwise

template <class Type> 
inline Boolean CoolAVL_Tree<Type>::remove (const Type& value) {
  return (CoolBinary_Tree<Type>::remove_internal (value, TRUE)) ;
}


// remove -- Remove node at current position in the tree and balance tree
// Input:    None
// Output:   Value of node removed from tree

template <class Type> 
inline Boolean CoolAVL_Tree<Type>::remove () {
  return (CoolBinary_Tree<Type>::remove_internal(this->value(),TRUE));
}


template <class Type> 
inline CoolAVL_Tree<Type>& CoolAVL_Tree<Type>::operator= (CoolAVL_Tree<Type>& av) {
  CoolBinary_Tree<Type>::operator= (av);        // Create the new Tree
  return *this;                                 // Return tree reference
}

template <class Type>
inline CoolAVL_Tree<Type>& CoolAVL_Tree<Type>::operator= (CoolBinary_Tree<Type>& bt) {
  CoolBinary_Tree<Type>::operator= (bt);        // Create the new Tree
  this->balance();                              // Do an AVL balance
  return *this;                                 // Return tree reference
}



template<class Type>
inline ostream& operator<< (ostream& os, const CoolAVL_Tree<Type>* av) {
  return operator<< (os, *av);
}

#endif                                          // End AVL_TREEH #if
