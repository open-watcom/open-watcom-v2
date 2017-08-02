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
// Created: MJF 05/22/89 -- Initial design.
// Updated: JCB 06/05/89 -- Implementation.
// Updated: LGO 08/09/89 -- Inherit from Generic
// Updated: MBN 08/20/89 -- Changed usage of template to reflect new syntax
// Updated: MBN 08/23/89 -- Added conditional exception handling and base class
// Updated: LGO 10/05/89 -- Efficiency improvements to popn
// Updated: MBN 10/19/89 -- Added optional argument to set_compare method
// Updated: MBN 11/01/89 -- Added constructor with user-provided storage param
// Updated: LGO 12/07/89 -- re-write push and pushn, added grow method
// Updated: LGO 12/07/89 -- Make compare_s default to NULL
// Updated: MBN 02/22/90 -- Changed size arguments from long to unsigned long
// Updated: MJF 05/31/90 -- Use memcpy in resize
// Updated: MJF 05/31/90 -- Use "delete [] data"
// Updated: MJF 06/30/90 -- Added base class name to constructor initializer
// Updated: VDN 02/21/92 -- New lite version
// Updated: JAM 08/19/92 -- removed DOS specifics, stdized #includes
// Updated: JAM 08/19/92 -- modernized template syntax, remove macro hacks
//                          non-template classes Cool_Stack=>CoolBase_Stack
//
// The Stack<Type>  class is publicly  derived  from  the base Stack  class and
// implements a one  dimensional  vector of  a  user-specified  type.   This is
// accomplished by using the parameterized type  capability of C++.   The stack
// will grow dynamically as necessary with the  amount  of growth determined by
// the  value of an  allocation  size slot.   Fixed   length stacks   are  also
// supported by setting the value of the allocation size slot to zero.
//
// Each Stack<Type> object contains a private  data section that  has a slot to
// hold the current size of the stack and a pointer to an allocated block large
// enough to hold "size elements" of type "Type."   A slot to  thold the number
// of elements currently on  the stack is also available.   The protected  data
// section has   a pointer  to a  compare function  that  is used   in equality
// operations. The default equality function used is the == operator.
//
// There  are four   constructors   for the   Stack<Type> class.   The    first
// constructor takes  no arguments and creates   an  empty  Stack object of the
// specified type.  The second constructor takes a required argument specifying
// the initial  size  of the stack.  The  third takes a pointer   to a block of
// user-defined storage    and  the number of  elements  the   stack  can hold.
// Finally,  the   third constructor  takes a  single argument  consisting of a
// reference to a Stack<Type> and duplicates its size and element values.
//
// Methods are provided to push and  pop items to  and from the  stack, examine
// the   item on the   top of the stack  without  removing  it, determine if an
// element is already  on the stack, report the  number of items in the  stack,
// check the empty status, and clear all items from the stack.  The assignment,
// output, and  equality operators are  overloaded. Finally, two methods to set
// the allocation growth size and compare function are provided.
//

#ifndef STACKH                                  // If no definition for Stack
#define STACKH                                  // Define stack symbol

#ifndef BASE_STACKH                             // If no definition for class
#include <cool/Base_Stack.h>                    // Include definition file
#endif

#ifndef NEWH
#include <new.h>                                // include the new header file
#define NEWH
#endif

template <class Type>
class CoolStack : public CoolBase_Stack {
public:
  typedef Boolean (*Compare) (const Type&, const Type&);

  CoolStack();                          // Simple constructor
  CoolStack(unsigned long);             // CoolStack of initial size
  CoolStack(void*, unsigned long);      // CoolStack with static storage
  CoolStack(const CoolStack<Type>&);    // Copy constructor
  ~CoolStack();                         // Destructor

  Boolean push (const Type&);                   // Push item on top of stack
  Boolean pushn (const Type&, long);            // Push n items w/initial value

  inline Type& pop ();                          // Pop top item off stack
  Type& popn (long);                            // Remove n items, return nth
  inline Type& top ();                          // Return top item on stack

  inline Type& operator[] (unsigned long);      // Zero-relative (top) index 
  long position (const Type&) const;            // Returns stack index of value
  Boolean find (const Type&);                   // Returns TRUE if found

  CoolStack<Type>& operator= (const CoolStack<Type>& s); // Assignment s = s2;
  Boolean operator== (const CoolStack<Type>& s) const; // is equal
  inline Boolean operator!= (const CoolStack<Type>& s) const ; // is not equal

  void resize (long);                           // Resize for at least count
  inline long set_length (long);                // Set number of elements
  inline void set_growth_ratio (float);         // Set growth percentage
  void set_compare(Compare = NULL); // Set compare function
  inline void set_alloc_size (int);              // Set alloc size

  template< class U >
  friend ostream& operator<< (ostream&, const CoolStack<U>&);
  template< class U >
  inline friend ostream& operator<< (ostream&, const CoolStack<U>*);

protected:
  Type* data;                                   // Pointer to allocated storage
  static Compare compare_s;     // Pointer operator== function
  Boolean grow (long min_size);                 // Grow on push

private:
  template< class U >
  friend Boolean CoolStack_is_data_equal (const U&, const U&);
};

// Type& top() -- Return the top item on this stack
// Input:         None
// Output:        Reference to top item on stack

template<class Type>
inline Type& CoolStack<Type>::top () {
#if ERROR_CHECKING
  if (this->number_elements > this->size)       // If index out of range
    this->top_error (#Type);                    // Raise exception
#endif
  return this->data[this->number_elements-1];   // The top is really at the end
}


// Type& pop () -- Pop top item off this stack and return it
// Input:          None
// Output:         Top item of stack

template<class Type>
inline Type& CoolStack<Type>::pop () {
#if ERROR_CHECKING
  if (this->number_elements > this->size)       // If index out of range
    this->pop_error (#Type);                    // Raise expception
#endif
  return (this->data[--this->number_elements]); // Remove/return top element
}


// Type& operator[](long) -- Return the nth element (zero-relative from top)
// Input:                    Integer n
// Output:                   Reference to the nth element of stack

template<class Type>
inline Type& CoolStack<Type>::operator[] (unsigned long n) {
#if ERROR_CHECKING
  if (unsigned(n) >= this->number_elements)     // If index out of range
    this->bracket_error (#Type, n);             // Raise exception
#endif
  return this->data[number_elements-n-1];       // Nth element from "top"
}


// Boolean operator!= (CoolStack<Type>&) -- Compare this stack with another stack;
//                                      return TRUE if they are not equal
// Input:  Reference to a stack
// Output: TRUE or FALSE

template<class Type>
inline Boolean CoolStack<Type>::operator!= (const CoolStack<Type>& s) const {
  return (!this->operator== (s));
}


// long set_length(long) -- Change number of elements in this stack
// Input:                   Integer number of elements
// Output:                  None

template<class Type>
inline long CoolStack<Type>::set_length (long n) {
  this->CoolBase_Stack::set_length (n, "Type");         // Pass size/type to base class
  return this->number_elements;                 // Return value
}


// void set_growth_ratio(float) -- Set growth percentage of this stack
// Input:                          Float ratio
// Output:                         None

template<class Type>
inline void CoolStack<Type>::set_growth_ratio (float ratio) {
  this->CoolBase_Stack::set_growth_ratio (ratio, "Type");       // Pass ratio/type to base
} 


// void set_alloc_size(int) -- Set the default allocation size
// Input:                      Integer size
// Output:                     None

template<class Type>
inline void CoolStack<Type>::set_alloc_size (int size) {
  this->CoolBase_Stack::set_alloc_size (size, "Type");
}

// operator<< -- Overload the output operator for CoolStack
// Input:        ostream reference, stack pointer
// Output:       CoolStack data is output to ostream

template<class Type>
inline ostream& operator<< (ostream& os, const CoolStack<Type>* s) {
  return operator<<(os, *s);
}

#endif                          // End #ifdef of STACKH

