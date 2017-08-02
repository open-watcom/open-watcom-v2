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
// Updated: JAM 09/23/92 -- added def and init for compare_s

#include <cool/Queue.h>

// compare_s -- Pointer operator== function
template<class Type> 
Boolean (*CoolQueue<Type>::compare_s)(const Type&, const Type&) = CoolQueue_is_data_equal;

// CoolQueue<Type> () -- Empty constructor for the CoolQueue class
// Input:            None
// Output:           None

template<class Type> 
CoolQueue<Type>::CoolQueue() {
  this->data = NULL;                            // Intialize data
}


// CoolQueue<Type> (unsigned int) -- constructor that specifies number of elements
// Input:                Integer number of elements
// Output:               None

template<class Type> 
CoolQueue<Type>::CoolQueue(unsigned int n)
 : CoolBase_Queue(n)
{
  this->data = (Type*) new Type[n];             // Allocate memory
}


// CoolQueue<Type> (void*, int) -- constructor that specifies user-defined storage
//                              and number of elements
// Input:                       Integer number of elements
// Output:                      None

template<class Type> 
CoolQueue<Type>::CoolQueue(void* s, unsigned int n)
 : CoolBase_Queue(n)
{
  this->data = (Type*) s;                       // Pointer to storage
  this->alloc_size = INVALID;                   // Indicate this is static size
}


// CoolQueue<Type> (CoolQueue<Type>&) -- Copy constructor
// Input:                        CoolQueue reference
// Output:                       None

template<class Type> 
CoolQueue<Type>::CoolQueue(const CoolQueue<Type>& s)
 : CoolBase_Queue(s)
{
  this->data = (Type*) new Type[s.limit];       // New memory allocation
  for (int i = 0; i < s.limit; i++)             // For each element
    this->data[i] = s.data[i];                  // Copy data into this
}


// ~CoolQueue<Type> -- Destructor for CoolQueue class that frees up storage
// Input:          None
// Output:         None

template<class Type> 
CoolQueue<Type>::~CoolQueue() {
  if (this->limit != 0 &&
      this->alloc_size != INVALID)              // If not static-size object
    delete [] this->data;                       // Free up the memory
}



// Type& get () -- Get and remove first-in item in this CoolQueue
// Input:          None
// Output:         Reference to first-in Type value

template<class Type> 
Type& CoolQueue<Type>::get () {
#if ERROR_CHECKING
  if (in == out) {                              // If no elements in CoolQueue
    printf ("CoolQueue<%s>::get(): No elements in queue.\n", #Type);
    abort ();
  }
#endif
  int result = out;                             // Get data
  if (++out >= limit) out = 0;                  // increment and wrap
  return data[result];
}

// Boolean get (Type& result) -- Get and remove first-in item in this CoolQueue
// Input:          None
// Output:         Reference to first-in Type value

template<class Type>
Boolean CoolQueue<Type>::get (Type& result) {
  if (in == out) return FALSE;
  result = this->data[out];             // Get data
  if (++out >= limit) out = 0;          // Increment and wrap
  return TRUE;
}

// Boolean unget (Type&) -- Return TRUE if able to put a Type value on the
//                          front-end of this CoolQueue
// Input:                   Reference to a Type value
// Output:                  TRUE or FALSE

template<class Type> 
Boolean CoolQueue<Type>::unget (const Type& value) {
  int save = out;
  if (--out < 0) out = limit - 1;       // decrement and wrap
  if (in == out) {                      // Check for full
    out = save;
    if (!this->grow()) return FALSE;
    if (--out < 0) out = limit - 1;     // decrement and wrap
  }
  data[out] = value;                    // Stuff data
  return TRUE;
}

// Boolean put (Type&) -- Put a new last-in item on this CoolQueue; return TRUE
///                       if successful
// Input:                 Reference to a Type value
// Output:                TRUE or FALSE

template<class Type> 
Boolean CoolQueue<Type>::put (const Type& value) {
  int save = in;
  if (++in >= limit) in = 0;            // Increment and Wrap
  if (in == out) {                      // Check for full
    in = save;
    if (!this->grow()) return FALSE;
    save = in;
    if (++in >= limit) in = 0;          // Increment and Wrap
  }
  data[save] = value;                   // Store
  curpos = in;                          // Invalidate curpos
  return TRUE;
}

// Type& unput () -- Remove and return last-in item of this CoolQueue
// Input:            None
// Output:           Reference to the Type value of the last-in item

template<class Type> 
Type& CoolQueue<Type>::unput () {
#if ERROR_CHECKING
  if (in == out) {                              // If no elements in queue
    printf ("CoolQueue<%s>::unput(): No elements in queue.\n", #Type);
    abort ();
  }
#endif
  if (--in < 0) in = limit - 1;                 // decrement and wrap
  curpos = in;                                  // Invalidate curpos
  return data[in];
}

template<class Type>
Boolean CoolQueue<Type>::unput (Type& result) {
  if (in == out) return FALSE;                  // If no elements in queue
  if (--in < 0) in = limit - 1;                 // decrement and wrap
  curpos = in;                                  // Invalidate curpos
  result = this->data[in];                      // Get data
  return TRUE;
}

// Boolean find (Type&) -- Return TRUE if value is found in this CoolQueue
// Input:                  Reference to a Type value
// Output:                 TRUE or FALSE

template<class Type> 
Boolean CoolQueue<Type>::find (const Type& value) {
  int i;
  if (in == out)                                // Nothing is in CoolQueue
    return FALSE;                               // Return failure
  if (in > out) {
    for (i = out; i < in; i++)                  // check from out to in-1
      if ((*(this->compare_s))(this->data[i], value)) { // If found
        this->curpos = i;                       // Set curpos to index 
        return TRUE;                            // Return success
      }
  }
  else {
    for (i = out; i < limit; i++)               // check from out to limit-1
      if ((*(this->compare_s))(this->data[i], value)) { // If found
        this->curpos = i;                       // Set curpos to index 
        return TRUE;                            // Return success
      }
    for (i = 0; i < in; i++)                    // check from first to in-1
      if ((*(this->compare_s))(this->data[i], value)) { // If found
        this->curpos = i;                       // Set curpos to index 
        return TRUE;                            // Return success
      }
  }
  return FALSE;                                 // Return failure
}

template<class Type> 
Boolean CoolQueue<Type>::grow () {
  int new_size;
  if (this->alloc_size == INVALID) return FALSE;
  if (growth_ratio_s > 0.0)
     new_size = (int)(limit * (1.0 + growth_ratio_s)); // New size?
  else
    new_size = limit + alloc_size;
  resize(new_size);
  return TRUE;
}
  

// void resize (int)-- Adjust the memory size of a CoolQueue to accomodate some
//                      new size
// Input:               Number of elements to hold
// Output:              None

template<class Type> 
void CoolQueue<Type>::resize (int s) {
#if ERROR_CHECKING
  if (this->alloc_size == INVALID) {            // If not allowed to grow
    this->resize_error (#Type);                 // Raise exception
    return;                                     // Return
  }
#endif
  Type* temp;                                   // Temporary variable
  Type* tp;
  int len = this->length();
  int i;
  tp = temp = (Type*) new Type[s];              // Allocate storage
  if (in > out) {
    for (i = out; i < in; i++)                  // copy from out to in-1
      *tp++ = data[i];                          // Copy value
  }
  if (in < out) {
    for (i = out; i < limit; i++)               // copy from out to limit-1
      *tp++ = data[i];                          // Copy value
    for (i = 0; i < in; i++)                    // copy from first to in-1
      *tp++ = data[i];                          // Copy value
  }
  if (this->limit != 0)
    delete [] this->data;                       // Free up old memory
  this->data = temp;                            // Assign new memory block
  this->limit = s;                              // Save new size value
  curpos = len;
  in = len;
  out = 0;
}


// Boolean remove () -- Destructively remove item at current position; return
//                      TRUE if successful
// Input:               None
// Output:              TRUE or FALSE
template<class Type> 
Boolean CoolQueue<Type>::remove () {
  int i;
  if (in == out) return FALSE;                  // Fail if nothing in queue
  if (in > out) {                               // Data is between out and in-1
    if (curpos < out || curpos >= in)           // Fail if invalid curpos
      return FALSE;
    in--;
    for (i = curpos; i < in; i++)
      data[i] = data[i+1];
  }                                            
  else {
    if (curpos >= out) {                       // Data is between out and limit
      if (curpos >= limit)                     // fail if invalid curpos
        return FALSE;
      out--;
      for (i = curpos; i >= out; i--)
        data[i] = data[i - 1];
    }
    else {                                     // data is between 0 and in-1
      if (curpos >= in)                        // fail if invalid curpos
        return FALSE;
      in--;
      for (i = curpos; i < in; i++)
        data[i] = data[i+1];
    }
  }
  return TRUE;                                  // Report success
}


// Boolean remove (Type&) -- Destructively remove the first occurence of an 
//                           element, starting from front of this CoolQueue; return
//                           TRUE if element is found and removed
// Input:                    Reference to Type value
// Output:                   TRUE or FALSE

template<class Type> 
Boolean CoolQueue<Type>::remove (const Type& value) {
 return find(value) && remove();        // find it, Remove it & return status
}


// CoolQueue<Type>& operator= (CoolQueue<Type>&) -- Assignment
// Input:  Reference to a CoolQueue
// Output: Reference to modified this

template<class Type> 
CoolQueue<Type>& CoolQueue<Type>::operator= (const CoolQueue<Type>& s) {
  if (this != &s) {
    int len = s.length();
    int i;
    if (this->limit < len) {                    // if not enough memory
#if ERROR_CHECKING
      if (this->alloc_size == INVALID)          // If static size queue
        this->assign_error (#Type);             // Raise exception
#endif
      if (this->limit != 0)
        delete [] this->data;                   // Free it up
      this->limit = s.limit;                    // New CoolQueue size
      this->data = (Type*) new Type[s.limit];   // Allocate bigger memory
    }
    out = 0;
    in = len;
    curpos = len;
    register Type* dp = this->data;
    if (s.in >= s.out)
      for (i = s.out; i < s.in; i++)            // copy from s.out to s.in-1
        *dp++ = s.data[i];                      // Copy value
    else {
      for (i = s.out; i < s.limit; i++)         // copy from s.out to s.limit-1
        *dp++ = s.data[i];                      // Copy value
      for (i = 0; i < s.in; i++)                // copy from first to s.in-1
        *dp++ = s.data[i];                      // Copy value
    }}
  return *this;                                 // Return reference
}


// Boolean operator== (CoolQueue<Type>&) -- Compare this CoolQueue with another
//                                      CoolQueue;
// Input:  Reference to a CoolQueue
// Output: TRUE or FALSE if equal/non equal.

template<class Type> 
Boolean CoolQueue<Type>::operator== (const CoolQueue<Type>& q) const {
  if (this->length() != q.length())             // If not same number
    return FALSE;                               // Then not equal
  int tp = this->out;
  int qp = q.out;
      while (tp != this->in) {
    if (!(*this->compare_s)(this->data[tp], q.data[qp]))
      return FALSE;                             // Return failure if no match
    if (++tp >= this->limit) tp = 0;            // increment and wrap
    if (++qp >= q.limit) tp = 0;                // increment and wrap
  }
  return TRUE;
}


// Boolean is_data_equal -- Default data comparison function if user has not
//                          provided another one.
// Input:                   Two Type references
// Output:                  TRUE or FALSE

template<class Type>
Boolean CoolQueue_is_data_equal (const Type& t1, const Type& t2) {
   return (t1 == t2);
}


// void set_compare(Compare) -- Set this CoolQueue's compare function
// Input:  Pointer to a compare function
// Output: None

template<class Type> 
void CoolQueue<Type>::set_compare (register Boolean (*cf) (const Type&, const Type&)) {
  if (cf == NULL)
    this->compare_s = &CoolQueue_is_data_equal; // Default equality function
  else
    this->compare_s = cf;                       // Else set to user function
}


// operator<< -- Overload the output operator for CoolQueue
// Input:        ostream reference, queue reference
// Output:       CoolQueue data is output to ostream

template<class Type>
ostream& operator<< (ostream& os, const CoolQueue<Type>& q) {
  return q.qprint(os);
}


// This is a method because Glockenspiel's Cfront 1.2 doesn't let 
// friend functions access the protected data members of our base class
template<class Type>
ostream& CoolQueue<Type>::qprint(ostream& os) const {
  if (in == out)                                // If no elements
    os << " Empty ";                            // Report empty status
  else {
    int i;
    os << "[ ";
    if (in >= out)
      for (i = out; i < in; i++)                // copy from out to in-1
        os << data[i] << " ";
    else {
      for (i = out; i < limit; i++)             // copy from out to limit-1
        os << data[i] << " ";
      for (i = 0; i < in; i++)                  // copy from first to in-1
        os << data[i] << " ";
    }
    os << "]";
  }
  return os;                                    // Return stream reference
}
