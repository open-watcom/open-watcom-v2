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
// Updated: MBN 06/07/89 -- Added growth ratio slot and resize method
// Updated: MBN 06/30/89 -- Created Gen_String class with regular expression
// Updated: DKM 07/07/89 -- To work around Xenix 31 char limit:
//                          Shortened is_less_than        to is_lt
//                                    is_greater_than     to is_gt
//                                    is_less_or_equal    to is_le
//                                    is_greater_or_equal to is_ge
//                          Removed is_equal_or_less and is_greater_or_less
// Updated: LGO 08/09/89 -- Inherit from Generic
// Updated: MBN 09/07/89 -- Added conditional exception handling
// Updated: LGO 10/28/89 -- Removed is_lt, is_gt, is_le, is_ge, is_equal
//                          and is_not_equal (use char* functions instead)
// Updated: LGO 11/07/89 -- Removed strcmp, strncmp
// Updated: LGO 11/07/89 -- Make check_memory strcpy and operator=
//                          take const arguments
// Updated: MBN 12/15/89 -- Sprinkled "const" qualifier all over the place!
// Updated: MBN 01/02/90 -- Made find search for the first/next regexp match 
// Updated: LGO 01/05/90 -- Removed strchr, strrchr
// Updated: MJF 05/21/90 -- Moved delete from grow_memory to validate
// Updated: MJF 06/15/90 -- Added inline strncpy(...,int) to remove ambiguity
// Updated: DLS 03/22/91 -- New lite version
// Updated: JAM 08/14/92 -- removed DOS specifics, stdized #includes
// Updated: JAM 08/14/92 -- defined set_growth_ratio as inline (prob. typo)
//
// The   Gen_String class   provides   dynamic,  efficient strings   for  a C++
// application programmer  with support for   reference counting, delayed copy,
// and  regular expression   pattern   matching/replacment.  The  private  data
// consists of a pointer to a structure of type String_Layout.   This structure
// is defined for use by the Gen_String class and  is public so that Gen_String
// constructors can  allocate correct   memory.  In addition, the  private data
// section contains a slot that maintains the length of the string (ie.  number
// of characters) and a pointer to the first character of the string.  Finally,
// a static for the entire Gen_String class  contains the allocation size to be
// used   when an  Gen_String object  needs  to grow  dynamically.   This has a
// default value that may be over-ridden  by the  user when the  constructor is
// invoked.
//
// The string layout structure has several fields.  The size variable maintains
// the actual  number of bytes allocated  to this string  object char* pointer.
// This may be different to the length slot in the Gen_String object due to the
// dynamic nature  of the  string  object  and/or  sub-string  operations.  The
// reference count variable  maintains  a  count  of  the number  of Gen_String
// pointers  that point  to  this  actual string.  This  is  utilized to reduce
// copying of  string objects  at   assignment, for example.   This  scheme  is
// similar to that used  in a virtual  memory management  system when a  parent
// process spawns a child process.  The  child process  inherits a copy  of the
// parents memory,  but  instead  of   copying,  a pointer  to these  pages,  a
// reference count is  kept.  If the child process  only reads a page, it never
// needs to be copied.  Only  when a change is  made  must a copy actually take
// place.   In  general, this  scheme drastically  cuts   down byte  copies and
// improves runtime efficieny.   The following diagram  displays two Gen_String
// objects that share a char* string.   Note  that the Gen_String2 object is in
// fact a sub-string of Gen_String1.
// 
//  +----------->-------------+
//  |                         |
//  |         Gen_String1     V          Virtual Function Table
//  |        +-----------+    |                +----------+
//  |        |  V_Table--+----+------------->  :Alloc_Size:
//  |        +-----------+                     :  ....    :
//  |        | Length=16 |                     :  ....    :       +----------+
//  |        +-----------+                     +----------+       |  Regular |
//  |        |  *rgexp --+--------------------------------------->|Expression|
//  |        +-----------+                                        |  Object  |
//  |        |rgexp_index|                                        +----------+
//  |        +-----------+
//  ^        |    p    --+---+
//  |        +-----------+   |
//  |        |   str   --+---)-------------->-------------+
//  |        +-----------+   |                            |
//  |                        |                            |
//  |                        |                            |
//  |     Gen_String2        |        Layout_Struct       |
//  |    +-----------+       +------->+-----------+       |
//  +----+--V_Table  |       |        | Ref_Cnt=2 |       |
//       +-----------+       |        +-----------+       |
//       | Length=6  |       |        |  Size=25  |       V
//       +-----------+       |        +-----------+      +--------------------+
//    +--+-- *rgexp  |       |        | str_start-+----->|This is a string    |
//    |  +-----------+       |        +-----------+      +--------------------+
//    |  |rgexp_index|       |                                      ^
//    |  +-----------+       |                                      |
//    |  |    p    --+-------+                                      |
//    |  +-----------+                                              |
//    |  |   str   --+--------------------->------------------------+
//    |  +-----------+   
//    |                  +----------+
//    |                  |  Regular |
//    +----------------->|Expression|
//                       |  Object  |
//                       +----------+
//
// There are  several   constructors  for this   class.  The empty  constructor
// initializes an Gen_String object  and allocates the  layout structure with a
// default allocation of  MEM_BLK_SZ bytes for  the char* pointer.   The second
// constructor takes a char* argument and provides for the initialization of an
// Gen_String object  to a single character or  character  string.    The third
// constructor provides  initialization  of an   Gen_String  Object to that  of
// another Gen_String  object.  Finally,  the two  constructors with an integer
// argument allow  the user  to initialize  a  Gen_String object with either  a
// char* or another Gen_String object and specify the initial memory allocation
// size for the char* pointer.  This is useful  for a user  who expect a String
// object to be fairly dynamic; allocating  a  larger chunk initially cuts down
// on the number of new() requests needed to grow the object later.
//
// The standard  ANSI "str____" function names are  all overloaded for use with
// both  char*  and  String   objects.  Operators  for  String   concatenation,
// assignment, and  comparison are also provided.   In addition,  functions for
// case conversion and string token trimming are provided.   Finally, note that
// the operator functions use corresponding functions with the  "case" flag set
// to SENSITIVE.  A user  can perform case-INSENSITIVE operations by explicitly
// calling the appropriate function.
//
// Envelope is not needed, because the Gen_Strings already share the
// same layout structure, and that copy constructor, only copy pointers, and
// increment ref_count.

#ifndef GEN_STRINGH                             // If no Gen_String class
#define GEN_STRINGH                             // Include the header file

#ifndef CHARH                                   // If extended char* not here
#include <cool/char.h>
#endif

#ifndef REGEXPH                                 // If no regular expressions
#include <cool/Regexp.h>                        // Include definition
#endif

#include <stdlib.h>             // Include standard c library support

#define MEM_BLK_SZ 100

struct String_Layout {                          // Internal layout structure 
  long size;                                    // Bytes allocated
  short ref_count;                              // Pointers to other Gen_String
  char* start;                                  // Pointer to start of string
};

class CoolGen_String {
public:
  CoolGen_String ();                           // CoolGen_String x;
  CoolGen_String (char);                       // CoolGen_String x = 'A';
  CoolGen_String (const char*);                // CoolGen_String x = "ABCDEFG";
  CoolGen_String (const CoolGen_String&);      // Copy constructor
  CoolGen_String (const CoolGen_String&, long); // CoolGen_String x = y; memory size
  CoolGen_String (const char*, long);          // CoolGen_String x = "AB"; mem size
  
  ~CoolGen_String();                           // Destructor for CoolGen_String

  Boolean insert (const char*, long);          // Insert chars at index
  Boolean remove (long, long);                 // Remove chars between indexes
  Boolean replace (const char*, long, long);   // Replace chars between indexes

  void yank (CoolGen_String&, long, long);      // Delete and set the given 
                                               // CoolGen_String to the characters
                                               // at index

  void sub_string (CoolGen_String&, long, long);   // Set the given Gen_String to 
                                               // the chars between indexes

  friend CoolGen_String& strncpy (CoolGen_String&, const char*, long);// Copy "n" chars
  /*##inline*/ friend CoolGen_String& strncpy (CoolGen_String&, const char*, int);

  friend ostream& operator<< (ostream&, const CoolGen_String&);
  /*##inline*/ friend ostream& operator<< (ostream&, const CoolGen_String*);
  
  inline CoolGen_String& operator= (char);      // x = 'A';
  inline CoolGen_String& operator= (const char*); // x = "ABCDEFG";
  inline CoolGen_String& operator= (const CoolGen_String&); // x = y;
  
  inline CoolGen_String& operator+= (char);     // Concat with assignment 
  inline CoolGen_String& operator+= (const char*);
  inline CoolGen_String& operator+= (const CoolGen_String&);
  
  /*##inline*/ friend CoolGen_String operator+ (const CoolGen_String&, char);
  /*##inline*/ friend CoolGen_String operator+ (const CoolGen_String&, const char*);
  /*##inline*/ friend CoolGen_String operator+ (const CoolGen_String&, const CoolGen_String&);
  
  inline operator const char*() const;          // Convert Gen_String to const char*
  void compile (const char*);                   // Compile regexp in argument
  Boolean find ();                              // Search for first/next regexp
  inline long start ();                         // Return start index of match
  inline long end ();                           // Return end index of match
  inline Boolean is_valid ();                   // TRUE/FALSE validates regexp
  
  void reverse ();                              // Reverse character order
  void clear ();                                // Reset NULL terminator
  void resize (long);                           // Allocate at least min size
  inline char& operator[] (long i);             // x[4] --> fifth character

  inline long capacity() const;                 // Returns maximum size string 
  inline void set_alloc_size (int);             // Set memory block alloc size
  inline void set_growth_ratio (float);         // Set growth percentage
  
  inline Boolean operator== (const CoolGen_String&) const;       
  inline Boolean operator== (const char*) const;                  
  
  inline Boolean operator!= (const CoolGen_String&) const;       
  inline Boolean operator!= (const char*) const;                  
  
  inline Boolean operator< (const CoolGen_String&) const;         
  inline Boolean operator< (const char*) const;           
  
  inline Boolean operator> (const CoolGen_String&) const;         
  inline Boolean operator> (const char*) const; 
  
  inline Boolean operator<= (const CoolGen_String&) const;       
  inline Boolean operator<= (const char*) const;        
  
  inline Boolean operator>= (const CoolGen_String&) const;       
  inline Boolean operator>= (const char*) const;        
  
  /*##inline*/ friend long strlen (const CoolGen_String&); // Return length of string
  
  friend CoolGen_String& strcat (CoolGen_String&, const CoolGen_String&);
  friend CoolGen_String& strcat (CoolGen_String&, const char*);
  friend CoolGen_String& strcat (CoolGen_String&, char);
  
  friend CoolGen_String& strncat (CoolGen_String&, const CoolGen_String&, int);
  friend CoolGen_String& strncat (CoolGen_String&, const char*, int);
  
  friend CoolGen_String& strcpy (CoolGen_String&, char);
  friend CoolGen_String& strcpy (CoolGen_String&, const char*);
  friend CoolGen_String& strcpy (CoolGen_String&, const CoolGen_String&); 
  
  friend long strtol(const CoolGen_String&, char** ptr=NULL, int radix=10);
  friend long atol (const CoolGen_String&);               
  friend int atoi (const CoolGen_String&);      
  
  friend double strtod (const CoolGen_String&, char** ptr=NULL);
  /*##inline*/ friend double atof (const CoolGen_String&);
  
  friend CoolGen_String& trim (CoolGen_String&, const char*);
  friend CoolGen_String& left_trim (CoolGen_String&, const char*); 
  friend CoolGen_String& right_trim (CoolGen_String&, const char*);
  
  friend CoolGen_String& upcase (CoolGen_String&);
  friend CoolGen_String& downcase (CoolGen_String&);
  friend CoolGen_String& capitalize (CoolGen_String&);

private:
  long length;                                 // Number of characters 
  char* str;                                   // Pointer to string
  String_Layout *p;                            // Pointer to layout structure
  float growth_ratio;                          // If non-zero, grow by this %
  static int alloc_size_s;                     // Memory allocation growth size
  CoolRegexp* rgexp;                           // Pointer to regular expression
  long rgexp_index;                            // Start index for regexp search

  void bracket_error (long);                    // Raise exception
  void ratio_error (float);                     // Raise exception
  void find_error ();                           // Raise exception
  void size_error (int);                        // Raise exception
  
  void validate(Boolean dont_copy=FALSE);       // check ref_count and memory
  void update_ref_count();                      // Don't share string
  void grow_memory (Boolean dont_copy);         // Grow string
};

// operator<< -- Overload output operator for CoolGen_String objects
// Input:        CoolGen_String pointer
// Output:       Formatted output and stream descriptor

inline ostream& operator<< (ostream& os, const CoolGen_String* s) {
  return os << s->str;                          
}

// operator[] -- Return a single character element from string
// Input:        this*, index "i"
// Output:       The "ith-1" character from String

inline char& CoolGen_String::operator[] (long i) {
#if ERROR_CHECKING
  if (i >= this->length)                        // If index out of range
    this->bracket_error (i);                    // Raise exception
#endif
  return (this->str[i]);
}


// capacity -- Determine the maximum size string possible with growing
// Input:      None
// Output:     Maximum number of characters before growth is required

inline long CoolGen_String::capacity() const {
  return (this->p->size-1);                     
}


// set_alloc_size -- Set the default allocation size growth rate
// Input:            Growth size in number of elements
// Output:           None

inline void CoolGen_String::set_alloc_size (int n) {
#if ERROR_CHECKING
  if (n < 0)                                    // If negative growth size
    this->size_error (n);                       // Raise exception
#endif
  this->alloc_size_s = n;                       // Set growth size
}


// set_growth_ratio -- Set the growth percentage for this instance of String
// Input:              Percentage growth rat
// Output:             None

inline void CoolGen_String::set_growth_ratio (float ratio) {
#if ERROR_CHECKING
  if (ratio <= 0.0)                             // If positive growth factor
    this->ratio_error (ratio);                  // Raise exception
#endif
  this->growth_ratio = ratio;                   // Set growth size
}


// operator const char* -- Provide an accessor to the character pointer
// Input:            this* 
// Output:           this->str character pointer
inline CoolGen_String::operator const char*() const {
  return (this->str);
}

// start -- Return the zero-relative start index of regular expression match
// Input:   None
// Output:  Start index into string

inline long CoolGen_String::start () {
  return (this->rgexp_index + this->rgexp->start ());
}


// End -- Return the zero-relative end index of the regular expression match
// Input:   None
// Output:  End index into string

inline long CoolGen_String::end () {
  return (this->rgexp_index + this->rgexp->end ());
}


// is_valid -- Returns the validity of the regular expression
// Input:      None
// Output:     TRUE/FALSE

inline Boolean CoolGen_String::is_valid () {
  return (this->rgexp != NULL) &&               // If a Regexp object
         this->rgexp->is_valid ();              // return Regexp status
}


// operator= -- Assignment to a single character: x = 'A';
// Input:       Single character
// Output:      CoolGen_String object containing character string

inline CoolGen_String& CoolGen_String::operator= (char c) {
  return (strcpy (*this, c));                   
}


// operator= -- Assignment to a character string: x = "ABCDEFG";
// Input:       Character string
// Output:      CoolGen_String object containing character string

inline CoolGen_String& CoolGen_String::operator= (const char* c) {
  return (strcpy (*this, c));                   
}


// operator= -- Assignment to another CoolGen_String: x = y;
// Input:       Reference to CoolGen_String object
// Output:      CoolGen_String object sharing memory with other CoolGen_String

inline CoolGen_String& CoolGen_String::operator= (const CoolGen_String& s) {
  return (strcpy (*this, s));                   
}


// operator+= -- Concatenation of a character: x += 'A';
// Input:        Character
// Output:       CoolGen_String object concatenated with character

inline CoolGen_String& CoolGen_String::operator+= (char c) {
  return (strcat (*this, c));                   
}

// operator+ -- String concatenation of a character: y = x + 'A';
// Input:       Character
// Output:      new CoolGen_String object concatenated with character

inline CoolGen_String operator+ (const CoolGen_String& s, char c) {
  CoolGen_String temp(s);                       // temporary copy of s
  temp += c;                                    // mutate temp with += c
  return temp;                                  // return new copy by value
}

// operator+= -- Concatenation of a character string: x += "ABCDEFG";
// Input:        Character string
// Output:       CoolGen_String object concatenated with character string

inline CoolGen_String& CoolGen_String::operator+= (const char* c) {
  return (strcat (*this, c));                   
}


// operator+ -- String concatenation of a character string: y = x + "ABCDEFG";
// Input:       Character string
// Output:      new CoolGen_String object concatenated with character string

inline CoolGen_String operator+ (const CoolGen_String& s, const char* c) {
  CoolGen_String temp(s);                       // temporary copy of s
  temp += c;                                    // mutate temp with += c
  return temp;                                  // return new copy by value
}


// operator+= -- Concatenation of another CoolGen_String: x += y;
// Input:        CoolGen_String reference
// Output:       CoolGen_String object concatenated with CoolGen_String contents

inline CoolGen_String& CoolGen_String::operator+= (const CoolGen_String& s) {
  return (strcat (*this, s));                   
}

// operator+ -- String concatenation of another string: z = x + y;
// Input:       CoolGen_String reference
// Output:      CoolGen_String object concatenated with String contents

inline CoolGen_String operator+ (const CoolGen_String& s1, const CoolGen_String& s2) {
  CoolGen_String temp(s1);                      // temporary copy of s1
  temp += s2;                                   // mutate temp with += s2
  return temp;                                  // return new copy by value
}



// operator== -- Test for equality of two CoolGen_String objects
// Input:        this*, CoolGen_String reference
// Output:       Boolean TRUE/FALSE

inline Boolean CoolGen_String::operator== (const CoolGen_String& s) const{
  return (is_equal (this->str, s.str, SENSITIVE));
}


// operator== -- Test for equality of an CoolGen_String and char*
// Input:        this*, char* pointer
// Output:       Boolean TRUE/FALSE

inline Boolean CoolGen_String::operator== (const char* c) const {
  return (is_equal (this->str, c, SENSITIVE));
}


// operator!= -- Test for inequality of two CoolGen_String objects
// Input:        this*, CoolGen_String reference
// Output:       Boolean TRUE/FALSE

inline Boolean CoolGen_String::operator!= (const CoolGen_String& s) const {
  return (is_not_equal (this->str, s.str, SENSITIVE));
}


// operator!= -- Test for inequality of two CoolGen_String objects
// Input:        this*, char* pointer
// Output:       Boolean TRUE/FALSE

inline Boolean CoolGen_String::operator!= (const char* c) const {
  return (is_not_equal (this->str, c, SENSITIVE));
}


// operator< -- Test for lexical ordering before a CoolGen_String
// Input:       this*, CoolGen_String reference
// Output:      Boolean TRUE/FALSE

inline Boolean CoolGen_String::operator< (const CoolGen_String& s) const {
  return (is_lt (this->str, s.str, SENSITIVE));
}


// operator< -- Test for lexical ordering before a CoolGen_String
// Input:       this*, char* pointer
// Output:      Boolean TRUE/FALSE

inline Boolean CoolGen_String::operator< (const char* c) const {
  return (is_lt (this->str, c, SENSITIVE));
}


// operator> -- Test for lexical ordering after a CoolGen_String
// Input:       this*, CoolGen_String reference
// Output:      Boolean TRUE/FALSE

inline Boolean CoolGen_String::operator> (const CoolGen_String& s) const {
  return (is_gt (this->str, s.str, SENSITIVE));
}


// operator> -- Test for lexical ordering after a CoolGen_String
// Input:       this*, char* pointer
// Output:      Boolean TRUE/FALSE

inline Boolean CoolGen_String::operator> (const char* c) const {
  return (is_gt (this->str, c, SENSITIVE));
}


// operator<= -- Test for lexical ordering before or equal to a CoolGen_String
// Input:        this*, CoolGen_String reference
// Output:       Boolean TRUE/FALSE

inline Boolean CoolGen_String::operator<= (const CoolGen_String& s) const {
  return (is_le (this->str, s.str, SENSITIVE));
}


// operator<= -- Test for lexical ordering before or equal to a CoolGen_String
// Input:        this*, char* pointer
// Output:       Boolean TRUE/FALSE

inline Boolean CoolGen_String::operator<= (const char* c) const {
  return (is_le (this->str, c, SENSITIVE));
}


// operator>= -- Test for lexical ordering after or equal to a CoolGen_String
// Input:        this*, String reference
// Output:       Boolean TRUE/FALSE

inline Boolean CoolGen_String::operator>= (const CoolGen_String& s) const {
  return (is_ge (this->str, s.str, SENSITIVE));
}


// operator>= -- Test for lexical ordering after or equal to a CoolGen_String
// Input:        this*, char* pointer
// Output:       Boolean TRUE/FALSE

inline Boolean CoolGen_String::operator>= (const char* c) const {
  return (is_ge (this->str, c, SENSITIVE));
}




// strlen -- Return the number of characters in the string
// Input:    CoolGen_String reference
// Output:   Length of the string

inline long strlen (const CoolGen_String& s) {
  return (s.length);
}


// atof -- Equivalent to strtod (str, (char**)END_OF_STRING)
// Input:  Reference to CoolGen_String object
// Output: Double representing value contained in string

inline double atof (const CoolGen_String& s) {
  return (strtod ((char *)s.str, (char **) END_OF_STRING));
}


// strncpy -- Returns s, with the first length characters of source copied
//            into it.  The old value of s is lost.
// Input   -- A reference to a CoolGen_String s, a char* source, and an int length.
// Output  -- The modified CoolGen_String s.

inline CoolGen_String& strncpy(CoolGen_String& s, const char* source, int n) {
  return strncpy(s, source, (long) n);
}


#endif                                          // End of GEN_STRINGH


