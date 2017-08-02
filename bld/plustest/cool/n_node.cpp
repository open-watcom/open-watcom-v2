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

#include <cool/N_Node.h>

// compare_s -- Compare function for class
template <class Type, int nchild> 
//##CoolN_Node<Type,nchild>::Compare CoolN_Node<Type,nchild>::compare_s = &default_CoolN_Node_compare;
Boolean (*CoolN_Node<Type,nchild>::compare_s)(const Type&, const Type&) = &default_CoolN_Node_compare;


// CoolN_Node -- Simple constructor that allocates enough storage for a vector of
//           pointers to CoolN_Node objects
// Input:    None
// Output:   None

template <class Type, int nchild> 
CoolN_Node<Type,nchild>::CoolN_Node () {
  for (int i = 0; i < nchild; i++)              // For each pointer in vector
    this->sub_trees[i] = NULL;                  // Insure NULL pointer value
}


// CoolN_Node -- Simple constructor that allocates enough storage for a vector of
//           pointers to CoolN_Node objects and assigns an initial data value
// Input:    Data slot value
// Output:   None

template <class Type, int nchild> 
CoolN_Node<Type,nchild>::CoolN_Node(const Type& value) {
  this->data = value;                           // Copy initial data value
  for (int i = 0; i < nchild; i++)              // For each pointer in vector
    this->sub_trees[i] = NULL;                  // Insure NULL pointer value
}


// CoolN_Node -- Copy constructor makes deep copy
// Input:    Reference to CoolN_Node
// Output:   None

template <class Type, int nchild> 
CoolN_Node<Type,nchild>::CoolN_Node(const CoolN_Node<Type,nchild>& n) {
  for (int i = 0; i < nchild; i++)              // For each pointer in vector
    this->sub_trees[i] = copy_nodes(n.sub_trees[i]); // Deep copy of subnodes
  this->data = n.data;                               // Copy data value
  this->compare_s = n.compare_s;                // Set compare method
}


// ~CoolN_Node -- Destructor for the CoolN_Node<Type,nchild> class
// Input:     None
// Output:    None

template <class Type, int nchild> 
CoolN_Node<Type,nchild>::~CoolN_Node() {
  for (int i = 0; i < nchild; i++)              // For each pointer in vector
    delete this->sub_trees[i];                  // Invoke destructor
}

// is_leaf -- Determine if node has any children
// Input:     None
// Output:    TRUE if no children, else FALSE

template <class Type, int nchild> 
Boolean CoolN_Node<Type,nchild>::is_leaf () const {
  for (int i = 0; i < nchild; i++)
    if (this->sub_trees[i])
      return (FALSE);
  return TRUE;
}


// operator[] -- Overload the brackets operator to provide a mechanism to set
//               and/or get a sub-tree pointer of a node whose zero-relative
//               index is specified from left to right
// Input:        Zero-relative index into vector of sub-tree pointers
// Output:       Reference to a pointer value

template <class Type, int nchild> 
inline /*CoolN_Node<Type,nchild>::CoolN_Node_p##*/CoolN_Node<Type,nchild>*& CoolN_Node<Type,nchild>::operator[] (int index) {
#if ERROR_CHECKING
  if (index >= nchild)                          // If index out of range
    this->index_error ("operator[]", index);    // Raise exception
#endif
  return (this->sub_trees[index]);
}


// operator= -- Overload the assignment operator to copy all values from one
//              node object to another. This routine could potentially result
//              in a complete deep copy, since for each valid sub_tree pointer,
//              a new node is allocated and its sub_tree pointers copied.
// Input:       Reference to CoolN_Node
// Output:      Rererence to updated CoolN_Node

template <class Type, int nchild> 
CoolN_Node<Type,nchild>& CoolN_Node<Type,nchild>::operator= (const CoolN_Node<Type,nchild>& n) {
  for (int i = 0; i < nchild; i++) {            // Invoke destructor recursively
    delete this->sub_trees[i];                  // for all subnodes
    this->sub_trees[i] = copy_nodes(n.sub_trees[i]); // and make new deep copy
  }
  this->data = n.data;                          // Copy data value
  return *this;                                 // Return reference
}


// insert_before -- Insert sub-tree pointer to child before the specified
//                  zero-relative sub-tree index (numbered from left to right)
// Input:           Pointer to child node, zero-relative index
// Output:          TRUE/FALSE

template <class Type, int nchild> 
Boolean CoolN_Node<Type,nchild>::insert_before (CoolN_Node<Type,nchild>& n, int index) {
#if ERROR_CHECKING
  if (index < 0 || index >= nchild) {           // If index out of range
    this->index_error ("insert_before", index); // Raise exception
    return FALSE;                               // Return failure status
  }
#endif
  int i;
  for (i = nchild-1; i > index; i--)            // For each pointer after index
    this->sub_trees[i] = this->sub_trees[i-1];  // Move up one in vector
  this->sub_trees[i] = &n;                      // Pointer to new sub-tree
  return TRUE;                                  // Return success status
}


// insert_after -- Insert sub-tree pointer to child after the specified
//                 zero-relative sub-tree index (numbered from left to right)
// Input:          Pointer to child node, zero-relative index
// Output:         TRUE/FALSE

template <class Type, int nchild> 
Boolean CoolN_Node<Type,nchild>::insert_after (CoolN_Node<Type,nchild>& n, int index) {
#if ERROR_CHECKING
  if (index < 0 || index >= nchild) {           // If index out of range
    this->index_error ("insert_after", index);  // Raise exception
    return FALSE;                               // Return failure status
  }
#endif
  int i;
  for (i = nchild-1; i > index+1; i--)          // For each pointer after index
    this->sub_trees[i] = this->sub_trees[i-1];  // Move up one in vector
  this->sub_trees[i] = &n;                      // Pointer to new sub-tree
  return TRUE;                                  // Return success status
}


// copy_nodes -- Copies this node and all its subnodes
// Input:       pointer to node to be copied
// Output:      pointer to new copy of node with all new subnodes.

template <class Type, int nchild>
CoolN_Node<Type,nchild>* CoolN_Node<Type,nchild>::copy_nodes (const CoolN_Node<Type,nchild>* n) const {
  if (n == NULL)                                
    return NULL;
  CoolN_Node<Type,nchild>* new_n = new CoolN_Node<Type,nchild>; 
  for (int i = 0; i < nchild; i++)              // For each pointer in vector
    new_n->sub_trees[i] = copy_nodes(n->sub_trees[i]); // Deep copy of subnodes
  new_n->data = n->data;                               // Copy data value
  return new_n;                                       // Return copied node.
}


// index_error -- Raise exception invalid index
// Input:         Function name, invalid index
// Output:        None

template <class Type, int nchild> 
void CoolN_Node<Type,nchild>::index_error (const char* fcn, int n) {
  //RAISE Error, SYM(CoolN_Node), SYM(Out_Of_Range),
  printf ("CoolN_Node<%s,%d>::%s: Index %d out of range.\n", "Type", nchild, 
          fcn, n);
  abort ();
}

// default_CoolN_Node_compare -- Default node comparison function utilizing builtin
//                           less than, equal, and greater than operators
// Input:                    Reference to two Type data values
// Output:                   -1, 0, or 1 if less than, equal to, or greater than

template <class Type>
int default_CoolN_Node_compare (const Type& v1, const Type& v2) {
  if (v1 == v2)                         // If data items equal
    return 0;                                   // Return zero
  if (v1 < v2)                          // If this less than data
    return -1;                          // Return negative one
  return 1;                                     // Else return positive one
}
