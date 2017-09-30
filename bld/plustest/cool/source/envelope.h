// 
// Copyright (C) 1992 General Electric Company.  
// 
// Permission is granted to any individual or institution to use, copy, modify, 
// and distribute this software, provided that this complete copyright and 
// permission notice is maintained, intact, in all copies and supporting
// documentation.  
// 
// General Electric Company 
// provides this software "as is" without express or implied warranty.  
// 
// Created: VDN 05/13/92 -- Initial design with ATT.
// Updated: JAM 08/11/92 -- removed 'inline' from friend declarations
// Updated: JAM 08/12/92 -- added 'inline' to shallow_swap definition
//                          (probably just typo)
// Updated: JAM 08/18/92 -- (optionally) made macros/CoolEnvelope into
//                          a template instead of requiring user to
//                          #define CoolLetter; made shallow_swap() static
//                          and public (so operators not friends)
// Updated: JAM 09/28/92 -- removed operator CoolLetter() conversions
//                          since not necessary because publicly derived
//                          
//
// This envelope is a generic template, has no data, publicly inherits
// from the letter, and wraps outside a letter so that we can redefine 
// the copy constructor to do a shallow copy instead of a deep copy,
// when a return by value is required. 
//
// Such returns by value occur for example in: 
// Matrix a, b, c; 
// c = (a + b) + c; 
// Note that the sum of a and b, (a+b), has to be deleted by the compiler,
// and this requires (a+b) being returned by value. Return by value will
// call the copy constructor of matrix, and so does a deep copy. 
// There is one more copy at the assignment =. 
// Note that the compiler generates temporary handles to the objects returned
// by value and delete these objects when the current scope is exited.
//
// Arithmetic operations such as: +, -, *, /, %, and logical operations such 
// as: |, &, ^ are overloaded to take envelopes, to avoid unecessary copying. 
// Arithmetic, logical, shift operations are assumed non symmetric in general.
//
// Sometimes, the mutators +=, -=, ..., can be done without temporary memory,
// such as matrix addition and negation, string concatenation, etc..
// In these cases, use the flag ENVELOPE_* to generate versions of +, 
// based on +=, etc... 
#ifdef CoolLetter    // old way --Jam
// Note that, the envelope is a macro rather than a parameterized
// template, to allow expansion of member functions as needed.
#endif
//
// This is an implementation of Coplien's envelope-letter idiom, 
// that do not require reimplementation of the letter's member functions 
// at the envelope level.
// References: 
// 1. Coplien (1992) Advanced C++ programming styles and idioms. 
//    Addison-Wesley.


#ifdef CoolLetter    // if old user

// Remember to include the letter class before this point.
// #include <cool/CoolLetter.h>

class CoolEnvelope : public CoolLetter {        // Inherit all from class CoolLetter
friend class CoolLetter;                        

public:                                         
  inline CoolEnvelope();                        // Empty Constructor
  inline CoolEnvelope(CoolEnvelope&);           // Copy constructor
  inline ~CoolEnvelope();                       // Destructor

  inline operator CoolLetter& ();               // Conversion to letter
  inline operator CoolLetter& () const;         

  friend ostream& operator<< (ostream&, const CoolEnvelope&); 
  friend ostream& operator<< (ostream&, const CoolEnvelope*);

  // should be defined as member function of Letter class
  // friend CoolLetter& operator= (CoolLetter&, CoolEnvelope&);

  // Defining operator + based on += with these macros, 
  // if and only if += can be done in place, like += of matrices.

#define DECLARE_ENVELOPE_OPERATOR(op)                                            \
  friend CoolEnvelope operator op (const CoolLetter&, const CoolLetter&); \
  friend CoolEnvelope operator op (const CoolLetter&, const CoolEnvelope&);\
  friend CoolEnvelope& operator op (CoolEnvelope&, const CoolLetter&);   \
  friend CoolEnvelope& operator op (CoolEnvelope&, const CoolEnvelope&); 

  // Arithmetic operators
#ifdef ENVELOPE_PLUS                            // iff operator+= can be in place 
  DECLARE_ENVELOPE_OPERATOR(+)
#endif
#ifdef ENVELOPE_MINUS                           // iff operator-= can be in place 
  DECLARE_ENVELOPE_OPERATOR(-)
#endif
#ifdef ENVELOPE_STAR                            // iff operator*= can be in place 
  DECLARE_ENVELOPE_OPERATOR(*)
#endif
#ifdef ENVELOPE_SLASH                           // iff operator/= can be in place 
  DECLARE_ENVELOPE_OPERATOR(/)
#endif
#ifdef ENVELOPE_PERCENT                         // iff operator%= can be in place 
  DECLARE_ENVELOPE_OPERATOR(%)
#endif

  // Logical operators
#ifdef ENVELOPE_VERTICAL                        // iff operator|= can be in place
  DECLARE_ENVELOPE_OPERATOR(|)
#endif
#ifdef ENVELOPE_AMPERSAND                       // iff operator&= can be in place
  DECLARE_ENVELOPE_OPERATOR(&)
#endif
#ifdef ENVELOPE_CARET                           // iff operator^= can be in place
  DECLARE_ENVELOPE_OPERATOR(^)
#endif

  // Shift operators
#ifdef ENVELOPE_DOUBLE_LEFT_BRACKET             // iff operator<<= can be in place
  DECLARE_ENVELOPE_OPERATOR(<<) 
#endif
#ifdef ENVELOPE_DOUBLE_RIGHT_BRACKET            // iff operator>>= can be in place
  DECLARE_ENVELOPE_OPERATOR(>>)
#endif

#undef DECLARE_ENVELOPE_OPERATOR                

protected:
  inline void shallow_swap (CoolEnvelope*, CoolEnvelope*);
};                                              

// Envelope() -- Empty constructor creates an envelope wrapping an empty letter
// Input:     none
// Ouput:     envelope reference

inline CoolEnvelope::CoolEnvelope() 
: CoolLetter()                                  // create empty letter inside
{}                                              // nothing for envelope


// Envelope() -- Copy constructor, swapping the contents of letter over.
// Input:     envelope wraping a non null letter
// Ouput:     envelope with contents of letter being swapped over.

inline CoolEnvelope::CoolEnvelope(CoolEnvelope& env) {
  this->shallow_swap (this, &env);              // swap contents over
}

// ~Envelope() -- Destructor, destroy letter too if non null.
// Input:     none
// Output:    none

inline CoolEnvelope::~CoolEnvelope() {}         // delete letter by inherit.


// operator Letter&() -- Automatic conversion to letter, if necessary.
// Input:     envelope reference
// Ouput:     letter reference

inline CoolEnvelope::operator CoolLetter& () {
  return *((CoolLetter*) this);                 // same physical entity.
}

inline CoolEnvelope::operator CoolLetter& () const {
  return *((CoolLetter*) this);                 // const version of above
}


// operator=  -- Assignment from a envelope back to real letter.
//               Should be defined in CoolLetter class, not here.
// Input:     envelope reference
// Output:    letter reference with contents in envelope being swapped over
// 
// inline CoolLetter& operator= (CoolLetter& let, CoolEnvelope& env) {
//   shallow_swap ((CoolEnvelope*) &let, &env);  // same physical layout
//   return let;
// }            

// shallow_swap() -- Swap contents by doing shallow copy of bytes.
// Input:    pointer to two envelopes
// Output:   none, contents of envelopes are swapped.

inline void CoolEnvelope::shallow_swap (CoolEnvelope* env1, CoolEnvelope* env2) {
  int n = sizeof(CoolEnvelope);                 // n = sizeof(CoolLetter)
  char* temp = new char[n];                     // temporary space for swap
  char* env1_contents = (char*) env1;           // copy n bytes starting from ptr
  char* env2_contents = (char*) env2;   
  memcpy(temp, env1_contents, n);               // shallow swap the contents of
  memcpy(env1_contents, env2_contents, n);      // env1 and env2
  memcpy(env2_contents, temp, n);
  delete [] temp;                               // free temp space for swap
}

// operator<<  -- Overload output operator to print envelope
// Input:    os, envelope reference
// Output:   os

inline ostream& operator<< (ostream& os, const CoolEnvelope& env) {
  return os << *((CoolLetter*) &env);
}

// operator<<  -- Overload output operator to print envelope
// Input:    os, envelope pointer
// Output:   os

inline ostream& operator<< (ostream& os, const CoolEnvelope* env) {
  return os << *((CoolLetter*) env);
}


// operator op -- Use operator op_equal, and return an envelope by value
//                so that deep copy of the object is avoided.
//                For generality, operations are assumed non symmetric.
// Input:    type reference, envelope reference, all 4 permutations.
// Output:   envelope returned by value


#define IMPLEMENT_ENVELOPE_OPERATOR(op,op_equal)                              \
inline CoolEnvelope operator op (const CoolLetter& arg1, const CoolLetter& arg2) {\
  CoolLetter temp(arg1);                        /*Deep copy of arg1*/         \
  temp op_equal arg2;                           /*Mutate with op arg2*/       \
  CoolEnvelope& result = *((CoolEnvelope*) &temp);/*Same physical object*/    \
  return result;                                /*Copy envelope only*/        \
}                                                                             \
                                                                              \
inline CoolEnvelope operator op (const CoolLetter& arg1, const CoolEnvelope& arg2){\
  CoolLetter temp(arg1);                        /*Deep copy of arg1*/         \
  temp op_equal *((CoolLetter*) &arg2);         /*Mutate with op arg2*/       \
  CoolEnvelope& result = *((CoolEnvelope*) &temp);/*Same physical object*/    \
  return result;                                /*Copy envelope only*/        \
}                                                                             \
                                                                              \
inline CoolEnvelope& operator op (CoolEnvelope& arg1, const CoolLetter& arg2) {\
  CoolLetter& temp = *((CoolLetter*) &arg1);    /*Reuse arg1*/                \
  temp op_equal arg2;                           /*Mutate in place*/           \
  return arg1;                                  /*Envelope not copied*/       \
}                                                                             \
                                                                              \
inline CoolEnvelope& operator op (CoolEnvelope& arg1, const CoolEnvelope& arg2) {\
  CoolLetter& temp = *((CoolLetter*) &arg1);    /*Reuse arg1*/                \
  temp op_equal *((CoolLetter*) &arg2);         /*Mutate in place*/           \
  return arg1;                                  /*Envelope not copied*/       \
}                                                                             

  // Arithmetic operators:
#ifdef ENVELOPE_PLUS                            // iff operator+= can be in place
  IMPLEMENT_ENVELOPE_OPERATOR(+,+=)
#endif

#ifdef ENVELOPE_MINUS                           // iff operator-= can be in place
  IMPLEMENT_ENVELOPE_OPERATOR(-,-=)
#endif
#ifdef ENVELOPE_STAR                            // iff operator*= can be in place
  IMPLEMENT_ENVELOPE_OPERATOR(*,*=)
#endif
#ifdef ENVELOPE_SLASH                           // iff operator/= can be in place
  IMPLEMENT_ENVELOPE_OPERATOR(/,/=)
#endif
#ifdef ENVELOPE_PERCENT                         // iff operator%= can be in place
  IMPLEMENT_ENVELOPE_OPERATOR(%,%=)
#endif

  // Logical operators:
#ifdef ENVELOPE_VERTICAL                        // iff operator|= can be in place
  IMPLEMENT_ENVELOPE_OPERATOR(|,|=)
#endif
#ifdef ENVELOPE_AMPERSAND                       // iff operator&= can be in place
  IMPLEMENT_ENVELOPE_OPERATOR(&,&=)
#endif
#ifdef ENVELOPE_CARET                           // iff operator^= can be in place
  IMPLEMENT_ENVELOPE_OPERATOR(^,^=)
#endif

  // Shift operators:
#ifdef ENVELOPE_DOUBLE_LEFT_BRACKET             // iff operator<<= can be in place
  IMPLEMENT_ENVELOPE_OPERATOR(<<,<<=)
#endif
#ifdef ENVELOPE_DOUBLE_RIGHT_BRACKET            // iff operator>>= can be in place
  IMPLEMENT_ENVELOPE_OPERATOR(>>,>>=)
#endif

#undef IMPLEMENT_ENVELOPE_OPERATOR              // Delete macro definition

#else    // new way: CoolEnvelope as class template

#ifndef CoolEnvelope_H
#define CoolEnvelope_H

template<class CoolLetter>
class CoolEnvelope : public CoolLetter {        // Inherit all from class CoolLetter
friend class CoolLetter;                        

public:                                         
  /*inline##*/ CoolEnvelope();                  // Empty Constructor
  /*inline##*/ CoolEnvelope(CoolEnvelope<CoolLetter>&);         // Copy constructor
  /*inline##*/ ~CoolEnvelope();                 // Destructor

  template< class U >
  friend ostream& operator<< (ostream&, const CoolEnvelope<U>&); 
  template< class U >
  friend ostream& operator<< (ostream&, const CoolEnvelope<U>*);

  // should be defined as member function of Letter class
  // friend CoolLetter& operator= (CoolLetter&, CoolEnvelope&);

  static inline void shallow_swap (CoolEnvelope*, CoolEnvelope*);
protected:
};

// Envelope() -- Empty constructor creates an envelope wrapping an empty letter
// Input:     none
// Ouput:     envelope reference

template<class CoolLetter>
inline CoolEnvelope<CoolLetter>::CoolEnvelope() 
: CoolLetter()                                  // create empty letter inside
{}                                              // nothing for envelope


// Envelope() -- Copy constructor, swapping the contents of letter over.
// Input:     envelope wraping a non null letter
// Ouput:     envelope with contents of letter being swapped over.

template<class CoolLetter>
inline CoolEnvelope<CoolLetter>::CoolEnvelope(CoolEnvelope<CoolLetter>& env) {
  this->shallow_swap (this, &env);              // swap contents over
}

// ~Envelope() -- Destructor, destroy letter too if non null.
// Input:     none
// Output:    none

template<class CoolLetter>
inline CoolEnvelope<CoolLetter>::~CoolEnvelope() {}             // delete letter by inherit.


// operator=  -- Assignment from a envelope back to real letter.
//               Should be defined in CoolLetter class, not here.
// Input:     envelope reference
// Output:    letter reference with contents in envelope being swapped over
// 
// inline CoolLetter& operator= (CoolLetter& let, CoolEnvelope& env) {
//   shallow_swap ((CoolEnvelope*) &let, &env);  // same physical layout
//   return let;
// }            

// shallow_swap() -- Swap contents by doing shallow copy of bytes.
// Input:    pointer to two envelopes
// Output:   none, contents of envelopes are swapped.

template<class CoolLetter>
inline void CoolEnvelope<CoolLetter>::shallow_swap (CoolEnvelope<CoolLetter>* env1, CoolEnvelope<CoolLetter>* env2) {
  int n = sizeof(CoolEnvelope<CoolLetter>);                     // n = sizeof(CoolLetter)
  char* temp = new char[n];                     // temporary space for swap
  char* env1_contents = (char*) env1;           // copy n bytes starting from ptr
  char* env2_contents = (char*) env2;   
  memcpy(temp, env1_contents, n);               // shallow swap the contents of
  memcpy(env1_contents, env2_contents, n);      // env1 and env2
  memcpy(env2_contents, temp, n);
  delete [] temp;                               // free temp space for swap
}

// operator<<  -- Overload output operator to print envelope
// Input:    os, envelope reference
// Output:   os

template<class CoolLetter>
inline ostream& operator<< (ostream& os, const CoolEnvelope<CoolLetter>& env) {
  return os << *((CoolLetter*) &env);
}

// operator<<  -- Overload output operator to print envelope
// Input:    os, envelope pointer
// Output:   os

template<class CoolLetter>
inline ostream& operator<< (ostream& os, const CoolEnvelope<CoolLetter>* env) {
  return os << *((CoolLetter*) env);
}

// operator op -- Use operator op_equal, and return an envelope by value
//                so that deep copy of the object is avoided.
//                For generality, operations are assumed non symmetric.
// Input:    type reference, envelope reference, all 4 permutations.
// Output:   envelope returned by value


#define PASTE_(x,y) x##y
#define PASTE(x,y) PASTE_(x,y)
#define CoolEnvOp(name) PASTE(PASTE(CoolEnvelope,_),name)
//## Should make 'name' a member func of CoolEnvelope instead, but
// BC++ template bug expands out all member funcs even if never used

// Define for each operator OP that can be done in place:
//inline CoolEnvelope<CoolLetter> operator OP (const CoolLetter&arg1, const CoolLetter&arg2)
//   { return CoolEnvOp(NAME)(arg1, arg2); }

#define DEFINE_ENVELOPE_OPERATOR(name,op,op_equal)                            \
template<class CoolLetter> \
inline CoolEnvelope<CoolLetter> CoolEnvOp(name) (const CoolLetter& arg1, const CoolLetter& arg2) {\
  CoolLetter temp(arg1);                        /*Deep copy of arg1*/         \
  temp op_equal arg2;                           /*Mutate with op arg2*/       \
  CoolEnvelope<CoolLetter>& result = *((CoolEnvelope<CoolLetter>*) &temp);/*Same physical object*/    \
  return result;                                /*Copy envelope only*/        \
}                                                                             \
                                                                              \
template<class CoolLetter> \
inline CoolEnvelope<CoolLetter> operator op (const CoolLetter& arg1, const CoolEnvelope<CoolLetter>& arg2) {\
  CoolLetter temp(arg1);                        /*Deep copy of arg1*/         \
  temp op_equal *((CoolLetter*) &arg2);         /*Mutate with op arg2*/       \
  CoolEnvelope<CoolLetter>& result = *((CoolEnvelope<CoolLetter>*) &temp);/*Same physical object*/    \
  return result;                                /*Copy envelope only*/        \
}                                                                             \
                                                                              \
template<class CoolLetter> \
inline CoolEnvelope<CoolLetter>& operator op (CoolEnvelope<CoolLetter>& arg1, const CoolLetter& arg2) {\
  CoolLetter& temp = *((CoolLetter*) &arg1);    /*Reuse arg1*/                \
  temp op_equal arg2;                           /*Mutate in place*/           \
  return arg1;                                  /*Envelope not copied*/       \
}                                                                             \
                                                                              \
template<class CoolLetter> \
inline CoolEnvelope<CoolLetter>& operator op (CoolEnvelope<CoolLetter>& arg1, const CoolEnvelope<CoolLetter>& arg2) {\
  CoolLetter& temp = *((CoolLetter*) &arg1);    /*Reuse arg1*/                \
  temp op_equal *((CoolLetter*) &arg2);         /*Mutate in place*/           \
  return arg1;                                  /*Envelope not copied*/       \
} \

  // Arithmetic operators:
DEFINE_ENVELOPE_OPERATOR(add,+,+=)
DEFINE_ENVELOPE_OPERATOR(minus,-,-=)
DEFINE_ENVELOPE_OPERATOR(star,*,*=)
DEFINE_ENVELOPE_OPERATOR(slash,/,/=)
DEFINE_ENVELOPE_OPERATOR(percent,%,%=)

  // Logical operators:
DEFINE_ENVELOPE_OPERATOR(vertical,|,|=)
DEFINE_ENVELOPE_OPERATOR(ampersand,&,&=)
DEFINE_ENVELOPE_OPERATOR(caret,^,^=)

  // Shift operators:
DEFINE_ENVELOPE_OPERATOR(double_left_bracket,<<,<<=)
DEFINE_ENVELOPE_OPERATOR(double_right_bracket,>>,>>=)

#undef DEFINE_ENVELOPE_OPERATOR         // Delete macro definition


#endif // CoolEnvelope_H

#endif // #ifdef CoolLetter

