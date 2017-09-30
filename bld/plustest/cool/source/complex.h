//
// Copyright (C) 1991 Texas Instruments Incorporated.
// Copyright (C) 1992 General Electric Company.
//
// Permission is granted to any individual or institution to use, copy, modify,
// and distribute this software, provided that this complete copyright and
// permission notice is maintained, intact, in all copies and supporting
// documentation.
//
// Texas Instruments Incorporated and General Electric Company
// provides this software "as is" without express or implied warranty.
//
//
// Created: MBN 10/25/89 -- Initial design and implementation
// Updated: MBN 03/04/90 -- Added execption for DIVIDE_BY_ZERO
// Updated: MJF 07/31/90 -- Added terse print
// Updated: DLS 03/22/91 -- New lite version
// Updated: VDN 06/29/92 -- roots of real polynomial, degree <= 4.
// Updated: JAM 09/28/92 -- removed DOS specifics, stdized #includes
// 
// The Complex  class implements  Complex  numbers  and arithmetic.   A Complex
// object has the same  precision and range of  values  as  the system built-in
// type double.  Implicit conversion  to  the  system defined types short, int,
// long, float,    and  double   is supported by   overloaded  operator  member
// functions.  Although   the  Complex class  makes   judicous use   of  inline
// functions and deals only with floating point values, the user is warned that
// the Complex double arithmetic class is still  slower than  the built-in real
// data types.
//
// The Complex   class implements common   arithmetic   exception  handling and
// provides  the  application  with support  for   detecting negative infinity,
// positive  infinity, overflow, and underflow as  a  result of some arithmetic
// expression. If one  of these conditions  or  an attempt   to  convert from a
// Complex with no value to a built-in type is detected,  an Error exception is
// raised. The application programmer can provide an exception handler  to take
// care of this problem. If no  such handler is  available, an error message is
// printed and the application terminates.
//
// The Complex class requires several constants be defined to insure precision
// and accuracy   of conversion.  The   preprocessor  symbols MINSHORT, MININT,
// MINLONG, MAXSHORT, MAXINT, and MAXLONG  are calculated  in the <COOL/misc.h>
// header file  via various  bit  manipulation  macros. The  symbols  MINFLOAT,
// MINDOUBLE,  MAXFLOAT,  and MAXDOUBLE  are  system dependent  and cannot   be
// calculated.  Most systems typically have  values  for these constants in the
// system header  file <values.h>.  Values  for a  specific  machine should  be
// copied into the <cool/misc.h> header file as necessary.
//
// The private data section of the Complex class contains two double  data type
// slots,  one for  the real portion  and one for the  imaginary portion.   The
// Complex  class  also contains   a   private  data slot  providing arithmetic
// exception status.  There are four constructors for the  Complex class.   The
// first is a simple inline constructor that  initializes the state and private
// data slots.  The second  takes two integers (short, int,  or long)  and uses
// them  as the initial value for  the object. The  second argument is optional
// and if not supplied, defaults to zero.  The third takes  two reals (float or
// double) and uses  them as  the  initial  value for  the object.  The  second
// argument is optional  and  if not supplied, defaults  to zero.  Finally, the
// fourth takes a const reference to another Complex object and  duplicates its
// state and value.
//
// The Complex  class provides overloaded  operators for addition, subtraction,
// multiplication,  division,  cosine, sine,  hyperbolic cosine  and hyperbolic
// sine.  Also available are  inequality  and equality,  assignment, increment,
// decrement, unary minus, and  output.  Methods to get  the real and imaginary
// part are supported.  Finally,  five virtual operator conversion functions to
// short, int, long, float, and double are provided.
//
// Find roots of a real polynomial in a single variable, with degree <=4.
// Reference: Winston, P.H, Horn, B.K.P. (1984) "Lisp", Addison-Wesley.


#ifndef COMPLEXH                                // If no Complex definition
#define COMPLEXH                                // define the Complex symbol

#ifndef MATHH
#include <math.h>               // include the standard math library
#define MATHH
#endif

#ifndef STREAMH                 // If the Stream support not yet defined,
#include <iostream.h>           // include the Stream class header file
#define STREAMH
#endif

#ifndef MISCELANEOUSH                   // If no misc.h file
#include <cool/misc.h>                  // Include useful defintions
#endif  

double curt (double d);                         // cubic root of a double

class CoolComplex {
public:
  inline CoolComplex (double real = 0, double imag = 0); // from real&imaginary
  inline CoolComplex (const CoolComplex&);      // Copy constructor
  inline ~CoolComplex();                        // Destructor

  inline double real () const;                  // Get the numerator
  inline double imaginary () const;             // Get the denominator
  inline N_status status () const;              // Return Number status

  inline double modulus () const;               // magnitude or norm of vector
  inline double argument () const;              // angle of vector with x-axis
  
  CoolComplex& operator= (const CoolComplex&);          // Overload assignment
  inline Boolean operator== (const CoolComplex&) const; // Overload equality
  inline Boolean operator!= (const CoolComplex&) const; // Overload inequality

  inline CoolComplex invert () const;           // Return reciprical of CoolComplex
  inline CoolComplex operator-() const;         // Unary minus operator
  inline CoolComplex conjugate () const;        // Conjugate of complex number
  inline Boolean operator!() const;             // Logical NOT operator

  /*inline##*/ friend CoolComplex operator+ (const CoolComplex&, const CoolComplex&);
  /*inline##*/ friend CoolComplex operator- (const CoolComplex&, const CoolComplex&);
  /*inline##*/ friend CoolComplex operator* (const CoolComplex&, const CoolComplex&);
  /*inline##*/ friend CoolComplex operator/ (const CoolComplex&, const CoolComplex&);    

  inline CoolComplex& operator+= (const CoolComplex&);  // Overload add/assigne
  inline CoolComplex& operator-= (const CoolComplex&);  // Overload subtract/assigne
  inline CoolComplex& operator*= (const CoolComplex&);  // Overload multi/assigne
  CoolComplex& operator/= (const CoolComplex&);         // Overload divide/assigne

  inline CoolComplex& operator++ ();            // Overload increment
  inline CoolComplex& operator-- ();            // Overload decrement

  friend ostream& operator<< (ostream&, const CoolComplex&); 
  /*inline##*/ friend ostream& operator<< (ostream&, const CoolComplex*);

  void print(ostream&);                         // terse print

  inline CoolComplex cos () const;              // Cosine of a complex 
  inline CoolComplex sin () const;              // Sine of a complex
  inline CoolComplex tan () const;              // Tangent of a complex
  inline CoolComplex cosh () const;             // Hyperbolic cosine of complex
  inline CoolComplex sinh () const;             // Hyperbolic sine of complex
  inline CoolComplex tanh () const;             // Hyperbolic tangent
  
  operator short ();                            // Virtual implicit conversion
  operator int ();                              // Virtual implicit conversion
  operator long ();                             // Virtual implicit conversion
  operator float ();                            // Virtual implicit conversion
  operator double ();                           // Virtual implicit conversion


  // finding roots of a polynomial in one variable with degree <= 4.
  static int roots_of_linear (const double& a, const double& b, 
                              CoolComplex& r);

  static int roots_of_quadratic (const double& a, const double& b, const double& c, 
                                 CoolComplex& r1, CoolComplex& r2);

  static int roots_of_cubic (const double& a, const double& b, 
                             const double& c, const double& d,
                             CoolComplex& r1, CoolComplex& r2, CoolComplex& r3);

  static int roots_of_quartic (const double& a, const double& b, 
                               const double& c, const double& d, const double& e, 
                               CoolComplex& r1, CoolComplex& r2, 
                               CoolComplex& r3, CoolComplex& r4);

protected:
  void minus_infinity (const char*) const;      // Raise - infinity exception
  void plus_infinity (const char*) const;       // Raise + infinity exception
  void overflow (const char*) const;            // Raise overflow error
  void underflow (const char*) const;           // Raise overflow error
  void no_conversion (const char*) const;       // Raise no conversion error
  void divide_by_zero (const char*) const;      // Raise divide by zero

private:
  double r;                                     // Real portion
  double i;                                     // Imaginary portion
  N_status state;                               // Exception status
};


// CoolComplex -- Constructor that takes a floating point real part and optional
//            floating point imaginary part to make a complex number
// Input:     Double for real part, optional double for imaginary part
// Output:    None

inline CoolComplex::CoolComplex (double rp, double ip) {
  this->r = rp;                                 // Set real part
  this->i = ip;                                 // Set imaginary part
  this->state = N_OK;                           // Set status to OK
}


// CoolComplex -- Copy constructor
// Input:     Reference to CoolComplex object
// Output:    None

inline CoolComplex::CoolComplex (const CoolComplex& c) {
  this->r = c.r;                                // Copy real part
  this->i = c.i;                                // Copy imaginary part
  this->state = c.state;                        // Copy state
}

// ~CoolComplex -- Destructor does nothing
// Input:     None
// Output:    None

inline CoolComplex::~CoolComplex() {}           // Nothing.


// real -- Return the real portion of the CoolComplex
// Input:  None
// Output: Real part of CoolComplex

inline double CoolComplex::real () const {
  return this->r;                               // Return real part
}


// imaginary -- Return the imaginary portion of the CoolComplex
// Input:       None
// Output:      Imaginary part of CoolComplex

inline double CoolComplex::imaginary () const {
  return this->i;                               // Return imaginary part
}


// status -- Return the status of Number
// Input:    None
// Output:   N_status enum value

inline N_status CoolComplex::status () const {
  return this->state;
}


// modulus -- Return the magnitude or norm of Complex vector

inline double CoolComplex::modulus () const {
  return sqrt((this->r * this->r) + (this->i * this->i));
}

// argument -- Return the angle from x-axis to Complex vector

inline double CoolComplex::argument () const {
  return atan2(this->i, this->r);
}


// operator== -- Overload the equality operator for the CoolComplex class
// Input:        Reference to CoolComplex object
// Output:       TRUE/FALSE

inline Boolean CoolComplex::operator== (const CoolComplex& c) const {
  return (this->r == c.r && this->i == c.i);
}


// operator!= -- Overload the inequality operator for the CoolComplex class
// Input:        Reference to a constant CoolComplex object
// Ouput:        TRUE/FALSE

inline Boolean CoolComplex::operator!= (const CoolComplex& c) const {
  return !(*this == c);
}


// operator<< -- Overload the output operator for a pointer to a CoolComplex
// Input:        Ostream reference, pointer to a CoolComplex object
// Output:       Ostream reference

inline ostream& operator<< (ostream& os, const CoolComplex* c) {
  return operator<< (os, *c);
}


// invert -- Calculate the reciprical of a complex number
// Input:    None
// Output:   Reciprical of complex

inline CoolComplex CoolComplex::invert () const {
  double normalize = (this->r * this->r)+(this->i * this->i);
  return CoolComplex ((this->r / normalize), (-this->i / normalize));
}


// operator- -- Overload the unary minus operator for the CoolComplex class
// Input:       None
// Output:      Negated CoolComplex value

inline CoolComplex CoolComplex::operator- () const {
  return CoolComplex (-this->r, -this->i);              // Get negative value
}


// conjugate -- Provide conjugate (that is, negate imaginary part) of complex
// Input:       None
// Output:      Negated CoolComplex value

inline CoolComplex CoolComplex::conjugate () const {
  return CoolComplex (this->r, -this->i);               // Get negative value
}


// operator! -- Overload the negation operator for the CoolComplex class
// Input:       None
// Output:      TRUE/FALSE

inline Boolean CoolComplex::operator! () const {
  return ((this->r == 0.0) ? TRUE : FALSE);     // Return logical state
}


// operator++ -- Overload the increment operator for the CoolComplex class
// Input:        None
// Output:       Reference to updated CoolComplex object

inline CoolComplex& CoolComplex::operator++ () {
  this->r++ ;                                   // Increment real part
  return *this;                                 // Return updated object
}
  

// operator-- -- Overload the decrement operator for the CoolComplex class
// Input:        None
// Output:       Reference to updated CoolComplex object

inline CoolComplex& CoolComplex::operator-- () {
  this->r--;                                    // Decrement real part
  return *this;                                 // Return updated object
}


// operator+ -- Overload the addition operator for the CoolComplex class
// Input:       Reference to complex object
// Output:      Reference to new complex object

inline CoolComplex operator+ (const CoolComplex& c1, const CoolComplex& c2) {
  return CoolComplex (c1.real()+c2.real(), c1.imaginary()+c2.imaginary());
}


// operator- -- Overload the subtraction operator for the CoolComplex class
// Input:       Reference to CoolComplex object
// Output:      Reference to new CoolComplex object

inline CoolComplex operator- (const CoolComplex& c1, const CoolComplex& c2) {
  return CoolComplex (c1.real()-c2.real(), c1.imaginary()-c2.imaginary());
}


// operator* -- Overload the multiplication operator for the CoolComplex class
// Input:       Reference to complex object
// Output:      Reference to new complex object

inline CoolComplex operator* (const CoolComplex& c1, const CoolComplex& c2) {
  return CoolComplex (((c1.r * c2.r)-(c1.i * c2.i)),
                      ((c1.r * c2.i)+(c1.i * c2.r)));
}


// operator/ -- Overload the division operator for the CoolComplex class
// Input:       Reference to complex object
// Output:      Reference to new complex object

inline CoolComplex operator/ (const CoolComplex& c1, const CoolComplex& c2) {
  CoolComplex result = c1;
  result /= c2;
  return result;
}


// operator+= -- Overload the addition/assign operator for the CoolComplex class
// Input:        Reference to complex object
// Output:       None

inline CoolComplex& CoolComplex::operator+= (const CoolComplex& c) {
  this->r += c.r;                               // Add real part
  this->i += c.i;                               // Add imaginary part
  return *this;
}


// operator-= -- Overload the subtraction/assign operator for the CoolComplex class
// Input:        Reference to complex object
// Output:       None

inline CoolComplex& CoolComplex::operator-= (const CoolComplex& c) {
  this->r -= c.r;                               // Subtract real part
  this->i -= c.i;                               // Subtract imaginary part
  return *this;
}


// operator*= -- Overload the multiplication/assign operator for CoolComplex class
// Input:        Reference to complex object
// Output:       None

inline CoolComplex& CoolComplex::operator*= (const CoolComplex& c) {
  this->r = (this->r * c.r)-(this->i * c.i);    // Multiply real part
  this->i = (this->r * c.i)+(this->i * c.r);    // Multiply imaginary part
  return *this;
}


// sin --  Calculate the sine of a CoolComplex number
// Input:  Reference to a complex
// Output: Reference to a new complex whose value is the answer

inline CoolComplex CoolComplex::sin () const {
  return CoolComplex (::sin (this->r), ::sin (this->i));
}


// cos --  Calculate the cosine of a Complex number
// Input:  Reference to a complex
// Output: Reference to a new complex whose value is the answer

inline CoolComplex CoolComplex::cos () const {
  return CoolComplex (::cos (this->r), ::cos (this->i));
}


// tan --  Calculate the tangent of a CoolComplex number
// Input:  Reference to a complex
// Output: Reference to a new complex whose value is the answer

inline CoolComplex CoolComplex::tan () const {
  return CoolComplex (::tan (this->r), ::tan (this->i));
}


// sinh -- Calculate the hyperbolic sine of a CoolComplex number
// Input:  Reference to a complex
// Output: Reference to a new complex whose value is the answer

inline CoolComplex CoolComplex::sinh () const {
  return CoolComplex (::sinh (this->r), ::sinh (this->i));
}


// cosh -- Calculate the hyperbolic cosine of a CoolComplex number
// Input:  Reference to a complex
// Output: Reference to a new complex whose value is the answer

inline CoolComplex CoolComplex::cosh () const {
  return CoolComplex (::cosh (this->r), ::cosh (this->i));
}


// tanh -- Calculate the hyperbolic tangent of a CoolComplex number
// Input:  Reference to a complex
// Output: Reference to a new complex whose value is the answer

inline CoolComplex CoolComplex::tanh () const {
  return CoolComplex (::tanh (this->r), ::tanh (this->i));
}

#endif
