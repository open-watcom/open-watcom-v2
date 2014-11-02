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
// Created: MBN 10/31/89 -- Initial design and implementation
// Updated: MBN 03/04/90 -- Added execption for DIVIDE_BY_ZERO
// Updated: MJF 03/12/90 -- Added group names to RAISE
// Updated: MJF 07/31/90 -- Added terse print
// Updated: DLS 04/01/91 -- New lite version
// Updated: JAM 08/11/92 -- anach. form() replaced
//
// The CoolRational class  implements rational numbers  and arithmetic.  A CoolRational
// object has the same precision and range of values as the built-in type long.
// Implicit conversion to the system defined types short, int, long, float, and
// double is supported by  overloaded  operator member functions.  Although the
// CoolRational class makes judicous use  of inline  functions and  deals only with
// integral values, the user  is warned that  the CoolRational  integer  arithmetic
// class is still considerably slower than the built-in  integer data types. If
// the range  of values  anticipated will  fit into a  built-in  type, use that
// instead.
//

#ifndef RATIONAL_H                              // If no CoolRational class
#include <cool/Rational.h>                      // Include class definition
#endif

#if defined(DOS)
extern "C" {
#include <stdlib.h>                             // For exit()
}
#else
#include <stdlib.h>                             // For exit()
#endif
#include <stdio.h>                              // For printf()


// normalize -- Private function to normalize the numerator and denominator of
//              a rational number
// Input:       None
// Output:      None

void CoolRational::normalize () {
  if (this->num != 1 && this->den != 1) {       // Is there something to do?
    long common = this->gcd (this->num, this->den); // Calculate GCD
    if (common != 1) {                          // If GCD is not one
      this->num /= common;                      // Calculate new numerator
      this->den /= common;                      // Calculate new denominator
    }
  }
  if (this->den < 0) {                          // If sign is in denominator
    this->num *= -1;                            // Multiply num and den by -1
    this->den *= -1;                            // To keep sign in numerator
  }
  this->state = N_OK;                           // Set state to OK
}


// gcd -- calculate the GCD for two integer values
// Input: Two long numbers
// Output: Greatest common denominator

long CoolRational::gcd (long l1, long l2) {
  long temp;
  while (l2) {                                  // While non-zero value
    temp = l2;                                  // Save current value
    l2 = l1 % l2;                               // Assign remainder of division
    l1 = temp;                                  // Copy old value
  }
  return l1;                                    // Return GCD of numbers
}


// CoolRational -- Constructor with initial long arguments
// Input:      Numerator, optional denominator
// Output:     None

CoolRational::CoolRational (long n, long d) {
  this->num = n;                                // Set numerator
#if ERROR_CHECKING                              // If exception handling 
  if (d == 0) {                                 // Then if denominator is zero
    //RAISE (Error, SYM(CoolRational), SYM(Zero_Denominator),
    printf ("CoolRational::CoolRational(): Denominator of zero specified.\n");
    abort ();                                   // terminate in error with exit
  }
#endif
  this->den = d;                                // Set denominator
  this->normalize ();                           // Normalize rational
}


// operator short -- Impilict conversion operator from CoolRational to a short
// Input:            None
// Output:           Short value equivalent to truncated rational

CoolRational::operator short () {
  long temp = this->truncate ();                // Return truncated rational
  if (temp > MAXSHORT) {                        // If too big for a short
    this->state = N_OVERFLOW;                   // Set condition state
    this->overflow ("operator short()");        // Raise exception
  }
  if (temp < MINSHORT) {                        // If too small for a short
    this->state = N_UNDERFLOW;                  // Set condition state
    this->underflow ("operator short()");       // Raise exception
  }
  return (short)temp;                           // Return converted value
}


// operator int -- Impilict conversion operator from CoolRational to an int
// Input:          None
// Output:         Int value equivalent to truncated rational

CoolRational::operator int () {
  long temp = this->truncate ();                // Return truncated rational
  if (temp > MAXINT) {                          // If too big for an int
    this->state = N_OVERFLOW;                   // Set condition state
    this->overflow ("operator int()");          // Raise exception
  }
  if (temp < MININT) {                          // If too small for an int
    this->state = N_UNDERFLOW;                  // Set condition state
    this->underflow ("operator int()");         // Raise exception
  }
  return (int)temp;                             // Return converted value
}


// operator long -- Implicit conversion operator from CoolRational to a long
// Input:           None
// Output:          Long value equivalent to truncated rational

CoolRational::operator long () {
  long temp = this->truncate ();                // Return truncated rational
  if (temp > MAXLONG) {                         // If too big for an int
    this->state = N_OVERFLOW;                   // Set condition state
    this->overflow ("operator int()");          // Raise exception
  }
  if (temp < MINLONG) {                         // If too small for an int
    this->state = N_UNDERFLOW;                  // Set condition state
    this->underflow ("operator int()");         // Raise exception
  }
  return (long)temp;                            // Return converted value
}


// operator float -- Implicit conversion operator from CoolRational to a float
// Input:            None
// Output:           Float value equivalent to rational

CoolRational::operator float ()
{
  if (this->num > MAXFLOAT || this->den > MAXFLOAT){// Out of range?
    this->state = N_OVERFLOW;                       // Set condition state
    this->overflow ("operator float()");            // Raise exception
  }
  if ((this->num < 0.0 && (-this->num) < MINFLOAT) ||
      (this->den < 0.0 && (-this->den) < MINFLOAT)){ // Out of ranbge?
    this->state = N_UNDERFLOW;                      // Set condition state
    this->underflow ("operator float()");           // Raise exception
  }
  return ((float)this->num)/((float)this->den);     // Float answer
}


// invert -- Invert rational number
// Input:    None
// Output:   Reference to inverted number

CoolRational& CoolRational::invert () {
  if (this->num == 0 && this->den != 0) {       // If zero numerator only
    if (this->den < 0) {                        // If negative denominator
      this->state = N_MINUS_INFINITY;           // Set condition state
      this->minus_infinity ("invert");          // And raise exception
    }
    else {
      this->state = N_PLUS_INFINITY;            // Set condition state
      this->plus_infinity ("invert");           // And raise exception
    }
  }
  long temp = this->num;                        // Save numerator
  if (temp > 0) {
    this->num = this->den;
    this->den = temp;
  } else {
    this->num = this->den * -1;                 // Switch denominator/numerator
    this->den = temp * -1;                      // And keep sign in numerator
  }
  return *this;                                 // Return reference
}


// round -- Converts rational value by rounding to the nearest integer
// Input:   None
// Output:  Long

long CoolRational::round () const {
  long ival = this->num / this->den;            // Calculate integer answer
  if (ival > MAXDOUBLE)                         // If too big
    this->overflow ("operator double()");       // Raise exception
  if (ival < 0 && (-ival) < MINDOUBLE)          // If out of range
    this->underflow ("operator double()");      // Raise exception
  double fval = ((double)this->num)/((double)this->den); // Double answer
  if (ival < 0)                                       // If negative
    return (ival-(((-fval+ival) >= 0.5) ? 1 : 0));    // Round down if needed
  else                                                // Else positive, so
    return (ival + (((fval - ival) >= 0.5) ? 1 : 0)); // Round up if needed
}


// operator+= -- Overload the addition with assignment operator for rational
// Input:        Reference to a rational number
// Output:       Mutated *this

CoolRational& CoolRational::operator+= (const CoolRational& r) {
  if (this->den == r.den)                       // If same denominator
    this->num += r.num;                         // Just add to numerator
  else {
    this->num = (this->num*r.den) + (this->den*r.num); // New numerator
    this->den *= r.den;                                // New denominator
  }
  this->normalize ();                           // Normalize rational
  return *this;
}


// operator-= -- Overload the substraction with assignment operator for rational
// Input:        Reference to a rational number
// Output:       Mutated *this

CoolRational& CoolRational::operator-= (const CoolRational& r) {
  if (this->den == r.den)                       // If same denominator
    this->num -= r.num;                         // Just add to numerator
  else {
    this->num = (this->num*r.den) - (this->den*r.num); // New numerator
    this->den *= r.den;                                // New denominator
  }
  this->normalize ();                           // Normalize rational
  return *this;
}


// operator*= -- Overload the multiply/assign operator for rational
// Input:       Reference to a rational number
// Output:      Reference to modified rational

CoolRational& CoolRational::operator*= (const CoolRational& r) {
  this->num = this->num * r.num;                // Multiply numerators
  this->den = this->den * r.den;                // Multiply denominators
  this->normalize ();                           // Normalize rational
  return *this;                                 // Return result
}


// operator/= -- Overload the divide with assignment operator for rational
// Input:        Reference to a rational number
// Output:       None

CoolRational& CoolRational::operator/= (const CoolRational& r) {
  if (r.num == 0) {
    this->state = N_DIVIDE_BY_ZERO;             // Set condition state
    this->divide_by_zero ("operator/=()");
  }
  CoolRational temp(r);                 // Create temporary work copy
  return (*this *= temp.invert());
}

// operator %= -- Overload the modulo with assignment operator for rational
// Input:         Reference to a rational number
// Output:        Mutated *this

CoolRational& CoolRational::operator%= (const CoolRational& r) {
  if (this->den == r.den)                       // If same denominator
    this->num %= r.num;                         // Just calculate modulus
  else {
    this->num = (this->num*r.den) % (this->den*r.num); // New numerator
    this->den *= r.den;                                // New denominator
  }
  this->normalize ();                           // Normalize rational
  return *this;
}

// minus_infinity -- Raise Error exception for negative infinity
// Input:            Character string of derived class and function
// Output:           None

void CoolRational::minus_infinity (const char* name) const {
  //RAISE (Error, SYM(CoolRational), SYM(Minus_Infinity),
  printf ("CoolRational::%s: Operation results in negative infinity value.\n",
          name);
  abort ();                                     // terminate in error with exit
}


// plus_infinity -- Raise Error exception for positive infinity
// Input:           Character string of derived class and function
// Output:          None

void CoolRational::plus_infinity (const char* name) const {
  //RAISE (Error, SYM(CoolRational), SYM(Plus_Infinity),
  printf ("CoolRational::%s: Operation results in positive infinity value.\n",
          name);
  abort ();                                     // terminate in error with exit
}


// overflow -- Raise Error exception for overflow occuring during conversion
// Input:      Character string of derived class and function
// Output:     None

void CoolRational::overflow (const char* name) const {
  //RAISE (Error, SYM(CoolRational), SYM(Overflow),
  printf ("CoolRational::%s: Overflow occured during type conversion.\n", name);
  abort ();                                     // terminate in error with exit
}


// underflow -- Raise Error exception for underflow occuring during conversion
// Input:       Character string of derived class name and function
// Output:      None

void CoolRational::underflow (const char* name) const {
  //RAISE (Error, SYM(CoolRational), SYM(Underflow),
  printf ("CoolRational::%s: Underflow occured during type conversion.\n", name);
  abort ();                                     // terminate in error with exit
}


// divide_by_zero -- Raise Error exception for divide by zero
// Input:            Character string of derived class name and function
// Output:           None

void CoolRational::divide_by_zero (const char* name) const {
  //RAISE (Error, SYM(CoolRational), SYM(Divide_By_Zero),
  printf ("CoolRational::%s: Divide by zero.\n", name);
  abort ();                                     // terminate in error with exit
}


// print --  terse print function for CoolRational
// Inputs:   reference to output stream
// Outputs:  none

void CoolRational::print(ostream& os) {
  os << "                               /* CoolRational " << (long)this << " */";
}
