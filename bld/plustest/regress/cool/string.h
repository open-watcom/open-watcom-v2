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
// Updated: MBN 08/03/89 -- Changed operator= argument to const
// Updated: LGO 08/09/89 -- Inherit from Generic
// Updated: MBN 09/06/89 -- Added conditional exception handling
// Updated: LGO 10/28/89 -- Removed is_lt, is_gt, is_le, is_ge, is_equal
//                          and is_not_equal (use char* functions instead)
// Updated: LGO 11/07/89 -- Removed strcmp, strncmp
// Updated: MBN 12/15/89 -- Sprinkled "const" all over the place!
// Updated: LGO 01/05/90 -- Removed strchr, strrchr
// Updated: MBN 01/19/90 -- Made operator=(const char*) take a const char*
// Updated: MJF 06/15/90 -- Added inline strncpy(...,int) to remove ambiguity
// Updated: DLS 03/22/91 -- New lite version
// Updated: JAM 08/11/92 -- removed DOS specifics, stdized #includes
// Updated: JAM 09/23/92 -- use CoolEnvelope template instead of macro
//
// The String class provides dynamic, efficient  strings for a C++ application
// programmer.  The string private data consists of a  slot that maintains the
// length  of   the string  (ie.   number of characters),   a  size  slot that
// maintains the  actual number of  bytes allocated to  a string  object char*
// pointer, and a pointer to the first character of the string. In addition, a
// floating point slot can contain a non-negative percentage between 0 and 1.0
// that  indicates   the ratio by which  a   string object  should  grow  when
// necessary.  Finally,  a static for  the  entire  string class  contains the
// allocation size to be used  when a string  object needs to grow dynamically
// if the growth ratio for the particular instance has not been set.  This has
// a default value that may be over-ridden by the user when the constructor is
// invoked.
//
//              String               Virtual Function Table
//           +-----------+                +----------+
//           |  V_Table--+------------->  :Alloc_Size:
//           +-----------+                :  ....    :
//           | Length=16 |                :          :
//           +-----------+                +----------+
//           | size=100  |                
//           +-----------+   
//           | ratio=0.0 |
//           +-----------+       +--------------------- ... --+
//           |   str   --+------>|This is a string            |
//           +-----------+       +--------------------- ... --+
//
//                               \_____________  _____________/
//                                             \/
//                                     100 bytes allocated
//
// There are  several constructors  for  this class.  The   empty  constructor
// initializes a String object and allocates the default size block of memory.
// The second, third,  and fourth constructors  take char, char*,   and String
// arguments, respectively, and initialize the  String object accordingly. The
// fifth and sixth constructors take either a char* or String argument, and an
// integer argument that specifies the initial
//
// The standard ANSI "str____" function names are all overloaded  for use with
// both  char* and   String  objects.   Operators   for  String concatenation,
// assignment, and comparison are also provided.   In  addition, functions for
// case conversion and string token trimming are provided.  Finally, note that
// the operator functions use corresponding functions with the "case" flag set
// to SENSITIVE.  A user can perform case-INSENSITIVE operations by explicitly
// calling the appropriate function.
//

#ifndef STRINGH                                 // If no String class defined
#define STRINGH                                 // Indicate its done now

#ifndef CHARH                                   // If extended char* not here
#include <cool/char.h>
#endif

#include <stdlib.h>             // Include standard c library support

#define MEM_BLK_SZ 100

//## BC++ 3.1 bug
#undef CoolEnvelope_H
#define CoolEnvelope CoolEnvelope_String

template<class CoolLetter> class CoolEnvelope;
class CoolString;
typedef CoolEnvelope<CoolString> CoolStringE;                           // forward dec. of envelope

class CoolString {  
public:
  CoolString ();                                // CoolString x;
  CoolString (char);                            // CoolString x = 'A';
  CoolString (const char*);                     // CoolString x = "ABCDEFG";
  CoolString (const CoolString&);               // CoolString x = y;
  CoolString (const CoolString&, long);         // CoolString x = y; memory size
  CoolString (const char*, long);               // CoolString x = "ABCDEFG"; size

  ~CoolString();                                // Destructor for CoolString class

  Boolean insert (const char*, long);           // Insert chars at index
  Boolean remove (long, long);                  // Remove chars between indexes
  Boolean replace (const char*, long, long);    // Replace chars between index
  void yank (CoolString&, long, long);          // Delete/set to chars at index
  void sub_string (CoolString&, long, long);    // Set to chars between indexes

  friend CoolString& strncpy (CoolString&, const char*, long); // Copy "n" chars
  /*inline##*/ friend CoolString& strncpy (CoolString&, const char*, int);

  friend ostream& operator<< (ostream&, const CoolString&);
  friend ostream& operator<< (ostream&, const CoolString*);
  
  inline CoolString& operator= (char);          // x = 'A';
  inline CoolString& operator= (const char*);   // x = "ABCDEFG";
  inline CoolString& operator= (const CoolString&);     // x = y;
  inline CoolString& operator= (CoolStringE&);  // from envelope back to string

  inline CoolStringE operator+ (char) const;    // Concatenation operators
  inline CoolStringE operator+ (const char*) const;     

// Avoid deep copy and concatenate strings in place with envelope
//   inline friend CoolString operator+ (const CoolString&, const CoolString&);
  
  inline CoolString& operator+= (char);         // Concatentation w/ assignment
  inline CoolString& operator+= (const char*);
  inline CoolString& operator+= (const CoolString&);
  
  inline operator const char*() const;          // String to const char*
  void reverse ();                              // Reverse character order
  void clear ();                                // Reset NULL terminator
  void resize (long);                           // Allocate at least min size
  inline char& operator[] (long i);             // Specific char from CoolString

  inline long capacity() const;                 // Returns maximum size string 
  inline void set_alloc_size (int);             // Set memory block alloc size
  inline void set_growth_ratio (float);         // Set growth percentage
  
  inline Boolean operator== (const CoolString&) const; // Equality operator
  inline Boolean operator== (const char*) const;
  
  inline Boolean operator!= (const CoolString&) const; // Inequality operator
  inline Boolean operator!= (const char*) const;
  
  inline Boolean operator< (const CoolString&) const; // Lexical less than
  inline Boolean operator< (const char*) const;
  
  inline Boolean operator> (const CoolString&) const; // Lexical greater than
  inline Boolean operator> (const char*) const;
  
  inline Boolean operator<= (const CoolString&) const; // Lexical less than/equal
  inline Boolean operator<= (const char*) const;
  
  inline Boolean operator>= (const CoolString&) const; // Lexical greater than/eq
  inline Boolean operator>= (const char*) const;
  
  /*inline##*/ friend long strlen (const CoolString&);  // Return length of string
  friend CoolString& strcat(CoolString&, const CoolString&); // Appends a copy of second
  friend CoolString& strcat (CoolString&, const char*);
  friend CoolString& strcat (CoolString&, char);
  
  friend CoolString& strncat (CoolString&, const CoolString&, int); // Append "n" chars
  friend CoolString& strncat (CoolString&, const char*, int);
  
  friend CoolString& strcpy (CoolString&, char);        // CoolString copy functions
  friend CoolString& strcpy (CoolString&, const char*);
  friend CoolString& strcpy (CoolString&, const CoolString&); 
  
  friend long strtol(const CoolString&, char** ptr=NULL, int radix=10); // to long
  friend long atol (const CoolString&);         // Convert string to long
  friend int atoi (const CoolString&);          // Convert string to int
  
  friend double strtod (const CoolString&, char** ptr=NULL); // string to double
  /*inline##*/ friend double atof (const CoolString&);  // Convert string to double
  
  friend CoolString& trim (CoolString&, const char*);       // Trim characters 
  friend CoolString& left_trim (CoolString&, const char*);  // Trim prefix chars
  friend CoolString& right_trim (CoolString&, const char*); // Trim suffix chars
  
  friend CoolString& upcase (CoolString&);              // Convert CoolString to upper 
  friend CoolString& downcase (CoolString&);            // Convert string to lower
  friend CoolString& capitalize (CoolString&);          // Capitalize each word

private:
  long length;                                  // Number of characters 
  long size;                                    // Allocated memory size
  char* str;                                    // Pointer to string
  float growth_ratio;                           // If non-zero, grow by %
  static int alloc_size_s;                      // Memory growth size

  void bracket_error (long);                    // Raise exception
  void growth_error (int);                      // Raise exception
  void ratio_error (float);                     // Raise exception
  friend void update_memory (CoolString&);              // Adjust memory size
};



// Avoid deep copy, and concatenate strings in place with envelope
#include <cool/Envelope.h>                      // Include envelope template

// Avoid deep copy and concatenate strings in place with envelope
inline CoolEnvelope<CoolString> operator+ (const CoolString&arg1, const CoolString&arg2)
{ return (CoolStringE &) CoolEnvOp(add)(arg1, arg2); }


// operator[] -- Return a single character element from CoolString
// Input:        this* CoolString pointer, index "i"
// Output:       The "ith-1" character from CoolString

inline char& CoolString::operator[] (long i) {
#if ERROR_CHECKING
  if (i > this->length)                         // If index out of range
    this->bracket_error (i);                    // Raise exception
#endif
  return (this->str[i]);
}


// capacity -- Determine the maximum size string possible with growing
// Input:      None
// Output:     Maximum number of characters before growth is required

inline long CoolString::capacity() const {
  return (this->size-1);                        // Allocated size -1 for NULL
}


// set_alloc_size -- Set the default allocation size growth rate
// Input:            Growth size in number of elements
// Output:           None

inline void CoolString::set_alloc_size (int n) {
#if ERROR_CHECKING
  if (n < 0)                                    // If negative growth size
    this->growth_error (n);                     // Raise exception
#endif
  this->alloc_size_s = n;                       // Set growth size
}


// set_growth_ratio -- Set the growth percentage for this instance of String
// Input:              Percentage growth rat
// Output:             None

inline void CoolString::set_growth_ratio (float ratio) {
#if ERROR_CHECKING
  if (ratio <= 0.0)                             // If negative growth factor
    this->ratio_error (ratio);                  // Raise exception
#endif
  this->growth_ratio = ratio;                   // Set growth size
}


// operator const char* -- Provide an accessor to the String character pointer
// Input:            this* CoolString pointer
// Output:           this->str character pointer

inline CoolString::operator const char*() const {return this->str;}

// operator= -- CoolString assignment to a single character: x = 'A';
// Input:       Single character
// Output:      CoolString object containing character string

inline CoolString& CoolString::operator= (char c) {
  return (strcpy (*this, c));                   
}


// operator= -- CoolString assignment to a character string: x = "ABCDEFG";
// Input:       Character string
// Output:      CoolString object containing character string

inline CoolString& CoolString::operator= (const char* c) {
  return (strcpy (*this, c));                   
}


// operator= -- CoolString assignment to another CoolString: x = y;
// Input:       Reference to CoolString object
// Output:      CoolString object sharing memory with other CoolString

inline CoolString& CoolString::operator= (const CoolString& s) {
  return (strcpy (*this, s));                   
}

// operator=  -- Assignment from an envelope back to real string
// Input:     envelope reference
// Output:    string reference with contents in envelope being swapped over

inline CoolString& CoolString::operator= (CoolStringE& env){
  env.shallow_swap((CoolStringE*)this, &env);   // same physical layout
  return *this;
}


// operator+= -- CoolString concatenation of a character: x += 'A';
// Input:        Character
// Output:       CoolString object concatenated with character

inline CoolString& CoolString::operator+= (char c) {
  return (strcat (*this, c));           
}

// operator+ -- CoolString concatenation of a character: x = x + 'A';
// Input:       Character
// Output:      new CoolString object concatenated with character

inline CoolStringE CoolString::operator+ (char c) const {
  CoolString temp(*this);                       // Temporary string
  temp += c;                                    // Concatenate temp with c
  CoolStringE& result = (CoolStringE&) temp;    // same physical object
  return result;                                // copy of envelope
}

// operator+= -- CoolString concatenation of a character string: x += "ABCDEFG";
// Input:        Character CoolString
// Output:       CoolString object concatenated with character CoolString

inline CoolString& CoolString::operator+= (const char* c) {
  return (strcat (*this, c));           
}


// operator+ -- CoolString concatenation of a character string: x = x + "ABCDEFG";
// Input:       Character string
// Output:      CoolString object concatenated with character string

inline CoolStringE CoolString::operator+ (const char* c) const {
  CoolString temp(*this);                       // Temporary string
  temp += c;                                    // Concatenate temp with c
  CoolStringE& result = (CoolStringE&) temp;    // same physical object
  return result;                                // copy of envelope
}

// operator+= -- CoolString concatenation of another string: x += y;
// Input:        CoolString reference
// Output:       CoolString object concatenated with CoolString contents

inline CoolString& CoolString::operator+= (const CoolString& s) {
  return (strcat (*this, s));           
}


// operator+ -- CoolString concatenation of another string: x = x + y;
//              Implemented as a friend function in CoolEnvelope
// Input:       CoolString reference
// Output:      CoolString object concatenated with CoolString contents

// CoolString CoolString::operator+ (const CoolString& s) const {
//   CoolString temp(*this);                    // Temporary string
//   strcat(temp, s);                           // Concatenate temp with s
//   return temp;                               // Return by value
// }


// operator== -- Test for equality of two CoolString objects
// Input:        this* CoolString pointer, CoolString reference
// Output:       Boolean TRUE/FALSE

inline Boolean CoolString::operator== (const CoolString& s) const {
  return (is_equal (*this, s.str, SENSITIVE));
}


// operator== -- Test for equality of a CoolString and char*
// Input:        this* CoolString pointer, char* pointer
// Output:       Boolean TRUE/FALSE

inline Boolean CoolString::operator== (const char* c) const {
  return (is_equal (*this, c, SENSITIVE));
}


// operator!= -- Test for inequality of two CoolString objects
// Input:        this* CoolString pointer, CoolString reference
// Output:       Boolean TRUE/FALSE

inline Boolean CoolString::operator!= (const CoolString& s) const {
  return (is_not_equal (*this, s.str, SENSITIVE));
}


// operator!= -- Test for inequality of two CoolString objects
// Input:        this* CoolString pointer, char* pointer
// Output:       Boolean TRUE/FALSE

inline Boolean CoolString::operator!= (const char* c) const {
  return (is_not_equal (*this, c, SENSITIVE));
}


// operator< -- Test for lexical ordering before a CoolString
// Input:       this* CoolString pointer, CoolString reference
// Output:      Boolean TRUE/FALSE

inline Boolean CoolString::operator< (const CoolString& s) const {
  return (is_lt (*this, s.str, SENSITIVE));
}


// operator< -- Test for lexical ordering before a CoolString
// Input:       this* CoolString pointer, char* pointer
// Output:      Boolean TRUE/FALSE

inline Boolean CoolString::operator< (const char* c) const {
  return (is_lt (*this, c, SENSITIVE));
}


// operator> -- Test for lexical ordering after a CoolString
// Input:       this* CoolString pointer, CoolString reference
// Output:      Boolean TRUE/FALSE

inline Boolean CoolString::operator> (const CoolString& s) const {
  return (is_gt (*this, s.str, SENSITIVE));
}


// operator> -- Test for lexical ordering after a CoolString
// Input:       this* CoolString pointer, char* pointer
// Output:      Boolean TRUE/FALSE

inline Boolean CoolString::operator> (const char* c) const {
  return (is_gt (*this, c, SENSITIVE));
}


// operator<= -- Test for lexical ordering before or equal to a CoolString
// Input:        this* CoolString pointer, CoolString reference
// Output:       Boolean TRUE/FALSE

inline Boolean CoolString::operator<= (const CoolString& s) const {
  return (is_le (*this, s.str, SENSITIVE));
}


// operator<= -- Test for lexical ordering before or equal to a CoolString
// Input:        this* CoolString pointer, char* pointer
// Output:       Boolean TRUE/FALSE

inline Boolean CoolString::operator<= (const char* c) const {
  return (is_le (*this, c, SENSITIVE));
}


// operator>= -- Test for lexical ordering after or equal to a CoolString
// Input:        this* CoolString pointer, CoolString reference
// Output:       Boolean TRUE/FALSE

inline Boolean CoolString::operator>= (const CoolString& s) const {
  return (is_ge (*this, s.str, SENSITIVE));
}


// operator>= -- Test for lexical ordering after or equal to a CoolString
// Input:        this* CoolString pointer, char* pointer
// Output:       Boolean TRUE/FALSE

inline Boolean CoolString::operator>= (const char* c) const {
  return (is_ge (*this, c, SENSITIVE));
}


// strlen -- Return the number of characters in the CoolString
// Input:    CoolString reference
// Output:   Length of the string

inline long strlen (const CoolString& s) {
  return (s.length);
}


// atof -- Equivalent to strtod (str, (char**)END_OF_STRING)
// Input:  Reference to CoolString object
// Output: Double representing value contained in CoolString

inline double atof (const CoolString& s) {
  return (strtod ((char *)s.str, (char **) END_OF_STRING));
}


// strncpy -- Returns s, with the first length characters of source copied
//            into it.  The old value of s is lost.
// Input   -- A reference to a CoolString s, a char* source, and an int length.
// Output  -- The modified CoolString s.

inline CoolString& strncpy(CoolString& s, const char* source, int n) {
  return strncpy(s, source, (long) n);
}

//## BC++ 3.1 bug
#undef CoolEnvelope

#endif                                          // End #ifdef of STRINGH


