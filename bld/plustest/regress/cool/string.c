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
// Created: MBN 06/27/89 -- Initial design and implementation
// Updated: DKM 07/07/89 -- To work around Xenix 31 char limit:
//                          Shortened is_less_than        to is_lt
//                                    is_greater_than     to is_gt
//                                    is_less_or_equal    to is_le
//                                    is_greater_or_equal to is_ge
//                          Removed is_equal_or_less and is_greater_or_less
// Updated: MBN 09/07/89 -- Added conditional exception handling
// Updated: MBN 09/26/89 -- Fixed bug to set size when growth size ratio given
// Updated: LGO 10/11/89 -- Removed operator>>
// Updated: MBN 10/24/89 -- Fixed backwards comparison in is_lt and is_gt
// Updated: LGO 10/28/89 -- Removed is_lt, is_gt, is_le, is_ge, is_equal
//                          and is_not_equal (use char* functions instead)
// Updated: LGO 01/03/90 -- Avoid use of strncat
// Updated: LGO 01/03/90 -- Re-write trim, right_trim, left_trim, upcase,
//                          downcase, capitalize, insert, replace, yank,
//                          sub_string, and strncpy to reduce heap allocation
// Updated: LGO 01/04/90 -- Delay memory allocation (see Empty_String_g)
// Updated: MJF 03/12/90 -- Added group names to RAISE
// Updated: DLS 03/22/91 -- New lite version
// Updated: JAM 08/11/92 -- removed DOS specifics, stdized #includes
// Updated: JAM 08/11/92 -- added static data member def
//
// This file contains  member and  friend function implementation code  for the
// CoolString  class defined in the String.h   header file.   Where appropriate and
// possible,  interfaces to,  and us  of,   existing  system functions has been
// incorporated. An overview of the structure of the CoolString class, along with a
// synopsis of each member  and friend function, can be  found  in the String.h
// header file.
//

#ifndef STRINGH                                 // If class not been defined
#include <cool/String.h>                        // Include class header file
#endif

#include <ctype.h>              // Include character processing macros


char* Empty_String_g = "";              // Used to allocate empty CoolStrings
                                        // this allows operator char* to work

// update_memory -- Private method to perform common memory check/adjustment
// Input:           CoolString reference, pointer to initial CoolString value
// Output:          None

void update_memory (CoolString& s) {
  long size = s.size;
  if (s.growth_ratio != 0.0) {          // If a growth ratio is set
    size = long(size * (1.0 + s.growth_ratio)); // Get new size
    if (size < s.length)                // But if this is still not big enough
      size = s.length;                  // grow to big enough size
  }
  else if ((s.length - size) < s.alloc_size_s) // If small growth factor
    size += s.alloc_size_s;             // Increment by allocation size
  else                                  // Else adding lots of characters
    size = s.length;                    // so grow to bigger size
  s.resize(size);                       // Grow the String
}


// CoolString() -- Simple constructor for a string object. Memory is allocated
//             for the char* pointer of size MEM_BLK_SIZE
// Input:      None
// Output:     CoolString reference


CoolString::CoolString () {             
  if (this->alloc_size_s == 0)          // If allocation size is uninitialized
    this->alloc_size_s = MEM_BLK_SZ;    // Set memory block allocation size
  this->growth_ratio = 0.0;             // Initialize growth ratio
  this->length = 0;                     // Zero characters in string
  this->size = 0;                       // Size of allocated block 
  this->str = Empty_String_g;           // No memory allocated
}


// CoolString(char) -- Constructor to initialize a string object with a char
// Input:          character
// Output:         CoolString reference to new object

CoolString::CoolString (char c) {       
  if (this->alloc_size_s == 0)          // If allocation size is uninitialized
    this->alloc_size_s = MEM_BLK_SZ;    // Set memory block allocation size
  this->growth_ratio = 0.0;             // Initialize growth ratio
  this->length = 1;                     // Length of character is 1
  this->size = this->length+1;          // Size of allocated block
  this->str = new char[size];           // Allocate memory for string
  *this->str = c;                       // Assign character
  *(this->str+1) = END_OF_STRING;       // END_OF_STRING terminator for string
}


// CoolString(char*) -- Constructor to initialize a string object with a char*
// Input:           char* pointer
// Output:          CoolString reference to new object

CoolString::CoolString (const char* c) { 
  if (this->alloc_size_s == 0)           // If allocation size is uninitialized
    this->alloc_size_s = MEM_BLK_SZ;     // Set memory block allocation size
  this->growth_ratio = 0.0;              // Initialize growth ratio
  this->length = strlen (c);             // Determine length of string
  this->size = this->length+1;           // Size of allocated block
  this->str = new char[size];            // Allocate memory for string
  strcpy ((char *)this->str, c);         // Copy characters 
}


// CoolString(CoolString&) -- Constructor to initialize a string object to
//                    that of another String.
// Input:             CoolString reference
// Output:            CoolString reference to new object

CoolString::CoolString (const CoolString& s) { 
  if (this->alloc_size_s == 0)          // If allocation size is uninitialized
    this->alloc_size_s = MEM_BLK_SZ;    // Set memory block allocation size
  this->growth_ratio = s.growth_ratio;  // Initialize growth ratio
  this->length = s.length;              // length of character string
  if (s.length > 0) {
    this->str = new char[s.size];       // Allocate memory for string
    strcpy ((char *)this->str, s.str);  // Copy characters
    this->size = s.size;                // Size of allocated block
  }
  else {
    this->str = Empty_String_g;         // Don't allocate when empty
    this->size = 0;
  }
}


// CoolString(char*, size) -- Constructor to initialize a string object with
//                        a char* and provide an initial size value
// Input:                 char* pointer and size value
// Output:                CoolString reference to new object

CoolString::CoolString (const char* c, long sz) {
  if (this->alloc_size_s == 0)          // If allocation size is uninitialized
    this->alloc_size_s = MEM_BLK_SZ;    // Set memory block allocation size
  this->length = strlen (c);            // Determine length of character string
  if (this->length > sz)                // If initial string is larger
    this->size = length+1;              // Size of allocated block
  else
    this->size = sz+1;                  // Size of allocated block
  this->str = new char[size];           // Allocate memory for string 
  strcpy ((char *)this->str, c);        // Copy characters
}


// CoolString(CoolString&, long) -- Constructor to initialize a string
//                          object with a String and provide
//                          an initial block size value
// Input:                   CoolString reference and size value
// Output:                  CoolString reference to new object

CoolString::CoolString (const CoolString& s, long sz) { 
  if (this->alloc_size_s == 0)          // If allocation size is uninitialized
    this->alloc_size_s = MEM_BLK_SZ;    // Set memory block allocation size
  this->length = s.length;              // Determine length of character string
  if (this->length > sz)                // If initial string is larger
    this->size = length+1;              // Size of allocated block
  else
    this->size = sz+1;                  // Size of allocated block
  this->str = new char[size];           // Allocate memory for string
  strcpy ((char *)this->str, s.str);    // Copy characters over
}


// ~CoolString() -- CoolString object destructor
// Input:       CoolString object
// Output:      None

CoolString::~CoolString () {                    // CoolString object destructor
  if (this->str != Empty_String_g)
    delete this->str;                   // Deallocate string memory
}


// clear -- Flush the character string from the string object by setting
//          the char* pointer to NULL and the length to zero.
// Input:   this* 
// Output:  None

void CoolString::clear() {
  *(this->str) = NULL;                  // Clear this CoolString by using NULL
  this->length = 0;                     // Set new string length
}


// strcpy -- CoolString copy of a single character to a CoolString object
// Input:    CoolString reference and a character
// Output:   CoolString object containing a character string

CoolString& strcpy (CoolString& s, char c) {
  if (s.size < 2) {                     // Allocate memory block
    if (s.str != Empty_String_g)        // When memory allocated
      delete s.str;                     // Delete block
    s.size = s.alloc_size_s;            // Size of allocated block
    s.str = new char[s.size];           // Allocate memory for string
  }
  s.length = 1;                         // Character string length
  *(s.str) = c;                         // Assign character
  *(s.str+1) = END_OF_STRING;           // END_OF_STRING terminator for string
  return s;                             // Return CoolString reference
}  


// strcpy -- CoolString copy of a char* to a CoolString object
// Input:    CoolString reference and a char* 
// Output:   CoolString object containing a character string

CoolString& strcpy (CoolString& s, const char *c) {
  s.length = strlen (c);                // Determine length of character string
  if (s.size <= s.length)               // If not enough memory allocated
    update_memory (s);                  // Adjust/update memory if necessary
  strcpy (s.str, c);                    // Else just copy new string value
  return s;                             // Return CoolString reference
}


// strcpy -- CoolString copy of one CoolString object to another. 
// Input:    Two CoolString references
// Output:   Updated CoolString object 

CoolString& strcpy (CoolString& s1, const CoolString& s2) {
  s1.length = s2.length;                // Determine length of character string
  if (s1.size > s2.length &&            // If string fits in allocated memory
      s1.str != Empty_String_g)         // and s1 has memory allocated
    strcpy (s1.str, s2.str);            // Copy string value
  else {
    if (s1.str != Empty_String_g)       // When memory allocated
      delete s1.str;                    // Delete block
    if (s2.length == 0) {
      s1.str = Empty_String_g;          // Don't allocate when empty
      s1.size = 0;
    }
    else {
      s1.size = s2.length+1;            // Set new block size
      s1.str = new char[s1.size];       // Allocate new block of memory
      strcpy (s1.str, s2.str);          // Copy string value
    }
  }
  return s1;                            // Return string reference
}

// strcat -- Concatenate a single character to a CoolString object
// Input:    CoolString reference and a character
// Output:   CoolString object concatenated with character 

CoolString& strcat (CoolString& s, char c) {
  s.length += 1;                        // Determine length of new string
  if (s.size <= s.length)               // If not enough allocated memory
    update_memory (s);                  // Adjust/update memory if necessary
  s.str[s.length-1] = c;                // Append new character
  s.str[s.length] = END_OF_STRING;      // END_OF_STRING terminator
  return s;                             // Return CoolString
}


// strcat -- Concatenate a CoolString and a char*
// Input:    CoolString reference and a char*
// Output    CoolString object concatentated with character string

CoolString& strcat (CoolString& s, const char* c) {
  long start_length = s.length;         // Find initial string length
  s.length += strlen (c);               // Determine length of new string
  if (s.size <= s.length)               // If not enough allocated memory
    update_memory (s);                  // Adjust/update memory if necessary
  strcpy (s.str+start_length, c);       // Concatenate characters   
  return s;                             // Return CoolString
}


// strncat -- Concatentate a CoolString with "n" characters from char*
// Input:     CoolString reference, char*, number of characters
// Output:    CoolString object concatenedate with "n" characters

CoolString& strncat (CoolString& s, const char* c, int n) {
#if ERROR_CHECKING
  if (n < 0) {                          // If invalid length specified
    printf ("CoolString::strncat(): Negative length %d.\n", n);
    exit (1);
  }
#endif
  long start_length = s.length;         // Find initial string length
  s.length += n;                        // Determine length of new string
  if (s.size <= s.length)               // If not enough allocated memory
    update_memory (s);                  // Adjust/update memory if necessary
  strncpy (s.str+start_length, c, size_t(n));   // Concatenate characters 
  return s;                             // Return CoolString
}


// strcat -- Concatenate two CoolString objects
// Input:    Two CoolString references
// Output:   CoolString object concatenated with CoolString object

CoolString& strcat (CoolString& s1, const CoolString& s2) {
  long start_length = s1.length;
  s1.length += s2.length;               // Determine length of new string
  if (s1.size <= s1.length)             // If not enough allocated memory
    update_memory (s1);                 // Adjust/update memory if necessary
  strcpy (s1.str+start_length, s2.str); // Concat characters   
  return s1;                            // Return CoolString
}


// strncat -- Concatentate a CoolString with "n" characters from a CoolString
// Input:     Two CoolString references and number of characters
// Output:    CoolString object concatenedate with "n" characters

CoolString& strncat (CoolString& s1, const CoolString& s2, int n) {
#if ERROR_CHECKING
  if (n < 0) {                          // If invalid length specified
    printf ("CoolString::strncat(): Negative length %d.\n", n);
    exit (1);
  }
#endif
  long start_length = s1.length;        // Find initial string length
  s1.length += n;                       // Determine length of new string
  if (s1.size <= s1.length)             // If not enough allocated memory
    update_memory (s1);                 // Adjust/update memory if necessary
  strncpy (s1.str+start_length, s2.str, size_t(n)); // Concat chars 
  return s1;                            // Return CoolString
}


// reverse -- Reverse the order of the characters in CoolString object
// Input:     CoolString object
// Output:    CoolString object with character order reverse

void CoolString::reverse () {
  char c;                                       // Temporary variable
  for (long i = 0, j = this->length-1;          // Counting from front and rear
       i < this->length / 2; i++, j--) {        // until we reach the middle
    c = this->str[i];                           // Save front character
    this->str[i] = this->str[j];                // Switch with rear character
    this->str[j] = c;                           // Copy new rear character
  }
}


// resize -- Adjust the memory size of a string to accomodate some size
// Input:    CoolString object
// Output:   None

void CoolString::resize (long sz) {
#if ERROR_CHECKING
  if (sz < 0) {                         // If invalid size
    printf ("CoolString::resize(): Negative resize %d.\n", sz);
    exit (1);
  }
#endif
  char* temp = this->str;               // Save pointer to existing string
  this->size = sz+1;                    // Save size of allocated memory
  this->str = new char[sz+1];           // Allocate memory for desired size
  strcpy ((char *)this->str, temp);     // Copy original string back
  if (temp != Empty_String_g)
    delete temp;                        // Deallocate old memory
}


// operator<< -- Overload output operator for CoolString objects
// Input:        CoolString reference
// Output:       Formatted output and stream descriptor

ostream& operator<< (ostream& os, const CoolString& s) {
  return os << s.str;                           // Output char* and newline
}


// operator<< -- Overload output operator for string objects
// Input:        CoolString pointer
// Output:       Formatted output and stream descriptor

ostream& operator<< (ostream& os, const CoolString* s) {
  return operator<< (os, *s);
}


// strtol -- Returns as a long integer the value represented by the
//           CoolString pointer to by s, scanning upto the first character
//           that is inconsistent with the base. Leading white space is
//           ignored.
// Input:    Reference to CoolString object
//           Radix of number
// Output:   Long integer representing value contained in the CoolString

long strtol (const CoolString& s, char** ptr, int radix) {
  return (strtol ((char *)s.str, ptr, radix)); 
}       


// atol -- Equivalent to: strtol (str, (char**)END_OF_STRING, 10)
// Input:  Reference to CoolString object
// Output: Long integer representing value contained in the CoolString

long atol (const CoolString& s) {               // Convert string to long
  return (strtol ((char *)s.str, (char **) END_OF_STRING, int(10)));
}


// atoi -- Equivalent to: (int)strtol (str, (char**)END_OF_STRING, 10)
// Input:  Reference to CoolString object
// Output: Integer representing value contained in the CoolString

int atoi (const CoolString& s) {                // Convert string to int
  return int(strtol ((char *)s.str, (char **) END_OF_STRING, int(10))); 
}


// strtod -- Returns as a double-precision floating-point number the
//           value represented by a CoolString object. Characters are
//           scanned upto the first unrecognized character.
// Input:    Reference to CoolString object
// Output:   Double representing value contained in CoolString

double strtod (const CoolString& s, char** ptr) {       // Convert string to double
  return (strtod ((char *)s.str, ptr));
}


// trim -- Removes any occurrence of the character(s) in "c" from "s"
// Input:  Simople_CoolString reference, character string
// Output: Modified CoolString "s" object

CoolString& trim (CoolString& sr, const char* rem) {
  char* s = sr.str;
  char* result = sr.str;
  long len = 0;
  register char c;
  while ((c=*s++) != END_OF_STRING) {
    register const char* r = rem;
    register char t;
    while ((t=*r++) != END_OF_STRING && t != c); // Scan for match
    if (t == END_OF_STRING)                      // If no match found
      *result++ = c, len++;
  }
  *result = END_OF_STRING;                      // NULL terminate string
  sr.length = len;
  return sr;                                    // Return string
}


// left_trim -- Removes any occurrence of the character(s) in "c" from
//              "s" that appear as a prefix to the string. The first
//              non-matching character encountered terminates the remove
//              operation and the rest of the string is copied intact.
// Input:       CoolString reference, character string
// Output:      Modified CoolString "s" object

CoolString& left_trim (CoolString& sr, const char* rem) { // Trim prefix from CoolString
  char* result = sr.str;
  char* s = sr.str;
  long len = 0;
  register char c;
  for (; (c=*s) != END_OF_STRING; s++) {
    register const char* r = rem;
    register char t;
    while ((t=*r++) != END_OF_STRING && t != c); // Scan for match
    if (t == END_OF_STRING)                      // If no match found
      break;
  }
  if (s != result)                                // when characters trimed
    while ((*result++ = *s++) != END_OF_STRING) len++; // shift string down
  sr.length = len;
  return sr;                            // Return string reference
}


// right_trim -- Removes any occurrence of the character(s) in "c" from
//               "s" that appear as a suffix to the string. The first
//               non-matching character encountered terminates the remove
//               operation and the rest of the string is copied intact.
// Input:        CoolString reference, character string
// Output:       Modified CoolString "s" object

CoolString& right_trim (CoolString& sr, const char* rem) {
  char* str = sr.str;
  char* s = str + strlen(str) - 1;              // last character of string
  for (; s >= str; s--) {
    register const char* r = rem;
    register char t;
    register char c = *s;
    while ((t=*r++) != END_OF_STRING && t != c); // Scan for match
    if (t == END_OF_STRING)                      // If no match found
      break;
  }
  *(s+1) = END_OF_STRING;
  sr.length = s - str;
  return sr;                            // Return CoolString reference
}


// upcase -- Convert all alphabetical characters to uppercase
// Input:    CoolString reference
// Output:   Updated string

CoolString& upcase (CoolString& s) {    // Convert entire string to upper case
  c_upcase(s.str);
  return s;                     // Return reference to modified string
}


// downcase -- Convert all alphabetical characters to lowercase
// Input:      CoolString reference
// Output:     Updated string

CoolString& downcase (CoolString& s) {  // Convert entire string to lower case
  c_downcase(s.str);
  return s;                     // Return reference to modified string
}

// capitalize -- Capitalize all words in a CoolString. A word is define as
//               a sequence of characters separated by non alphanumerics
// Input:        CoolString reference
// Output:       Updated string

CoolString& capitalize (CoolString& s) {        // Capitalize each word in string
  c_capitalize(s.str);
  return s;                     // Return reference to modified string
}


// insert -- Insert a char* at the position specified by start.
// Input  -- A char* to be inserted and a long index.          
// Output -- A Boolean, true if insertion took place, false if error.

Boolean CoolString::insert (const char* ins, long start) {
  if (start<0 || start>this->length) return(FALSE);  // Boundary check
  long len=strlen(ins);                 // length of char*
  this->length += len;                  // Determine length of new string
  if (this->size <= this->length)       // If not enough allocated memory
    update_memory (*this);              // Adjust/update memory if necessary
  register char* ptr = this->str;
  register char* st = ptr + start;
  register char* end = ptr + this->length;
  for (ptr = end - len; ptr >= st;)             // Make space for ins
    *end-- = *ptr--;
  for (end = st+len; st < end; *st++ = *ins++); // Insert ins into that
  return(TRUE);                         // Insert worked
}


// remove -- Removes everything between the start and end indexes of string
//           The character at start is removed and all characters up to but 
//           not including the character at end.
// Input  -- A start and end index into the string.
// Output -- A Boolean, true if remove worked, false if error.

Boolean CoolString::remove (long start, long end) {
  if (start<0 || start > this->length || end<0  // Boundary check
      || end > this->length || end<=start)
    return(FALSE);                              // Out of bounds, failure
  this->length -= (end-start);                  // New (shorter) length
  for(; this->str[start]=this->str[end]; start++) end++; // Remove everything
  return(TRUE);                                 // Remove worked
}


// replace -- Removes everything between start and end as in remove(), and
//            inserts the char* argument into the place of what was removed.
// Input   -- A start and end for remove and a char*, c, to insert.
// Output  -- A Boolean, true if replace worked, false if error.

Boolean CoolString::replace (const char* c, long start, long end) {
  if (start<0 || start > this->length || end<0  // Boundary check
      || end > this->length || end<=start)      // out of bounds, failure
    return(FALSE); 
  long len=strlen(c);                   // length of c
  long delta = len - end + start;       // find overall change in length
  this->length += delta;                // set new length
  if (this->size <= this->length)       // If not enough allocated memory
    update_memory (*this);              // Adjust/update memory if necessary
  long ind;
  if (delta > 0) {                                // If replacement is bigger
    for (ind=this->length; start+len<=ind; ind--) // than chacters replaced
      this->str[ind] = this->str[ind-delta];
  }
  else if (delta < 0) {                 // Replacement is shorter
    char* st = this->str + start + len;
    char* ptr = this->str + end;
    char* end = this->str + this->length;
    while (st<=end) *st++ = *ptr++;
  }
  { char* ptr = this->str+start;
    char ch;
    while ((ch = *c++) != END_OF_STRING) // replace characters
      *ptr++ = ch;
  }
  return(TRUE);
}


// yank   -- Removes everything between start and end, copies it into a 
//           a string and returns that string.
// Input  -- A reference to a CoolString.  A start and end for the remove.
// Output -- none.  Sets the CoolString.

void CoolString::yank (CoolString& s, long start, long end) {
#if ERROR_CHECKING
  if (start < 0 || start > this->length || end < 0      // Boundary check
      || end > this->length || end <= start) {
    printf ("CoolString::sub_yank(): Start %d and/or end %d index invalid.\n",
            start, end);
    exit (1);
  }
#endif
  long len = end - start;
  strncpy(s, this->str+start, len);     // Copy stuff to yank into s
  this->length -= len;                  // set new length
  for(; this->str[start]=this->str[end]; start++) end++;  // Do the remove
}


// sub_string -- Returns a new string initialized to what is between the
//               start and end indexes.
// Input      -- A reference to a String.  Two longs, a start and an end
// Output     -- none.  Sets the CoolString.

void CoolString::sub_string (CoolString& s, long start, long end) {
#if ERROR_CHECKING
  if (start < 0 || start > this->length || end < 0      // Boundary check
      || end > this->length || end <= start) {
    printf ("CoolString::sub_string(): Start %d and/or end %d index invalid.\n",
            start, end);
    exit (1);
  }
#endif
  strncpy(s, this->str+start, end - start);     // Copy substring into s
}


// strncpy -- Returns s, with the first length characters of source copied
//            into it.  The old value of s is lost.
// Input   -- A reference to a CoolString s, a char* source, and a long length.
// Output  -- The modified CoolString s.

CoolString& strncpy(CoolString& s, const char* source, long length) {
#if ERROR_CHECKING
  if (length < 0) {                     // Boundary check
    printf ("CoolString::strncpy(): Negative length %d.\n", length);
    exit (1);
  }
#endif
  s.length = length;                    // Set new string length
  if (s.size <= length) {               // If not enough allocated memory
    if (s.str && s.str != Empty_String_g)
        *s.str = END_OF_STRING;         // Don't copy old string
    update_memory (s);                  // Adjust/update memory if necessary
  }
  char* p = s.str;
  while (length-- > 0)                  // Copy source into string
    *p++ = *source++;
  *p = END_OF_STRING;                   // set the end byte
  return s;
}


// bracket_error -- Raise exception for operator[]
// Input:           Index value
// Output:          None

void CoolString::bracket_error (long n) {
  printf ("CoolString::operator[](): Index %d out of range.\n", n);
  exit (1);
}


// growth_error -- Raise exception for set_alloc_size()
// Input:          Growth value
// Output:         None

void CoolString::growth_error (int i) {
  printf ("CoolString::set_alloc_size(): Negative growth size %d.\n", i);
  exit (1);
}

// ratio_error -- Raise exception for set_growth_ratio()
// Input:         Growth ration
// Output:        None

void CoolString::ratio_error (float ratio) {
  printf ("CoolString::set_growth_ratio(): Negative growth ratio %f.\n", ratio);
  exit (1);
}

int CoolString::alloc_size_s = 0;
