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
// Updated: MBN 09/15/89 -- Added conditional exception handling
// Updated: DKM 11/05/89 -- Replaced cache traversal with stack traversal
//                          Also added support for AVL balancing
// Updated: LGO 12/04/89 -- operator<< not inline
// Updated: MBN 12/21/89 -- Added optional argument to set_compare method
// Updated: MBN 02/20/90 -- Cut operators <, >, <=, >=; fixed operators ==, !=
// Updated: MJF 06/30/90 -- Added base class name to constructor initializer
// Updated: VDN 02/21/92 -- New lite version
// Updated: JAM 08/19/92 -- modernized template syntax, remove macro hacks
//
// The Binary_Tree<Type> class is publicly derived  from the  Base_Binary_Tree class
// and  implements simple, dynamic, sorted sequences.   Users requiring  a data
// structure for  unsorted  sequences whose structure  and organization is more
// under the control  of the programmer  are refered  to the N_Tree class.  The
// Binary_Tree<Type>  class  is  a   friend   of the  Binary Node  class,  also
// parameterized over the  same Type.  There is no  attempt made  to balance or
// prune the tree.  Nodes  are added to  a particular sub-tree at the direction
// of  the colating function.  As a  result, a tree parameterized  for integers
// and that  uses the default integer  comparison operators whose  elements are
// added in increasing order would result  in a lopsided  tree.  Likewise after
// many items have been added and removed.
//
// The Binary_Tree<Type> class supports  the concept of  a current position  as
// the tree is  traversed via  next, prev,  and find.   method that changes the
// tree structure invalidates the current position state
//
// The  Binary_Tree<Type>  class supports   the concept of  a current position.
// Once  the  current position is  initialized, the first  call  to advance the
// current position causes an internal dynamic cache of pointers to nodes to be
// created.  This cache is created by  an inorder traversal  of the tree. Other
// current  position  advance and  decrement methods then  act  based  upon the
// information in  the  cache.  Any  method  that changes  the  tree  structure
// invalidates the cache.
//
// There are two  public constructors. The first takes  no arguments and simply
// allocates initial storage for a  Binary_Tree<Type> object.  The second takes
// a reference to an existing Binary_Tree<Type> object  and duplicates its size
// and contents.  The Binary_Tree<Type> class has four private data slots.  The
// first contains a pointer to the root of  the  tree, the second maintains the
// current  position, the   third   contains a  pointer  to a  dynamic cache of
// pointers to nodes used  by  the current  position  methods, and  the  fourth
// contains a pointer  to the default  node comparison function.   In addition,
// there are two private  methods.  The  first  is used  to create the cache of
// pointers to nodes upon the first dispatch  to  advance the current position,
// and the second is  the default  node  comparison function to  be used if the
// user does not chose to provide one.
//
// Methods are available to put, remove, and find a node in a tree.  The reset,
// next, prev, value,  remove, and find  methods provide a mechanism to iterate
// through the nodes of  a tree based  upon the current position.  In addition,
// all nodes can  be  removed from the  tree with  the clear method.  A balance
// method is provided to allow  the user to shake the  tree at some appropriate
// time in  order  to balance  the  left and  right  sub-trees.  This  might be
// particularly useful in the case of static binary  trees, where the structure
// becomes fixed and the impetus for fast, efficient searches is high. Finally,
// the  equality, inequality,  less   than,  and greater  than    operators are
// overloaded to provide node comparison functionality.
//

#ifndef BINARY_TREEH                            // If no definition for class
#define BINARY_TREEH

#ifndef BASE_BINARY_TREEH                       // If no definition for class
#include <cool/Base_Binary_Tree.h>              // Include definition file
#endif

#ifndef BINARY_NODEH                            // If no definition for class
#include <cool/Binary_Node.h>                   // include definition file
#endif

template <class Type>
class CoolBinary_Tree : public CoolBase_Binary_Tree {
public:
  typedef int (*Compare)(const Type&, const Type&);

  CoolBinary_Tree();                            // Simple constructor
  CoolBinary_Tree(const CoolBinary_Tree<Type>&);        // Copy constructor
  ~CoolBinary_Tree();                           // Destructor 

  Boolean find (const Type&);                   // Find value in tree

  Type& value ();                               // Return value at current pos
  inline CoolBinary_Node<Type>* get_root () const; // Return root node
  inline CoolBinary_Node<Type>* node ();           // Return current node
  inline Boolean remove ();                     // Remove node at current pos
  inline Boolean put (const Type&);             // Add value to tree 
  inline Boolean remove (const Type&);          // Remove value from tree
  CoolBinary_Tree<Type>& operator= (CoolBinary_Tree<Type>&); // Assignment operator
  void balance ();                              // Balance the tree

  void set_compare (Compare = NULL); // Set compare function
  Boolean operator== (CoolBinary_Tree<Type>&);          // Overload equality
  inline Boolean operator!= (CoolBinary_Tree<Type>&);   // Overload not equal

  template< class U >
  friend ostream& operator<< (ostream&, const CoolBinary_Tree<U>&);
  template< class U >
  inline friend ostream& operator<< (ostream&, const CoolBinary_Tree<U>*);

protected:
  Boolean put_internal    (const Type&, Boolean avl=NULL);// adds a node
  Boolean remove_internal (const Type&, Boolean avl=NULL);// removes a node
  inline CoolBinary_Node<Type>* copy_nodes(const CoolBinary_Node<Type>*) const; // Copy subnodes

private:
  Compare compare;                      // Compare function
  CoolBinary_Node<Type>* baltree (long);        // Build balanced subtree
  template< class U >
  friend void print_tree (const CoolBinary_Node<U>*, ostream&);
  template< class U >
  friend int default_node_compare (const U&, const U&);
};


// get_root -- return node that roots this tree
// Input:      None
// Output:     Pointer to CoolBinary_Node of type Type.

template <class Type>
inline CoolBinary_Node<Type>* CoolBinary_Tree<Type>::get_root () const {
  return (CoolBinary_Node<Type>*)CoolBase_Binary_Tree::get_root ();
}

// node -- return node pointed to by the current position
// Input:      None
// Output:     Pointer to CoolBinary_Node of type Type.

template <class Type>
inline CoolBinary_Node<Type>* CoolBinary_Tree<Type>::node () {
  return (CoolBinary_Node<Type>*)CoolBase_Binary_Tree::node ();
}

// put -- Add a value to the sorted binary tree if it is not already there
// Input: Reference to value to add to tree
// Output: TRUE if item added, FALSE otherwise

template <class Type>
inline Boolean CoolBinary_Tree<Type>::put (const Type& value) {
  return this->put_internal (value);
}


// remove -- Remove a value from the sorted binary tree. Deletion of a node
//           that has both left and right subtrees is done by descending down 
//           the rightmost branch of the left subtree of the element to be
//           deleted until a leaf is encountered, at which point the change is
//           propagated back.
// Input:    Reference to value to remove
// Output:   TRUE if item removed, FALSE otherwise

// For a Binary Tree, call remove_internal without the avl flag
template <class Type>
inline Boolean CoolBinary_Tree<Type>::remove (const Type& value) {
  return this->remove_internal(value);
}


// remove -- Remove node at current position in the tree
// Input:    None
// Output:   Value of node removed from tree

template <class Type>
inline Boolean CoolBinary_Tree<Type>::remove () {
  return this->remove_internal (this->value());
}



// operator<< -- Output a binary tree by printing it sideways where the root is
//               printed at the left margin. To obtain the normal orientation,
//               rotate the output 90 degrees clockwise
// Input:        Reference to output stream, reference to CoolBinary_Tree<Type>
// Output:       Reference to output stream

template <class Type>
inline ostream& operator<< (ostream& os, const CoolBinary_Tree<Type>* b) {
  return os << *b;
}


// operator!= -- Compare binary trees for different values and/or structure
// Input:        constant reference to another binary tree
// Output:       TRUE/FALSE

template <class Type>
inline Boolean CoolBinary_Tree<Type>::operator!=(CoolBinary_Tree<Type>& t) {
  return (! (*this == t));
}

#endif                                          // End BINARY_TREEH #if


