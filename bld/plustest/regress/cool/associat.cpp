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

#ifndef ASSOCIATIONC
#define ASSOCIATIONC

#include <cool/Association.h>

// compare_keys_s
template <class Ktype, class Vtype>
//##Key_Compare CoolAssociation<Ktype,Vtype>::compare_keys_s = &CoolAssociation_keys_eql;
Boolean (*CoolAssociation<Ktype,Vtype>::compare_keys_s)(const Ktype&, const Ktype&) = &CoolAssociation_keys_eql;

// compare_values_s;
template <class Ktype, class Vtype>
//##Value_Compare CoolAssociation<Ktype,Vtype>::compare_values_s = &CoolAssociation_values_eql;
Boolean (*CoolAssociation<Ktype,Vtype>::compare_values_s)(const Vtype&, const Vtype&) = &CoolAssociation_values_eql;

// CoolAssociation -- Empty constructor for the CoolAssociation class
// Input:         None
// Output:        None

template <class Ktype, class Vtype>
CoolAssociation<Ktype,Vtype>::CoolAssociation() {
}


// CoolAssociation -- constructor that specifies number of elements
// Input:         Number of elements of Type T
// Output:        None

template <class Ktype, class Vtype>
CoolAssociation<Ktype,Vtype>::CoolAssociation(size_t n)
 : CoolVector< CoolPair<Ktype,Vtype> >(n)
{
}


// CoolAssociation -- constructor that specifies number of elements and storage
// Input:         Pointer to storage and number of elements of Type T
// Output:        None

template <class Ktype, class Vtype>
CoolAssociation<Ktype,Vtype>::CoolAssociation(void* s,size_t n)
 : CoolVector< CoolPair<Ktype,Vtype> >(s,n)
{
}


// CoolAssociation -- constructor for reference to another CoolAssociation object
// Input:         CoolAssociation reference
// Output:        None

template <class Ktype, class Vtype>
CoolAssociation<Ktype,Vtype>::CoolAssociation(const CoolAssociation<Ktype,Vtype>& a)
 : CoolVector< CoolPair<Ktype,Vtype> >(a)
{
}


// CoolAssociation -- Destructor (not inline because it's virtual)
// Input:         None
// Output:        None

template <class Ktype, class Vtype>
CoolAssociation<Ktype,Vtype>::~CoolAssociation() {;}


// set_key_compare -- Set the compare function for the key of the pair
// Input:             Pointer to new function
// Output:            None

template <class Ktype, class Vtype>
void CoolAssociation<Ktype,Vtype>::set_key_compare( register /*Key_Compare##*/Boolean (*cf)(const Ktype&, const Ktype&)) {
  if (cf == NULL)
    this->compare_keys_s=&CoolAssociation_keys_eql; // Default equal   
  else
    this->compare_keys_s = cf;
}

// set_value_compare -- Set the compare function for the value of the pair
// Input:               Pointer to new function
// Output:              None

template <class Ktype, class Vtype>
void CoolAssociation<Ktype,Vtype>::set_value_compare( register /*Value_Compare##*/Boolean (*cf)(const Vtype&, const Vtype&)) {
  if (cf == NULL)
    this->compare_values_s = &CoolAssociation_values_eql; // Default
  else
    this->compare_values_s = cf;
}

// find -- Find first occurence of element in an CoolAssociation
// Input:  Element value searching for
// Output: TRUE/FALSE; current_position updated when success

template <class Ktype, class Vtype>
Boolean CoolAssociation<Ktype,Vtype>::find (const Ktype& key) {
  size_t i = this->number_elements;
  while (i > 0) { // Search most recent first
    --i;
    if ((*this->compare_keys_s)(this->data[i].get_first(), key) == TRUE) {
      this->curpos = i;                         // Update current position if found
      return TRUE;                              // Return success
    }
  }
  return FALSE;                                 // Return failure
}


// get -- Get the associated value for a key in the CoolAssociation; return TRUE 
//        and modify the given value variable if the key is found, otherwise 
//        return FALSE and do not modify the value variable
// Input:  Reference to a key, reference to a value 
// Output: TRUE or FALSE

template <class Ktype, class Vtype>
Boolean CoolAssociation<Ktype,Vtype>::get (const Ktype& key, Vtype& value) {
  size_t i = this->number_elements;
  while (i > 0) { // Search most recent first
    --i;
    if ((*this->compare_keys_s)(this->data[i].get_first(), key) == TRUE) {
      value = this->data[i].second();           // Modify value only if found
      return TRUE;                              // Return success
    }
  }
  return FALSE;                                 // Return failure
}


// get_key -- Get the associated key for a value in the CoolAssociation; return 
//            TRUE and modify the given key variable if the value is found, 
//            otherwise return FALSE and do not modify the value variable
// Input:     Reference to a value, reference to a key 
// Output:    TRUE or FALSE

template <class Ktype, class Vtype>
Boolean CoolAssociation<Ktype,Vtype>::get_key (const Vtype& value, Ktype& key) const {
  size_t i = this->number_elements;
  while (i > 0) { // Search most recent first
   --i;
   if ((*this->compare_values_s)(this->data[i].get_second(), value) == TRUE) {
     key = this->data[i].first();               // Modify key only if found
     return TRUE;                               // Return success
   }
  }
  return FALSE;                                 // Return failure
}


// remove() -- Remove the pair at the current position and return the 
//             value associated with that pair. Order is not preserved.
// Input:      None.
// Output:     Value of the removed pair, returned by value.

template <class Ktype, class Vtype>
Vtype CoolAssociation<Ktype,Vtype>::remove () {
  if (this->curpos == INVALID) this->remove_error("CoolPair");
  Vtype value = this->data[this->curpos].second(); // Copy value removed
  this->number_elements--;                      // Update element count
  if (this->curpos == this->number_elements)    // If past end of vector
    this->curpos = INVALID;                     // Invalidate current position
  else                                          // fill hole with last elmt
    this->data[this->curpos] = this->data[this->number_elements]; 
  return value;                                 // Return removed item
}

// remove (key) -- Search for key and remove the pair associated with it, 
//                 if found, and return TRUE; otherwise return FALSE
// Input:          Reference to a key
// Output:         TRUE or FALSE

template <class Ktype, class Vtype>
Boolean CoolAssociation<Ktype,Vtype>::remove (const Ktype& key) {
  if (this->find(key)) {                        // Search for pair with key
    this->remove();                             // Remove and reset curpos
    return TRUE;                                // Return success/failure
  }
  else
    return FALSE;                               
}

// put -- Add a key-value pair to the CoolAssociation; if the key already exists,
//        replace its associated value in the CoolAssociation; return TRUE if 
//        successful
// Input:  Reference to a key, reference to a value
// Output: TRUE or FALSE

template <class Ktype, class Vtype>
Boolean CoolAssociation<Ktype,Vtype>::put (const Ktype& key, const Vtype& value) {
  size_t i = this->number_elements;
  while (i > 0) { // Search most recent first
    --i;
    if ((*this->compare_keys_s)(this->data[i].get_first(), key) == TRUE) {
      this->data[i].set_second(value);          // Update value if found
      this->curpos = i;                         // Update current position
      return TRUE;                              // Return success
    }
  }
  if (this->number_elements == this->size) {    // If not enough memory 
    if (this->alloc_size == INVALID) {          // If not allowed to grow
#if ERROR_CHECKING
      //RAISE (Error, SYM(CoolAssociation), SYM(Static_Size)),
      printf ("CoolAssociation<%s,%s>::put(): Static-size CoolAssociation.\n", #Ktype,#Vtype);
#endif
      return FALSE;                             // Return failure flag
    }
    CoolPair<Ktype,Vtype>* temp;                // Temporary storage
    if (this->growth_ratio != 0.0)              // If growth ratio specified
      this->size = (size_t)(this->size * (1.0 + growth_ratio)); // New size
    else
      this->size += alloc_size;                 // Update vector size
    temp = new CoolPair<Ktype,Vtype>[this->size];       // Allocate storage
    for (size_t i = 0; i < this->length(); i++) // For all elements
      temp[i] = this->data[i];                  // Copy data
    delete [] this->data;                       // Free up old memory
    this->data = temp;                          // Assign new memory block
  }
  this->curpos = this->length();                // Set current position
  this->data[this->length()].set_first(key);    // Set the key (first) value
  this->data[this->length()].set_second(value); // Set the value (second)
  this->number_elements++;                      // Increment element count
  return TRUE;                                  // Return success status
}


// operator== -- Compare the elements of two CoolAssociations of Type Type using
//               the Compare pointer to funtion (default is ==). If one 
//               CoolAssociation has more elements than another CoolAssociation, 
//               then result is FALSE
// Input:        Reference to CoolAssociation object
// Output:       TRUE/FALSE

template <class Ktype, class Vtype>
Boolean CoolAssociation<Ktype,Vtype>::operator==
        (const CoolAssociation<Ktype,Vtype>& a) const {
  if (this->number_elements != a.number_elements) // If not same number
    return FALSE;                                 // Then not equal
  for (size_t i = 0; i < this->number_elements; i++) { // For each element
    for (size_t j = 0; j < a.number_elements; j++) {
      if ((*this->compare_keys_s)(this->data[i].get_first(),
                                  a.data[j].get_first()) == TRUE) {
        if ((*this->compare_values_s) (this->data[i].second(),
                                       a.data[j].second()) == TRUE)
          goto match;                           // Match
        return FALSE;                           // Values different
      }
    }
    return FALSE;                               // Key not found
  match: ;
  }
  return TRUE;                                  // All are equal
}

// operator<< -- Overload the output operator for reference to CoolAssociation
// Input:        ostream reference, CoolAssociation reference
// Output:       CoolAssociation data is output to ostream

template <class Ktype, class Vtype>
ostream& operator<< (ostream& os, const CoolAssociation<Ktype,Vtype>& v) {
  return operator<< (os, (const CoolVector< CoolPair<Ktype,Vtype> >&) v);
}


// keys_eql -- Compare two keys to see if they are equal using operator==
// Input:      Two Ktype references
// Output:     TRUE or FALSE

template <class Ktype>
Boolean CoolAssociation_keys_eql (const Ktype& k1, const Ktype& k2) {
  return (k1 == k2);
}


// values_eql -- Compare two values to see if they are equal using operator==
// Input:        Two Vtype references
// Output:       TRUE or FALSE

template <class Vtype>
Boolean CoolAssociation_values_eql(const Vtype& v1,const Vtype& v2) {
  return (v1 == v2);
}

#endif // ASSOCIATIONC

