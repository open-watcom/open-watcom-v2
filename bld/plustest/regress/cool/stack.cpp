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

#include <cool/Stack.h>

// compare_s -- Pointer operator== function
template<class Type>
Boolean (*CoolStack<Type>::compare_s)(const Type&, const Type&) = CoolStack_is_data_equal;

// CoolStack<Type> () -- Empty constructor for the CoolStack class
// Input:            None
// Output:           None

template<class Type>
CoolStack<Type>::CoolStack() {
  this->data = NULL;                            // Initialize data
}


// CoolStack<Type> (long) -- constructor that specifies number of elements
// Input:                Integer number of elements
// Output:               None

template<class Type>
CoolStack<Type>::CoolStack(unsigned long n)
 : CoolBase_Stack(n)
{
  this->data =  new Type[n];                    // Allocate memory
}


// CoolStack<Type> (void*, long) -- constructor that specifies static-sized storage
//                              and number of elements
// Input:                       Integer number of elements
// Output:                      None

template<class Type>
CoolStack<Type>::CoolStack(void* s, unsigned long n)
 : CoolBase_Stack(n)
{
  this->data = (Type*) s;                       // Pointer to storage block
  this->alloc_size_s = INVALID;                 // Indicate static-size object
}


// CoolStack<Type> (CoolStack<Type>&) -- constructor for reference to another stack
// Input:                        CoolStack reference
// Output:                       None

template<class Type>
CoolStack<Type>::CoolStack(const CoolStack<Type>& s)
 : CoolBase_Stack(s)
{
  this->data =  new Type[s.size];               // New memory allocation
  for (long i = 0; i < s.number_elements; i++)  // For each element
    this->data[i] = s.data[i];                  // Copy data into this
}


// ~CoolStack<Type> -- Destructor for CoolStack class that frees up storage
// Input:          None
// Output:         None

template<class Type>
CoolStack<Type>::~CoolStack() {
  if (this->size != 0 &&                        
      this->alloc_size_s != INVALID)            // If not user-provided storage
    delete [] this->data;                       // Free up the memory
}


// Boolean push(Type&) -- Push item on top of this stack
// Input:                 Reference to a Type value to push onto stack
// Output:                TRUE or FALSE

template<class Type>
Boolean CoolStack<Type>::push (const Type& value) {
  if (this->number_elements == this->size) {    // If not enough memory
    if (!this->grow(this->size+1)) return FALSE; // Report failure
  }
  this->data[number_elements++] = value;        // Top item becomes value
  return TRUE;                                  // Report success
}


// Boolean pushn(Type&, long) -- Push n items with a fill value onto this stack
// Input:  Reference to Type value, integer number of items to be pushed
// Output: TRUE or FALSE

template<class Type>
Boolean CoolStack<Type>::pushn (const Type& value, long n) {
  if (n < 0)                                    // Range-checking error
    return FALSE;                               // Return FALSE
  if (n == 0) {                                 // For push(foo, 0)
    this->data[number_elements-1] = value;      // Replace top item
    return TRUE;                                // Return success
  }
  if (n == 1)                                   // If just push one item
    return (push(value));                       // Push one item == push
  if (this->number_elements+n > this->size) {   // If not enough memory
    if (!this->grow(this->size+1)) return FALSE; // Report failure
  }
  this->number_elements += n;                   // Add n to number of elements
  for (long i = number_elements - n; i < number_elements; i++) // For "n" elts
    this->data[i] = value;                      // Fill with value
  return TRUE;                                  // Report success  
}


// Type& popn(long) -- Pop n items off this stack, from the top; return the
//                     nth item
// Input:              Integer number of items
// Output:             nth element of stack

template<class Type>
Type& CoolStack<Type>::popn (long n) {
#if ERROR_CHECKING
  if (n < 0)                                    // If index out of range
    this->popn_error (#Type, n);                // Raise exception
#endif
  if (n == 0)                                   // If top element
    return (this->data[number_elements-1]);     // Return top item
  if (n > this->number_elements)  {             // If arg n > # elements
//     this->number_elements = 0;               // Reset # of elements
    return this->data[0];                       // Return last element
  } 
  return (this->data[this->number_elements -= n]); // Return nth from "top"
}


// CoolStack<Type>& operator= (CoolStack<Type>&) -- Assigns this stack to another stack
// Input:  Reference to a stack
// Output: Reference to modified this

template<class Type>
CoolStack<Type>& CoolStack<Type>::operator= (const CoolStack<Type>& s) {
  if (this != &s) {
    if (this->size < s.size) {                  // If not enough memory
#if ERROR_CHECKING
      if (this->alloc_size_s == INVALID)        // If static size queue
        this->assign_error (#Type);             // Raise exception
#endif
      if (this->size != 0)
        delete [] this->data;                   // Free it up
      this->data = new Type[s.size];            // Allocate bigger memory
      this->size = s.size;                      // New stack size
    }
    this->CoolBase_Stack::operator= (s);                // Call base class assignment
    for (long i = 0; i < s.number_elements; i++) // For each element
      this->data[i] = s.data[i];                 // Copy value
  }
  return *this;                         // Return reference
}


// Boolean operator== (CoolStack<Type>&) -- Compare this stack with another
//                                      stack; return TRUE if they are equal
// Input:  Reference to a stack
// Output: TRUE or FALSE

template<class Type>
Boolean CoolStack<Type>::operator== (const CoolStack<Type>& s) const {
  if (this->number_elements != s.number_elements) // If not same number
    return FALSE;                                 // Then not equal
  for (long i = 0; i < this->number_elements; i++) // For each element
    if (!(*this->compare_s)(this->data[i],s.data[i])) // If not equal
      return FALSE;                                   // Return failure
  return TRUE;                                        // Return sucess
}

// Boolean is_data_equal -- Default data comparison function if user has not
//                          provided another one.
// Input:                   Two Type references
// Output:                  TRUE or FALSE

template<class Type>
Boolean CoolStack_is_data_equal (const Type& t1, const Type& t2) {
    return (t1 == t2);
}

// void set_compare(Type##_CoolStack_Compare) -- Set this stack's compare function
// Input:                                    Pointer to a compare function
// Output:                                   None

template<class Type>
void CoolStack<Type>::set_compare (register Boolean (*cf) (const Type&, const Type&)) {
  if (cf)
    this->compare_s = cf;                       // Set to user function
  else
    this->compare_s = &CoolStack_is_data_equal; // or set to default
}


// Boolean find (Type&) -- Return TRUE if value is found in this stack
// Input:                  Reference to a Type value
  // Output:                 TRUE or FALSE

template<class Type>
Boolean CoolStack<Type>::find (const Type& value) {
  return this->position(value) >= 0;
}


// long position (Type&) -- Return the index (from top) of value if found;
//                          otherwise return -1
// Input:                   Reference to a Type value
// Output:                  Integer index or -1

template<class Type>
long CoolStack<Type>::position (const Type& value) const {
  for (long i = this->number_elements - 1; i >= 0;  i--) // Search from "top"
    if ((*(this->compare_s))(this->data[i], value))      // If found
      return (this->number_elements-i-1);                // Index from "top"
  return -1;                                             // Failure
}

// grow -- grow stack on push overflow
// Input:    None
// Output:   None

template<class Type>
Boolean CoolStack<Type>::grow (long min_size) {
  if (this->alloc_size_s == INVALID) {          // If not allowed to grow
#if ERROR_CHECKING
    this->push_error (#Type);                   // Raise exception
#endif
  return FALSE;                                 // Report failure
  }
  if (this->growth_ratio != 0.0 &&
      (this->size * (1.0+growth_ratio)) >= min_size)
    min_size = (long)(this->size * (1.0 + growth_ratio)); // New size
  else
    min_size += alloc_size_s;
  resize(min_size);
  return TRUE;
}

// resize -- Adjust the memory size of a CoolStack to accomodate some size
// Input:    Number of elements to hold
// Output:   None

template<class Type>
void CoolStack<Type>::resize (long new_size) {
  if (new_size <= this->size) {         // Don't bother shrinking stack
#if ERROR_CHECKING
    if (new_size < 0)                           // If index out of range
      this->resize_error (#Type, new_size);     // Raise exception
#endif
    return;
  }
  Type* temp =  new Type[new_size];             // Allocate storage
  for (long i = 0; i < this->number_elements; i++) // For each element in Vector
    temp[i] = this->data[i];                       // Copy into new storage
  if (this->size != 0)
    delete [] this->data;                       // Free up old memory
  this->data = temp;                    // Assign new memory block
  this->size = new_size;                // Save new size value
}

// operator<< -- Overload the output operator for CoolStack
      // Input:        ostream reference, stack reference
// Output:       Stack data is output to ostream

template<class Type>
ostream& operator<< (ostream& os, const CoolStack<Type>& s) {
  if (s.length() == 0)                          // If no elements
    os << " Empty ";                            // Report empty status
  else {
    os << "[ ";                                 // From top of stack
    for (long i = s.length() - 1; i >= 0; i--)  // For each element
      os << s.data[i] << " ";                   // Print data items
    os << "]";                                  // Bottom of stack
  }
  return os;                                    // Return stream reference
}
