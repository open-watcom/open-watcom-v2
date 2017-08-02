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
// Created: MBN 07/11/89 -- Initial design
// Updated: MBN 08/10/89 -- Initial implementation
// Updated: MBN 08/11/89 -- Inherit from Generic
// Updated: MBN 09/19/89 -- Added conditional exception handling
// Updated: MBN 12/21/89 -- Added optional argument to set_compare method
// Updated: MBN 02/27/90 -- Added operator= for pointer argument
// Updated: MJF 03/12/90 -- Added group names to RAISE
// Updated: VDN 02/21/92 -- New lite version and fix memory leaks
// Updated: JAM 08/19/92 -- modernized template syntax, remove macro hacks
// Updated: JAM 08/19/92 -- insert*() now take non-const Node& since will own
// Updated: JAM 09/28/92 -- added ItemType and max_children members for
//                          new N_Tree convention
//
// The D_Node<Type,nchild> class implements parameterized  nodes of  a  dynamic
// size for N-ary  trees.   This node class  is parameterized for  the type and
// some initial  "N",  the   number  of  subtrees  each  node may   have.   The
// D_Node<Type,nchild>  class  is dynamic   in  the  sense that the   number of
// subtrees allowed for each  node is not  fixed.  D_Node<Type,nchild> utilizes
// the Vector<Type>  class,  supporting runtime growth charactersitics.    As a
// result, the D_Node<Type,nchild> class  should be  used as the node  type for
// the N_Tree<Node,Type,nchild> class when  the number of subtrees is variable,
// unknown at  compile time, or   needs to  increase on   a per-node  basis  at
// runtime.  This capability is suited for heirarchical trees such as  might be
// used  in an   organization  chart.   In  addition,   specialization  of  the
// N_Tree<Node,Type,nchild>  class  would  allow   for  the   relatively   easy
// implementation of a Diagram class.
//
// There are three public constructors for the  D_Node class to allow  the user
// to  create nodes and  control the building and   structure of an  N-ary tree
// where the  ordering   can  have a  specific  meaning.   The  first takes  no
// arguments and initializes the pointer  and data  slots to NULL.   The second
// takes an argument of type Type and initializes  the data slot to that value.
// The third takes a reference to another D_Node object and duplicates the data
// value, but not its children.
//
// The private  data section  contains just two  slots.  The  first is a Vector
// object of some initial size "N"  that  contains pointers to  D_Node objects,
// one for each  child.  The second  is a data slot  of type Type  to hold  the
// value of the data item.
//
// Methods are provided to set and get the node data value, determine if a node
// is a leaf or the root of some subtree,  and implement member-wise assignment
// from one  D_Node to another via  the overloaded operator=. In  addition, the
// brackets operator is overloaded to  provided  a mechanism to efficiently get
// and set individual  subtree pointers in  a specific node,  thus allowing the
// user to   control  the  orgranization  and  structure   of a tree.  Finally,
// insertion, and removal  methods to allow the  user to control  placement and
// ordering of sub-trees of a node are available.
//

#ifndef D_NODEH                                 // If no definition for class
#define D_NODEH

#ifndef MISCELANEOUSH           // If we have not included this file,
#include <cool/misc.h>          // include miscelaneous useful definitions.
#endif

#ifndef VECTORH                                 // If no Vector class defined
#include <cool/Vector.h>
#endif

template <class Node>
class CoolN_Tree;

template <class Type, int nchild>
class CoolD_Node {
public:
  typedef Boolean (*Compare)(const Type&, const Type&);
  typedef CoolD_Node<Type,nchild>* CoolD_Node_p; // Pointer to class

  typedef Type ItemType;
  static int max_children() { return nchild; }

  CoolD_Node();                 // Simple constructor
  CoolD_Node(const Type& value);        // constructor with data value
  CoolD_Node(const CoolD_Node<Type,nchild>&);   // Copy constructor
  ~CoolD_Node();                        // Destructor

  inline void set (const Type&);                // Set node data to value
  inline Type& get () const;                    // Get node data value
  Boolean is_leaf () const;                     // TRUE if node has no children
  inline CoolD_Node_p& operator[] (int); // Set/Get pointers
  inline int num_subtrees() const;               // number of subtree slots in node
  
  inline void set_compare (Compare = NULL); // Set compare
  inline Boolean operator== (const Type&) const;     // Overload operator==
  inline Boolean operator!= (const Type&) const;     // Overload operator!=
  inline Boolean operator< (const Type&) const;      // Overload operator<
  inline Boolean operator> (const Type&) const;      // Overload operator>
  inline Boolean operator<= (const Type&) const;     // Overload operator<=
  inline Boolean operator>= (const Type&) const;     // Overload operator>=

  CoolD_Node<Type,nchild>& operator= (const CoolD_Node<Type,nchild>&); // Assignment
  Boolean insert_before (CoolD_Node<Type,nchild>&, int);     // Insert before 
  Boolean insert_after (CoolD_Node<Type,nchild>&, int);      // Insert after

private:
public: //## when friend below works with BC++, delete this line
  CoolVector<CoolD_Node<Type,nchild>*> sub_trees; // Vector of subtree pointers
  Type data;                                    // Slot to hold data value
  static Compare compare_s;     // Compare function for class

  CoolD_Node<Type,nchild>* copy_nodes(const CoolD_Node<Type,nchild>*) const; // Deep copy
  void index_error (const char* fcn, int i);    // Raise exception

//##  friend class CoolN_Tree<CoolD_Node<Type,nchild> >;        // Friend class to access data
  template< class U >
  friend int default_CoolD_Node_compare (const U&, const U&);
};


// num_subtrees -- Returns number of slots available for subtrees in node
// Input:          None
// Output:         int length of the CoolVector allocated  for Subtrees.

template <class Type, int nchild>
inline int CoolD_Node<Type,nchild>::num_subtrees () const {
  return sub_trees.length();
}


// set -- Set value of data slot in node
// Input: Reference to data slot value
// Output: None

template <class Type, int nchild> 
inline void CoolD_Node<Type,nchild>::set (const Type& value) {
  this->data = value;                           // Set data slot value
}


// get -- Get value of data slot in node
// Input: None
// Output: Reference to data slot value

template <class Type, int nchild> 
inline Type& CoolD_Node<Type,nchild>::get () const {
  return ((CoolD_Node<Type,nchild>*)this)->data; // Return data slot value
}


// set_compare -- Specify the comparison function to be used in logical tests
//                of node data values
// Input:         Pointer to a compare function
// Output:        None

template <class Type, int nchild> 
inline void CoolD_Node<Type,nchild>::set_compare ( register /*CoolD_Node<Type,nchild>::Compare##*/Boolean (*c)(const Type&, const Type&)) {
  if (c == NULL)
    this->compare_s = &default_CoolD_Node_compare; // Default equality
  else
    this->compare_s = c;                        // Else use one provided
}


// operator== -- Overload the equality operator to use the compare function
// Input:        constant reference to Type value
// Output:       TRUE/FALSE

template <class Type, int nchild> 
inline Boolean CoolD_Node<Type,nchild>::operator== (const Type& value) const {
  return ((((*this->compare_s)(this->data,value)) == 0) ? TRUE : FALSE);
}


// operator!= -- Overload the inequality operator to use the compare function
// Input:        constant reference to Type value
// Output:       TRUE/FALSE

template <class Type, int nchild> 
inline Boolean CoolD_Node<Type,nchild>::operator!= (const Type& value) const {
  return ((((*this->compare_s)(this->data,value)) == 0) ? FALSE : TRUE);
}


// operator< -- Overload the less than operator to use the compare function
// Input:       constant reference to Type value
// Output:      TRUE/FALSE

template <class Type, int nchild> 
inline Boolean CoolD_Node<Type,nchild>::operator< (const Type& value) const {
  return ((((*this->compare_s)(this->data,value)) < 0) ? TRUE : FALSE);
}


// operator> -- Overload the greater than operator to use the compare function
// Input:       constant reference to Type value
// Output:      TRUE/FALSE

template <class Type, int nchild> 
inline Boolean CoolD_Node<Type,nchild>::operator> (const Type& value) const {
  return ((((*this->compare_s)(this->data,value)) > 0) ? TRUE : FALSE);
}


// operator<= -- Overload the less than or equal operator to use the compare
//               function
// Input:        constant reference to Type value
// Output:       TRUE/FALSE

template <class Type, int nchild> 
inline Boolean CoolD_Node<Type,nchild>::operator<= (const Type& value) const {
  return ((((*this->compare_s)(this->data,value)) > 0) ? FALSE : TRUE);
}


// operator>= -- Overload the greater than or equal operator to use the compare
//               function
// Input:        constant reference to Type value
// Output:       TRUE/FALSE

template <class Type, int nchild> 
inline Boolean CoolD_Node<Type,nchild>::operator>= (const Type& value) const {
  return ((((*this->compare_s)(this->data,value)) < 0) ? FALSE : TRUE);
}


// operator[] -- Overload the brackets operator to provide a mechanism to set
//               and/or get a sub-tree pointer of a node whose zero-relative
//               index is specified from left to right
// Input:        Zero-relative index into vector of sub-tree pointers
// Output:       Reference to a pointer value

template <class Type, int nchild> 
inline /*CoolD_Node<Type,nchild>::CoolD_Node_p##*/CoolD_Node<Type,nchild>*& CoolD_Node<Type,nchild>::operator[](int index) {
#if ERROR_CHECKING
  if (index >= this->num_subtrees())            // If index out of range
    this->index_error ("operator[]", index);    // Raise exception
#endif
  return (this->sub_trees[index]);
}



#endif                                          // End D_NODEH #if
