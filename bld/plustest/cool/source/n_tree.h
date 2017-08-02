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
// Created: MBN 07/06/89 -- Initial design
// Updated: MBN 08/15/89 -- Inherit from Generic and initial implementation
// Updated: MBN 09/19/89 -- Added conditional exception handling
// Updated: MBN 02/27/90 -- Added constructor that takes a pointer to node and
//                          the current_depth() member function
// Updated: MJF 03/12/90 -- Added group names to RAISE
// Updated: VDN 02/21/92 -- New lite version
// Updated: JAM 08/19/92 -- modernized template syntax, remove macro hacks
// Updated: JAM 08/19/92 -- made *_state typedef a nested typedef "IterState"
//                          as per new Iterator convention
// Updated: JAM 09/28/92 -- made Node class have ItemType and nchild members
//                          so don't have to give template parameters twice
//
// The  N_Tree  class  implements  N-ary  trees,  providing  the organizational
// structure for a tree  (collection) of nodes,  but  knowing nothing about the
// specific type of node  used. N_Tree is parameterized  over a node type and a
// data type, where the node specified  must have a data slot  of the same type
// Type as the N_Tree  class. Two node classes are  provided, but others  could
// also be written.  The  N_Node class implements  static-sized nodes for  some
// particular "N" number of sub-trees, and the D_Node class  implements dymamic
// sized nodes derived from the Vector class.
//
// Since the organization of a tree is important (as  with an expression tree),
// the user must supervise the construction  of  the tree by directing specific
// node and sub-tree assignment and layout.   No attempt  is made by the N_Tree
// class to balance or prune the tree.
//
// The N_Tree class supports the concept  of a  current  position and a current
// traversal mode. When  the traversal  mode  is  set, the current position  is
// invalidated.  The  first call   to advance the   current position causes  an
// internal dynamic cache of pointers to nodes to  be created ordered according
// to the traversal mode. Future current position  methods then act  based upon
// the information in  the cache.   Any method that  changes the tree structure
// invalidates the cache.
//
// There are two  public constructors. The  first takes a  reference  to a Node
// object and constructs an N_Tree object whose root is the supplied node.  The
// second takes a reference  to an existing  N_Tree  object and duplicates  its
// size and contents.  The N_Tree class has four private data slots.  The first
// contains a pointer to the root of the tree, the second maintains the current
// position,  the  third contains a pointer to  a  dynamic cache of pointers to
// nodes used  by  the current  position  methods, and  the   fourth contains a
// pointer to the default node comparison function.  In addition, there are two
// private methods.  The first is used to create the cache of pointers to nodes
// upon the first dispatch to  advance the current position,  and the second is
// the default node comparison function to be used  if the user  does not chose
// to provide one.
//
// All methods  in the  N_Tree class  support  the organization, structure, and
// traversal of a tree.  Methods to allow manipulation  of individual nodes and
// sub-trees is located in the node classes. N_Tree has methods to search for a
// sub-tree,  find a  node with a specific value,  and return a  pointer to the
// parent of the current  node. The reset, next, prev,  value, remove, and find
// methods provide a mechanism to  iterate  through the nodes  of a tree  based
// upon  the current position. The specific  traversal mechanism  for  use with
// this iteration can be set with  the set_traversal method,  and all nodes can
// be removed  from the tree   with  clear.  Finally,  methods  are provided to
// traverse the  tree in  either preorder, inorder,  or  postorder and apply  a
// user-specified function to each node.
//

#ifndef N_TREEH                                 // If no definition for class
#define N_TREEH

#ifndef N_TREE_STATEH
#include <cool/NT_State.h>                      // Include NT_State
#endif

#ifndef BASE_BINARY_TREEH
enum Left_Right {NONE, LEFT, RIGHT};
enum Traversal_Type {PREORDER, INORDER, POSTORDER,
                     PREORDER_REVERSE, INORDER_REVERSE, POSTORDER_REVERSE};
#endif


template <class Node>
class CoolN_Tree {
public:
  typedef CoolNT_State IterState;
  typedef Boolean (*Apply_Function)(const Node::ItemType&);

  CoolN_Tree(Node* root=NULL); // Default
  CoolN_Tree(Node& root); // Simple constructor
  CoolN_Tree(const CoolN_Tree<Node>&);  // Copy
  ~CoolN_Tree();                // Destructor

  void clear ();                                // Empty the tree
  inline long count ();                         // Return number of nodes
  inline void reset ();                         // Current position invalid
  inline Traversal_Type& traversal ();          // Set/Get the traversal mode
  inline Node*& operator[] (int); // Set/Get pointers

  inline Boolean next ();                       // Advance to next node
  Boolean prev ();                              // Backup to previous node
  Node::ItemType& value (){/*##;*/                      // Get value at current position
#if ERROR_CHECKING 
      if (this->state.stack.is_empty() )                // If no position established
         this->value_error ();                  // Raise exception
#endif
      CoolNT_Stack_Entry stack_entry = this->state.stack.top();
      return (((Node*)stack_entry.get_first())->get());
      }
  Boolean find (const Node::ItemType& value){/*##;*/                    // Search for item in tree
      for (this->reset (); this->next (); )     // For each node in tree
         if (this->value() == value)                    // If node found in tree
            return TRUE;                                // Inidicate success
      return FALSE;                                     // Inidicate failure
      }
  inline CoolNT_State& current_position();              // Get/Set Tree's curpos
  inline long current_depth () const;           // Depth of curpos in tree

  void preorder (Apply_Function fn){/*##;*/     // Preorder traversal
      if (this->t_mode != PREORDER)             // If incorrect traversal mode
         this->t_mode = PREORDER;                       // Set preorder mode
      for (this->reset() ; this->next (); )             // For each preorder node
         (*fn)(this->value());                  // Apply function
      }
  void inorder (Apply_Function fn){/*##;*/              // Inorder traversal
      if (this->t_mode != INORDER)              // If incorrect traversal mode
         this->t_mode = INORDER;                        // Set inorder mode
      for (this->reset() ; this->next (); )             // For each preorder node
         (*fn)(this->value());                  // Apply function
      }
  void postorder (Apply_Function fn){/*##;*/    // Postorder traversal
      if (this->t_mode != POSTORDER)            // If incorrect traversal mode
         this->t_mode = POSTORDER;                      // Set postorder mode

      for (this->reset() ; this->next (); )             // For each preorder node
         (*fn)(this->value());                  // Apply function
      }

  CoolN_Tree<Node>& operator= (const CoolN_Tree<Node>&);
  inline operator Node*() const; // Conversion to node ptr

private:
  Node* root;           // Root of tree
  long number_nodes;                            // Number of nodes in tree
  Traversal_Type t_mode;                        // Retains traversal type
  CoolNT_State state;                           // Iterator state for CoolN_Tree

  void do_count (Node*);                // Count nodes in tree
  Boolean next_internal (Traversal_Type);       // Moves current_position
  inline Node* copy_nodes(const Node*) const; // Copy subnodes

  void value_error ();                          // Raise exception
};

// Reset -- Initialize current position of Tree
// Input:   None
// Output:  None

template <class Node> 
inline void CoolN_Tree<Node>::reset () {
  this->state.stack.clear();
}


// count -- Return number of nodes in tree
// Input:   None
// Output:  Number of nodes in tree

template <class Node> 
inline long CoolN_Tree<Node>::count () {
  this->number_nodes = 0;                       // Initialize count
  this->do_count (this->root);                  // Count nodes in tree
  return this->number_nodes;                    // Return node count
}


// set_traversal -- Set traversal mode
// Input:           Traversal type
// Output:          None

template <class Node> 
inline Traversal_Type& CoolN_Tree<Node>::traversal () {
  return (this->t_mode);                        // Traversal mode
}


// operator[] -- Overload the brackets operator to provide a mechanism to set
//               and/or get a sub-tree pointer of a node whose zero-relative
//               index is specified from left to right
// Input:        Zero-relative index into vector of sub-tree pointers
// Output:       Reference to a pointer value

template <class Node> 
inline Node*& CoolN_Tree<Node>::operator[] (int index) {
  return (this->root->sub_trees[index]);
}


// next -- Move current position to next node in tree. If no more nodes
//         return FALSE
// Input:  None
// Output: TRUE/FALSE

template <class Node> 
inline Boolean CoolN_Tree<Node>::next () {
  return next_internal (this->t_mode);
}



// current_depth -- Get current depth of current position in tree
// Input:           None
// Output:          Depth of current position node in tree

template <class Node>
inline long CoolN_Tree<Node>::current_depth () const {
  return this->state.stack.length() - 1;
}


// operator Node -- Provide an accessor to the encapsulated Node object
// Input:           None
// Output:          Pointer to node object

template <class Node> 
inline CoolN_Tree<Node>::operator Node* () const
{
  return this->root;
}


template <class Node>
inline Node* CoolN_Tree<Node>::copy_nodes(const Node* n) const{
  Node* new_nodes = NULL;
  if (n)
    new_nodes = n->copy_nodes(n);               // recursive deep copy
  return new_nodes;
}

#endif                                          // End N_TREEH #if
