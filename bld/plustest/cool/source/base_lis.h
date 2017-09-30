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
// Created: MJF 03/27/89 -- Initial design and implementation
// Updated: MJF 04/15/89 -- Implemented Base list class.
// Updated: MJF 06/01/89 -- Added const to member function arguments.
// Updated: JCB 06/05/89 -- Fixed sort and merge.
// Updated: JCB 06/20/89 -- Added protected slot, traversal, for use by
//                          next_lunion, next_intersection, etc.
//                          Modified reset() to initialize traversal flag.
// Updated: MJF 06/21/89 -- Changed return types from List& to void or Boolean.
// Updated: LGO 07/03/89 -- Inherit from Generic
// Updated: MJF 08/10/89 -- Changed return values of operator+= etc to List ref
// Updated: LGO 09/07/89 -- Make base-list constructor and destructor inline.
// Updated: MBN 09/20/89 -- Added conditional exception handling
// Updated: MBN 10/10/89 -- Added current_position() method for Iterator<Type>
// Updated: MBN 10/11/89 -- Changed "current_position" to "curpos" and also
//                          "previous_position" to "prevpos"
// Updated: LGO 10/18/89 -- Get rid of the value method.
// Updated: LGO 12/04/89 -- Make binary set operators not inline
// Updated: VDN 02/21/92 -- New lite version
// Updated: JAM 08/14/92 -- removed DOS specifics, stdized #includes
// Updated: JAM 08/14/92 -- modernized template syntax, remove macro hacks
//                          non-template classes Cool_List=>CoolBase_List
//                          CoolList_Node=>CoolBase_List_Node
// Updated: JAM 08/14/92 -- changed all void*s to const void*s; otherwise
//                          lots of assignments would need cast
// Updated: JAM 08/20/92 -- made *_state typedef a nested typedef "IterState"
//                          as per new Iterator convention
//
// A list is simply  made up of a collection  of nodes.   Each node contains  a
// reference  count,  a pointer to  the next  node  in  the  list, and the data
// object.
//
//                      +--------+         +--------+         +--------+
//                      | Ref=2  |         | Ref=1  |         | Ref=2  |
//    +-------+         +--------+         +--------+         +--------+
//    | CoolList--+---+---->| Next --+-------->| Next --+----+--->| Next 0 |
//    +-------+   |     +--------+         +--------+    |    +--------+
//                |     | Data   |         | Data   |    |    | Data   |
//                |     +--------+         +--------+    |    +--------+
//                |                                      |
//    +-------+   |                          +-------+   |
//    | CoolList--+                          | CoolList--+
//    +-------+                              +-------+
//
// The CoolList class is implemented with parameterized types. The specific type of
// the elements in the list is  left  as a  parameter for  the user.  Each list
// declared for elements of a different type  would be a  new class.  A list of
// ints,   CoolList<int>, would expand   to a class  CoolList_int  and  a list strings,
// CoolList<char*>, would expand  to a class CoolList_charp.  Note  that  the lists are
// homogeneous lists where each element of the list is of  the same known type.
// A heterogeneous list  could be maintained  by having the type  be void* or a
// Generic object type where the type of the actual data could be determined.
//
// The CoolList class private data consists of a pointer to a Node class.  The Node
// class contains a reference count, the data object, and a pointer to the next
// node.   The data in each node  of a  list is an object of  type "Type".  The
// CoolList  class is   derived from a  Base  CoolList class.  This file  contains  the
// implementation  of the Base   CoolList  class.  The  CoolList  class can be found in
// List.h
//
// The Base_List class implements the  generic list functionality.  This class
// is not usable as a standalone class, but rather is designed to be derived by
// the CoolList  class.   By providing generic operations   in  a base   class, the
// quantity of code generated for each implementation of the parameterized CoolList
// class is reduced considerably.
//
// All list nodes are  created dynamically and  mangaged with reference counts.
// In the node object,  the reference count  value indicates the number of list
// or node objects pointing to it.  When the value is zero the node  object and
// its data will  be freed.  The  reference count technique  is  used to ensure
// that the node and  its data gets  deallocated  when  the  node is  no longer
// referenced.
//
// The Node class was introduced in order  to maintain the reference count from
// each data  item in  the  list.  The CoolList  class  could have been implemented
// without a pointer to a Node and instead contain a pointer to  the data and a
// pointer to  the next list   item.   However, with  a reference  count it  is
// necessary to have a separate class.  Consider the following set of lists and
// sublists.  Both  List1 and List2  point to  (Node 1,  Node2,  Node3, Node4),
// List3 is a sublist pointing to (Node3,  Node4),  and List4 points to (Node5,
// Node3, Node4).  The  reference count totals for  each node  are shown in the
// ().  Without a separate  node for each data item,  it would be  difficult to
// determine when to deallocate the data when it is no longer referenced.
//
//  +-------+
//  | List1 |---+
//  +-------+   |
//              |
//              |
//              |     +-------+     +-------+     +-------+     +-------+
//              |     |       |     |       |     |       |     |       |
//              +---->| Node1 |---->| Node2 |---->| Node3 |---->| Node4 |---->0
//              |     |  (2)  |     |  (1)  |     |  (3)  |     |  (1)  |
//              |     +-------+     +-------+     +-------+     +-------+
//              |                                     ^
//              |                   +-------+         |   
//  +-------+   |                   | List3 |-------->+
//  | List2 |---+                   +-------+         ^
//  +-------+                                         |
//                                        +-------+   |
//                          +-------+     |       |   |
//                          | List4 |---->| Node5 |---+
//                          +-------+     |  (1)  |
//                                        +-------+
//
// Note that because we have chosen to have a CoolList be comprised of Nodes, it is
// not possible to have generic operations that will  scan a tree  comprised of
// Nodes  themselves containing CoolLists.   This  manifestation is unfortunate but
// necessary to implement heterogenous lists and the reference count scheme.
//
// There   are several constructors available for   the CoolList class.  The CoolList()
// constructor creates a null list,  setting the  node  pointer to  zero.   The
// CoolList(Type& a) constructor creates a list with one data node; a head value of
// a  and a nil  tail.  The CoolList(Type& a,  CoolList& b) constructor creates  a list
// with a head value of a and a b  as the tail.   The CoolList(CoolList& l) constructor
// creates a new list whose head node points to the same node as list l.
//
// The supported operations in the CoolList  class are mirrored closely after those
// in Common Lisp.  There are operations  which return the nth  tail of a list;
// the sublist starting at the nth last node of a list; and  all but the n last
// nodes of a list.  There are operations which return the length of  the list;
// get or set the nth element of the  list; return the  position of a specified
// element; test if the list is empty; clear a list; and test if  two lists are
// equal.   There are  also operations  to search  for  a  specified  member or
// sublist within a list; to reverse the list;  to copy the list; to  append or
// prepend an element or sublist; to set  the tail of  the list; to replace all
// or the first occurence of a specified item on the  list; to remove the first
// occurence  of  a specified item on  the list; to  remove  all duplicates; to
// insert a new item before or after  a specified item on  the list;  to sort a
// list; to merge two lists; to perform the  intersection; union; difference or
// exclusive-or of two lists.
//
// Unlike Common   Lisp,  the  destructive operations  do  not   come  with the
// equivalent non-destructive operations  are  supported.  Most operations will
// be  destructive, such that, the list  object  the message  is performed will
// always  be altered.  A  non-destructive  operation  is easily  done by first
// making a copy of   the  list  object  and  then executing   the  destructive
// operation on that copy.
//
// The CoolList class implements the  notion of a  current position. This is useful
// for  iterating  through the  elements of  a list.   The current position  is
// maintained as a node pointer and is set or reset by all operations affecting
// elements in  the  CoolList  class.  Operations to  reset,  move to the next  and
// previous, find, and get the value at the current position are provided.

#ifndef BASE_LISTH              // If the CoolList not defined,
#define BASE_LISTH              // indicate its done now

#include <iostream.h>

#ifndef MISCELANEOUSH           // If we have not included this file,
#include <cool/misc.h>          // include miscelaneous useful definitions.
#endif


typedef Boolean (*Compare) (const void*, const void*);  // Pointer to compate function
typedef int (*Predicate) (const void*, const void*);    // Pointer to Predicate
// returns -1 on less, 0 on equal and 1 on greater

class CoolBase_List_Node;                               // Forward reference class

class CoolBase_List_Node {                              // Define CoolBase_List_Node class
friend class CoolBase_List;                             // Friend class declaration
public:
  inline CoolBase_List_Node*& next_node();              // Return next node pointer
  friend ostream& operator<<(ostream& os, const CoolBase_List&); // Output operator
  
protected:
  int ref_count;                                // Reference counter
  CoolBase_List_Node* next;                             // Pointer to next node
  
  CoolBase_List_Node();                         // Constructor
  virtual ~CoolBase_List_Node();                        // Delete as ~CoolList_Node<Type>
  virtual const void* get_data();                       // Returns data element 
  virtual void  set_data(const void*);          // Set data element
};

// next_node() -- Returns next node pointer.
// Input:         None.
// Output:        The next node pointer

inline CoolBase_List_Node*& CoolBase_List_Node::next_node () {
  return this->next;
}


class CoolBase_List {                           // Define the CoolBase_List class
public:
  typedef CoolBase_List_Node* IterState;                // Curpos state for iterator

  CoolBase_List() {};                           // A Nil CoolBase_List
  virtual ~CoolBase_List();                             // Destructor is virtual
  
  inline void reset();                          // Reset current position
  inline Boolean next();                        // Increment current position
  Boolean prev();                               // Decrement current position
  
  Boolean operator==(const CoolBase_List& l) const;     // Equality test
  inline Boolean operator!=(const CoolBase_List& l) const; // Inequality test
  
  void tail(CoolBase_List& l, int n = 1);               // Sets l to the nth tail/cdr
  void last(CoolBase_List& l, int n = 1);               // Sets l to the n last nodes
  void but_last(CoolBase_List& l, int n = 1);   // Sets l to all but n last
  
  void clear();                                 // Removes all nodes 
  inline Boolean is_empty();                    // Any nodes in List?
  int length();                                 // Returns node count in List
  int position();                               // Returns current position
  
  Boolean search(const CoolBase_List& l);               // SubList search
  
  // returns true if THIS CoolBase_List contains the specified subList
  // and sets CoolBase_List l to a subList in THIS List starting at the 1st occurence
  // of CoolBase_List s
  Boolean sublist(CoolBase_List& l, const CoolBase_List& s);
  
  void copy(const CoolBase_List& l);                    // Copy l into *this
  void reverse();                               // Reverses order of elements
  Boolean prepend(const CoolBase_List& l);              // Prepends l to start of List
  Boolean append(const CoolBase_List& l);               // Appends  l to end of List
  
  Boolean set_tail(const CoolBase_List& l, int n = 1); // rplacd a l
  Boolean remove_duplicates();                    // Removes duplicate elements
  
  void set_intersection(const CoolBase_List& l);        // Intersection of two Lists
  void set_union(const CoolBase_List& l);               // Union of two Lists
  void set_difference(const CoolBase_List& l);  // Difference of two Lists
  void set_xor(const CoolBase_List& l);         // XOR of two Lists
  
  Boolean next_intersection(const CoolBase_List& l); // Current position intersect
  Boolean next_union(const CoolBase_List& l);   // Current position union
  Boolean next_difference(const CoolBase_List& l);      // Current position difference
  Boolean next_xor(const CoolBase_List& l);             // Current position XOR
  
  void describe(ostream& os);                   // Describes structure of List
  
  friend ostream& operator<<(ostream& os, const CoolBase_List& l); // Output 
  /*##inline*/ friend ostream& operator<<(ostream& os, const CoolBase_List* l); 

protected:
  CoolBase_List_Node* node_ptr;                 // Pointer to first node
  CoolBase_List_Node* curpos;                   // Maintains current position
  CoolBase_List_Node* prevpos;                  // Performance hack to previous
  Boolean traversal;                            // Traversal flag for curpos
  
  virtual CoolBase_List* new_list(CoolBase_List_Node*); // Creates a new CoolBase_List from node
  virtual CoolBase_List_Node* insert_before_node(const void* v, CoolBase_List_Node* next_np);
  virtual CoolBase_List_Node* insert_after_node(const void* v, CoolBase_List_Node* prev_np) const;
  
  virtual Boolean compare_data(const void*, const void*) const;
  virtual void output_data(ostream&, const CoolBase_List_Node*) const;
  
  CoolBase_List_Node* copy_nodes() const;               // Returns copy of nodes
  inline void reference(CoolBase_List_Node*);   // Increments reference count
  inline void dereference(CoolBase_List_Node*); // Decrements reference count
  // And if zero, deletes node
  void CoolBase_List::free_nodes(CoolBase_List_Node* np); // Deletes nodes
  
  CoolBase_List* operator=(const CoolBase_List& l);     // Assignment List1 = List2;
  CoolBase_List_Node* operator[](int n);                // X = l[n];
  
  int position(const void* x);                  // Returns 0-relative index 
  virtual Boolean do_find(CoolBase_List_Node* np, const void* x, // Used by find
                          CoolBase_List_Node*& cp, CoolBase_List_Node*& pp) const;
  // returns true if THIS CoolBase_List contains element x and 
  // sets CoolBase_List l to a subList in THIS List starting at the first occurence of x
  Boolean member(CoolBase_List& l, const void* x);
  
  Boolean push(const void* x);                  // Adds X to head of THIS List
  Boolean push_new(const void* x);              // Push(X) if not in THIS List
  Boolean push_end(const void* x);              // Adds X at end of THIS List
  Boolean push_end_new(const void* x);          // push_end(x) if not in List
  
  CoolBase_List_Node* pop();                            // Removes/returns head node
  CoolBase_List_Node* remove();                 // Removes item at curpos
  Boolean remove(const void* x);                // Removes first occurence
  
  Boolean replace(const void*, const void*);    // Replace first
  Boolean replace_all(const void*,const void*); // Replace all
  
  void sort(Predicate f);                       // Sort List using predicate
  void merge(const CoolBase_List& l, Predicate f);      // Merge List w/ sort predicate
  
  Boolean insert_before(const void* new_item);  // Insert item before curpos
  Boolean insert_after(const void* new_item);   // Insert item after curpos
  
  Boolean insert_before(const void*,const void*); // Insert item before
  Boolean insert_after(const void*, const void*); // Insert item after
  
  void value_error (const char*);               // Raise exception
  void get_error (const char*, int);            // Raise exception
  void before_error (const char*);              // Raise exception
  void after_error (const char*);               // Raise exception
  void bracket_error (const char*, int);        // Raise exception
  void pop_error (const char*);                 // Raise exception
  void remove_error (const char*);              // Raise exception
  void va_arg_error (const char*, int);         // Raise exception
};


// reference () -- Increments the reference count of a node pointer
// Input:          The node pointer to be referenced.
// Output:         None.

inline void CoolBase_List::reference(CoolBase_List_Node* np) {
  if (np != NULL) np->ref_count++;
}


// dereference() -- Decrements the reference count of the node pointer and
//                  deallocates storage if no longer referenced.
// Input:           The node pointer to be dereferenced.
// Output:          None.

inline void CoolBase_List::dereference(CoolBase_List_Node* np) {
  if (np != NULL && --(np->ref_count) <= 0)
    this->free_nodes(np);
}


// reset() -- sets current position to NULL
// Input:     None.
// Output:    None.

inline void CoolBase_List::reset() {
  this->curpos = NULL;
  this->prevpos = NULL;
  this->traversal = TRUE;
}


// next() -- Increment current position. If NULL, set it to first.
// Input:    None.
// Output:   TRUE or FALSE.

inline Boolean CoolBase_List::next() {
  register CoolBase_List_Node* cp = this->curpos;
  if (cp == NULL)                               // Current position valid?
    cp = this->node_ptr;                        // Set curpos to head node
  else
    (cp = cp->next);                            // Advance to next position
  return ((this->curpos = cp) != NULL);         // Return status
}


// operator!=() -- Returns TRUE if data in THIS CoolBase_List and the not the same
// Input:          A CoolBase_List reference.
// Output:         TRUE or FALSE.

inline Boolean CoolBase_List::operator!=(const CoolBase_List& l) const {
  return !this->operator==(l);
}


// is_empty() -- Indicates node presence in CoolBase_List
// Input:        None.
// Output:       TRUE or FALSE.

inline Boolean CoolBase_List::is_empty() {
  return this->node_ptr == NULL;
}

// operator<<() -- Overload output operator for CoolBase_List objects
// Input:          An output stream reference and CoolBase_List pointer.
// Output:         An output stream reference.

inline ostream& operator<<(ostream& os, const CoolBase_List* l) {
  return operator<<(os, *l);
}

#endif                                          // End of BASE_LISTH
