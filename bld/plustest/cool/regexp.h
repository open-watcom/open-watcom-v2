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
// Created: MNF 06/13/89 -- Initial Design and Implementation
// Updated: LGO 08/09/89 -- Inherit from Generic
// Updated: MBN 09/07/89 -- Added conditional exception handling
// Updated: MBN 12/15/89 -- Sprinkled "const" qualifiers all over the place!
// Updated: DLS 03/22/91 -- New lite version
// Updated: JAM 08/12/92 -- removed DOS specifics, stdized #includes
//
// This  is the header file  for the regular  expression class.   An object of
// this class contains a regular expression,  in  a special "compiled" format.
// This  compiled format consists  of  several slots   all kept as the objects
// private data.  The  Regexp class  provides a convenient  way  to  represent
// regular  expressions.  It makes it easy   to search  for  the  same regular
// expression in many different strings without having to  compile a string to
// regular expression format more than necessary.
//
// A regular  expression allows a programmer to  specify complex patterns that
// can be searched for  and  matched against the  character string of a String
// object.  In  its  simplest case, a   regular expression  is a  sequence  of
// characters with which you can search for exact character matches.  However,
// many times you may not know the exact sequence you want to find, or you may
// only want to find a match at the beginning or end of  a String.  The Regexp
// object  allows specification of  such patterns by  utilizing the  following
// regular  expression  meta-characters   (note   that  more  one  of    these
// meta-characters  can  be used in a single  regular  expression in  order to
// create complex search patterns):
//
//         ^    Match at beginning of line
//         $    Match at end of line
//         .    Match any single character
//         [ ]  Match any one character inside the brackets
//         [^ ] Match any character NOT inside the brackets
//         -    Match any character in range on either side of dash
//         *    Match preceding pattern zero or more times
//         +    Match preceding pattern one or more times
//         ?    Match preceding pattern zero or once only
//         ()   Save a matched expression and use it in a further match.
//
// There are three constructors for Regexp.  One  just creates an empty Regexp
// object.  Another creates a Regexp object  and initializes it with a regular
// expression  that is given  in  the form of a   char*.   The  third  takes a
// reference  to  a Regexp  object    as an  argument    and creates an object
// initialized with the information from the given Regexp object.
//
// The  find  member function  finds   the  first  occurence   of  the regualr
// expression of that object in the string given to find as an argument.  Find
// returns a boolean, and  if true,  mutates  the private  data appropriately.
// Find sets pointers to the beginning and end of  the thing last  found, they
// are pointers into the actual string  that was searched.   The start and end
// member functions return indicies  into the searched string that  correspond
// to the beginning   and  end pointers  respectively.   The    compile member
// function takes a char* and puts the  compiled version of the char* argument
// into the object's private data fields.  The == and  != operators only check
// the  to see  if   the compiled  regular  expression   is the same, and  the
// deep_equal functions also checks  to see if the  start and end pointers are
// the same.  The is_valid  function returns FALSE if  program is set to NULL,
// (i.e. there is no valid compiled exression).  The set_invalid function sets
// the  program to NULL  (Warning: this deletes the compiled  expression). The
// following examples may help clarify regular expression usage:
//
//   *  The regular expression  "^hello" matches  a "hello"  only at  the
//      beginning of a  line.  It would match "hello  there" but not "hi,
//      hello there".
//
//   *  The regular expression "long$" matches a  "long"  only at the end
//      of a line. It would match "so long\0", but not "long ago".
//
//   *  The regular expression "t..t..g"  will match anything that  has a
//      "t" then any two characters, another "t", any  two characters and
//      then a "g".   It will match  "testing", or "test again" but would
//      not match "toasting"
//
//   *  The regular  expression "[1-9ab]" matches any  number one through
//      nine, and the characters  "a" and  "b".  It would match "hello 1"
//      or "begin", but would not match "no-match".
//
//   *  The  regular expression "[^1-9ab]"  matches any character that is
//      not a number one  through nine, or  an "a" or "b".   It would NOT
//      match "hello 1" or "begin", but would match "no-match".
//
//   *  The regular expression "br* " matches  something that begins with
//      a "b", is followed by zero or more "r"s, and ends in a space.  It
//      would match "brrrrr ", and "b ", but would not match "brrh ".
//
//   *  The regular expression "br+ " matches something  that begins with
//      a "b", is followed by one or more "r"s, and ends in  a space.  It
//      would match "brrrrr ",  and  "br ", but would not  match "b  " or
//      "brrh ".
//
//   *  The regular expression "br? " matches  something that begins with
//      a "b", is followed by zero or one "r"s, and ends in  a space.  It
//      would  match  "br ", and "b  ", but would not match  "brrrr "  or
//      "brrh ".
//
//   *  The regular expression "(..p)b" matches  something ending with pb
//      and beginning with whatever the two characters before the first p
//      encounterd in the line were.  It would find  "repb" in "rep drepa
//      qrepb".  The regular expression "(..p)a"  would find "repa qrepb"
//      in "rep drepa qrepb"
//
//   *  The regular expression "d(..p)" matches something ending  with p,
//      beginning with d, and having  two characters  in between that are
//      the same as the two characters before  the first p  encounterd in
//      the line.  It would match "drepa qrepb" in "rep drepa qrepb".
//

#ifndef REGEXPH                                 // If no regular expression
#define REGEXPH

#ifndef CHARH
#include <cool/char.h>
#endif

#define MAGIC 0234
#define NSUBEXP 10

class CoolRegexp {
public:
  inline CoolRegexp ();                         // CoolRegexp with program=NULL
  inline CoolRegexp (const char*);              // CoolRegexp with compiled char*
  CoolRegexp (const CoolRegexp&);               // Copy constructor
  ~CoolRegexp();                                // Destructor 

  void compile (const char*);                   // Compiles char* --> regexp
  Boolean find (const char*);                   // TRUE if regexp in char* arg
  inline long start() const;                    // Index to start of first find
  inline long end() const;                      // Index to end of first find

  Boolean operator== (const CoolRegexp&) const; // Equality operator
  inline Boolean operator!= (const CoolRegexp&) const; // Inequality operator
  Boolean deep_equal (const CoolRegexp&) const; // Same regexp and state?
  
  inline Boolean is_valid() const;              // TRUE if compiled regexp
  inline void set_invalid();                    // Invalidates regexp

private: 
  const char* startp[NSUBEXP];
  const char* endp[NSUBEXP];
        char  regstart;                 // Internal use only
        char  reganch;                  // Internal use only
  const char* regmust;                  // Internal use only
        int   regmlen;                  // Internal use only
        char* program;   
        int   progsize;
  const char* searchstring;
}; 

// CoolRegexp -- Simple constructor
// Input:    None
// Output:   None

inline CoolRegexp::CoolRegexp () { 
  this->program = NULL;
}


// CoolRegexp -- Constructor that accepts a string argument and compiles as a
//           regular expression
// Input:    Character string form of regular expression
// Output:   None

inline CoolRegexp::CoolRegexp (const char* s) {  
  this->program = NULL;
  compile(s);
}


// Start -- Returns index into the searched string that corresponds to the
//          beginning pointer
// Input:   None
// Output:  Long index into searched string

inline long CoolRegexp::start () const {
  return(this->startp[0] - searchstring);
}


// End -- Returns index into the searched string that corresponds to the
//        end pointer
// Input: None
// Output: Long index into searched string

inline long CoolRegexp::end () const {
  return(this->endp[0] - searchstring);
}


// operator!= -- Overload inequality operator for CoolRegexp class
// Input:        Constance CoolRegexp reference
// Output:       Boolean TRUE/FALSE

inline Boolean CoolRegexp::operator!= (const CoolRegexp& r) const {
  return(!(*this == r));
}


// is_valid -- Indicates validity of regular expression, ie. is there a
//             compiled regular expression
// Input:      None
// Output:     Boolean TRUE/FALSE

inline Boolean CoolRegexp::is_valid () const {
  return (this->program != NULL);
}


// set_invalid -- Invalidates regular expression
// Input:         None
// Output:        None

inline void CoolRegexp::set_invalid () {
  delete [] this->program;
  this->program = NULL;
}

#endif
