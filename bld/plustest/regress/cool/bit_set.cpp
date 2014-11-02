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
//
// Created: MBN 06/13/89 -- Initial implementation
// Updated: MBN 09/31/89 -- Added conditional exception handling
// Updated: MBN 10/07/89 -- Changed operator[-~^&|] to allocate on stack
//                          Bit_Set::find(int) returns state of indicated bit
// Updated: MBN 10/12/89 -- Changed "current_position" to "curpos" and added
//                          the current_position() method for Iterator<Type>
// Updated: MBN 10/13/89 -- Changed from bit field to preprocessor bit macros
// Updated: LGO 11/09/89 -- Major changes to every method
// Updated: MBN 01/10/90 -- Fixed size-check bug in operator=
// Updated: MJF 03/12/90 -- Added group names to RAISE
// Updated: DLS 03/26/91 -- New lite version
// Updated: VDN 05/01/92 -- Copying by bytes only, to avoid out of bounds.
// Updated: JAM 08/12/92 -- removed DOS specifics, stdized #includes
// Updated: JAM 08/19/92 -- added static data def
//
// This file contains member and  friend function implementation  code for  the
// Bit Set class defined in the  Bit_Set.h header file.  Where  appropriate and
// possible,  interfaces to, and  us  of, existing system   functions  has been
// incorporated.  An overview of the structure of the CoolBit_Set class, along with
// a synopsis of each member and friend function, can be found in the Bit_Set.h
// header file.
//

#ifndef BIT_SET_H                               // If no class definition
#include <cool/Bit_Set.h>                       // Include class specification
#endif

#include <string.h>                             // For strcpy
#include <stdlib.h>                             // For exit()

#define BS_MAKE_POSITION(byte, offset) ((byte << 3) | offset)
#define BS_BYTE_COUNT(n) ((int) (n + 7) >> 3)   // Byte count from bit count

extern int bit_pos[];                           // Lowest/highest bit set masks
extern int bits_set[];                          // Number of bits set in mask
extern int powers_of_2_minus_1[];               // Number of contiguous bits


// alloc_size_s -- Allocation size for growth
int CoolBit_Set::alloc_size_s = BIT_SET_BLK_SZ;         // Set memory block size


// CoolBit_Set -- Simple constructor that takes no arguments and allocates no
//            storage
// Input:     None
// Output:    None

CoolBit_Set::CoolBit_Set () {
  this->data = NULL;                            // Zero initial memory
  this->size = 0;                               // Save number of bytes
  this->number_elements = 0;                    // Save number of elements
  this->curpos = INVALID;                       // Reset current position
  this->growth_ratio = 0.0;                     // Initialize growth ratio
}


// CoolBit_Set -- Constructor that takes an integer argument specifying an initial
//            number of elements for which storage must be allocated
// Input:     Initial number of elements
// Output:    None

CoolBit_Set::CoolBit_Set (int n) {
  int nbyte = BS_BYTE_COUNT(n);
  this->data = new unsigned char[nbyte];        // Allocate storage
  for (int i = 0; i < nbyte; i++) data[i] = 0;  // Initialize bits to zero
  this->size = nbyte;                           // Save number of bytes
  this->number_elements = 0;                    // Save number of elements
  this->curpos = INVALID;                       // Reset current position
  this->growth_ratio = 0.0;                     // Initialize growth ratio
}


// CoolBit_Set -- Constructor that takes a reference to another CoolBit_Set object and
//            duplicates its size and value
// Input:     Reference to Bit_Set object
// Output:    None

CoolBit_Set::CoolBit_Set (const CoolBit_Set& b) {
  this->data = new unsigned char[b.size];         // Allocate storage
  for (int i = 0; i < b.size; i++)                // For each byte in vector
    this->data[i] = b.data[i];                    // Copy value
  this->size = b.size;                            // Maintain number of bytes
  this->number_elements = b.number_elements;      // Save number of elements
  this->curpos = INVALID;                         // Reset current position
  this->growth_ratio = 0.0;                       // Initialize growth ratio
}


// ~CoolBit_Set -- Destructor for CoolBit_Set objects
// Input:      None
// Output:     None

CoolBit_Set::~CoolBit_Set () {
  delete [] this->data;                         // Free up memory allocated
}

// MACRO generate_next(operation=, excess_a=0, excess_b=0) {
//   int index, offset;
//   long pos = this->curpos;
//   if (pos == INVALID) {                      // If invalid current position
//     index = 0;                               // Start at first byte
//     offset = -1;                             // Start at zero'th bit
//   } else {
//     index = BS_BYTE_NUMBER(pos);             // Get current byte index
//     offset = BS_BYTE_OFFSET(pos);            // Get current bit offset
//   }
// 
// #if excess_a || excess_b  
//   int end = min(this->number_elements, b.number_elements);
// #else
//   int end = this->number_elements;
// #endif
//   register int value = (~powers_of_2_minus_1[offset+1]);
//   while(index < end) {
//     value &= this->data[index] operation;
//     if (value != 0) goto found;
//     value = -1;
//     index++;
//   }
// #if excess_a || excess_b  
//   if (index < (end = this->number_elements)) {
// #if excess_a
//     while(index < end) {
//       value &= this->data[index];
//       if (value != 0) goto found;
//       value = -1;
//       index++;
//     }
// #endif
//   } else {
// #if excess_b
//     end = b.number_elements;
//     while(index < end) {
//       value &= b.data[index];
//       if (value != 0) goto found;
//       value = -1;
//       index++;
//     }
// #endif
//   }
// #endif
//   this->curpos = INVALID;                    // Invalidate current position
//   return FALSE;                              // Return failure
//  found:
//   this->curpos = BS_MAKE_POSITION(index, bit_pos[value]); 
//   return TRUE;                               // Indicate success
// }


// next -- Increment the current position index
// Input:  None
// Output: Boolean TRUE/FALSE

Boolean CoolBit_Set::next () {
  //generate_next()                             // take out this macro expansion
  
  int index, offset;
  long pos = this->curpos;
  if (pos == (-1)) {
    index = 0;
    offset = -1;
  } else {
    index = ((int) (pos >> 3));
    offset = ((int) (pos & 0x07));
  }
  int end = this->number_elements;
  register int value = (~powers_of_2_minus_1[offset+1]);
  while(index < end) {
    value &= this->data[index] ;
    if (value != 0) goto found;
    value = -1;
    index++;
  }
  this->curpos = (-1);
  return (0);
 found:
  this->curpos = ((index << 3) | bit_pos[value]);
  return (1);
}


// next_intersection -- Position at the zero-relative integer of the next bit
//                      in the intersection of two bit sets.
// Input:               Reference to Bit Set object
// Output:              TRUE/FALSE, current position updated

Boolean CoolBit_Set::next_intersection (const CoolBit_Set& b) {
  if (this->number_elements > b.number_elements)
    this->number_elements = b.number_elements;
  //generate_next(&b.data[index],0,0)               // take out macro expansion
  
  int index, offset;
  long pos = this->curpos;
  if (pos == (-1)) {
    index = 0;
    offset = -1;
  } else {
    index = ((int) (pos >> 3));
    offset = ((int) (pos & 0x07));
  }
  int end = this->number_elements;
  register int value = (~powers_of_2_minus_1[offset+1]);
  while(index < end) {
    value &= this->data[index] &b.data[index];
    if (value != 0) goto found;
    value = -1;
    index++;
  }
  this->curpos = (-1);
  return (0);
 found:
  this->curpos = ((index << 3) | bit_pos[value]);
  return (1);
}


// next_union -- Position at the zero-relative integer of the next bit in 
//               the union of two bit sets.
// Input:        Reference to Bit Set object
// Output:       TRUE/FALSE, current position updated

Boolean CoolBit_Set::next_union (const CoolBit_Set& b) {
  //generate_next(|b.data[index],1,1)         // take out macro expansion
  
  int index, offset;
  long pos = this->curpos;
  if (pos == (-1)) {
    index = 0;
    offset = -1;
  } else {
    index = ((int) (pos >> 3));
    offset = ((int) (pos & 0x07));
  }
  int end = min(this->number_elements, b.number_elements);
  register int value = (~powers_of_2_minus_1[offset+1]);
  while(index < end) {
    value &= this->data[index] |b.data[index];
    if (value != 0) goto found;
    value = -1;
    index++;
  }
  if (index < (end = this->number_elements)) {
    while(index < end) {
      value &= this->data[index];
      if (value != 0) goto found;
      value = -1;
      index++;
    }
  } else {
    end = b.number_elements;
    while(index < end) {
      value &= b.data[index];
      if (value != 0) goto found;
      value = -1;
      index++;
    }
  }
  this->curpos = (-1);
  return (0);
 found:
  this->curpos = ((index << 3) | bit_pos[value]);
  return (1);
}

// next_difference -- Position at the zero-relative integer of the next bit in 
//                    the difference of two bit sets. That is all elements in
//                    "this" that are not in "b"
// Input:             Reference to Bit Set object
// Output:            TRUE/FALSE, current position updated

Boolean CoolBit_Set::next_difference (const CoolBit_Set& b) {
  //generate_next(&~b.data[index],1,0)      // take out macro expansion
  
  int index, offset;
  long pos = this->curpos;
  if (pos == (-1)) {
    index = 0;
    offset = -1;
  } else {
    index = ((int) (pos >> 3));
    offset = ((int) (pos & 0x07));
  }
  int end = min(this->number_elements, b.number_elements);
  register int value = (~powers_of_2_minus_1[offset+1]);
  while(index < end) {
    value &= this->data[index] &~b.data[index];
    if (value != 0) goto found;
    value = -1;
    index++;
  }
  if (index < (end = this->number_elements)) {
    while(index < end) {
      value &= this->data[index];
      if (value != 0) goto found;
      value = -1;
      index++;
    }
  } else {
  }
  this->curpos = (-1);
  return (0);
 found:
  this->curpos = ((index << 3) | bit_pos[value]);
  return (1);
}

// next_xor -- Position at the zero-relative integer of the next bit in 
//             the XOR of two bit sets.
// Input:      Reference to Bit Set object
// Output:     TRUE/FALSE, current position updated

Boolean CoolBit_Set::next_xor (const CoolBit_Set& b) {
  //generate_next(^b.data[index];,1,1)     // take out macro expansion
  
  int index, offset;
  long pos = this->curpos;
  if (pos == (-1)) {
    index = 0;
    offset = -1;
  } else {
    index = ((int) (pos >> 3));
    offset = ((int) (pos & 0x07));
  }
  int end = min(this->number_elements, b.number_elements);
  register int value = (~powers_of_2_minus_1[offset+1]);
  while(index < end) {
    value &= this->data[index] ^b.data[index];;
    if (value != 0) goto found;
    value = -1;
    index++;
  }
  if (index < (end = this->number_elements)) {
    while(index < end) {
      value &= this->data[index];
      if (value != 0) goto found;
      value = -1;
      index++;
    }
  } else {
    end = b.number_elements;
    while(index < end) {
      value &= b.data[index];
      if (value != 0) goto found;
      value = -1;
      index++;
    }
  }
  this->curpos = (-1);
  return (0);
 found:
  this->curpos = ((index << 3) | bit_pos[value]);
  return (1);
}


// prev -- Decrement the current position index
// Input:  None
// Output: Boolean TRUE/FALSE

Boolean CoolBit_Set::prev () {
  int index, value, offset;
  long pos = this->curpos;
  if (pos == INVALID) {                         // If invalid current position
    index = this->number_elements;              // Start at last byte
    offset = BS_BITSPERBYTE;                    // Start at last bit bit
  } else {
    index = BS_BYTE_NUMBER(pos);                // Get current byte index
    offset = BS_BYTE_OFFSET(pos);               // Get current bit offset
  }
  value = (this->data[index] & 
           (~powers_of_2_minus_1[offset-1]));   //Reset low bits
  while (value == 0) {                          // If no more bits set
    if (index <0){                              // If we are at start of vector
      this->curpos = INVALID;                   // Invalidate current position
      return FALSE;                             // Return failure
    }
    value = this->data[--index];                // Else get next byte value
  }
    this->curpos = BS_MAKE_POSITION(index,bit_pos[value]);
  return TRUE;                                  // Indicate success
}


// find -- Set the current position to the nth bit
// Input:  Bit position desired (really, it's an integer value)
// Output: TRUE/FALSE

Boolean CoolBit_Set::find (int n) {
#if ERROR_CHECKING
  if (BS_BYTE_NUMBER(n) >= this->size) {        // If outside allocated range
    this->find_error (n);                       // Raise exception
    this->curpos = INVALID;                     // Invalidate current position
    return FALSE;                               // Return failure
  }
#endif
    this->curpos = n;
  return(((this->data[BS_BYTE_NUMBER(n)]) >> BS_BYTE_OFFSET(n)) & 0x01);
}


// put -- Put an element to the set
// Input: Bit position desired (really, it's an integer value)
// Output: TRUE/FALSE

Boolean CoolBit_Set::put (int n) {
  int nbytes = BS_BYTE_COUNT(n+1);              // Calculate byte index
  if (nbytes >= this->number_elements) {        // If bigger than largest
    if (nbytes >= this->size)                   // If outside allocated range
      this->grow (n);                           // Grow the bit vector
    this->number_elements = nbytes;
  }
  this->data[BS_BYTE_NUMBER(n)] |= (1 << BS_BYTE_OFFSET(n)); // Set proper bit
  this->curpos = n;
  return TRUE;                                  // Return success
}


// put -- Put a range of elements to the set
// Input: Start, end bit positions (really, they're just integer values)
// Output: TRUE/FALSE

Boolean CoolBit_Set::put (int start, int end) {
  if (start > end) {                            // If start is passed the end!
#if ERROR_CHECKING
    this->put_error (start,end);                // Raise exception
#endif
    this->curpos = INVALID;                     // Invalidate current position
    return FALSE;                               // Return failure
  }
  int last = BS_BYTE_COUNT(end);
  if (last >= this->number_elements) {
    if (last >= this->size)
      this->grow (end);                         // Grow the bit vector
    this->number_elements = last;
  }
  // This could be made MUCH faster by banging a byte at a time
  for (int i = start; i <= end; i++) // For each element in range
    this->data[BS_BYTE_NUMBER(i)] |= (1 << (BS_BYTE_OFFSET(i))); // Set bit
  this->curpos = start;
  return TRUE;                                  // Return success
}


// remove -- Remove element from set at current position
// Input:    None
// Output:   TRUE/FALSE

Boolean CoolBit_Set::remove () {
#if ERROR_CHECKING
  if (this->curpos == INVALID) {                // If current position INVALID
    this->remove_error ();                      // Raise exception
    return FALSE;                               // Return failure
  }
#endif
  int mask = ~(1 << BS_BYTE_OFFSET(this->curpos)); // Make bit mask
  this->data[BS_BYTE_NUMBER(this->curpos)] &= mask; // Turn off bit
  return TRUE;
}


// remove -- Remove the specified element from the set
// Input:    Element to be removed (really just an integer value)
// Output:   TRUE/FALSE

Boolean CoolBit_Set::remove (int n) {
  if (BS_BYTE_NUMBER(n) >= this->number_elements) // If out of range
    return FALSE;                                 // Return failure
  int mask = ~(1 << BS_BYTE_OFFSET(n));           // Make bit mask
  this->data[BS_BYTE_NUMBER(n)] &= mask;          // Turn off bit
  this->curpos = n;
  return TRUE;                                  // Return success
}


// remove -- Remove range of elements from the set
// Input:    Start, end bit positions (really, they're just integer values)
// Output:   TRUE/FALSE

Boolean CoolBit_Set::remove (int start, int end) {
#if ERROR_CHECKING
  if (start > end) {
    this->rem_start_end_error (start,end);      // Raise exception
    this->curpos = INVALID;                     // Invalidate current pos
    return FALSE;                               // Return failure
  }
#endif
    if (BS_BYTE_NUMBER(start) >= this->number_elements) {
    this->curpos = INVALID;                     // Invalidate current pos
    return FALSE;                               // Return failure
  }
  if (BS_BYTE_NUMBER(end) >= this->number_elements)
    end = this->number_elements * BS_BITSPERBYTE;
  for (int i = start; i <= end; i++)            // For each element in range
    this->data[BS_BYTE_NUMBER(i)] &= ~(1 << (BS_BYTE_OFFSET(i))); // Reset bit
  this->curpos = start;
  return TRUE;                                  // Return success
}


// search -- Determine if one Bit Set is a subset of another
// Input:    Reference to a Bit Set object
// Output:   TRUE/FALSE

Boolean CoolBit_Set::search (const CoolBit_Set& b) const {
  int len = min(this->number_elements, b.number_elements);
  int i;
  for (i = 0; i < len; i++)                       // For each byte in set
    if ((this->data[i] & b.data[i]) != b.data[i]) // If not as first set
      return FALSE;                               // Then not a subset
  if (i < b.number_elements)                      // If more elements in 2nd
    for (; i < b.number_elements; i++)            // Its still subset if all
      if (b.data[i] != 0)                         // other elemenst are 0
        return FALSE;                             // Nope! Return failure
  return TRUE;                                    // Subset; return success
}


// operator- -- Overload unary minus operator to return elements not in set
// Input:       None
// Output:      Bit Set object containing complement of this set

CoolBit_SetE CoolBit_Set::operator- () {
  CoolBit_Set temp (this->number_elements * BS_BITSPERBYTE);    // New bit set
  for (int i = 0; i < this->number_elements; i++) // For each byte in vector
    temp.data[i] = ~(this->data[i]);            // Copy complement of set
  temp.curpos = INVALID;                        // Reset current position
  temp.number_elements = this->number_elements; // Update number of elements
  CoolBit_SetE& result = (CoolBit_SetE&) temp;  // same physical object
  return result;                                // shallow swap on return
}


// MACRO generate_set_operator(a, b, op, excess=0) {
//   int a_size = BS_WORD_COUNT(a->number_elements);
//   int b_size = BS_WORD_COUNT(b.number_elements);
//   unsigned int* a_data = (unsigned int*) a->data;
//   unsigned int* b_data = (unsigned int*) b.data;
//   int min_size = min(a_size, b_size);
//   for (int i = 0; i < min_size; i++)
//     a_data[i] = a_data[i] op b_data[i];              // 
//   for (; i < b_size; i++)
//     a_data[i] = excess;                              // operate on excess b's
// }


#define generate_set_operator(a, b, op, excess)                               \
  int a_size = a->number_elements;                                            \
  int b_size = b.number_elements;                                             \
  unsigned char* a_data = a->data;                                            \
  unsigned char* b_data = b.data;                                             \
  int min_size = min(a_size, b_size);                                         \
  int i;                                                                      \
  for (i = 0; i < min_size; i++)                                              \
     a_data[i] = a_data[i] op b_data[i];        /*operate on common sets*/    \
  for (; i < b_size; i++)                                                     \
     a_data[i] = excess;                        /*operate on excess b's*/


// operator|= -- Determine the union of two sets, that is all elements in each
//               set and destructively modify the first set with the result
// Input:        Reference to a bit set
// Output:       Updated Bit Set object containing union of two sets

CoolBit_Set& CoolBit_Set::operator|= (const CoolBit_Set& b) {
  if (b.number_elements > this->size)
    this->grow(b.number_elements * BS_BITSPERBYTE);
  generate_set_operator(this, b, |, b_data[i]); // Calculate the union
  if (this->number_elements < b.number_elements)
    this->number_elements = b.number_elements;
  this->curpos = INVALID;                       // Invalidate current position
  return *this;                                 // Return refenerce
}


// operator-= -- Determine the difference of two sets, that is all elements in
//               the first set that are not in the second and destructively
//               modify the first with the result
// Input:        Reference to bit set
// Output:       Updated Bit Set object containing union of two sets

CoolBit_Set& CoolBit_Set::operator-= (const CoolBit_Set& b) {
  generate_set_operator(this, b, &~, 0);        // Calculate the difference
  this->curpos = INVALID;                       // Invalid current position
  return *this;                                 // Return refenerce
}


// operator^= -- Determine the exclusive-OR of two sets, that is all elements
//               in the first set that are not in the second and all elements
//               in the second set that are not in the first and destructively 
//               modify the first with the result
// Input:        Reference to bit set
// Output:       Updated Bit Set object containing XOR of two sets

CoolBit_Set& CoolBit_Set::operator^= (const CoolBit_Set& b) {
  if (b.number_elements > this->size)
    this->grow(b.number_elements * BS_BITSPERBYTE);
  generate_set_operator(this, b, ^, b_data[i]); // Calculate the exclusive-OR
  if (this->number_elements < b.number_elements)
    this->number_elements = b.number_elements;
  this->curpos = INVALID;                       // Invalidate current position
  return *this;                                 // Return refenerce
}


// operator&= -- Determine the intersection of two sets, that is all elements 
//               that are in both sets and destructively modify the first with
//               the result
// Input:        Reference to Bit Set object
// Output:       Updated Bit Set object containing intersection of two sets

CoolBit_Set& CoolBit_Set::operator&= (const CoolBit_Set& b) {
  generate_set_operator(this, b, &, 0);         // Calculate the exclusive-OR
  if (this->number_elements > b.number_elements) {
    this->number_elements = b.number_elements;  // shorten ourself
    for (; i < a_size; i++) a_data[i] = 0;      // and Zap excess bits
  }
  this->curpos = INVALID;                       // Invalid current position
  return *this;                                 // Return refenerce
}


// clear -- Remove all elements from the set
// Input:   None
// Output:  None

void CoolBit_Set::clear () {
  for (int i = 0; i < this->number_elements; i++) 
    this->data[i] = 0;                          // Initialize bits to zero
  this->curpos = INVALID;                       // Invalidate current position
}


// grow -- resize the bit set (private method)
// Input:    Minimum size requirement
// Output:   none

void CoolBit_Set::grow (int min_size) {
  if (this->growth_ratio != 0.0 &&
      (this->size * (1.0+growth_ratio)) >= min_size)
    min_size = (int)(this->size * (1.0 + growth_ratio)); // New size
  else
    min_size += alloc_size_s;                   // Update vector size
  resize(min_size);
}


// resize -- Resize the bit set for at least some specified number of elements
// Input:    Minimum size requirement
// Output:   TRUE/FALSE

void CoolBit_Set::resize (int n) {
  int nbytes = BS_BYTE_COUNT(n);
  unsigned char* temp = new unsigned char[nbytes]; // Allocate storage
  int i;
  for (i = 0; i < this->number_elements; i++) 
    temp[i] = this->data[i];                    // copy old data
  for (; i < nbytes; i++)
    temp[i] = 0;                                // clear new data
  delete [] this->data;                         // Free old storage
  this->data = temp;                            // Point to new storage
  this->size = nbytes;                          // Save number of bytes
  this->curpos = INVALID;                       // Reset current position
}


// operator= -- Overload the assignment operator for Bit Set objects
// Input:       Reference to Bit Set object
// Output:      Reference to Bit Set object

CoolBit_Set& CoolBit_Set::operator= (const CoolBit_Set& b) {
  if (this != &b) {
    int len = b.size;                           // Get size of object to copy
    if (this->size < len) {                     // If not enough storage
      delete [] this->data;                     // Deallocate old storage
      this->data = new unsigned char[len];      // Allocate same size storage
      this->size = len;                         // Maintain number of bytes
    }
    for (int i = 0; i < len; i++)
      this->data[i] = b.data[i];                // copy all bytes
    this->number_elements = b.number_elements;
    this->curpos = INVALID;                     // Reset current position
  }
  return *this;                                 // Return reference to object
}


// operator<< -- Overload the output operator for Bit Set objects
// Input:        Reference to stream, reference to Bit Set object
// Output:       Reference to stream

ostream& operator<< (ostream& os, const CoolBit_Set& b) {
  static char ascii_rep[10];                    // Static storage for 0's/1's
  os << "[ ";                                   // Output start of set bracket
  for (int i = 0; i < b.number_elements; i++) { // For each byte in the set
    strcpy (ascii_rep, "00000000 ");            // Assume majority of zeros
    for (int j = 7; j >= 0; j--)
      if (b.data[i] & (1 << j))
        ascii_rep[j] = '1';                     // Copy "1" character to string
    os << ascii_rep;                            // Output 0's and 1's for byte
  }
  os << "]\n";                                  // Output terminating bracket
  return os;                                    // Return reference to stream
}


// operator== -- Overload the equality operator for Bit Set objects
// Input:        Reference to Bit Set object
// Output:       TRUE/FALSE

Boolean operator== (const CoolBit_Set& b1, const CoolBit_Set& b2) {
  int len = min(b1.number_elements, b2.number_elements); // common length
  int i;
  for (i = 0; i < len; i++)                            
    if (b1.data[i] != b2.data[i])                 // Check for different bits
      return FALSE;                               // in common section
  for (i = len; i < b1.number_elements; i++)      // Check for nonzero bits in
    if (b1.data[i])                               // extra section of this
      return FALSE;                               
  for (i = len; i < b2.number_elements; i++)      // Check for nonzero bits in
    if (b2.data[i])                               // extra section of b
      return FALSE;                               
  return TRUE;                                    // Return success indication
}


// length -- Return number of elements in Set
// Input:    None
// Output:   Integer representing number of bits set 

int CoolBit_Set::length () const {
  int count = 0;                                // Temporary to hold count
  int len = this->number_elements;
  for (int i = 0; i < len; i++)                 // For each byte in the vector
    count += bits_set[this->data[i]];           // Add to count number bits set
  return count;                                 // Return element count 
}


// value_error -- Raise exception for CoolBit_Set::value() method
// Input:         None
// Output:        None

void CoolBit_Set::value_error () const {
  //RAISE (Error, SYM(CoolBit_Set), SYM(Invalid_Cpos),
  printf ("CoolBit_Set::value(): Invalid current position.\n");
  abort ();
}


// bracket_error -- Raise exception for CoolBit_Set::operator[]() method
// Input:           None
// Output:          None

void CoolBit_Set::bracket_error (int n) const {
  //RAISE (Error, SYM(CoolBit_Set), SYM(Out_Of_Range),
  printf ("CoolBit_Set::operator[](): Bit number %d out of range.\n", n);
  abort ();
}


// find_error -- Raise exception for CoolBit_Set::find() method
// Input:        None
// Output:       None

void CoolBit_Set::find_error (int n) const {
  //RAISE (Error, SYM(CoolBit_Set), SYM(Out_Of_Range),
  printf ("CoolBit_Set::find(): Bit number %d out of range.\n", n);
  abort ();
}


// put_error -- Raise exception for CoolBit_Set::put() method
// Input:       Start, end bit positions
// Output:      None

void CoolBit_Set::put_error (int start, int end) const {
  //RAISE (Error, SYM(CoolBit_Set), SYM(Invalid_Start_End),
  printf ("CoolBit_Set::put(): Start bit %d greater than end bit %d.\n",
          start, end);
  abort ();
}


// remove_error -- Raise exception for CoolBit_Set::remove() method
// Input:          None
// Output:         None

void CoolBit_Set::remove_error () const {
  //RAISE (Error, SYM(CoolBit_Set), SYM(Invalid_Cpos),
  printf ("CoolBit_Set::remove(): Invalid current position.\n");
  abort ();
}


// rem_start_end_error -- Raise exception for CoolBit_Set::remove(int,int) method
// Input:                 Start, end bit positions
// Output:                None

void CoolBit_Set::rem_start_end_error (int start, int end) const {
  //RAISE (Error, SYM(CoolBit_Set), SYM(Invalid_Start_End),
  printf ("CoolBit_Set::remove(): Start bit %d greater than end bit %d.\n",
          start, end);
  abort ();
}


// void set_growth_ratio (float) -- Set the growth percentage for the Vector
//                                  object.
// Input:                           Float ratio, type
// Output:                          None

void CoolBit_Set::set_growth_ratio (float ratio) {
#if ERROR_CHECKING
  if (ratio <= 0.0) {                           // If non-positive growth
    //RAISE (Error, SYM(CoolBit_Set), SYM(Negative_Ratio),
    printf ("CoolBit_Set::set_growth_ratio(): Negative growth ratio %f.\n",
            ratio);
    abort ();
  }
#endif
  this->growth_ratio = ratio;                   // Adjust ration
}


// void set_alloc_size (int) -- Set the default allocation size growth rate.
// Input:                       Growth size in number of elements, type
// Output:                      None

void CoolBit_Set::set_alloc_size (int n) {
#if ERROR_CHECKING
  if (n < 0) {                                  // If index out of range
    //RAISE (Error, SYM(CoolBit_Set), SYM(Negative_Size),
    printf ("CoolBit_Set::set_alloc_size(): Negative growth size %d.\n", n);
    abort ();
  }
#endif
  this->alloc_size_s = n;                       // Set growth size
}
