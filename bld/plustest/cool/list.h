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
// Created: MJF 03/27/89 -- Initial design and implementation.
// Updated: MJF 04/15/89 -- Added a Base List class.
// Updated: MJF 06/01/89 -- Added const to member function arguments.
// Updated: MJF 06/21/89 -- Changed return types from List& to void or Boolean.
// Updated: MJF 08/10/89 -- Changed return values of methods to List reference
// Updated: MBN 08/20/89 -- Changed template usage to reflect new syntax
// Updated: LGO 10/02/89 -- Fix reference count bug in insert_after_node
// Updated: LGO 10/02/89 -- Set reference count to 1 in node constructor and
//                          eliminate many reference method calls.
// Updated: MBN 10/11/89 -- Change "current_position" to "curpos" and also
//                          "previous_position" to "prevpos"
// Updated: LGO 10/18/89 -- Get rid of the sort_test method
// Updated: LGO 10/18/89 -- Simplify the value method to avoid yacc stack oflow
// Updated: MBN 10/19/89 -- Added optional argument to set_compare method
// Updated: MBN 10/19/89 -- Added optional starting position to find method
// Updated: MBN 02/14/90 -- Make push return FALSE if out of heap memory
// Updated: VDN 02/21/92 -- New lite version and plug all memory leaks
// Updated: JAM 08/14/92 -- removed DOS specifics, stdized #includes
// Updated: JAM 08/14/92 -- modernized template syntax, remove macro hacks
//                          non-template classes Cool_List=>CoolBase_List
//                          CoolList_Node=>CoolBase_List_Node
// Updated: JAM 08/14/92 -- consistent use of const void* like in Base_List
// Updated: JAM 08/20/92 -- made *_state typedef a nested typedef "IterState"
//                          as per new Iterator convention
//
// A list is simply  made up  of a  collection of nodes.   Each node contains a
// reference count, a  pointer to  the next node  in  the  list, and the   data
// object.  An overview of the structure of  the  List class, along with  a
// synopsis of each member and friend function, can be found in the Base_List.h
// header file.
//
//
//                      +--------+         +--------+         +--------+
//                      | Ref=2  |         | Ref=1  |         | Ref=2  |
//    +---------+       +--------+         +--------+         +--------+
//    |  List --+-+---->| Next --+-------->| Next --+----+--->| Next 0 |
//    +---------+ |     +--------+         +--------+    |    +--------+
//                |     | Data   |         | Data   |    |    | Data   |
//                |     +--------+         +--------+    |    +--------+
//                |                                      |
//    +---------+ |                          +---------+ |
//    |  List --+-+                          |  List --+-+
//    +---------+                            +---------+
//

#ifndef LISTH                                   // If LIST not yet defined,
#define LISTH                                   // indicate class List done

#include <stdarg.h>                             // for variable arglists

#ifndef BASE_LISTH                              // If Base LIST not defined,
#include <cool/Base_List.h>                     // include Base_List header
#endif


template <class Type> 
class CoolList_Node : public CoolBase_List_Node {
friend class CoolList<Type>;
public:
  CoolList_Node(CoolList_Node<Type>&);  // Copy constructor
  CoolList_Node(const Type& head, CoolList_Node<Type>* tail);
  virtual const void* get_data();                       // Returns data element 
  virtual void  set_data(const void*);          // Set data element

  Type data;                                    // Data of node
protected:
  virtual ~CoolList_Node();             // Destructor is virtual
};

template <class Type>
class CoolList : public CoolBase_List {
public:
  CoolList();                           // List<int> l;  a nil list.
  CoolList(int n, Type head, ...);      // List<int> l4 = (3,11,22,33);
  CoolList(const Type& head);           // List<String> l1 = "333";
  CoolList(const Type& head, CoolList<Type>& tail); //List<String>l2 = ("a", l1);
  CoolList(CoolList<Type>& tail);                         // Copy constructor

  virtual ~CoolList();                  // Destructor is virtual
  
  inline Type& value();                         // Value at current position
  int position();                               // Current position index
  
  void set_compare(Boolean (*cf)(const Type&, const Type&) = 0); // Sets Compare
  inline CoolList<Type>& operator=(const CoolList<Type>& l);  // list1 = list2;
  
  Type& operator[](int n);                      // x = l[n];
  inline Type& get(int n = 0);                  // Returns nth data-node
  Boolean put(const Type& x, int n = 0);        // Sets nth data-node to x
  
  inline int position(const Type& x);           // Returns zero-relative index 
  inline IterState& current_position ();        // Set/Get current position
  inline Boolean find(const Type& x, IterState s = NULL); // True if contains x
  
  // returns true if THIS list contains element x and 
  // sets list l to a sublist in THIS list starting at first occurrence of x
  inline Boolean member(CoolList<Type>& l, const Type& x);
  
  Boolean push(const Type& x);                  // Adds x to head of this list
  inline Boolean push_new(const Type& x);       // Push if not already member
  inline Boolean push_end(const Type& x);       // Adds x at end of this list
  inline Boolean push_end_new(const Type& x);   // Push_end(x) if not member
  
  Boolean pop(Type& result);                    // Removes head/returns data
  Type pop();                                   // Removes head/returns data
  
  Type remove();                                // Remove/return curpos
  inline Boolean remove(const Type& x);         // Removes first occurrence
  
  inline Boolean replace(const Type& old_data, const Type& new_data); 
  inline Boolean replace_all(const Type& old_data, const Type& new_data); 
  
  inline void sort(Boolean (*f)(const Type&, const Type&)); // Sort list using predicate
  inline void merge(const CoolList<Type>& l, Boolean (*f)(const Type&, const Type&)); // Merge
  
  inline Boolean insert_before(const Type& new_item); // Insert item before
  inline Boolean insert_after(const Type& new_item);  // Insert item after
  
  inline Boolean insert_before(const Type& new_item, const Type& targ_item);
  inline Boolean insert_after(const Type& new_item, const Type& targ_item);
  
  inline CoolList<Type>& operator&=(const CoolList<Type>& l); // Intersection/assign
  inline CoolList<Type>& operator|=(const CoolList<Type>& l); // Union/assign
  inline CoolList<Type>& operator-=(const CoolList<Type>& l); // Difference/assign
  inline CoolList<Type>& operator^=(const CoolList<Type>& l); // XOR/assign
  inline CoolList<Type>& operator+=(const CoolList<Type>& l); // Concatenation/assign
  
  inline CoolList<Type> operator&(const CoolList<Type>& l); // Intersection
  inline CoolList<Type> operator|(const CoolList<Type>& l); // Union
  inline CoolList<Type> operator-(const CoolList<Type>& l); // Difference
  inline CoolList<Type> operator^(const CoolList<Type>& l); // XOR
  inline CoolList<Type> operator+(const CoolList<Type>& l); // Concatenation

private:
  static Boolean (*compare_s)(const Type&, const Type&);        // Function used by == test
  template< class U >
  friend Boolean CoolList_is_data_equal(const U& a, const U& b); // a==b
  
  CoolList(CoolList_Node<Type>*);               // Internal-use constructor
  virtual CoolBase_List* new_list(CoolBase_List_Node*); // Creates a new list from node
  
  virtual CoolBase_List_Node* insert_before_node(const void* v, CoolBase_List_Node* next_np);
  virtual CoolBase_List_Node* insert_after_node(const void* v, CoolBase_List_Node* prev_np) const;
  
  virtual Boolean compare_data(const void*, const void*) const;
  virtual Boolean do_find(CoolBase_List_Node* np, const void* x,
                          CoolBase_List_Node*& cp, CoolBase_List_Node*& pp) const;
  
  virtual void output_data(ostream&, const CoolBase_List_Node*) const;
};


// value() -- Returns value at current position.
// Input:     None.
// Output:    A Type reference of data at current position.

template <class Type> 
inline Type& CoolList<Type>::value() {
#ifdef ERROR_CHECKING  
  if (CoolBase_List::curpos == NULL)
    this->value_error (#Type);                  // Raise exception
#endif
  return ((CoolList_Node<Type>*) CoolBase_List::curpos)->data;
}

// position -- Return current position.
// Input:      THIS.
// Output:     An integer representing current position.

template <class Type> 
inline int CoolList<Type>::position() {
  return CoolBase_List::position();                     // can inherit only if has
}


// operator=() -- Assigns THIS to the specified list.
// Input:         A reference to a list which THIS will be assigned to.
// Output:        A reference to THIS.

template <class Type> 
inline CoolList<Type>& CoolList<Type>::operator=(const CoolList<Type>& l) {
  if (this != &l) 
    CoolBase_List::operator=(l);
  return *this;
}


// get() -- Returns the nth node of THIS. With no arguments, returns first node
// Input:   A positive integer index (default 0).
// Output:  A Type reference of data in the nth node of THIS.

template <class Type> 
inline Type& CoolList<Type>::get(int n) {
#if ERROR_CHECKING
  if (n < 0)
    this->get_error (#Type, n);
#endif
  return operator[](n);
}



// position() -- Returns the position of the specified data item in this list.
//               If item not in list, returns -1
// Input:        A Type reference to a data item.
// Output:       The integer position.

template <class Type> 
inline int CoolList<Type>::position(const Type& x) {
  return CoolBase_List::position((const void*)&x);
}

// current_position () -- Return current position state
// Input:                 None
// Output:                Reference to current position state

template <class Type> 
inline /*CoolList<Type>::IterState##*/CoolBase_List_Node*& CoolList<Type>::current_position () {
  return this->curpos;
}


// find() -- Returns TRUE if the specified element is a member of THIS list.
// Input:    A Type reference to data item to be searched.
// Output:   TRUE or FALSE.

template <class Type> 
inline Boolean CoolList<Type>::find(const Type& x, /*CoolList<Type>::IterState##*/CoolBase_List_Node* s) {
  if (s == NULL)                                // If no starting position?
    s = this->node_ptr;                         // Start at head of list
  return this->do_find(s, &x, this->curpos, this->prevpos); // Find and return
}


// member() -- Returns the tail of THIS list beginning with the first
//             occurrence of the specified data item.
// Input:      A Type reference to data item to be searched.
// Output:     A list reference of some tail of THIS.

template <class Type> 
inline Boolean CoolList<Type>::member(CoolList<Type>& l, const Type& x) {
  return CoolBase_List::member(l, (const void*)&x);
}



// push_new() -- Pushes the specified data item at front of this list if it is
//               not already a member 
// Input:        A reference to a new Type.
  // Output:       TRUE if item not on list, FALSE otherwise.

template <class Type> 
inline Boolean CoolList<Type>::push_new(const Type& x) {
  return CoolBase_List::push_new((const void*)&x);
}


// push_end() -- Appends the specified data item to the end of this list
// Input:        A Type reference to the data item to be appended.
// Output:       TRUE.

template <class Type> 
inline Boolean CoolList<Type>::push_end(const Type& x) {
  return CoolBase_List::push_end((const void*)&x);
}


// push_end_new() -- Appends the specified data item to the end of THIS list
//                   if not already a member 
// Input:            A Type reference to the data item to be appended.
// Output:           TRUE if item not on list, FALSE otherwise.

template <class Type> 
inline Boolean CoolList<Type>::push_end_new(const Type& x) {
  return CoolBase_List::push_end_new((const void*)&x);
}


// remove() -- Removes the first occurrence of the specified item in this list
// Input:      A refernce to data item to be removed.
// Output:     TRUE if item found and removed, FALSE otherwise.

template <class Type> 
inline Boolean CoolList<Type>::remove(const Type& x) {
  return CoolBase_List::remove((const void*)&x);
}


// replace() -- Replaces the first occurrence of specified data item in THIS
//              list with a new value
// Input:       A reference to the data item to be replaced and the new value.
// Output:      TRUE if item found and replaced, FALSE otherwise.

template <class Type> 
inline Boolean CoolList<Type>::replace(const Type& old_data, const Type& new_data) {
  return CoolBase_List::replace((const void*)&old_data, (const void*)&new_data);
}


// replace_all() -- Replaces all occurrences of the specified data item in THIS
//                  list with a new value.
// Input:           A reference to the data item to be replaced and new value.
// Output:          TRUE if at least one item found and replaced, else FALSE

template <class Type> 
inline Boolean CoolList<Type>::replace_all(const Type& old_d, const Type& new_d) {
  return CoolBase_List::replace_all((const void*)&old_d, (const void*)&new_d);
}


// sort() -- Sorts the elements of THIS using the specified predicate function.
// Input:    A predicate function pointer.
// Output:   None.

template <class Type> 
inline void CoolList<Type>::sort(register Boolean (*f)(const Type&, const Type&)) {
  CoolBase_List::sort((Predicate)f);
}


// merge() -- Merges the elements of list with the elements of the specified
//            list sorted with the specified predicate function
// Input:     A reference to a list to be merged and a predicate function
//            pointer.
// Output:    None.

template <class Type> 
inline void CoolList<Type>::merge(const CoolList<Type>& l, Boolean (*f)(const Type&, const Type&)) {
  CoolBase_List::merge(l, (Predicate)f);
}


// insert_before() -- Inserts the specified item before the current position
// Input:             A Type reference of new item.
// Output:            TRUE if current position is valid, FALSE otherwise.

template <class Type> 
inline Boolean CoolList<Type>::insert_before(const Type& new_item) {
#if ERROR_CHECKING
  if (this->curpos == NULL)
    this->before_error (#Type);
#endif
  return CoolBase_List::insert_before((const void*)&new_item);
}


// insert_after() -- Inserts the specified item after the current position
// Input:            A Type reference of new item.
// Output:           TRUE if current position is valid, FALSE otherwise.

template <class Type> 
inline Boolean CoolList<Type>::insert_after(const Type& new_item) {
#if ERROR_CHECKING
  if (this->curpos == NULL)
    this->after_error (#Type);
#endif
  return CoolBase_List::insert_after((const void*)&new_item);
}


// insert_before() -- Inserts the specified new item before the specified
//                    target item in this list
// Input:             Two data Type references.
// Output:            TRUE if target item found, FALSE otherwise.

template <class Type> 
inline Boolean CoolList<Type>::insert_before(const Type& new_item,
                                         const Type& target_item) {
  return CoolBase_List::insert_before((const void*)&new_item,(const void*)&target_item);
}

// Boolean insert_after(Type&, Type&) -- inserts the specified new item
//                                       after the specified target item
//                                       in THIS list.
//
// Input:   Two data Type references.
// Output:  TRUE if target item found, FALSE otherwise.

template <class Type> 
inline Boolean CoolList<Type>::insert_after(const Type& item, const Type& target) {
  return CoolBase_List::insert_after((const void*)&item, (const void*)&target);
}


// operator&=() -- Intersection of THIS list with the specified list.
// Input:          A reference to the list.
// Output:         A modified THIS.

template <class Type> 
inline CoolList<Type>& CoolList<Type>::operator&=(const CoolList<Type>& l) {
  this->set_intersection(l);
  return *this;
}


// operator|=() -- Union of THIS list with the specified list.
// Input:          A reference to the list.
// Output:         A reference to a new list containing a copy of the elements
//                 of THIS list and the elements of the specified list.

template <class Type> 
inline CoolList<Type>& CoolList<Type>::operator|=(const CoolList<Type>& l) {
  this->set_union(l);
  return *this;
}


// operator-=() -- Difference of THIS list with the specified list.
// Input:          A reference to the list.
// Output:         A modified THIS.

template <class Type> 
inline CoolList<Type>& CoolList<Type>::operator-=(const CoolList<Type>& l) {
  this->set_difference(l);
  return *this;
}


// operator^=() -- Exclusive-or of THIS list with the specified list.
// Input:          A reference to the list.
// Output:         A modified THIS.

template <class Type> 
inline CoolList<Type>& CoolList<Type>::operator^=(const CoolList<Type>& l) {
  this->set_xor(l);
  return *this;
}


// operator+=() -- Concatenates THIS list with the specified list.
// Input:          A reference to the list to be concatenated.
// Output:         A modified THIS.
//

template <class Type> 
inline CoolList<Type>& CoolList<Type>::operator+=(const CoolList<Type>& l) {
  this->append(l);
  return *this;
}
 
// operator&() -- Intersection of a copy of THIS list with the specified list
// Input:         A reference to the list.
// Output:        A new list ret. by value containing a copy of the elements
//                of THIS and the elements of the specified list.

template <class Type> 
inline CoolList<Type> CoolList<Type>::operator&(const CoolList<Type>& l) {
  CoolList<Type> new_copy;                      // temporary deleted on exit
  new_copy.copy(*this);                         // copy nodes of this 
  new_copy.set_intersection(l);                 // mutate with inters. of l
  return new_copy;                              // will copy only list header
}


// operator|() -- Union of a copy of THIS list with the specified list
// Input:         A reference to the list.
// Output:        A reference to a new list containing a copy of the elements
//                of THIS and the elements of the specified list.

template <class Type> 
inline CoolList<Type> CoolList<Type>::operator|(const CoolList<Type>& l) {
  CoolList<Type> new_copy;                      // temporary deleted on exit
  new_copy.copy(*this);                         // copy nodes of this
  new_copy.set_union(l);                        // mutate with union of l
  return new_copy;                              // will copy only list header
}


// operator-() -- Difference of a copy of THIS list with the specified list.
// Input:         A reference to the list.
// Output:        A reference to a new list containing a copy of the elements
//                of THIS and the elements of the specified list.

template <class Type> 
inline CoolList<Type> CoolList<Type>::operator-(const CoolList<Type>& l) {
  CoolList<Type> new_copy;                      // temporary deleted on exit
  new_copy.copy(*this);                         // copy nodes of this 
  new_copy.set_difference(l);                   // mutate with diff of l
  return new_copy;                              // will copy only list header
}


// operator^() -- Exclusive-or of a copy of THIS list with the specified list.
// Input:         A reference to the list.
// Output:        A reference to a new list containing a copy of the elements
//                of THIS and the elements of the specified list.

template <class Type> 
inline CoolList<Type> CoolList<Type>::operator^(const CoolList<Type>& l) {
  CoolList<Type> new_copy;                      // temporary deleted on exit
  new_copy.copy(*this);                         // copy nodes of this
  new_copy.set_xor(l);                          // mutate with xor of l
  return new_copy;                              // will copy only list header
}


// operator+() -- Concatenates a copy of THIS list with the specified list.
// Input:         A reference to the list to be concatenated.
// Output:        A reference to a new list containing a copy of the elements
//                of THIS and the elements of the specified list.

template <class Type> 
inline CoolList<Type> CoolList<Type>::operator+(const CoolList<Type>& l) {
  CoolList<Type> new_copy;                      // temporary deleted on exit
  new_copy.copy(*this);                         // copy nodes of this 
  new_copy.append(l);                           // mutate with concat. of l
  return new_copy;                              // will copy only list header
}

#endif                          // End #ifdef of LISTH
