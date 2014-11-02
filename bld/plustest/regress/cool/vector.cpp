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
// Updated: JAM 08/12/92 -- modernize template syntax, remove macro hacks
// Updated: JAM 08/12/92 -- split up comma expr in some funcs because of
//                          a a BC++ bug when Type was a float or double

#ifndef VECTORC
#define VECTORC

#include <cool/Vector.h>

// compare_s -- static data member of CoolVector, pointer to Type operator== 
template<class Type> 
Boolean (*CoolVector<Type>::compare_s)(const Type&, const Type&) = &CoolVector_is_data_equal;

// CoolVector<Type> () -- Empty constructor for the CoolVector class.
// Input:             None
// Output:            None

template<class Type> 
CoolVector<Type>::CoolVector () {
  this->data = NULL;                            // NULL data pointer
}


// CoolVector<Type> (size_t) -- constructor that specifies number of elements.
// Input:                 Number of elements 
// Output:                None

template<class Type> 
CoolVector<Type>::CoolVector (size_t n)
 : CoolBaseVector(n)
{
  this->data  = new Type[n];            // Allocate memory
}


// CoolVector<Type> (void*, size_t) -- constructor that specifies user-defined static
//                               size storage and number of elements
// Input:                        Pointer to storage, number of elements 
// Output:                       None

template<class Type> 
CoolVector<Type>::CoolVector (void* s, size_t n)
 : CoolBaseVector(n)
{
  this->data  = (Type*) s;                      // Pointer to storage block
  this->alloc_size = INVALID_ALLOCSZ();                 // Indicate static-size object
}


// CoolVector<Type> (size_t, Type&) -- constructor that specifies number of elements
//                               and also an initialization value to set all
//                               elements.
// Input:   Number of elements and Type reference to initialization value
// Output:  None

template<class Type> 
CoolVector<Type>::CoolVector (size_t n, const Type& value)
 : CoolBaseVector(n)
{
  this->data = new Type[n];                     // Allocate memory
  for (size_t i = 0; i < n; i++)                        // For each element in Vector
    this->data[i] = value;                      // Set initial value
  this->number_elements = n;                    // Set element count
}


// CoolVector<Type> (size_t,size_t, ...) -- constructor that specifies number of elements
//                                 and also a variable number of references to
//                                 initialization values for successive elements
//                                 in the vector. 
// Input:   Number of elements, number of initial values, and initial values
// Output:  None
// Note: Arguments in ... can only be pointers, primitive types like int,
//       and NOT OBJECTS, passed by reference or value, like vectors, matrices;
//       because constructors must be known and called at compile time!!!
//       Cannot have char in ..., because char is 1 instead of 4 bytes, and 
//       va_arg expects sizeof(Type) a multiple of 4 bytes.

template<class Type> 
CoolVector<Type>::CoolVector (size_t n, size_t nv, Type v0, ...)
 : CoolBaseVector(n)
{
#if ERROR_CHECKING
  if (((sizeof(Type) % 4) != 0) ||              // Cause alignment problems
      (sizeof(Type) > 8))                       // User defined classes?
    this->va_arg_error(#Type, sizeof(Type));    // So, cannot use this constructor
#endif
  typedef Type xType; // need Type expanded all the way before call to macro va_arg

  this->data = new Type[n];                     // Allocate memory
  this->number_elements = nv;                   // Update element count
  this->data[0] = v0;
  va_list argp;                                 // Declare argument list
  va_start (argp, v0);                          // Initialize macro
  for (size_t i = 1; i < nv; i++)                       // For remaining values given
    //this->data[i] = va_arg(argp,Type);        // Extract and assign 
    this->data[i] = va_arg(argp, xType);        // for v2.1 with fakeup cpp
  va_end (argp);
}


// CoolVector<Type> (CoolVector<Type>&) -- Copy constructor
// Input:   CoolVector reference
// Output:  None

template<class Type> 
CoolVector<Type>::CoolVector (const CoolVector<Type>& v)
 : CoolBaseVector(v)
{
  this->data = new Type[v.size];        // Allocate enough storage
  for (size_t i = 0; i < v.number_elements; i++)        // For each element in CoolVector
     this->data[i] = v.data[i];                 // Copy value
}


// ~CoolVector<Type> () -- Destructor for CoolVector class that frees up storage.
// Input:              None
// Output:             None

template<class Type> 
CoolVector<Type>::~CoolVector () {
  if (this->size != 0 &&
      this->alloc_size != INVALID_ALLOCSZ())            // If not user-provide storage
    delete [] this->data;                       // Free up the memory
}

// void fill () -- Fill elements `start' through `end' inclusive with valuen
// Input:          Type reference, start inclusive, and end index exclusive
// Output:         None


template<class Type> 
void CoolVector<Type>::fill (const Type& value, size_t start, size_t end) {
#if ERROR_CHECKING  
  if (start >= this->number_elements)           // If start out of range
    this->fill_start_error (#Type, start);      // Raise exception
  if (end < start || end > this->number_elements) // If end out of range
    this->fill_end_error (#Type, end);          // Raise exception
#endif
  for (size_t i = start; i < end; i++)          // For each element in range
    this->data[i] = value;                      // Assign new value
  this->curpos = INVALID_ALLOCSZ();                     // Invalidate current position
}


// void copy () -- Copy a specified range of elements from one vector to this
// Input:          CoolVector reference, start inclusive, and end index exclusive
// Output:         None

template<class Type> 
void CoolVector<Type>::copy (const CoolVector<Type>& v, size_t start, size_t end){
#if ERROR_CHECKING  
  if (start >= v.number_elements)               // If start out of range
    this->copy_start_error (#Type, start);      // Raise exception
  if (end < start || end > v.number_elements)   // If end out of range
    this->copy_end_error (#Type, end);          // Raise exception
#endif
  if (this->size < end) {                       // If not enough memory
#if ERROR_CHECKING
    if (alloc_size == INVALID_ALLOCSZ())                        // If static size vector
      this->copy_error (#Type);                 // Raise exception
#endif
    this->grow(end);
  }
  size_t i;
  for (i = start; i < end; i++)                 // For each element in range
    this->data[i] = v.data[i];                  // Assign new value
  if (i > this->number_elements)                // If we added new elements
    this->number_elements = i;                  // Update element count
  this->curpos = i;                             // Update current position
}


// search -- Find a subsequence of elements in a CoolVector
// Input:    CoolVector reference to subsequence searching for,
//           start inclusive, and end index exclusive
// Output:   TRUE/FALSE; current position updated if found

template<class Type> 
Boolean CoolVector<Type>::search (const CoolVector<Type>& v, size_t start, size_t end) {
  register size_t i, j;
  end -= v.number_elements;
  for (i = start; i < end; i++) {               // Find start of subsequence
    for (j = 0; j < v.number_elements; j++)
      if (!(*this->compare_s)(this->data[i+j],v.data[j])) // If not equal
        goto again2;                                      // try again

    this->curpos = i;                           // Point to start of sequence
    return TRUE;                                // Return starting index
  again2:;
  }
  return FALSE;
}


// Boolean operator== () -- Compare the elements of this CoolVector with a second
// Input:                   Reference to vector object
// Output:                  TRUE/FALSE

  template<class Type> 
Boolean CoolVector<Type>::operator== (const CoolVector<Type>& v) const {
  register size_t i;
  if (this->number_elements != v.number_elements) // If not same number
    return FALSE;                                 // Then not equal
  for (i = 0; i < this->number_elements; i++)     // For each element in vector
    if (!(*this->compare_s)(this->data[i],v.data[i]))
      return FALSE;
  return TRUE;
}


// Boolean is_data_equal () -- Default data comparison function if user has
//                             not provided another method
// Input:                      Two Type references
// Output:                     TRUE/FALSE

template<class Type>
Boolean CoolVector_is_data_equal (const Type& t1, const Type& t2) {
    return (t1 == t2);
}

// void set_compare () -- Specify the comparison function to be used in 
//                        logical comparisons of vector objects
// Input:                 Pointer to a compare function
// Output:                None

template<class Type> 
void CoolVector<Type>::set_compare (register Boolean (*cf)(const Type&, const Type&)) {
  if (cf)
    this->compare_s = cf;                       // Set to user function 
  else
    this->compare_s = &CoolVector_is_data_equal;        // or to default
}


// CoolVector<Type&> operator= () -- Overload the assignment operator
// Input:                        Reference to CoolVector object
// Output:                       Reference to copied CoolVector object

template<class Type> 
CoolVector<Type>& CoolVector<Type>::operator= (const CoolVector<Type>& v) {
  if (this->size < v.size) {                    // If not enough memory
#if ERROR_CHECKING
    if (alloc_size == INVALID_ALLOCSZ())                        // If static size vector
      this->assign_error (#Type);               // Raise exception
#endif
    if (this->size != 0)
      delete [] this->data;                     // Free it up
    this->data = new Type[v.size];              // and allocate bigger memory
  this->size = v.size;                          // Set new vector size
  }
  this->CoolBaseVector::operator= (v);             // Base class assignment
  for (size_t i = 0; i < v.number_elements; i++)        // For each element
    this->data[i] = v.data[i];                  // Copy value
  return *this;                                 // Return CoolVector reference
}


// CoolVector<Type>& operator= () -- Overload the assignment operator to assign a
//                               single value to all the elements of a vector
// Input:                        Type reference to fill value
// Output:                       Reference to updated CoolVector object

template<class Type> 
CoolVector<Type>& CoolVector<Type>::operator= (const Type& value) {
  for (size_t i = 0; i < this->number_elements; i++) // For each element in CoolVector
    this->data[i] = value;                         // Set initial value
  this->curpos = INVALID_POS();                    // Point to first element
  return *this;                                    // Return CoolVector reference
}

// Boolean find () -- Find first/next occurrence of element in a CoolVector.
//                    from start or end of vector respectively if dir = +1, -1.
// Input:             Type reference to search value, starting index
// Output:            TRUE/FALSE

template<class Type> 
Boolean CoolVector<Type>::find (const Type& value, size_t start, int dir) {
  register size_t i = start;
  register size_t n = this->number_elements - start; // max number of elmts to search
  if (dir == -1) n = start + 1;
  for (; n > 0; n--, i += dir)
    if ((*(this->compare_s))(this->data[i], value) == TRUE) {
      this->curpos = i;
      return TRUE;
    }
  return FALSE;
}

// void grow () -- Adjust the memory size of a CoolVector to accomodate some size
// Input:            Interger number of elements to hold
// Output:           None

template<class Type> 
void CoolVector<Type>::grow (size_t min_size) {
  size_t new_size = (size_t)(this->size * (1.0 + growth_ratio)); // New size?
  if (new_size < min_size) new_size = min_size + alloc_size;
  resize(new_size);
}

// void resize () -- Adjust the memory size of a CoolVector to accomodate some size
// Input:            Interger number of elements to hold
// Output:           None

template<class Type> 
void CoolVector<Type>::resize (size_t new_size) {
  if (new_size <= this->size) {                 // Don't bother shrinking
#if ERROR_CHECKING  
    if (new_size < 0)                           // If index out of range
      this->resize_error (#Type,new_size);      // Raise exception
#endif
    return;
  }
#if ERROR_CHECKING  
  if (alloc_size == INVALID_ALLOCSZ())                  // If static size vector
    this->static_error (#Type);                 // Raise exception
#endif
  Type* temp = new Type[new_size];              // Allocate storage
  for (size_t i = 0; i < this->number_elements; i++)// For each element in Vector
    temp[i] = this->data[i];                    // Copy into new storage
  if (this->size != 0)
    delete [] this->data;                       // Free up old memory
  this->data = temp;                            // Assign new memory block
  this->size = new_size;                        // Save new size value
}


// push -- Add element to end of the CoolVector
// Input:  Type reference
// Output: TRUE if successful, FALSE if could not grow vector

template<class Type> 
Boolean CoolVector<Type>::push (const Type& value) {
  if (this->number_elements == this->size) {    // If not enough memory
    if (this->alloc_size == INVALID_ALLOCSZ())          // If not allowed to grow
      return FALSE;                             // Return failure flag
    this->grow(this->size+1);                   // Else grow the CoolVector
  }
  this->curpos = this->number_elements;         // Set current position
  this->data[this->number_elements++] = value;  // Assign new value
  return TRUE;                                  // Return success status
}


// push_new -- Add an element if it is not already there
// Input:      Reference to Type
// Output:     TRUE/FALSE

template<class Type> 
Boolean CoolVector<Type>::push_new (const Type& value) {
  if (this->number_elements == this->size && this->alloc_size == INVALID_ALLOCSZ())
    return FALSE;                               // Return failure flag
  if (this->find(value, this->number_elements-1, -1)) 
    return FALSE;               // If already in CoolVector
  if (this->number_elements >= this->size)      // If not enough memory
    this->grow(this->size+1);                   // Else grow the Vector
  this->curpos = this->number_elements;         // Set current position
  this->data[this->number_elements++] = value;  // Assign new value
  return TRUE;                                  // Return success status
}


// pop --   Return last element in the CoolVector
// Input:   None
// Output:  Type reference to last element in CoolVector

template<class Type> 
Type& CoolVector<Type>::pop () {
  if (this->number_elements > 0) {              // If there are elements
    this->curpos = INVALID_POS();                       // Invalidate current position
    return (this->data[--this->number_elements]); // Return the last one
  }
   return (this->data[0]); // Return an arbitrary one
}


// reverse -- Destructively reverse the order of elements in a CoolVector
// Input:     None
// Output:    None

template<class Type> 
void CoolVector<Type>::reverse () {
  Type temp;
  for (size_t i = 0, j = this->number_elements-1;       // Counting from front and rear
       i < this->number_elements / 2; i++, j--) { // until we reach the middle
    temp = this->data[i];                       // Save front data item
    this->data[i] = this->data[j];              // Switch with rear data item
    this->data[j] = temp;                       // Copy new rear data item
  }
  this->curpos = INVALID_POS();                 // Invalidate current position
}


// remove -- Destructively remove item at current position.
//           Shift remaining elmts to preserve order in vector.
// Input:   None
// Output:  Type reference to item removed

template<class Type> 
Type CoolVector<Type>::remove () {
  IterState i = this->curpos;
  if (i == INVALID_POS()) this->remove_error("Type");
  Type value = this->data[i];
  Type* elmts = &this->data[i];                 // Shift elements by 1 place
  while (++i < this->number_elements) {         // Copy over current
    *elmts = *(elmts + 1);              // Use assignment of class Type
    elmts++;            // Use assignment of class Type
  }
  this->number_elements--;                      // Update element count
  if (this->curpos == this->number_elements)    // If past end of vector
    this->curpos = INVALID_POS();                       // Invalidate current position
  return value;                                 // Return Vector reference
}


// remove -- Destructively remove the first occurrence of an element
// Input:   Type reference
// Output:  TRUE if element found and removed, FALSE otherwise

template<class Type> 
Boolean CoolVector<Type>::remove (const Type& value, int dir) {
  size_t start = 0;
  if (dir == -1) start = this->number_elements - 1;
  if (this->find(value, start, dir)) {          // When found
    size_t i = this->curpos;
    Type* elmts = &this->data[i];               // Shifts remaining elements
    while (++i < this->number_elements) {       // by 1 place
      *elmts = *(elmts + 1);            // Use assignment of class Type
      elmts++;          // Use assignment of class Type
    }
    this->number_elements--;                    // Update element count
    return TRUE;
  } else 
    return FALSE;
}


// remove_duplicates -- Destructively remove duplicate elements from CoolVector
// Input:               None
// Output:              TRUE if any removed, FALSE otherwise

template<class Type> 
Boolean CoolVector<Type>::remove_duplicates () {
  this->curpos = INVALID_POS();                 // Invalidate current position
  if (this->size == 0) return FALSE;
  Boolean success = FALSE;                      // Return value
  Type* old = this->data;                       // Old data
  this->data = new Type[this->size];            // New data
  size_t n = this->number_elements;             // length of old data
  this->number_elements = 0;
  for (size_t i = 0; i < n; i++) {              // For each element
    if (!this->find(old[i], this->number_elements-1, -1)) { // if not copied yet
      this->data[this->number_elements++] = old[i]; // copy value
      success = TRUE;                           // Set flag
    }
  }
  delete [] old;                                // destroy old data
  return success;
}


// replace -- Destructively replace the first occurrence of an element
// Input:     Two Type references
// Output:    TRUE if element found and replaced, FALSE otherwise

template<class Type> 
Boolean CoolVector<Type>::replace (const Type& t1, const Type& t2, int dir) {
  size_t start = 0;
  if (dir == -1) start = this->number_elements -1;
  if (this->find(t1, start, dir)) {
    this->data[this->curpos] = t2;              
    if (dir == +1) this->curpos++;              // Point to item after replace
    else this->curpos--;                        // in direction of dir
    if (this->curpos > this->number_elements) this->curpos = INVALID_POS();
    return TRUE;
  } else return FALSE;
}


// replace_all -- Destructively replace all occurrences of an element
// Input:         Two Type references
// Output:        TRUE if any element replaced, FALSE otherwise

template<class Type> 
Boolean CoolVector<Type>::replace_all (const Type& t1, const Type& t2) {
  Boolean success = FALSE;                      // Return value
  this->curpos = 0;
  for (;;) {
    if (!this->find(t1, this->curpos, +1)) return success;
    this->data[this->curpos++] = t2;            // Point to item after replace
    if (this->curpos > this->number_elements) this->curpos = INVALID_POS();
  }
}


// prepend -- Destructively insert the elements of some CoolVector at the beginning
// Input:     CoolVector reference
// Output:    TRUE if successful, FALSE if could not grow CoolVector

template<class Type> 
Boolean CoolVector<Type>::prepend (const CoolVector<Type>& v) {
  Type* temp;                                   // Declare temporary
  size_t old_size = this->size;                 // Keep old size
  size_t new_size = this->number_elements + v.number_elements; // Minimum size
  if (this->size < new_size) {                  // Enough memory?
    if (this->alloc_size == INVALID_ALLOCSZ())          // If not allowed to grow
      return FALSE;                             // Return failure flag
    this->size = (size_t)(this->size * (1.0 + growth_ratio)); // New size
    if (this->size < new_size)                  // If not enough
      this->size = new_size + this->alloc_size; // use alloc_size
  }
  temp = new Type[this->size];                  // Allocate required storage
  size_t i;
  for (i = 0; i < v.number_elements; i++)        // For all elements
    temp[i] = v.data[i];                        // Copy to beginning of CoolVector
  this->curpos = i;                             // Set current position
  for (i = 0; i < this->number_elements; i++)   // For each element
    temp[i+v.number_elements] = this->data[i];  // Copy into new storage
  if (old_size != 0)
    delete [] this->data;                       // Free up old memory
  this->data = temp;                            // Assign new memory block
  this->number_elements += v.number_elements;   // Update element count
  return TRUE;                                  // Return success flag
}


// append -- Destructively append the elements of some CoolVector at the beginning
// Input:    CoolVector reference
// Output:   TRUE if successful, FALSE if could not grow CoolVector

template<class Type> 
Boolean CoolVector<Type>::append (const CoolVector<Type>& v) {
  size_t new_size = this->number_elements + v.number_elements; // Minimum size
  if (this->size < new_size) {                  // Enough memory?
    if (this->alloc_size == INVALID_ALLOCSZ())          // If not allowed to grow
      return FALSE;                             // Return failure flag
    this->grow(new_size);
  }
  this->curpos = this->number_elements - 1;     // Set current position
  for (size_t i = 0; i < v.number_elements; i++)        // For each element
    this->data[i+this->number_elements] = v.data[i]; // Copy to end of CoolVector
  this->number_elements += v.number_elements;   // Update element count
  return TRUE;                                  // Return CoolVector reference
}


// insert_before -- Destructively insert an element before current position
// Input:           Type reference
// Output:          TRUE if successful, FALSE if could not grow CoolVector

template<class Type> 
Boolean CoolVector<Type>::insert_before (const Type&  value) {
  if (this->curpos == INVALID_POS())                    // If no current position
    return FALSE;                               // Return failure flag
  return this->insert_before(value, this->curpos);
}


// insert_after -- Destructively insert an element after current position
// Input:          Type reference
// Output:         TRUE if successful, FALSE if could not grow CoolVector

template<class Type> 
Boolean CoolVector<Type>::insert_after (const Type&  value) {
  if (this->curpos == INVALID_POS())                    // If no current position
    return FALSE;                               // Return failure flag
  return this->insert_after(value, this->curpos);
}


// insert_before -- Destructively insert an element before specified index
// Input:           Type reference and an index position
// Output:          TRUE if successful, FALSE if could not grow CoolVector

template<class Type> 
Boolean CoolVector<Type>::insert_before (const Type&  value, size_t index) {
  size_t n = this->number_elements+1;
  if (n >= this->size) {                        // If not enough memory
    if (this->alloc_size == INVALID_ALLOCSZ())          // If not allowed to grow
      return FALSE;                             // Return failure flag
    this->grow(n);
  }
  this->curpos = index;                         // Current position at insertion
  Type* elmts = &this->data[n-1];               // Use operator= of Type
  while (--n > index) {                         // shift elements by 1 place
    *elmts = *(elmts - 1);                      // Copy over current
    elmts--;
  }    
  this->data[this->curpos] = value;             // Assign new value
  this->number_elements++;
  return TRUE;                                  // Return CoolVector reference
}


// insert_after -- Destructively insert an element after specified index
// Input:          Type reference and an index position
// Output:         TRUE if successful, FALSE if could not grow CoolVector

template<class Type> 
Boolean CoolVector<Type>::insert_after (const Type&  value, size_t index) {
  size_t n = this->number_elements+1;
  if (n >= this->size) {                        // If not enough memory
    if (this->alloc_size == INVALID_ALLOCSZ())          // If not allowed to grow
      return FALSE;                             // Return failure flag
    this->grow(n);
  }
  this->curpos = ++index;                       // Current position at insertion
  Type* elmts = &this->data[n-1];               // Use operator= of Type
  while (--n > index) {                         // shift elements by 1 place
    *elmts = *(elmts - 1);                      // Copy over current
    elmts--;
  }    
  this->data[this->curpos] = value;             // Assign new value
  this->number_elements++;
  return TRUE;                                  // Return CoolVector reference
}



// merge -- Destructively merge two CoolVectors of the same size using a user
//          supplied predicate function. The predicate function returns
//          TRUE if and only if the first element preceeds the second.
// Input:   CoolVector reference and pointer to Predicate function
// Output:  None

template<class Type> 
void CoolVector<Type>::merge(const CoolVector<Type>& v, int (*pred)(const Type&, const Type&)) {
  size_t old_size = this->size;                 // Keep old size
  size_t total = this->number_elements+v.number_elements; // Min space required
  if (this->size < total) {
    this->size = (size_t)(this->size*(1.0+growth_ratio)); // New size by ratio
    if (this->size < total)                           // check growth
      this->size = total + this->alloc_size;          // New size by growth
  }
  Type* temp = new Type[this->size];            // Allocated storage
  for (size_t i=0,j=0,k=0; i < total; i++) {    // For all elements
    if (j >= this->number_elements) {           // If past end of CoolVector
      for (; k < v.number_elements; k++, i++)   // Copy rest of other vector
        temp[i] = v.data[k];                    // Copy element value
      break;                                    // Break out of loop when done
    }
    else if (k >= v.number_elements) {          // If past end of other vector
      for (; j < this->number_elements;j++,i++) // Copy rest of other vector
        temp[i] = this->data[j];                // Copy element value
      break;                                    // Break out of loop when done
    }
    else {
      if ((*pred)(this->data[j], v.data[k]) < 0) // If it goes here
        temp[i] = this->data[j++];              // Copy first element
      else
        temp[i] = v.data[k++];                  // Copy second element
    }
  }
  if (old_size != 0)
    delete [] this->data;                       // Free up old memory
  this->data = temp;                            // Assign new memory block
  this->number_elements = total;                // Update element count
  this->curpos = INVALID_POS();                 // Invalidate current position
}


// sort -- Destructively sort the elements of a vector using the supplied
//         predicate function. The predicate function returns TRUE if and only
//         if the first element preceeds the second. This routine uses the 
//         Heap Sort algorithm as given in "Numerical Recipes in C" p247.
// Input:   Pointer to predicate function
// Output:  None

#ifdef __cplusplus
 
#include <stdlib.h>             // include the standard c library

extern "C" {
typedef int (*_cmp_rtn)( const void *, const void * );
};

template<class Type> 
void CoolVector<Type>::sort (register int (*p)(const Type&, const Type&)) {
  if (this->number_elements > 1)                // If more than one element
    qsort((void*) this->data,
          (size_t)(unsigned int) this->number_elements, (size_t)sizeof(Type),
          (_cmp_rtn) p);
}

#else                                           // Not all machines have qsort

template<class Type> 
void CoolVector<Type>::sort (int (*p)(const Type&, const Type&)) {
  if (this->number_elements > 1) {              // If more than one element
    size_t n = this->number_elements;
    size_t l, j, ir, i;
    Type temp;
    Type* v_prime = this->data - 1;             // Adjust for 1-based index
    l = (n >> 1) + 1;
    ir = n;
    while (TRUE) {
      if (l > 1)
        temp = v_prime[--l];
      else {
        temp = v_prime[ir];
        v_prime[ir] = v_prime[1];
        if (--ir == 1) {
          v_prime[1] = temp;
          break;
        }
      }
      i = l;
      j = i << 1;
      while (j <= ir) {
        if (j < ir && (*p)(v_prime[j], v_prime[j+1]) < 0)
          ++j;
        if ((*p)(temp, v_prime[j]) < 0) {
          v_prime[i] = v_prime[j];
          j += (i = j);
        }
        else
          j = ir + 1;
      }
      v_prime[i] = temp;
    }
    this->curpos = INVALID_POS();                       // Invalidate current position
  }
}
#endif
  
// ostream& operator<< () -- Overload the output operator for CoolVector reference
// Input:                    Ostream reference and CoolVector reference
// Output:                   Ostream reference

template<class Type>
ostream& operator<< (ostream& os, const CoolVector<Type>& v) {
  if (v.length() == 0)                          // If no elements in vector
    os << " Empty ";                            // Indicate empty status
  else {
    os << "[ ";
    for (size_t i = 0; i < v.length(); i++)     // For each element
      os << v.data[i] << " ";                   // Output value to stream
    os << "]";
  }
  return os;                                    // Return ostream reference
}



// shuffle_remove -- Destructively remove item at current position
//           Shuffle last element up to fill hole.
// Input:   None
// Output:  Type reference to item removed

template<class Type> 
Type CoolVector<Type>::shuffle_remove () {
  size_t i = this->curpos;
  if (i == INVALID_POS()) this->remove_error("Type");
  Type value = this->data[i];
  this->data[this->curpos] =                    // fill hole at curpos
    this->data[this->number_elements - 1];      // with last element in vector
  this->number_elements--;                      // Update element count
  if (this->curpos == this->number_elements)    // If past end of vector
    this->curpos = INVALID_POS();                       // Invalidate current position
  return value;                                 // Return Vector reference
}



// shuffle_remove -- Destructively remove the first occurrence of an element
//           Shuffle last element up to fill hole
// Input:   Type reference
// Output:  TRUE if element found and removed, FALSE otherwise

template<class Type> 
Boolean CoolVector<Type>::shuffle_remove (const Type& value, int dir) {
  size_t start = 0;
  if (dir == -1) start = this->number_elements - 1;
  if (this->find(value, start, dir)) {          // When found
    this->data[this->curpos] =                  // fill hole at curpos
      this->data[this->number_elements - 1];    // with last element in vector
    this->number_elements--;                    // Update element count
    return TRUE;
  } else 
    return FALSE;
}


// shuffle_insert_before -- Destructively insert an element before current position
//            Shuffle last element up to fill hole
// Input:           Type reference
// Output:          TRUE if successful, FALSE if could not grow CoolVector

template<class Type> 
Boolean CoolVector<Type>::shuffle_insert_before (const Type&  value) {
  if (this->curpos == INVALID_POS())                    // If no current position
    return FALSE;                               // Return failure flag
  return this->shuffle_insert_before(value, this->curpos);
}


// shuffle_insert_after -- Destructively insert an element after current position
//            Shuffle last element up to fill hole
// Input:          Type reference
// Output:         TRUE if successful, FALSE if could not grow CoolVector

template<class Type> 
Boolean CoolVector<Type>::shuffle_insert_after (const Type&  value) {
  if (this->curpos == INVALID_POS())                    // If no current position
    return FALSE;                               // Return failure flag
  return this->shuffle_insert_after(value, this->curpos);
}

// shuffle_insert_before -- Destructively insert an element before specified index
//            Shuffle element at current position to end of vector
// Input:           Type reference and an index position
// Output:          TRUE if successful, FALSE if could not grow CoolVector

template<class Type> 
Boolean CoolVector<Type>::shuffle_insert_before (const Type&  value, size_t index) {
  size_t n = this->number_elements+1;
  if (n >= this->size) {                        // If not enough memory
    if (this->alloc_size == INVALID_ALLOCSZ())          // If not allowed to grow
      return FALSE;                             // Return failure flag
    this->grow(n);
  }
  this->curpos = index;                         // Current position at insertion
  this->data[this->number_elements] =           // Move data at curpos
    this->data[this->curpos];                   // to end of vector
  this->data[this->curpos] = value;             // Assign new value
  this->number_elements++;
  return TRUE;                                  // Return CoolVector reference
}



// shuffle_insert_after -- Destructively insert an element after specified index
//            Shuffle element at current position to end of vector
// Input:          Type reference and an index position
// Output:         TRUE if successful, FALSE if could not grow CoolVector

template<class Type> 
Boolean CoolVector<Type>::shuffle_insert_after (const Type&  value, size_t index) {
  size_t n = this->number_elements+1;
  if (n >= this->size) {                        // If not enough memory
    if (this->alloc_size == INVALID_ALLOCSZ())          // If not allowed to grow
      return FALSE;                             // Return failure flag
    this->grow(n);
  }
  this->curpos = ++index;                       // Current position at insertion
  this->data[this->number_elements] =           // Move data at curpos
    this->data[this->curpos];                   // to end of vector
  this->data[this->curpos] = value;             // Assign new value
  this->number_elements++;
  return TRUE;                                  // Return CoolVector reference
}

#endif // VECTORC

