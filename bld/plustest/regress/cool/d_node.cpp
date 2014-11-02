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

#include <cool/D_Node.h>

// compare_s -- Compare function for class
template <class Type, int nchild> 
//##CoolD_Node<Type,nchild>::Compare CoolD_Node<Type,nchild>::compare_s = &default_CoolD_Node_compare;
Boolean (*CoolD_Node<Type,nchild>::compare_s)(const Type&, const Type&) = &default_CoolD_Node_compare;


// CoolD_Node -- Simple constructor that allocates enough storage for a vector of
//           pointers to CoolD_Node objects
// Input:    None
// Output:   None

template <class Type, int nchild> 
CoolD_Node<Type,nchild>::CoolD_Node() {
  sub_trees.set_alloc_size(nchild);             // fix growth ratio instead
  for (int i = 0; i < nchild; i++)              // For each pointer in vector
    this->sub_trees.push(NULL);                 // Insure NULL pointer value
}


// CoolD_Node -- Simple constructor that allocates enough storage for a vector of
//           pointers to CoolD_Node objects and assigns an initial data value
// Input:    Data slot value
// Output:   None

template <class Type, int nchild> 
CoolD_Node<Type,nchild>::CoolD_Node(const Type& value) {
  for (int i = 0; i < nchild; i++)              // For each pointer in vector
    this->sub_trees.push(NULL);                 // Insure NULL pointer value
  this->set(value);                             // Copy initial data value
}


// CoolD_Node -- Copy constructor makes deep copy
// Input:    Reference to CoolD_Node
// Output:   None

template <class Type, int nchild> 
CoolD_Node<Type,nchild>::CoolD_Node(const CoolD_Node<Type,nchild>& n) {
  for (int i = 0; i < n.sub_trees.length(); i++) // For each pointer in vector
    this->sub_trees.push(copy_nodes(n.sub_trees[i])); // Deep copy of subnodes
  this->set(n.get());                           // Copy data value
  this->compare_s = n.compare_s;                // Set compare method
}


// ~CoolD_Node -- Destructor for the CoolD_Node<Type,nchild> class
// Input:     None
// Output:    None

template <class Type, int nchild> 
CoolD_Node<Type,nchild>::~CoolD_Node() {
  for (int i = 0; i < this->num_subtrees(); i++) // For each pointer in vector
    delete this->sub_trees[i];                  // Invoke destructor
}


// is_leaf -- Determine if node has any children
// Input:     None
// Output:    TRUE if no children, else FALSE

template <class Type, int nchild> 
Boolean CoolD_Node<Type,nchild>::is_leaf () const {
  for (int i = 0; i < this->num_subtrees(); i++)
    if (this->sub_trees[i])
      return (FALSE);
  return TRUE;
}


// operator= -- Overload the assignment operator to copy all values from one
//              node object to another. This routine could potentially result
//              in a complete deep copy, since for each valid sub_tree pointer,
//              a new node is allocated and its sub_tree pointers copied.
// Input:       Reference to CoolD_Node
// Output:      Rererence to updated CoolD_Node

template <class Type, int nchild> 
CoolD_Node<Type,nchild>& CoolD_Node<Type,nchild>::operator= (const CoolD_Node<Type,nchild>& n) {
  int i;
  for (i = 0; i < this->num_subtrees(); i++) // Recursively delete old tree
    delete this->sub_trees.pop();                // and pop from vector
  for (i = 0; i < n.num_subtrees(); i++)        // Push in new subtrees
    this->sub_trees.push(copy_nodes(n.sub_trees[i]));
  this->set(n.get());                           // Copy data value
  return *this;                                 // Return reference
}

// insert_before -- Insert sub-tree pointer to child before the specified
//                  zero-relative sub-tree index (numbered from left to right)
// Input:           Pointer to child node, zero-relative index
// Output:          TRUE/FALSE

template <class Type, int nchild> 
Boolean CoolD_Node<Type,nchild>::insert_before (CoolD_Node<Type,nchild>& n, int index) {
#if ERROR_CHECKING
  if (index < 0) {                              // If index out of range
    this->index_error ("insert_before", index); // Raise exception
    return FALSE;                               // Return failure status
  }
#endif
  this->sub_trees.insert_before(&n, index);     // Pointer to new sub-tree
  return TRUE;                                  // Return success status
}


// insert_after -- Insert sub-tree pointer to child after the specified
//                 zero-relative sub-tree index (numbered from left to right)
// Input:          Pointer to child node, zero-relative index
// Output:         TRUE/FALSE

template <class Type, int nchild> 
Boolean CoolD_Node<Type,nchild>::insert_after (CoolD_Node<Type,nchild>& n, int index) {
#if ERROR_CHECKING
  if (index < 0) {                              // If index out of range
    this->index_error ("insert_after", index);  // Raise exception
    return FALSE;                               // Return failure status
  }
#endif
  this->sub_trees.insert_after(&n, index);      // Pointer to new sub-tree
  return TRUE;                                  // Return success status
}


// copy_nodes -- Copies this node and all its subnodes
// Input:       pointer to node to be copied
// Output:      pointer to new copy of node with all new subnodes.

template <class Type, int nchild>
CoolD_Node<Type,nchild>* CoolD_Node<Type,nchild>::copy_nodes (const CoolD_Node<Type,nchild>* n) const {
  if (n == NULL)                                
    return NULL;
  CoolD_Node<Type,nchild>* new_n = new CoolD_Node<Type,nchild>; 
  for (int i = 0; i < n->num_subtrees(); i++)   // For each pointer in vector
    new_n->sub_trees.push(copy_nodes(n->sub_trees[i])); // Deep copy of subnodes
  new_n->data = n->data;                               // Copy data value
  return new_n;                                   // Return copied node.
}


// index_error -- Raise exception for invalid index
// Input:         Function name, invalid index
// Output:        None

template <class Type, int nchild> 
void CoolD_Node<Type,nchild>::index_error (const char* fcn, int n) {
  //RAISE Error, SYM(CoolD_Node), SYM(Out_Of_Range),
  printf ("CoolD_Node<%s,%d>::%s: Index %d out of range.\n", "Type", nchild,
          fcn, n);
  abort ();
}


// default_CoolD_Node_compare -- Default node comparison function utilizing builtin
//                           less than, equal, and greater than operators
// Input:                    Reference to two Type data values
// Output:                   -1, 0, or 1 if less than, equal to, or greater than

template <class Type>
int default_CoolD_Node_compare (const Type& v1, const Type& v2) {
    if (v1 == v2)                               // If data items equal
      return 0;                                 // Return zero
    if (v1 < v2)                                // If this less than data
      return -1;                                // Return negative one
    return 1;                                   // Else return positive one
}
