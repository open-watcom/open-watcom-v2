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
// Created: MBN 03/17/89 -- Initial design and implementation
// Updated: MBN 04/06/89 -- Performance enhancement by changing several
//                          methods and friends to inline, and addition
//                          of two private friend functions.
// Updated: LGO 06/03/89 -- Fix string growth algorithm.
// Updated: MBN 06/06/89 -- Performance enhancement by making reference count
//                          and memory check/adjustment inline, addition of a
//                          resize method and a growth ratio data slot, and
//                          change of constructors to set specific object size,
//                          not class as a whole
// Updated: DKM 06/23/89 -- To get around a bug in the Glockenspiel translator
//                          we had to explicitly typecast arguments for calls
//                          to the overloaded unix string functions, including
//                          strchr, strrchr, strcpy, strcmp, strncmp, strtod,
//                          and strtol.
// Updated: MNF 06/27/89 -- Added the insert, remove, replace, yank, and
//                          sub_string public member functions.
// Updated: MBN 06/30/89 -- Updated to include support for regular expressions
// Updated: DKM 07/07/89 -- To work around Xenix 31 char limit:
//                          Shortened is_less_than        to is_lt
//                                    is_greater_than     to is_gt
//                                    is_less_or_equal    to is_le
//                                    is_greater_or_equal to is_ge
//                          Removed is_equal_or_less and is_greater_or_less
// Updated: MBN 09/07/89 -- Added conditional exception handling
// Updated: MBN 09/26/89 -- Fixed bug to set size when growth size ratio given
// Updated: LGO 10/11/89 -- Removed operator>>
// Updated: LGO 10/28/89 -- Removed is_lt, is_gt, is_le, is_ge, is_equal
//                          and is_not_equal (use char* functions instead)
// Updated: MBN 12/15/89 -- Sprinkled "const" qualifier all over the place!
// Updated: MBN 01/02/90 -- Made find search for the first/next regexp match 
// Updated: LGO 01/03/90 -- Avoid use of strncat
// Updated: LGO 01/05/90 -- Partial re-write of most everything to simplify
//                          and speed-up the code
// Updated: MJF 03/12/90 -- Added group names to RAISE
// Updated: MJF 05/21/90 -- Moved delete from grow_memory to validate
// Updated: DLS 03/22/91 -- New lite version
// Updated: JAM 08/14/92 -- removed DOS specifics, stdized #includes
// Updated: JAM 08/14/92 -- added static alloc_size_s def and initialization
//
// This  file contains member and  friend function  implementation code for the
// CoolGen_String class defined  in  the  Gen_String.h and Regexp.h header   files.
// Where appropriate and  possible, interfaces to, and  us  of, existing system
// functions   has been incorporated.  An overview    of the structure   of the
// Edit_String class, along with a synopsis of each member and friend function,
// can be found in the String.h header file.
//

#ifndef GEN_STRINGH                             // If class not defined
#include <cool/Gen_String.h>            // Include class specification
#endif

#include <ctype.h>              // Include character processing macros

// alloc_size_s -- Memory allocation growth size
int CoolGen_String::alloc_size_s = MEM_BLK_SZ;  // Init allocation size

// update_ref_count -- Gives "this" it's own (unshared) copy of a string
// Input:          None
// Output:         None

void CoolGen_String::update_ref_count() {
  this->p->ref_count--;              // Decrement reference count
  this->p = new String_Layout;       // Allocate layout structure
  this->p->ref_count = 1;            // One object at the moment
  this->p->size = 0;                 // No memory allocated yet
  this->p->start = this->str;        // Pointer to start of string
}


// validate -- protected method called when a string is modified
// Input:        before calling, set this->length to new length.
//               dont_copy - set to TRUE If you don't want the old string
//               copied on a grow, or ref-count copy
// Output:       None
void CoolGen_String::validate(Boolean dont_copy) {
  if (this->p->ref_count > 1) {   // If sharing memory with another CoolGen_String
    this->update_ref_count();     // decrement reference count and 
    this->grow_memory(dont_copy); // allocate memory for own copy
  }
  else if (this->length >= this->p->size) { // If not long enough,
    char* ostr = this->str;                 // save old string to delete later
    this->grow_memory(dont_copy);           // allocate memory,
    if (ostr != NULL) delete ostr;          // and free old memory block
  }
}

// grow_memory -- Called when string isn't long enough, or to allocate memory
//                after update_ref_count inside validate.
// Input:         dont_copy - TRUE If you don't want the old string copied.
// Output:        None

void CoolGen_String::grow_memory (Boolean dont_copy) {
  char* old_chars = this->str;
  long size = this->p->size;
  if (this->length >= size) { // Called from validate just to allocate memory
    if (this->growth_ratio != 0.0)     // If a growth ratio is set
      size += long(size * this->growth_ratio);
    else
      size += this->alloc_size_s;    // Increment by allocation size
    if (size < this->length)         // But if still not big enough
      size = this->length+1;         // Grow to big enough size
    this->p->size = size;
  }
  this->str = new char[size];        // Allocate memory for string
  this->p->start = this->str;        // Pointer to start of string
  if (old_chars != NULL) {
    if (!dont_copy)
      strcpy ((char *)this->str, old_chars);   // Copy initial string
  }
  else
    *this->str = END_OF_STRING;
}


// CoolGen_String() -- Simple constructor for a CoolGen_String object. Memory is
//                 allocated for the layout structure and char* pointers are
//                 set to the "alloc_size_s" value.
// Input:          None
// Output:         CoolGen_String reference


CoolGen_String::CoolGen_String () {             
  this->growth_ratio = 0.0;             // Initialize growth ratio
  this->str = new char[alloc_size_s];   // Allocate block of memory
  this->length = 0;                     // Zero characters in string
  this->p = new String_Layout;          // Allocate layout structure
  this->p->ref_count = 1;               // One String object at the moment
  this->p->size = alloc_size_s;         // Size of allocated block 
  this->p->start = str;                 // Pointer to start of string 
  *this->str = END_OF_STRING;           // END_OF_STRING terminate for safety
  this->rgexp = NULL;                   // No regular expression yet
}


// CoolGen_String(char) -- Constructor to initialize object with a char
// Input:              character
// Output:             CoolGen_String reference to new object

CoolGen_String::CoolGen_String (char c) {               
  this->growth_ratio = 0.0;             // Initialize growth ratio
  this->length = 1;                     // Length of character is 1
  this->str = new char[length+1];       // Allocate memory for string
  this->p = new String_Layout;          // Allocate layout structure
  this->p->ref_count = 1;               // One String object at the moment
  this->p->size = this->length+1;       // Size of allocated block
  this->p->start = this->str;           // Pointer to start of string
  *this->str = c;                       // Assign character
  *(this->str+1) = END_OF_STRING;       // END_OF_STRING terminator for string
  this->rgexp = NULL;                   // No regular expression yet
}


// CoolGen_String(char*) -- Constructor to initialize object with a char*.
// Input:               char* pointer
// Output:              CoolGen_String reference to new object

CoolGen_String::CoolGen_String (const char* c) {                
  this->growth_ratio = 0.0;             // Initialize growth ratio
  this->length = strlen (c);            // Determine length of character string
  this->str = new char[length+1];       // Allocate memory for string
  this->p = new String_Layout;          // Allocate layout structure
  this->p->ref_count = 1;               // One String object at the moment
  this->p->size = this->length+1;       // Size of allocated block
  this->p->start = this->str;           // Pointer to start of string
  strcpy ((char *)this->str, c);        // Copy characters
  this->rgexp = NULL;                   // No regular expression yet
}


// CoolGen_String(CoolGen_String&) -- Constructor to initialize object to CoolGen_String
//                            Note that this involves use of delayed copy via 
//                            the reference count mechanism.
// Input:                     CoolGen_String reference
// Output:                    CoolGen_String reference to new object

CoolGen_String::CoolGen_String (const CoolGen_String& s) {
  this->growth_ratio = s.growth_ratio;  // Initialize growth ratio
  this->length = s.length;              // Determine length of character string
  this->str = s.str;                    // Point to same string memory block
  this->p = s.p;                        // Point to same layout structure
  this->p->ref_count++;                 // Increment reference count
  this->rgexp = NULL;                   // No regular expression yet
}


// CoolGen_String(char*, size) -- Constructor to initialize object with a char*
//                            and control initial allocation size
// Input:                     char* pointer and size value
// Output:                    CoolGen_String reference to new object

CoolGen_String::CoolGen_String (const char* c, long sz) {
  this->length = strlen (c);            // Determine length of character string
  this->p = new String_Layout;          // Allocate layout structure
  if (this->length > sz) {              // If initial string is larger
    this->str = new char[length+1];     // Allocate memory for string
    this->p->size = length+1;           // Size of allocated block
  }
  else {
    this->str = new char[sz+1];         // Allocate memory for string 
    this->p->size = sz+1;               // Size of allocated block
  }
  this->p->ref_count = 1;               // One String object at the moment
  this->p->start = this->str;           // Pointer to start of string
  strcpy ((char *)this->str, c);        // Copy characters
  this->rgexp = NULL;                   // No regular expression yet
}


// CoolGen_String(CoolGen_String&, long) -- Constructor to initialize object to a
//                                  CoolGen_String and control initial alloc size
// Input:                           CoolGen_String reference and size value
// Output:                          CoolGen_String reference to new object

CoolGen_String::CoolGen_String (const CoolGen_String& s, long sz) { 
  this->length = s.length;              // Determine length of character string
  this->p = new String_Layout;          // Allocate layout structure
  if (this->length > sz) {              // If initial string is larger
    this->str = new char[length+1];     // Allocate memory for string
    this->p->size = length+1;           // Size of allocated block
  }
  else {
    this->str = new char[sz+1];         // Allocate memory for string 
    this->p->size = sz+1;               // Size of allocated block
  }
  this->p->ref_count = 1;               // One String object at the moment
  this->p->start = this->str;           // Pointer to start of string
  strcpy ((char *)this->str, s.str);    // Copy characters over
  this->rgexp = NULL;                   // No regular expression yet
}


// ~CoolGen_String() -- CoolGen_String object destructor frees up allocated memory and
//                  decrements reference count if necessary
// Input:           CoolGen_String object
// Output:          None

CoolGen_String::~CoolGen_String () {                    
  if (this->p->ref_count == 1) {      // If not sharing mem with other String
    delete this->str;                 // then delete String object
    delete this->p;                   // Delete layout structure
  } else 
    this->p->ref_count--;               // Else decrement reference counter
  delete this->rgexp;                   // Delete regular expression object
}


// clear -- Flush the character string from the string object by setting
//          the char* pointer to NULL and the length to zero.
// Input:   this*
    // Output:  None

void CoolGen_String::clear() {
  this->length = 0;                     // Set new string length
  if (this->rgexp)                      // If there is a Regexp object
    this->rgexp->set_invalid();         // Invalidate compiled expression
  if (this->p->ref_count == 1)          // If not sharing mem with other String
    *(this->str) = NULL;                // Clear this String by using NULL
  else {
    this->str = NULL;                   // Invalidate pointer
    this->validate();                   // Check for sharing memory and reset
  }
}


// strcpy -- String copy of a single character to an CoolGen_String object
// Input:    CoolGen_String reference and a character
// Output:   CoolGen_String object containing a character string

CoolGen_String& strcpy (CoolGen_String& s, char c) {
  s.length = 1;                         // Character string length
  s.validate(TRUE);                     // Ensure enough memory and not shared
  *(s.str) = c;                         // Assign character
  *(s.str+1) = END_OF_STRING;           // END_OF_STRING terminator for string
  return s;                             // Return CoolGen_String reference
}  


// strcpy -- String copy of a char* to a String object
// Input:    CoolGen_String reference and a char* 
// Output:   String object containing a character string

CoolGen_String& strcpy (CoolGen_String& s, const char *c) {
  s.length = strlen (c);                // Determine length of character string
  s.validate(TRUE);                     // Ensure enough memory and not shared
  strcpy(s.str, c);                     // Copy into string
  return s;                             // Return CoolGen_String reference
}


// strcpy -- String copy of one String object to another. Note that the
//           copy is delayed via the reference count mechanism until it
//           is really needed.
// Input:    Two CoolGen_String references
// Output:   CoolGen_String object pointing to another CoolGen_String object

CoolGen_String& strcpy (CoolGen_String& s1, const CoolGen_String& s2) {
  if (s1.p->ref_count > 1)              // If sharing memory with other string
    s1.p->ref_count--;                  // Decrement that reference count
  else {                                // Else must be some memory to free
    delete s1.p;                        // Free layout structure memory
    if (s1.str != END_OF_STRING)        // If pointing to a memory block
      delete s1.str;                    // Free that up too
  }
  s1.length = s2.length;                // Determine length of character string
  s1.str = s2.str;                      // Point to same string memory block
  s1.p = s2.p;                          // Point to same layout structure
  s1.p->ref_count++;                    // Increment reference count
  return s1;                            // Return CoolGen_String reference
}


// strcat -- Concatenate a single character to a CoolGen_String object
// Input:    CoolGen_String reference and a character
// Output:   CoolGen_String object concatenated with character 

CoolGen_String& strcat (CoolGen_String& s, char c) {
  s.length += 1;                        // Determine length of new string
  s.validate();                         // Ensure enough memory and not shared
  s.str[s.length-1] = c;                // Append new character
  s.str[s.length] = END_OF_STRING;      // END_OF_STRING terminator
  return s;                             // Return String
}


// strcat -- Concatenate a String and a char*
// Input:    CoolGen_String reference and a char*
// Output    CoolGen_String object concatentated with character string

CoolGen_String& strcat (CoolGen_String& s, const char* c) {     
  long old_len = s.length;              // Save length for efficiency hack
  s.length = old_len + strlen (c);      // Determine length of new string
  s.validate();                         // Ensure enough memory and not shared
  strcpy (s.str+old_len, c);            // Concatenate characters   
  return s;                             // Return String
}


// strncat -- Concatentate a String with "n" characters from char*
// Input:     CoolGen_String reference, char*, number of characters
// Output:    CoolGen_String object concatenedate with "n" characters

CoolGen_String& strncat (CoolGen_String& s, const char* c, int n) {
#if ERROR_CHECKING
  if (n < 0) {                          // If invalid length
    printf ("CoolGen_String::strncat(): Negative length %d.\n", n);
    exit (1);
  }
#endif
  long old_len = s.length;              // Save length for efficiency hack
  s.length = old_len + n;               // Determine length of new string
  s.validate();                         // Ensure enough memory and not shared
  strncpy (s.str+old_len, c, size_t(n));        // Concatenate characters   
  return s;                             // Return String
}


// strcat -- Concatenate two CoolGen_String objects
// Input:    Two CoolGen_String references
// Output:   CoolGen_String object concatenated with CoolGen_String object

CoolGen_String& strcat (CoolGen_String& s1, const CoolGen_String& s2) {
  long old_len = s1.length;             // Save length for efficiency hack
  s1.length = old_len + s2.length;      // Determine length of new string
  s1.validate();                        // Ensure enough memory and not shared
  strcpy (s1.str+old_len, s2.str);      // Copy remaining characters 
  return s1;                            // Return String
}


// strncat -- Concatentate a String with "n" characters from a String
// Input:     Two CoolGen_String references and number of characters
// Output:    CoolGen_String object concatenedate with "n" characters

CoolGen_String& strncat (CoolGen_String& s1, const CoolGen_String& s2, int n) {
#if ERROR_CHECKING
  if (n < 0) {                          // If invalid length
    printf ("CoolGen_String::strncat(): Negative length %d.\n", n);
    exit (1);
  }
#endif
  long old_len = s1.length;             // Save length for efficiency hack
  s1.length = old_len + n;              // Determine length of new string
  s1.validate();                        // Ensure enough memory and not shared
  strncpy (s1.str+old_len, s2.str, size_t(n));  // Copy remaining characters 
  return s1;                            // Return String
}


// compile -- Compile a regular expression ready for pattern matching. If no
//            regexp object in string, create one
// Input:     Regular expression to compile
// Output:    None

void CoolGen_String::compile (const char* s) {
  if (this->rgexp == NULL)                      // If no regexp object 
    this->rgexp = new CoolRegexp;                       // Allocate storage for regexp
  this->rgexp->compile (s);                     // Compile regular expression
  this->rgexp_index = 0;                        // Reset search index position
}


// find -- Search the string for the previously compiled regular expression
// Input:  None
// Output: Boolean TRUE/FALSE

Boolean CoolGen_String::find () {
#if ERROR_CHECKING
  if (this->rgexp == NULL || !this->rgexp->is_valid()) // Valid regexp?
    this->find_error ();                        // Raise exception
#endif
  this->rgexp_index += this->rgexp->end ();     // Use index from last search
  if (this->rgexp_index >= this->length)        // If at end of search
    return FALSE;                               // Indicate failure
  Boolean result = this->rgexp->find (this->str + this->rgexp_index);
  return result;                                // Return match result
}


// reverse -- Reverse the order of the characters in CoolGen_String object
// Input:     CoolGen_String object
// Output:    CoolGen_String object with character order reverse

void CoolGen_String::reverse () {
  char c;
  for (long i = 0, j = this->length-1;          // Counting from front and rear
       i < this->length / 2; i++, j--) {        // until we reach the middle
    c = this->str[i];                           // Save front character
    this->str[i] = this->str[j];                // Switch with rear character
    this->str[j] = c;                           // Copy new rear character
  }
}


// resize -- Adjust the memory size of a string to accomodate some size
// Input:    CoolGen_String object
// Output:   None

void CoolGen_String::resize (long sz) {
#if ERROR_CHECKING
  if (sz < 0) {                         // If invalid resize
    printf ("CoolGen_String::resize(): Negative resize %d.\n", sz);
    exit (1);
  }
#endif
  int shared_memory = FALSE;            // If TRUE, memory shared
  char* temp = this->str;               // Save pointer to existing string
  if (this->p->ref_count > 1) {         // If sharing memory with other String
    this->update_ref_count();           // Adjust and update
    shared_memory = TRUE;               // Set flag for later use
  }
  this->str = new char[sz+1];           // Allocate memory for desired size
  this->p->start = str;                 // Pointer to start of string
  this->p->size = sz+1;                 // Save size of allocated memory
  if (temp != NULL)                     // If original string had data
    strcpy ((char *)this->str, temp);   // Copy original string back
  if (shared_memory == FALSE)           // If did not share memory
    delete temp;                        // Deallocate memory
}


// operator<< -- Overload output operator for CoolGen_String objects
// Input:        CoolGen_String object
// Output:       Formatted output and stream descriptor

ostream& operator<< (ostream& os, const CoolGen_String& s) {
  return os << s.str;                           // Output char* and newline
}


// strtol -- Returns as a long integer the value represented by the
//           string pointer to by s, scanning upto the first character
//           that is inconsistent with the base. Leading white space is
//           ignored.
// Input:    Reference to CoolGen_String object
//           Radix of number
// Output:   Long integer representing value contained in the String

long strtol (const CoolGen_String& s, char** ptr, int radix) {
  return (strtol ((char *)s.str, ptr, radix)); 
}       


// atol -- Equivalent to: strtol (str, (char**)END_OF_STRING, 10)
// Input:  Reference to CoolGen_String object
// Output: Long integer representing value contained in the String

long atol (const CoolGen_String& s) {           
  return (strtol ((char *)s.str, (char **) END_OF_STRING, int(10)));
}


// atoi -- Equivalent to: (int)strtol (str, (char**)END_OF_STRING, 10)
// Input:  Reference to CoolGen_String object
// Output: Integer representing value contained in the String

int atoi (const CoolGen_String& s) {            
  return int(strtol ((char *)s.str, (char **) END_OF_STRING, int(10))); 
}


// strtod -- Returns as a double-precision floating-point number the
//           value represented by a CoolGen_String object. Characters are
//           scanned upto the first unrecognized character.
// Input:    Reference to CoolGen_String object
// Output:   Double representing value contained in String

double strtod (const CoolGen_String& s, char** ptr) {           
  return (strtod ((char *)s.str, ptr));
}


// trim -- Removes any occurrence of the character(s) in "c" from "s"
// Input:  CoolGen_String reference, character string
// Output: Modified String "s" object

CoolGen_String& trim (CoolGen_String& sr, const char* rem) {
  sr.validate();                        // Ensure not shared
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
// Input:       CoolGen_String reference, character string
// Output:      Modified String "s" object

CoolGen_String& left_trim (CoolGen_String& sr, const char* rem) {
  sr.validate();                        // Ensure not shared
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
// Input:        CoolGen_String reference, character string
// Output:       Modified String "s" object

CoolGen_String& right_trim (CoolGen_String& sr, const char* rem) {      
  sr.validate();                        // Ensure not shared
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
  return sr;                            // Return String reference
}


// upcase -- Convert all alphabetical characters to uppercase
// Input:    CoolGen_String reference
// Output:   Updated string

CoolGen_String& upcase (CoolGen_String& s) {    // Convert entire string to upper case
  s.validate();                         // Ensure not shared
  c_upcase(s.str);
  return s;                             // Return reference to modified string
}


// downcase -- Convert all alphabetical characters to lowercase
// Input:      CoolGen_String reference
// Output:     Updated string

CoolGen_String& downcase (CoolGen_String& s) {  // Convert entire string to lower case
  s.validate();                 // Ensure not shared
  c_downcase(s.str);
  return s;                     // Return reference to modified string
}

// capitalize -- Capitalize all words in a String. A word is define as
//               a sequence of characters separated by white space
// Input:        CoolGen_String reference
// Output:       Updated string

CoolGen_String& capitalize (CoolGen_String& s) {        
  s.validate();                 // Ensure not shared
  c_capitalize(s.str);
  return s;                     // Return reference to modified string
}


// insert -- Insert a char* at the position specified by start.
// Input  -- A char* to be inserted and a long index.          
// Output -- A Boolean, true if insertion took place, false if error.

Boolean CoolGen_String::insert (const char* ins, long start) {
  if (start<0 || start>this->length) return(FALSE);  // Boundary check
  long len=strlen(ins);                 // length of char*
  this->length += len;                  // Determine length of new string
  this->validate();                     // Ensure not shared and enough memory
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

Boolean CoolGen_String::remove (long start, long end) {
#if ERROR_CHECKING
  if (start < 0 || start > this->length || end < 0
      || end > this->length || end <= start) {
    printf ("CoolGen_String::remove(): Start %d and/or end %d index invalid.\n",
            start, end);
    exit (1);
  }
#endif
  this->validate();                          // Ensure not shared
  this->length -= (end-start);               // New (shorter) length
  for(; this->str[start]=this->str[end]; start++) end++; // Remove everything
  return(TRUE);                              // Remove worked
}


// replace -- Removes everything between start and end as in remove(), and
//            inserts the char* argument into the place of what was removed.
// Input   -- A start and end for remove and a char*, c, to insert.
// Output  -- A Boolean, true if replace worked, false if error.

Boolean CoolGen_String::replace (const char* c, long start, long end) {
#if ERROR_CHECKING
  if (start < 0 || start > this->length || end < 0            // Boundary check
      || end > this->length || end <= start) {
    printf ("CoolGen_String::replace(): Start %d and/or end %d index invalid.\n",
            start, end);
    exit (1);
  }
#endif
  long len=strlen(c);                   // length of c
  long delta = len - end + start;       // find overall change in length
  this->length += delta;                // set new length
  this->validate();                     // Ensure not shared and enough memory
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
// Input  -- A reference to a CoolGen_String. A start and end for the remove.
// Output -- none.  the CoolGen_String passed in gets set.

void CoolGen_String::yank (CoolGen_String& s, long start, long end) {
#if ERROR_CHECKING
  if (start < 0 || start > this->length || end < 0      // Boundary check
      || end > this->length || end <= start) {
    printf ("CoolGen_String::yank(): Start %d and/or end %d index invalid.\n",
            start,end);
    exit (1);
  }
#endif
  this->validate();                     // Ensure not shared
  long len = end - start;
  strncpy(s, this->str+start, len);     // Copy stuff to yank into s
  this->length -= len;                  // set new length
  for(; this->str[start]=this->str[end]; start++) end++;  // Do the remove
}


// sub_string -- Returns a new string initialized to what is between the
//               start and end indexes.
// Input      -- A CoolGen_String reference.  Two longs, a start and an end
// Output     -- none.  Sets the given CoolGen_String.

void CoolGen_String::sub_string (CoolGen_String& s, long start, long end) {
#if ERROR_CHECKING
  if (start < 0 || start > this->length || end < 0      // Boundary check
      || end > this->length || end <= start) {
    printf ("CoolGen_String::sub_string(): Start %d and/or end %d index invalid.\n",
            start, end);
    exit (1);
  }
#endif
  strncpy(s, this->str+start, end - start);     // Copy substring into s
}


// strncpy -- Returns s, with the first length characters of source copied
//            into it.  The old value of s is lost.
// Input   -- A reference to a CoolGen_String s, a char* source, and a long length.
// Output  -- The modified CoolGen_String s.

CoolGen_String& strncpy(CoolGen_String& s, const char* source, long length) {
#if ERROR_CHECKING  
  if (length < 0) {                     // Boundary check
    printf ("CoolGen_String::strncpy(): Negative length %d.\n", length);
    exit (1);
  }
#endif
  s.length = length;                    // Set new string length
  s.validate(TRUE);                     // Ensure not shared and enough memory
  char* p = s.str;
  while (length-- > 0)                  // Copy source into string
    *p++ = *source++;
  *p = END_OF_STRING;                   // set the end byte
  return s;
}


// bracket_error -- Raise exception for operator[]
// Input:           Index
// Output:          None

void CoolGen_String::bracket_error (long n) {
  printf ("CoolGen_String::operator[](): Index %d out of range.\n", n);
  exit (1);
}


// ratio_error -- Raise exception for set_growth_ratio
// Input:         Ratio
// Output:        None

void CoolGen_String::ratio_error (float r) {
  printf ("CoolGen_String::set_growth_ratio(): Negative growth ratio %f.\n", r);
  exit (1);
}


// size_error -- Raise exception for set_alloc_size
// Input:        Growth size
// Output:       None

void CoolGen_String::size_error (int n) {
  printf ("CoolGen_String::set_alloc_size(): Negative growth size %d.\n", n);
  exit (1);
}


// find_error -- Raise exception for find()
// Input:        None
// Output:       None

void CoolGen_String::find_error () {
  printf ("CoolGen_String::find(): Invalid regular expression.\n");
  exit (1);
}
