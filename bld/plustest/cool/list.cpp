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

#include <cool/List.h>

// compare_s -- Function used by == test
template <class Type>
Boolean (*CoolList<Type>::compare_s)(const Type&, const Type&) = &CoolList_is_data_equal;

// CoolList_Node<Type>() -- A CoolList Node constructor to create a node from head and
//                      tail specified
// Input:               A Type reference and CoolList reference.
// Output:              None.

template <class Type> 
CoolList_Node<Type>::CoolList_Node(const Type& head, CoolList_Node<Type>* tail) {
  this->ref_count = 1;
  this->next = tail;
  this->data = (Type&) head;
}

template <class Type> 
CoolList_Node<Type>::~CoolList_Node() { // data is deleled when object
}                                               // is stored.

// get_data()  -- Gets data of node.
// Input:         None.
// Output:        A void* pointer.

template <class Type> 
const void* CoolList_Node<Type>::get_data() {
  return &this->data;
}


// set_data()  -- Sets data of node to specified value.
// Input:         A void* pointer.
// Output:        None.

template <class Type> 
void CoolList_Node<Type>::set_data(const void* value) {
  this->data = *(Type*)value;
}



// CoolList<Type>() -- a CoolList constructor to create a CoolList instance with no
//                 elements.  The node pointer is initialized to NULL.
// Input:          None.
// Output:         None.

template <class Type> 
CoolList<Type>::CoolList() {
  this->reset();
  this->node_ptr = NULL;
}


// CoolList<Type>() -- A CoolList constructor to create a CoolList with one element
// Input:          A Type reference.
// Output:         None.

template <class Type> 
CoolList<Type>::CoolList(const Type& head) {
  this->reset();
  this->node_ptr = new CoolList_Node<Type>(head, (CoolList_Node<Type>*)NULL);
}


// CoolList<Type>() -- A CoolList constructor to create a CoolList with the specified
//                 head element and list tail.
// Input:          A Type reference and CoolList reference.
// Output:         None.

template <class Type> 
CoolList<Type>::CoolList(const Type& head, CoolList<Type>& tail) {
  this->reset();
  this->node_ptr = new CoolList_Node<Type>(head, (CoolList_Node<Type>*)tail.node_ptr);
  this->reference(tail.node_ptr);
}


// CoolList<Type>() -- A CoolList constructor to create a CoolList from the specified list.
// Input:          A CoolList reference.
// Output:         None.

template <class Type> 
CoolList<Type>::CoolList(CoolList<Type>& tail) {
  this->reset();
  this->node_ptr = (CoolList_Node<Type>*)tail.node_ptr;
  this->reference(this->node_ptr);
}

// CoolList<Type>() -- A CoolList constructor to initialize a CoolList with "n" elements.
// Input:          The number of elements, and "n" data Type elements
// Output:         None.
// Note: Arguments in ... can only be pointers, primitive types like int, double,
//       and NOT OBJECTS, passed by reference or value, like vectors, matrices;
//       because constructors must be known and called at compile time!!!
//       Cannot have char in ..., because char is 1 instead of 4 bytes, and 
//       va_arg expects sizeof(Type) a multiple of 4 bytes.

template <class Type> 
CoolList<Type>::CoolList(int n, Type head, ...) {
#if ERROR_CHECKING
  if (((sizeof(Type) % 4) != 0) ||              // Cause alignment problems
      (sizeof(Type) > 8))                       // User defined classes?
    this->va_arg_error(#Type, sizeof(Type));    // So, cannot use this constructor
#endif
  this->reset();
  if (n > 0) {
    va_list argp;
    va_start(argp, head);
    // add head node
    this->node_ptr = new CoolList_Node<Type>(head, (CoolList_Node<Type>*)NULL);
    // add remaining nodes
    CoolBase_List_Node* prev_np = this->node_ptr;
    CoolBase_List_Node* next_np;
    for (int i = 1; i < n; i++) {
      Type temp = va_arg(argp, Type);
      next_np = new CoolList_Node<Type>(temp, (CoolList_Node<Type>*)NULL);
      prev_np->next_node() = next_np;
      prev_np = next_np;
    }
    va_end(argp);
  }
  else  this->node_ptr = (CoolList_Node<Type>*)0;
}

// ~CoolList<Type>() -- the CoolList destructor will decrement reference counts
//                  starting at head node and free up memory allocated by
//                  these nodes if they are no longer referenced.
// Input:           None.
// Output:          None.

template <class Type> 
CoolList<Type>::~CoolList() {
  this->dereference(this->node_ptr);
}

// operator[]() -- Returns the nth node of THIS list.
// Input:          A positive integer index.
// Output:         A Type reference of data in the nth node of THIS.

template <class Type> 
Type& CoolList<Type>::operator[] (int n) {
  CoolList_Node<Type>* np = (CoolList_Node<Type>*)CoolBase_List::operator[](n);
#if ERROR_CHECKING
  if (np == NULL)
    this->bracket_error (#Type, n);
#endif
  return np->data;
}

// put() -- Replaces the data slot of the nth node of this list and if
//          successful, returns the  new data item. With no arguments,
//          replaces the data at the first node.
// Input:   A Type reference and a positive integer index.
// Output:  TRUE if nth node exists, FALSE otherwise.

template <class Type> 
Boolean CoolList<Type>::put(const Type& x, int n) {
  CoolList_Node<Type>* np = (CoolList_Node<Type>*)CoolBase_List::operator[](n);
  if (np != NULL) {
    np->data = x;
    return TRUE;
  }
  else return FALSE;
}

template <class Type> 
Boolean CoolList<Type>::do_find(CoolBase_List_Node* np, const void* x, CoolBase_List_Node*& cp, CoolBase_List_Node*& pp) const {
  CoolBase_List_Node* prev_np = NULL;
  for (; np != NULL; 
       prev_np = np, np = np->next_node())
    if ((*compare_s)(((const CoolList_Node<Type>*) np)->data, *(const Type*)x)) {
      cp = np;                                  // found x
      pp = prev_np;
      return TRUE;
    }
  cp = NULL;
  pp = NULL;
  return FALSE;
}

// push() -- Prepends the specified data item to the front of this list
// Input:    A Type reference to the data item to be prepended.
// Output:   TRUE.

template <class Type> 
Boolean CoolList<Type>::push(const Type& x) {
  CoolBase_List_Node* anode = new CoolList_Node<Type>(x, (CoolList_Node<Type>*) this->node_ptr);
  if (!anode)
    return FALSE;
  this->curpos = anode;
  this->node_ptr = anode;
  this->prevpos = NULL;
  return TRUE;
}


// pop() -- Removes and returns head element of THIS list.
// Input:   None.
// Output:  A copy of the head data element of THIS list.

template <class Type> 
Type CoolList<Type>::pop() {
  CoolList_Node<Type>* head = (CoolList_Node<Type>*)CoolBase_List::pop();
#if ERROR_CHECKING
  if (head == NULL)
    this->pop_error (#Type);
#endif
  Type head_data = head->data;
  this->dereference(head);
  return head_data;
}


// pop(Type& result) -- Removes head node and returns data
// Input:   Reference to place to copy the result.
// Output:  returns TRUE when the list is empty

template <class Type> 
Boolean CoolList<Type>::pop(Type& result) {
  CoolList_Node<Type>* head = (CoolList_Node<Type>*)CoolBase_List::pop();
#if ERROR_CHECKING
  if (head == NULL) 
    this->pop_error (#Type);
#endif
  result = head->data;
  this->dereference(head);
  return TRUE;
}


// remove() -- Removes item at current position.
// Input:      None.
// Output:     The item removed.

template <class Type> 
Type CoolList<Type>::remove() {
  CoolList_Node<Type>* np = (CoolList_Node<Type>*)CoolBase_List::remove();
#if ERROR_CHECKING
  if (np == NULL)
    this->remove_error (#Type);
#endif
  Type removed_data = np->data;                 // temp deleted on exit
  this->dereference(np);                        // remove node
  return removed_data;                          // copy of temp returned
}

// CoolList<Type>() -- A CoolList constructor used internally by CoolList member functions
//                 to initialize a CoolList object with an object with a pointer
//                 to a node structure
// Input:          The node pointer.
// Output:         None.

template <class Type> 
CoolList<Type>::CoolList(CoolList_Node<Type>* head_node) {
  this->node_ptr = head_node;
  this->reference(head_node);
}

// new_list() -- Returns a new list with head node initialized to node.
// Input:        The node pointer.
// Output:       A pointer to the new list.

template <class Type> 
CoolBase_List* CoolList<Type>::new_list(CoolBase_List_Node* head_node) {
  CoolList<Type>* l = new CoolList<Type>((CoolList_Node<Type>*)head_node);
  return (CoolList*) l;
}


// insert_before_node() -- Insert a new node before the specified node.
// Input:                  A Type reference and a Node pointer.
// Output:                 A pointer to the new node.

template <class Type> 
CoolBase_List_Node* CoolList<Type>::insert_before_node(const void* value, CoolBase_List_Node* next_np) {
  CoolBase_List_Node* anode = new CoolList_Node<Type>(*(Type*)value, 
                                              (CoolList_Node<Type>*)next_np);
  return anode;
}


// insert_after_node() -- Inserts a new node after the specified node.
// Input:                 A Type reference and a Node pointer.
// Output:                A pointer to the new node.

template <class Type> 
CoolBase_List_Node* CoolList<Type>::insert_after_node(const void* value, CoolBase_List_Node* prev_np) const {
  CoolBase_List_Node* anode = new CoolList_Node<Type>(*(Type*)value, (CoolList_Node<Type>*)NULL);
  // note that if prev_np is NULL,
  // then we just return a new node with no reference to it.
  if (prev_np != NULL) {
     anode->next_node() = prev_np->next_node();
     prev_np->next_node() = anode;
   }
  return anode;
}

// is_data_equal() -- A private friend function used internnaly returning TRUE
//                    if a == b.  The default value of  the CoolList Compare slot
//                    is set to the address of this function. This must be
//                    done to get the address of operator==.
// Input:             The two void* pointers to be compared.
// Output:            TRUE if items are equal, FALSE otherwise. 

template<class Type>
Boolean CoolList_is_data_equal (const Type& a, const Type& b) {
    return (a == b);
}

// set_compare() -- Sets the Compare function.
// Input:           A compare function pointer.
// Output:          None.

template <class Type> 
void CoolList<Type>::set_compare(register Boolean (*cf)(const Type&, const Type&)) {
  if (cf == NULL)                               // If no value supplied
    this->compare_s = &CoolList_is_data_equal; // Use default compare
  else
    this->compare_s = cf;                       // Else set to user function
}


// compare_data() -- Compares data using default compare function of this list
// Input:            Two void* pointers which will be type cast.
// Output:           None.

template <class Type> 
Boolean CoolList<Type>::compare_data(const void* a, const void* b) const {
  return (*compare_s)(*(const Type*)a, *(const Type*)b);
}


// output_data()  -- Outputs node data from specified stream and is called
//                   by operator<<.
// Input:            An output stream reference and node pointer.
// Output:           A void* pointer of data.

template <class Type> 
void CoolList<Type>::output_data(ostream& os, const CoolBase_List_Node* np) const {
  os << ((const CoolList_Node<Type>*)np)->data;
}

