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
// Created: MBN 10/24/89 -- Initial design and implementation
// Updated: MJF 07/31/90 -- Added terse print
// Updated: DLS 03/22/91 -- New lite version
// Updated: VDN 05/01/92 -- intializer needed to create Bignums for .so libs
// Updated: JAM 08/11/92 -- removed 'inline' from friend declarations
//
// The Bignum class implements infinite precision integer arithmetic.  A Bignum
// object grows and  shrinks in size as  necessary to  accommodate its  current
// value.  All of the  standard C operations defined on  integers  in Kernighan
// and Ritchie,   Appendix  A carry  over  directly  to  Bignums   by means  of
// overloading,  except for those  operations that  make assumptions  about the
// underlying representation.   For  example, we have  not  implemented bitwise
// operators &,|,^ because they would have to make assumptions about our Bignum
// implementation.  On the other  hand,  we have,  for example  implemented the
// operators   << and   >>,  because   these can   be always   be   interpreted
// arithmetically  as multiplication  or  division   by 2, respectively, of the
// underlying integer type.  The four standard arithmetic operations +,-,*, and
// / utilize algorithms from Knuth, Volume 2 for efficiency.  However, the user
// is warned that the  Bignum integer  arithmetic class  is  still considerably
// slower than the built-in integer data types.
//
// The  Bignum class  implements   common  arithmetic exception  handling   and
// provides  the  application  with support  for  detecting negative  infinity,
// positive infinity, overflow,  and underflow as a result  of some  arithmetic
// expression. If  one of these   conditions or an attempt  to  convert from  a
// Bignum with no value to  a built-in type  is detected, an Error exception is
// raised. The application programmer can provide an exception handler  to take
// care of this problem. If  no such handler  is available, an error message is
// printed and the application terminates.
//
// The Bignum class requires several constants be defined to  insure precision
// and  accuracy of conversion.   The preprocessor  symbols  MINSHORT, MININT,
// MINLONG, MAXSHORT, MAXINT, and MAXLONG  are calculated in the <COOL/misc.h>
// header file via  various  bit manipulation  macros. The  symbols  MINFLOAT,
// MINDOUBLE,  MAXFLOAT,  and MAXDOUBLE  are system   dependent  and cannot be
// calculated.  Most systems typically have values for these constants  in the
// system  header file <values.h>.  Values  for a  specific machine  should be
// copied into the <COOL/misc.h> header file as necessary.
//
// The  private  data section of  the Bignum class  has a pointer to storage of
// consecutive short  blocks large enough  to  hold the  value.  The number of
// blocks is  contained in  a  private integer  slot.   The Bignum  class  also
// contains a private data slot providing  arithmetic  exception status.  There
// are  five constructors for the  Bignum class.  The first  is a simple inline
// constructor  that initializes the state and  private data slots.  The second
// takes an integer (short, int, or long)  and uses that  as  the initial value
// for the object.  The third takes a real  (float or double)  and uses that as
// the  initial   value for the object.    The  fourth takes  a null-termianted
// character string  in either octal, decimal, hex, or exponential format   and
// converts it  to the appropriate  Bignum value.   Finally,  the fifth takes a
// const reference to another Bignum object and duplicates its state and value.
// The Bignum class provides overloaded  operators  for addition,  subtraction,
// multiplication,  division, and  modulus.  Also  available  are equality  and
// inequality,  assignment, increment, decrement, unary minus, ones-complement,
// output,  less than, greater than,  less than  or equal, and  greater than or
// equal.  Finally, five type conversion functions to  short, int, long, float,
// and double are provided.
//
// Use envelope to turn a deep copy into a shallow copy, when Bignums are
// returned by value from operations like +,-,*,/,%,<<,>>.
//
// WARNINGS
// JAM -- make code super portable/correct and doesn't assume anything
//        about data types other than char<=short<=int<=long
//

#ifndef BIGNUMH                                 // If no definition for Bignum
#define BIGNUMH                                 // define the bignum symbol

#ifndef REGEXPH                                 // If no regular expressions
#include <cool/Regexp.h>                        // include definition
#endif


typedef unsigned int Counter;
typedef unsigned short Data;

class CoolBignumE;                              // forward dec. of envelope

class CoolBignum {
public:
  // CoolBignum constructors
  CoolBignum ();                                // Void constructor
  CoolBignum (const long);                      // Long constructor
  CoolBignum (const char*);                     // String constructor
  CoolBignum (const double);                    // Double constructor
  CoolBignum (const CoolBignum&);               // Copy constructor 
  ~CoolBignum ();                               // Destructor

  // Conversion operators
  operator short () const;                      // Implicit type conversion
  operator int () const;                        // Implicit type conversion
  operator long () const;                       // Implicit type conversion
  operator float () const;                      // Implicit type conversion
  operator double () const;                     // Implicit type conversion

  // Overloaded operators
  CoolBignumE operator- () const;               // Unary minus operator
  inline Boolean operator! () const;            // Not operator

  CoolBignum& operator= (const CoolBignum&);    // Assignment operator
  inline CoolBignum& operator=(CoolBignumE&);   // From envelope back to Bignum

  friend CoolBignumE operator+ (const CoolBignum&, const CoolBignum&); // Plus operator
  friend CoolBignumE operator- (const CoolBignum&, const CoolBignum&); // Minus operator
  friend CoolBignumE operator* (const CoolBignum&, const CoolBignum&); // Multiplication op
  friend CoolBignumE operator/ (const CoolBignum&, const CoolBignum&); // Division operator
  friend CoolBignumE operator% (const CoolBignum&, const CoolBignum&); // Modulus operator
  friend CoolBignumE operator<< (const CoolBignum&, long l); // Arithmetic left shift
  friend CoolBignumE operator>> (const CoolBignum&, long l); // Arithmetic right shift
  inline CoolBignum& operator+= (const CoolBignum&);    // plus/assign
  inline CoolBignum& operator-= (const CoolBignum&);    // minus/assign
  inline CoolBignum& operator*= (const CoolBignum&);    // multiply/assign
  inline CoolBignum& operator/= (const CoolBignum&);    // division/assign
  inline CoolBignum& operator%= (const CoolBignum&);    // modulus/assign
  inline CoolBignum& operator<<= (const CoolBignum&);   // left shift/assign
  inline CoolBignum& operator>>= (const CoolBignum&);   // right shift/assign
  
  CoolBignum& operator++ ();                    // increment
  CoolBignum& operator-- ();                    // decrement

  Boolean operator== (const CoolBignum&) const; // equality
  inline Boolean operator!= (const CoolBignum&) const; // inequality
  Boolean operator< (const CoolBignum&) const;  // less than
  inline Boolean operator<= (const CoolBignum&) const; // less than/equal
  Boolean operator> (const CoolBignum&) const;  // greater than
  inline Boolean operator>= (const CoolBignum&) const; // greater than/eq.

  friend ostream& operator<< (ostream&, const CoolBignum&);     // Output reference
  friend ostream& operator<< (ostream&, const CoolBignum*); // Output ptr
  
  inline N_status status () const;              // Return CoolBignum status
  void dump (ostream& = cout);                  // Dump contents of CoolBignum
  void print (ostream&);                        // terse print

protected:
  void minus_infinity (const char*) const;      // Raise - infinity exception
  void plus_infinity (const char*) const;       // Raise + infinity exception
  void overflow (const char*) const;            // Raise overflow error
  void underflow (const char*) const;           // Raise overflow error
  void no_conversion (const char*) const;       // Raise no conversion error
  void divide_by_zero (const char*) const;      // Raise divide by zero

private:
  Counter count;                                // Number of data elements
  int sign;                                     // Sign of CoolBignum (+,-,or 0)
  Data* data;                                   // Pointer to data value
  N_status state;                               // Exception status

  void xtoBignum (const char *s);               // convert hex to CoolBignum
  int  dtoBignum (const char *s);               // convert decimal to CoolBignum
  void otoBignum (const char *s);               // convert octal to CoolBignum
  void exptoBignum (const char *s);             // convert exponential to Big.

  friend int magnitude_cmp (const CoolBignum&, const CoolBignum&); // Compare mags
  friend void add (const CoolBignum&, const CoolBignum&, CoolBignum&); // Unsigned add
  friend void subtract (const CoolBignum&, const CoolBignum&, CoolBignum&); // Unsigned sub
  friend void multiply_aux (const CoolBignum&, Data d, CoolBignum&, Counter i); 
  friend Data normalize (const CoolBignum&, const CoolBignum&,
                         CoolBignum&, CoolBignum&);     // Normalize for division
  friend void divide_aux (const CoolBignum&, Data, CoolBignum&, Data&); // Divide digit
  friend Data estimate_q_hat (const CoolBignum&, const CoolBignum&, Counter); // Est quo
  friend Data multiply_subtract (CoolBignum&, const CoolBignum&, Data q_hat,
                                Counter);       // Multiply quotient and subt.
  friend void divide (const CoolBignum&, const CoolBignum&, CoolBignum&, CoolBignum&); // Divide
  void resize (Counter);                        // Resize CoolBignum data
  CoolBignum& trim ();                          // Trim CoolBignum data
  friend CoolBignumE left_shift (const CoolBignum& b1, long l);
  friend CoolBignumE right_shift (const CoolBignum& b1, long l);
};


// Avoid deep copy with envelope. 
// None of the operations +=... can be done in place

#define CoolLetter CoolBignum
#define CoolEnvelope CoolBignumE                

#include <cool/Envelope.h>                      // Include envelope macros

#undef CoolLetter
#undef CoolEnvelope


// Need and initializer to construct static bignums.

class CoolBignum_Init {
public:
  CoolBignum_Init ();                           // Create constant Bignums
  ~CoolBignum_Init();                           // and destroy them once only
private:
  static int count;                             // number of init objects created
};

static CoolBignum_Init bignum_init1_s;          // trigger init of Bignums


// 

// status -- returns status of a CoolBignum
// Inputs:  none
// Outputs:  status of this CoolBignum
inline N_status CoolBignum::status () const {
  return this->state;                           // Return this->state
}

// operator! -- overloaded not operator for CoolBignums
// Inputs:  none
// Outputs:  Boolean not of this CoolBignum
inline Boolean CoolBignum::operator! () const {
  return (Boolean) (this->count == 0);          // Return 1 if CoolBignum's zero
}

// operator=  -- Assignment from an envelope back to real Bignum
// Input:     envelope reference
// Output:    Bignum reference with contents in envelope being swapped over

inline CoolBignum& CoolBignum::operator= (CoolBignumE& env){
  env.shallow_swap((CoolBignumE*)this, &env);   // same physical layout
  return *this;
}

// operator- -- overloaded subtraction operator for CoolBignums
// Inputs:  references to two CoolBignums
// Outputs:  the CoolBignum difference of the two input CoolBignums

inline CoolBignumE operator- (const CoolBignum& b1, const CoolBignum& b2) {
  return (CoolBignumE &) (b1 + (-b2));                          // negate b2 and add to b1
}


// operator!= -- overloaded != comparison operator for CoolBignums
// Inputs:  reference to CoolBignum
// Outputs:  Boolean result of the comparison
inline Boolean CoolBignum::operator!= (const CoolBignum& b) const {
  return !(*this == b);                         // call CoolBignum operator==
}



// operator<= -- overloaded <= operator for CoolBignums
// Inputs:  reference to CoolBignum
// Outputs:  Boolean result of <= comparison
inline Boolean CoolBignum::operator<= (const CoolBignum& b) const {
  return !(*this > b);                          // call CoolBignum operator>
}



// operator>= -- overloaded >= operator for CoolBignums
// Inputs:  reference to CoolBignum
// Outputs:  Boolean result of >= comparison
inline Boolean CoolBignum::operator>= (const CoolBignum& b) const {
  return !(*this < b);                  
}



// operator+= -- overloaded addition assignment operator for CoolBignums
// Inputs:  reference to CoolBignum
// Outputs:  reference to modified CoolBignum
inline CoolBignum& CoolBignum::operator+= (const CoolBignum& b) {
  *this = *this + b;                            // call CoolBignum operator+
  return *this;
}



// operator-= -- overloaded subtraction assignment operator for CoolBignums
// Inputs:  reference to CoolBignum
// Outputs:  reference to modified CoolBignum
inline CoolBignum& CoolBignum::operator-= (const CoolBignum& b) {
  *this = *this - b;                            // call CoolBignum operator-
  return *this;
}



// operator*= -- overloaded multiplication assignment operator for CoolBignums
// Inputs:  reference to CoolBignum
// Outputs:  reference to modified CoolBignum
inline CoolBignum& CoolBignum::operator*= (const CoolBignum& b) {
  *this = *this * b;                            // call CoolBignum operator*
  return *this;
}



// operator/= -- overloaded division assignment operator for CoolBignums
// Inputs:  reference to CoolBignum
// Outputs:  reference to modified CoolBignum
inline CoolBignum& CoolBignum::operator/= (const CoolBignum& b) {
  *this = *this / b;                            // call CoolBignum operator/
  return *this;
}



// operator%= -- overloaded modulus assignment operator for CoolBignums
// Inputs:  reference to CoolBignum
// Outputs:  reference to modified CoolBignum
inline CoolBignum& CoolBignum::operator%= (const CoolBignum& b) {
  *this = *this % b;                            // call CoolBignum operator%
  return *this;
}



// operator<<= -- overloaded left shift assignment operator for CoolBignums
// Inputs:  reference to CoolBignum
// Outputs:  reference to modified CoolBignum
inline CoolBignum& CoolBignum::operator<<= (const CoolBignum& b) {
  *this = *this << long(b);                     // call CoolBignum operator<<
  return *this;
}



// operator>>= -- overloaded right shift assignment operator for CoolBignums
// Inputs:  reference to CoolBignum
// Outputs:  reference to modified CoolBignum
inline CoolBignum& CoolBignum::operator>>= (const CoolBignum& b) {
  *this = *this >> long(b);                     // call CoolBignum operator>>
  return *this;
}



// operator<< -- output operator for pointers to CoolBignums
inline ostream& operator<< (ostream& os, const CoolBignum* b) {
  if (b) os << *b;                              // call CoolBignum output operator
  return os;                                    
}

#endif

