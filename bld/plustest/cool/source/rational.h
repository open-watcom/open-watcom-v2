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
// Created: MBN 10/25/89 -- Initial design and implementation
// Updated: MBN 03/04/90 -- Added execption for DIVIDE_BY_ZERO
// Updated: MJF 07/31/90 -- Added terse print
// Updated: DLS 04/01/91 -- New lite version
// Updated: JAM 08/10/92 -- removed 'inline' from friend declarations
// Updated: JAM 08/11/92 -- removed DOS specifics, stdized #includes
//
// The Rational class  implements rational numbers  and arithmetic.  A Rational
// object has the same precision and range of values as the built-in type long.
// Implicit conversion to the system defined types short, int, long, float, and
// double is supported by  overloaded  operator member functions.  Although the
// Rational class makes judicous use  of inline  functions and  deals only with
// integral values, the user  is warned that  the Rational  integer  arithmetic
// class is still considerably slower than the built-in  integer data types. If
// the range  of values  anticipated will  fit into a  built-in  type, use that
// instead.
//
// The  Rational class implements   common  arithmetic exception  handling  and
// provides  the  application  with  support for  detecting  negative infinity,
// positive  infinity,  overflow, and underflow  as a result of some arithmetic
// expression.  If  one of  these conditions or  an  attempt to  convert from a
// Rational with no value to a built-in type is detected, an Error exception is
// raised. The application programmer can provide an  exception handler to take
// care of this problem. If  no such handler is  available, an error message is
// printed and the application terminates.
//
// The Rational class requires several constants be defined to insure precision
// and accuracy   of conversion.  The   preprocessor  symbols MINSHORT, MININT,
// MINLONG, MAXSHORT, MAXINT, and MAXLONG  are calculated  in the <misc.h>
// header file  via various  bit  manipulation  macros. The  symbols  MINFLOAT,
// MINDOUBLE,  MAXFLOAT,  and MAXDOUBLE  are  system dependent  and cannot   be
// calculated.  Most systems typically have  values  for these constants in the
// system header  file <values.h>.  Values  for a  specific  machine should  be
// copied into the <misc.h> header file as necessary.
//
// The private data section of the Rational class contains two long data slots,
// one for the numerator and one for the denominator.   The Rational class also
// contains a private  data slot  providing arithmetic exception status.  There
// are five constructors for the Rational class.  The first is  a simple inline
// constructor that initializes the state and private  data slots.   The second
// takes two integers (short, int, or long) and uses them as the  initial value
// for  the  object.   The second  argument is  optional  and if  not supplied,
// defaults to one.  The third  takes two longs  and  uses  them as the initial
// value for the object.  The second argument is optional and  if not supplied,
// defaults to one.  The  fourth takes a  double  and  calculates an equivalent
// normalized rational.  Finally, the fifth takes  a const reference to another
// Rational object and duplicates its state and value.
//
// The Rational class  provides overloaded operators for addition, subtraction,
// multiplication,  division, and  modulus.  Also  available are inequality and
// equality,  assignment, increment, decrement,  unary  minus, ones-complement,
// output, less than, greater than,  less than or  equal, and greater  than  or
// equal.  Methods to get the numerator  and denominator, invert, calculate the
// floor  and ceiling, truncate,  and round are  supported. Finally,  five type
// conversion functions to short, int, long, float, and double are provided.

#ifndef RATIONAL_H                              // If no Rational definition
#define RATIONAL_H                              // define the Rational symbol

#include <iostream.h>

#ifndef MISCELANEOUSH                   // If we have not included this file
#include <cool/misc.h>          // Include miscellaneous useful defs
#endif


class CoolRational {
public:
  inline CoolRational ();                       // Simple constructor
  CoolRational (long, long d = 1);              // Constructor with longs
  CoolRational (const CoolRational&);           // Copy constructor
  inline ~CoolRational();                       // destructor
  
  inline long numerator () const;               // Get the numerator
  inline long denominator () const;             // Get the denominator
  inline N_status status () const;              // Return Number status
  
  inline CoolRational& operator= (const CoolRational&); // Overload assignment
  inline Boolean operator== (const CoolRational&) const; // Overload equality
  inline Boolean operator!= (const CoolRational&) const; // Overload inequality
  
  inline CoolRational operator-() const;        // Unary minus operator
  inline Boolean operator!() const;             // Unary not operator
  
  CoolRational& operator+= (const CoolRational&); // Overload plus/assign
  CoolRational& operator-= (const CoolRational&); // Overload minus/assign
  CoolRational& operator*= (const CoolRational&); // Overload multiply/assign
  CoolRational& operator/= (const CoolRational&); // Overload divide/assign
  CoolRational& operator%= (const CoolRational&); // Overload modulus/assign
  
  friend CoolRational operator+ (const CoolRational&, const CoolRational&);
  friend CoolRational operator- (const CoolRational&, const CoolRational&);
  friend CoolRational operator* (const CoolRational&, const CoolRational&);
  friend CoolRational operator/ (const CoolRational&, const CoolRational&);
  friend CoolRational operator% (const CoolRational&, const CoolRational&); 

  inline CoolRational& operator++ ();           // Overload increment
  inline CoolRational& operator-- ();           // Overload decrement
  
  Boolean operator< (const CoolRational&) const; // Overload less than
  inline Boolean operator<= (const CoolRational&) const; // Overload less/equal
  Boolean operator> (const CoolRational&) const;         // Overload greater than
  inline Boolean operator>= (const CoolRational&) const; // Overload greater/equal
  
  friend ostream& operator<< (ostream&, const CoolRational&); // Output ref
  friend ostream& operator<< (ostream&, const CoolRational*); // Output ptr
  
  void print(ostream&);                         // Terse print
  
  CoolRational& invert ();                      // Invert the rational number
  inline long floor () const;                   // Truncate towards -infinity
  inline long ceiling () const;                 // Truncate towards +infinity
  inline long truncate () const;                // Truncate towards zero
  long round () const;                          // Truncate to nearest integer
  
  operator short ();                            // Implicit conversion
  operator int ();                              // Implicit conversion
  operator long ();                             // Implicit conversion
  operator float ();                            // Implicit conversion
  inline operator double ();                    // Implicit conversion

private:
  long num;                                     // Numerator portion
  long den;                                     // Denominator portion
  N_status state;                               // Exception status
  
  long gcd (long, long);                        // Calculate GCD
  void normalize ();                            // Normalize num/den 
  void minus_infinity (const char*) const;      // Raise - infinity exception
  void plus_infinity (const char*) const;       // Raise + infinity exception
  void overflow (const char*) const;            // Raise overflow error
  void underflow (const char*) const;           // Raise underflow error
  void divide_by_zero (const char*) const;      // Raise divide by zero error
};


// Rational -- Simple constructor
// Input:      None
// Output:     None

inline CoolRational::CoolRational () {
  this->num = this->den = 0;                    // Initialize data slots
  this->state = N_OK;                           // Set status to OK
}


// Rational -- Copy constructor
// Input:      Reference to rational object
// Output:     None

inline CoolRational::CoolRational (const CoolRational& r) {
  this->num = r.num;                            // Set numerator
  this->den = r.den;                            // Set denominator
  this->state = r.state;                        // Set state
}

// ~Rational -- Destructor does nothing

inline CoolRational::~CoolRational() {}

// numerator -- Return the numerator portion of the rational
// Input:       None
// Output:      Numerator of rational

inline long CoolRational::numerator () const {
  return this->num;                             // Return numerator
}


// denominator -- Return the denominator portion of the rational
// Input:         None
// Output:        Denominator of rational

inline long CoolRational::denominator () const {
  return this->den;                             // Return denominator
}


// status -- Return the status of Number
// Input:    None
// Output:   N_status enum value

inline N_status CoolRational::status () const {
  return this->state;
}


// operator= -- Overload the assignment operator for the Rational class
// Input:       Reference to rational object
// Output:      Reference to updated rational object

inline CoolRational& CoolRational::operator= (const CoolRational& r) {
  this->num = r.num;                            // Set numerator
  this->den = r.den;                            // Set denominator
  this->state = r.state;                        // Set state
  return *this;                                 // Return reference
}
  

// operator== -- Overload the equality operator for the rational class
// Input:        Reference to rational object
// Output:       TRUE/FALSE

inline Boolean CoolRational::operator== (const CoolRational& r) const {
  return (this->num == r.num && this->den == r.den);
}


// operator!= -- Overload the inequality operator for the Rational class
// Input:        Reference to a constant rational object
// Ouput:        TRUE/FALSE

inline Boolean CoolRational::operator!= (const CoolRational& r) const {
  return !(*this == r);
}


// operator< -- Overload operator less than for the rational class
// Input:       Reference to rational number
// Output:      TRUE/FALSE

inline Boolean CoolRational::operator< (const CoolRational& r) const {
  if (this->den == r.den)                       // If same denominator
    return (this->num < r.num);                 // Return less than sense
  else                                          // Else calculate common denom.
    return ((this->num * r.den) < (this->den * r.num)); // And return state
}


// operator> -- Overload operator greater than for the rational class
// Input:       Reference to rational number
// Output:      TRUE/FALSE

inline Boolean CoolRational::operator> (const CoolRational& r) const {
  if (this->den == r.den)                       // If same denominator
    return (this->num > r.num);                 // Return greater than sense 
  else                                          // Else calculate common denom.
    return ((this->num * r.den) > (this->den * r.num)); // And return state
}


// operator<= -- Overload the less than or equal to operator for rational
// Input:        Reference to a constant rational object
// Output:       TRUE/FALSE

inline Boolean CoolRational::operator<= (const CoolRational& r) const {
  return !(*this > r);
}


// operator>= -- Overload the greater than or equal to operator for rational
// Input:        Reference to a constant rational object
// Output:       TRUE/FALSE

inline Boolean CoolRational::operator>= (const CoolRational& r) const {
  return !(*this < r);
}


// operator<< -- Overload the output operator for a reference to a rational
// Input:        Reference to an ostream
// Output:       Reference to an ostream

inline ostream& operator<< (ostream& os, const CoolRational& r) {
  os << r.num << "/" << r.den;                  // Output rational number
  return os;                                    // Return ostream reference
}


// operator<< -- Overload the output operator for a pointer to a rational
// Input:        Ostream reference, pointer to a rational object
// Output:       Ostream reference

inline ostream& operator<< (ostream& os, const CoolRational* r) {
  return operator<< (os, *r);
}


// operator- -- Overload the unary minus operator for the Rational class
// Input:       None
// Output:      Negated rational value

inline CoolRational CoolRational::operator- () const {
  CoolRational r (-this->num, this->den);       // Get negative value
  return r;                                     // Return deferenced pointer
}


// operator! -- Overload the negation operator for the Rational class
// Input:       None
// Output:      TRUE/FALSE

inline Boolean CoolRational::operator! () const {
  return ((this->num == 0) ? TRUE : FALSE);     // Return logical state
}


// operator++ -- Overload the increment operator for the Rational class
// Input:        None
// Output:       Reference to updated rational object

inline CoolRational& CoolRational::operator++ () {
  this->num += this->den;                       // Increment numerator
  return *this;                                 // Return updated object
}
  

// operator-- -- Overload the decrement operator for the Rational class
// Input:        None
// Output:       Reference to updated rational object

inline CoolRational& CoolRational::operator-- () {
  this->num -= this->den;                       // Decrement numerator
  return *this;                                 // Return updated object
}


// truncate -- Converts rational value by truncating towards zero
// Input:      None
// Output:     long truncated value

inline long CoolRational::truncate () const {
  return (long) (this->num / this->den);        // Return truncated rational
}


// floor -- Converts rational value by truncating towards negative infinity
// Input:   None
// Output:  long value of truncated rational

inline long CoolRational::floor () const {
  long temp = this->truncate ();                // Get truncated value
  return ((this->num < 0.0 && (-(this->num%this->den) > 0.5*this->den)) ?
          (temp - 1) : temp);
}


// ceiling -- Converts rational value by truncating towards positive infinity
// Input:     None
// Output:    long value of truncated rational

inline long CoolRational::ceiling () const {
  long temp = this->truncate ();                // Get truncated value
  return ((this->num > 0.0 && ((this->num%this->den) > 0.5*this->den)) ?
          (temp + 1) : temp);
}


// operator+ -- Overload the addition operator for rational
// Input:       Reference to two rational numbers
// Output:      A new rational number

inline CoolRational operator+ (const CoolRational& r1, const CoolRational& r2) {
  CoolRational result(r1);
  result += r2;
  return result;
}

// operator- -- Overload the subtraction operator for rational
// Input:       Reference to two rational numbers
// Output:      A new rational number

inline CoolRational operator- (const CoolRational& r1, const CoolRational& r2) {
  CoolRational result(r1);
  result -= r2;
  return result;
}

// operator* -- Overload the multiplication operator for rational
// Input:       Reference to two rational numbers
// Output:      A new rational number

inline CoolRational operator* (const CoolRational& r1, const CoolRational& r2) {
  CoolRational result(r1);
  result *= r2;
  return result;
}

// operator/ -- Overload the division operator for rational
// Input:       Reference to two rational numbers
// Output:      A new rational number

inline CoolRational operator/ (const CoolRational& r1, const CoolRational& r2) {
  CoolRational result(r1);
  result /= r2;
  return result;
}

// operator% -- Overload the remainder operator for rational
// Input:       Reference to two rational numbers
// Output:      A new rational number

inline CoolRational operator% (const CoolRational& r1, const CoolRational& r2) {
  CoolRational result(r1);
  result %= r2;
  return result;
}

// operator double -- Implicit conversion operator from Rational to a double
// Input:             None
// Output:            Double value equivalent to rational

inline CoolRational::operator double () {
  return ((double)this->num)/((double)this->den);     // Double answer
}

#endif
