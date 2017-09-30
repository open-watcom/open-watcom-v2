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
// Created: MBN 07/19/89 -- Initial design and implementation
// Updated: MBN 08/11/89 -- Inherit from Generic
// Updated: MBN 09/19/89 -- Added conditional exception handling
// Updated: DKM 11/05/89 -- Support for Stack iterator (removed traversal cache)
// Updated: VDN 02/21/92 -- New lite version
// Updated: JAM 08/19/92 -- modernized template syntax, remove macro hacks
//                          non-template classes CoolBinary_Tree=>CoolBase_Binary_Tree
// Updated: JAM 08/19/92 -- made *_state typedef a nested typedef "IterState"
//                          as per new Iterator convention
//
// The CoolBase_Binary_Tree class implements the type-generic  structural methods of the
// parameterized CoolBinary_Tree<Type>  class  and is  a friend of  the Binary Node
// class.  The  CoolBase_Binary_Tree   class  is  intended for  the    sole  use  of the
// parameterized  CoolBinary_Tree<Type>    class.    The    CoolBinary_Tree<Type> class
// implements simple,  dynamic,   sorted  sequences.  Users who  require a data
// structure  for unsorted  sequences  whose structure and organization is more
// under the control of the programmer are refered to the N_Tree class.
//
// The CoolBase_Binary_Tree class  supports the  concept of  a current  position as the
// tree is traversed via next, prev,  and find.  method that changes  the tree
// structure invalidates the current position state
//
// There are two public constructors. The first takes no  arguments  and simply
// allocates initial storage  for a Binary   Tree object.  The   second takes a
// reference  to an existing  Binary Tree object   and duplicates its  size and
// contents.  The  CoolBase_Binary_Tree class    has  several private data   slots  that
// maintain a pointer  to the root of the  tree, the current iteration state, a
// pointer to the default node comparison function, the number  of nodes in the
// tree, and information  relating to the shallowest  and deepest nodes  in the
// tree.  In addition, there are four private methods.  The first two calculate
// the shallowest and deepest terminal nodes in the  tree, and the last two are
// used to  create the cache of pointers  to nodes  upon the first  dispatch to
// advance the current position.
//
// Methods are available to get the zero-relative tree depth, return the number
// of nodes in the tree, and get a pointer to the root  node of  the tree.  The
// reset, next,  and prev methods  provide a mechanism to iterate   through the
// nodes of a tree based upon the current position.  Finally,  all nodes can be
// removed from the tree with the clear method.
//

#ifndef BASE_BINARY_TREEH                       // If no definition for class
#define BASE_BINARY_TREEH

#ifndef BASE_BINARY_NODEH                       // If no definition for class
#include <cool/Base_Binary_Node.h>              // include definition file
#endif

#ifndef STACKH                                  // If no definition for class
#include <cool/Stack.h>                         // include definition file
#endif

#ifndef PAIRH                                   // If no definition for class
#include <cool/Pair.h>                          // include definition file
#endif

#ifndef N_TREEH
enum Left_Right {NONE, LEFT, RIGHT};
enum Traversal_Type {PREORDER, INORDER, POSTORDER,
                     PREORDER_REVERSE, INORDER_REVERSE, POSTORDER_REVERSE};
#endif


typedef CoolPair<CoolBase_Binary_Node*,int> Stack_Entry;
typedef CoolStack< CoolPair<CoolBase_Binary_Node*,int> > BT_Stack;

class CoolBT_State {                            // State bundles Stack&Boolean
public:
  BT_Stack stack;
  Boolean forward;

  inline CoolBT_State () {};                    // Simple constructor
  inline CoolBT_State (const CoolBT_State& s) {         // constructor with reference
    this->stack = s.stack;                      
    this->forward = s.forward;
  };
  ~CoolBT_State () {};                          // Destructor

  inline CoolBT_State& operator= (CoolBT_State& s) {    // Overload = operator
    this->stack = s.stack;
    this->forward = s.forward;
    return *this;
  };
};


class CoolBase_Binary_Tree {
public:
  typedef CoolBT_State IterState;               // Std name for Iterator class

  inline long tree_depth ();                    // Return deepest node depth
  inline long count () const;                   // Return number of nodes
  inline void reset ();                         // invalidate current position 
  inline CoolBase_Binary_Node* get_root() const;                // Accessor to get root pointer
  CoolBase_Binary_Node* node ();                                // Return node at current pos.
  inline Boolean next ();                       // Advance to next node
  inline Boolean prev ();                       // Backup to previous node
  inline CoolBT_State& current_position () const;       // Get/Set current position
  void clear ();                                // Empty the tree

protected:
  CoolBase_Binary_Node* root;                   // Root of binary tree
  long number_nodes;                            // Number of nodes in tree
  CoolBT_State state;                           // iterator object.

  CoolBase_Binary_Tree ();                              // Simple constructor
  ~CoolBase_Binary_Tree ();                             // Destructor 

  long calc_depth (CoolBase_Binary_Node*, long, Boolean b=FALSE); // calc depth of tree
  void avl_put_balance (BT_Stack&);             // Balance after put
  void avl_remove_balance (BT_Stack&);          // Balance after remove
  Boolean next_internal (Traversal_Type);       // Get's the next node
                                                // updating node balance
  void curpos_error (const char* type, const char* fcn); // Raise exception
};



// actual_height -- Return the zero-relative depth of the deepest terminal
//               node in the tree
// Input:        None
// Output:       Deepest depth

inline long CoolBase_Binary_Tree::tree_depth () {
  return (this->calc_depth (this->root, 0));
}
        


// Reset -- Invalidate pointer cache and current position index
// Input:   None
// Output:  None

inline void CoolBase_Binary_Tree::reset () {
  this->state.stack.clear();                    // empty the iteration stack
}


// count -- Return number of nodes in tree
// Input:   None
// Output:  Number of nodes in tree

inline long CoolBase_Binary_Tree::count () const {
  return this->number_nodes;
}


// get_root -- Accessor to get root pointer in base class
// Input:      None
// Output:     CoolBase_Binary_Node pointer

inline CoolBase_Binary_Node* CoolBase_Binary_Tree::get_root () const {
  return this->root;
}


// next -- Increment current position to next node in tree. 
// Input:  None
// Output: TRUE/FALSE

inline Boolean CoolBase_Binary_Tree::next () {
  return (this->next_internal (INORDER));
}


// prev -- Decrement current position to previous node in tree. 
// Input:  None
// Output: TRUE/FALSE

inline Boolean CoolBase_Binary_Tree::prev() {
  return (this->next_internal (INORDER_REVERSE));
}

// current_position -- Return the object holding current position info
// Input:  None
// Output: Current position state of the tree

inline CoolBT_State& CoolBase_Binary_Tree::current_position () const {
  return ((CoolBase_Binary_Tree*)this)->state;
}


#endif                                          // End BASE_BINARY_TREEH
