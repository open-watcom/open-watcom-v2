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
// Created: MBN 07/05/89 -- Initial design
// Updated: MBN 08/10/89 -- Initial implementation
// Updated: MBN 08/11/89 -- Inherit from Generic
// Updated: MBN 09/19/89 -- Added conditional exception handling
// Updated: MBN 12/21/89 -- Added optional argument to set_compare method
// Updated: MBN 02/27/90 -- Added operator= for pointer argument
// Updated: MJF 03/12/90 -- Added group names to RAISE
// Updated: VDN 02/21/92 -- New lite version
// Updated: JAM 08/19/92 -- modernized template syntax, remove macro hacks
// Updated: JAM 09/28/92 -- added ItemType and max_children members for
//                          new N_Tree convention
//
// The N_Node<Type,nchild>  class implements parameterized   nodes of a  static
// size  for N-ary trees.   This node class  is parameterized for both the type
// and some "N", the number of  subtrees each node  may have.  The constructors
// for  the  N_Node<Type,nchild> class are  declared  in the public  section to
// allow the user to create nodes and control  the building and structure of an
// N-ary  tree where  the   ordering can have  a specific   meaning, as with an
// expression tree.
//
// The  private data section contains  just two slots.  The  first is  a static
// sized vector of "N" pointers to N_Node objects, one for  each subtree.   The
// second is  a data  slot  of type Type  to hold the  value  of the data item.
// There are three  public constructors for the  N-Node class.  The first takes
// no arguments and initializes the pointer and data slots to NULL.  The second
// takes an argument of type Type and initializes the data  slot to that value.
// The  third takes a  reference to  another  N-Node object  and duplicates its
// values.
//
// Methods are provided to set and get the node data value, determine if a node
// is a leaf or the root of some subtree,  and implement member-wise assignment
// from one  N-Node to another via  the overloaded operator=. In  addition, the
// brackets operator is overloaded to  provided  a mechanism to efficiently get
// and set individual  subtree pointers in  a specific node,  thus allowing the
// user to   control  the  orgranization  and  structure   of a tree.  Finally,
// insertion, and removal  methods to allow the  user to control  placement and
// ordering of sub-trees of a node are available.
//

#ifndef N_NODEH                                 // If no definition for class
#define N_NODEH

#ifndef MISCELANEOUSH           // If we have not included this file,
#include <cool/misc.h>          // include miscelaneous useful definitions.
#endif

template <class Node>
class CoolN_Tree;       // Forward reference class

template <class Type, int nchild>
class CoolN_Node {
public:
  typedef Boolean (*Compare)(const Type&, const Type&);
  typedef CoolN_Node<Type,nchild>* CoolN_Node_p; //Pointer to class

  typedef Type ItemType;
  static int max_children() { return nchild; }

  CoolN_Node();                 // Simple constructor
  CoolN_Node(const Type&);      // constructor with data value
  CoolN_Node(const CoolN_Node<Type,nchild>&); // Copy constructor
  ~CoolN_Node();                        // Destructor

  inline void set (const Type&);                // Set node data to value
  inline Type& get ();                          // Get node data value
  Boolean is_leaf () const;                     // TRUE if node has no children
  inline CoolN_Node_p& operator[] (int); // Set/Get pointers
  inline int num_subtrees () const;              // Number of subtree slots
  
  inline void set_compare (Compare = NULL); // Set compare
  inline Boolean operator== (const Type&) const;     // Overload operator==
  inline Boolean operator!= (const Type&) const;     // Overload operator!=
  inline Boolean operator< (const Type&) const;      // Overload operator<
  inline Boolean operator> (const Type&) const;      // Overload operator>
  inline Boolean operator<= (const Type&) const;     // Overload operator<=
  inline Boolean operator>= (const Type&) const;     // Overload operator>=

  CoolN_Node<Type,nchild>& operator= (const CoolN_Node<Type,nchild>&); // Assignment
  Boolean insert_before (CoolN_Node<Type,nchild>&, int); // Insert subtree
  Boolean insert_after (CoolN_Node<Type,nchild>&, int);  // Insert subtree

private:
public: //## when friend below works with BC++, delete this line
//##  CoolN_Node_p sub_trees[nchild]; // Vector of subtree pointers
  CoolN_Node<Type,nchild>* sub_trees[nchild]; // Vector of subtree pointers
  Type data;                                    // Slot to hold data value
  static Compare compare_s;     // Compare function for class

  CoolN_Node<Type,nchild>* copy_nodes(const CoolN_Node<Type,nchild>*) const; // Deep copy
  void index_error (const char* fcn, int i);    // Raise exception

//##  friend class CoolN_Tree<CoolN_Node<Type,nchild> >;  // Friend class to access data
  template< class U >
  friend int default_CoolN_Node_compare (const U&, const U&);
};



// num_subtrees -- Returns number of slots available for subtrees in node
// Input:          None
// Output:         int length of the allocated array for Nodes

template <class Type, int nchild>
inline int CoolN_Node<Type,nchild>::num_subtrees () const {
  return nchild;
}

// set -- Set value of data slot in node
// Input: Reference to data slot value
// Output: None

template <class Type, int nchild> 
inline void CoolN_Node<Type,nchild>::set (const Type& value) {
  this->data = value;                           // Set data slot value
}


// get -- Get value of data slot in node
// Input: None
// Output: Reference to data slot value

template <class Type, int nchild> 
inline Type& CoolN_Node<Type,nchild>::get () {
  return this->data;                            // Return data slot value
}


// set_compare -- Specify the comparison function to be used in logical tests
//                of node data values
// Input:         Pointer to a compare function
// Output:        None

template <class Type, int nchild> 
inline void CoolN_Node<Type,nchild>::set_compare (register /*Compare##*/Boolean (*c)(const Type&, const Type&)) {
  if (c == NULL)
    this->compare_s = &default_CoolN_Node_compare; // Default equality
  else
    this->compare_s = c;                        // Else use one provided
}


// operator== -- Overload the equality operator to use the compare function
// Input:        constant reference to Type value
// Output:       TRUE/FALSE

template <class Type, int nchild> 
inline Boolean CoolN_Node<Type,nchild>::operator== (const Type& value) const {
  return ((((*this->compare_s)(this->data,value)) == 0) ? TRUE : FALSE);
}


// operator!= -- Overload the inequality operator to use the compare function
// Input:        constant reference to Type value
// Output:       TRUE/FALSE

template <class Type, int nchild> 
inline Boolean CoolN_Node<Type,nchild>::operator!= (const Type& value) const {
  return ((((*this->compare_s)(this->data,value)) == 0) ? FALSE : TRUE);
}


// operator< -- Overload the less than operator to use the compare function
// Input:       constant reference to Type value
// Output:      TRUE/FALSE

template <class Type, int nchild> 
inline Boolean CoolN_Node<Type,nchild>::operator< (const Type& value) const {
  return ((((*this->compare_s)(this->data,value)) < 0) ? TRUE : FALSE);
}


// operator> -- Overload the greater than operator to use the compare function
// Input:       constant reference to Type value
// Output:      TRUE/FALSE

template <class Type, int nchild> 
inline Boolean CoolN_Node<Type,nchild>::operator> (const Type& value) const {
  return ((((*this->compare_s)(this->data,value)) > 0) ? TRUE : FALSE);
}


// operator<= -- Overload the less than or equal operator to use the compare
//               function
// Input:        constant reference to Type value
// Output:       TRUE/FALSE

template <class Type, int nchild> 
inline Boolean CoolN_Node<Type,nchild>::operator<= (const Type& value) const {
  return ((((*this->compare_s)(this->data,value)) > 0) ? FALSE : TRUE);
}


// operator>= -- Overload the greater than or equal operator to use the compare
//               function
// Input:        constant reference to Type value
// Output:       TRUE/FALSE

template <class Type, int nchild> 
inline Boolean CoolN_Node<Type,nchild>::operator>= (const Type& value) const {
  return ((((*this->compare_s)(this->data,value)) < 0) ? FALSE : TRUE);
}



#endif                                          // End N_NODEH #if


