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
// Created: MBN 11/01/89 -- Initial implementation
// Updated: MBN 03/04/90 -- Added exception for DIVIDE_BY_ZERO
// Updated: MJF 03/12/90 -- Added group names to RAISE
// Updated: MJF 07/31/90 -- Added terse print
// Updated: DLS 03/22/91 -- New lite version
// Updated: VDN 06/29/92 -- roots of real polynomial, degree <= 4.
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
// Find roots of a real polynomial in a single variable, with degree <=4.
// Reference: Winston, P.H, Horn, B.K.P. (1984) "Lisp", Addison-Wesley.

#ifndef COMPLEXH                                // If no Complex class
#include <cool/Complex.h>                       // Include class definition
#endif

// minus_infinity -- Raise Error exception for negative infinity
// Input:            Character string of derived class and function
// Output:           None

void CoolComplex::minus_infinity (const char* name) const {
  //RAISE (Error, SYM(CoolComplex), SYM(Minus_Infinity),
  printf ("CoolComplex::%s: Operation results in negative infinity value",
          name);
  abort ();
}


// plus_infinity -- Raise Error exception for positive infinity
// Input:           Character string of derived class and function
// Output:          None

void CoolComplex::plus_infinity (const char* name) const {
  //RAISE (Error, SYM(CoolComplex), SYM(Plus_Infinity),
  printf ("CoolComplex::%s: Operation results in positive infinity value",
          name);
  abort ();
}


// overflow -- Raise Error exception for overflow occuring during conversion
// Input:      Character string of derived class and function
// Output:     None

void CoolComplex::overflow (const char* name) const {
  //RAISE (Error, SYM(CoolComplex), SYM(Overflow),
  printf ("CoolComplex::%s: Overflow occured during type conversion", name);
  abort ();
}


// underflow -- Raise Error exception for underflow occuring during conversion
// Input:       Character string of derived class name and function
// Output:      None

void CoolComplex::underflow (const char* name) const {
  //RAISE (Error, SYM(CoolComplex), SYM(Underflow),
  printf ("CoolComplex::%s: Underflow occured during type conversion",name);
  abort ();
}


// no_conversion -- Raise Error exception for no conversion from CoolComplex
// Input:           Character string of derived class name and function
// Output:          None

void CoolComplex::no_conversion (const char* name) const {
  //RAISE (Error, SYM(CoolComplex), SYM(No_Conversion),
  printf ("CoolComplex::%s: No conversion from CoolComplex", name);
  abort ();
}


// divide_by_zero -- Raise Error exception for divide by zero
// Input:            Character string of derived class and function
// Output:           None

void CoolComplex::divide_by_zero (const char* name) const {
  //RAISE (Error, SYM(CoolComplex), SYM(Divide_By_Zero),
  printf ("CoolComplex::%s: Divide by zero", name);
  abort ();
}


// operator= -- Overload the assignment operator for the CoolComplex class
// Input:       Reference to CoolComplex object
// Output:      Reference to updated CoolComplex object

CoolComplex& CoolComplex::operator= (const CoolComplex& c) {
  this->r = c.r;                                // Copy real part
  this->i = c.i;                                // Copy imaginary part
  this->state = c.state;                        // Copy state
  return *this;                                 // Return reference
}


// operator short -- Provide implicit conversion from CoolComplex to short
// Input:            None
// Output:           Converted number

CoolComplex::operator short () {
  if (this->i != 0.0) {                         // If there is an i-part
    this->state = N_NO_CONVERSION;              // Indicate exception case
    this->no_conversion ("operator short");     // And raise exception
  }
  else if (this->r > MAXSHORT) {                // If there is an overflow
    this->state = N_OVERFLOW;                   // Indicate exception case
    this->overflow ("operator short");          // And raise exception
  }
  else if (this->r < MINSHORT) {                // If there is an underflow
    this->state = N_UNDERFLOW;                  // Indicate exception case
    this->underflow ("operator short");         // And raise exception
  }
  return short(this->r);                        // Return converted number
}


// operator int -- Provide implicit conversion from CoolComplex to int
// Input:          None
// Output:         Converted number

CoolComplex::operator int () {
  if (this->i != 0.0) {                         // If there is an i-part
    this->state = N_NO_CONVERSION;              // Indicate exception case
    this->no_conversion ("operator int");       // And raise exception
  }
  else if (this->r > MAXINT) {                  // If there is an overflow
    this->state = N_OVERFLOW;                   // Indicate exception case
    this->overflow ("operator int");            // And raise exception
  }
  else if (this->r < MININT) {                  // If there is an underflow
    this->state = N_UNDERFLOW;                  // Indicate exception case
    this->underflow ("operator int");           // And raise exception
  }
  return int (this->r);                         // Return converted number
}


// operator long -- Provide implicit conversion from CoolComplex to long
// Input:           None
// Output:          Converted number

CoolComplex::operator long () {
  if (this->i != 0.0) {                         // If there is an i-part
    this->state = N_NO_CONVERSION;              // Indicate exception case
    this->no_conversion ("operator long");      // And raise exception
  }
  else if (this->r > MAXLONG) {                 // If there is an overflow
    this->state = N_OVERFLOW;                   // Indicate exception case
    this->overflow ("operator long");           // And raise exception
  }
  else if (this->r < MINLONG) {                 // If there is an underflow
    this->state = N_UNDERFLOW;                  // Indicate exception case
    this->underflow ("operator long");          // And raise exception
  }
  return long (this->r);                        // Return converted number
}


// operator float -- Provide implicit conversion from CoolComplex to float
// Input:            None
// Output:           Converted number

CoolComplex::operator float () {
  if (this->i != 0.0) {                         // If there is an i-part
    this->state = N_NO_CONVERSION;              // Indicate exception case
    this->no_conversion ("operator float");     // And raise exception
  }
  else if (this->r > MAXFLOAT) {                // If there is an overflow
    this->state = N_OVERFLOW;                   // Indicate exception case
    this->overflow ("operator float");          // And raise exception
  }
  else if (this->r < 0.0 && (-this->r) < MINFLOAT) { // Is there an underflow?
    this->state = N_UNDERFLOW;                  // Indicate exception case
    this->underflow ("operator float");         // And raise exception
  }
  return float (this->r);                       // Return converted number
}


// operator double -- Provide implicit conversion from CoolComplex to double
// Input:             None
// Output:            Converted number

CoolComplex::operator double () {
  if (this->i != 0.0) {                         // If there is an i-part
    this->state = N_NO_CONVERSION;              // Indicate exception case
    this->no_conversion ("operator double");    // And raise exception
  }
  return double (this->r);                      // Return converted number
}

// operator/= -- Overload the division/assign operator for the CoolComplex class
// Input:       Reference to complex object
// Output:      Reference to new complex object

CoolComplex& CoolComplex::operator/= (const CoolComplex& c2) {
  if (c2.r == 0.0 && c2.i == 0.0)               // If both num and den zero
    this->divide_by_zero ("operator/");         // Raise exception
  if (c2.r == 0.0)                              // If zero real part
    if (this->r < 0.0 && this->i >= 0.0)        // If negative complex
      this->minus_infinity ("operator/");       // Raise exception
    else
      this->plus_infinity ("operator/");        // Raise exception
  if (c2.i == 0.0)                              // If zero real part
    if (this->i < 0.0 && this->r >= 0.0)        // If negative complex
      this->minus_infinity ("operator/");       // Raise exception
    else
      this->plus_infinity ("operator/");        // Raise exception
  double normalize = (c2.r * c2.r) + (c2.i * c2.i);
  double new_r = (this->r * c2.r) + (this->i * c2.i); // multiply with conjugate
  double new_i = (this->i * c2.r) - (this->r * c2.i);
  this->r = new_r / normalize;
  this->i = new_i / normalize;
  return *this;
}


// operator<< -- Overload the output operator for a reference to a CoolComplex
// Input:        Ostream reference, reference to a CoolComplex object
// Output:       Ostream reference

ostream& operator<< (ostream& os, const CoolComplex& c) {
  os << "(" << c.r << "," << c.i << ")";
  return os;
}


// print --  terse print function for CoolComplex
// Input:    reference to output stream
// Output:   none

void CoolComplex::print(ostream& os) {
  os << "                                       /* CoolComplex %lx */" << (unsigned long) this;
}


// curt -- Cubic root of a double

double curt (double d) {
  if (d == 0.0) 
    return 0.0;
  else if (d < 0) 
    return -pow(-d, 1.0/3.0);
  else
    return pow(d, 1.0/3.0);
}

// roots of linear polynomial: a*x + b = 0.

int CoolComplex::roots_of_linear (const double& a, const double& b, 
                                  CoolComplex& r) {
  if (a == 0) {
    if (b == 0) {
      cout << "Homogenous has infinite number of roots." << endl;
      r = 0;                                    // least norm solution
      return 1;
    } else {
      cout << "Inconsistent equation. No root." << endl;
      return 0;
    }
  } else {
    r = -b / a;                                 // normal case, degree=1
    return 1;                                   
  }
}

// roots of quadratic -- a*x^2 + b*x + c = 0.
//         Return two roots, largest magnitude first.

int CoolComplex::roots_of_quadratic (const double& a, const double& b, 
                                     const double& c, 
                                     CoolComplex& r1, CoolComplex& r2) {
  if (a < 0) {                                  // a is make positive, try again.
    return roots_of_quadratic(-a, -b, -c, r1, r2);
  } else if (a == 0) {                          // special cases
    return roots_of_linear(b, c, r1);
  } else if (c == 0) {
    r2 = 0;
    return roots_of_linear(a, b, r1) + 1;
  } else {                                      // normal case
    double discriminant = (b * b) - (4.0 * a * c);
    double a2 = 2 * a;
    if (discriminant < 0) {
      double real = -b / a2;
      double imag = sqrt(-discriminant) / a2;
      r1 = CoolComplex(real, imag);             // two complex roots
      r2 = CoolComplex(real, -imag);
    } else if (discriminant == 0) {             // one double root
      r1 = r2 = -b / a2;
    } else {                                    // two real roots
      double n;
      if (b < 0) n = sqrt(discriminant) - b;
      else       n = -(sqrt(discriminant) + b);
      r1 = n / a2;                              // root with largest 
      r2 = (2 * c) / n;                         // magnitude first.
    }
    return 2;                                   // number of roots=2
  }
}

// roots of cubic -- a*x^3 + b*x^2 + c*x + d = 0.
//         Return three roots, largest magnitude first.


int CoolComplex::roots_of_cubic (const double& a, const double& b, 
                                 const double& c, const double& d,
                                 CoolComplex& r1, CoolComplex& r2, 
                                 CoolComplex& r3) {
  if (a < 0) {
    return roots_of_cubic(-a, -b, -c, -d, r1, r2, r3);
  } else if (a == 0) {                          // special cases
    return roots_of_quadratic(b, c, d, r1, r2);
  } else if (d == 0) {
    r3 = 0;
    return roots_of_quadratic(a, b, c, r1, r2) + 1;
  } else {                                      // normal case
    CoolComplex rs1, rs2;                       // roots of resolvent
    roots_of_quadratic(1, 
                       ((2*b*b*b) + (9 * a * ((3*a*d) - (b*c)))),
                       pow((b*b) - (3*a*c), 3),
                       rs1, rs2);
    if (rs1.imaginary() == 0) {                 // resolvent roots are real
      double r = curt(rs1.real());              // find cube roots of resolvents
      double s = curt(rs2.real());
      double a3 = 3 * a;
      r1 = (r + s - b) / a3;                    // real root first
      double real = (((r + s) / -2) - b) / a3;
      double imag = fabs(((r - s) * (sqrt(3.0) / 2)) / a3);
      r2 = CoolComplex(real, imag);             // two complex conjugate
      r3 = CoolComplex(real, -imag);            // roots last.
    } else {                                    // resolvent roots are complex
      double rho_3 = curt(rs1.modulus());
      double theta_3 = rs1.argument() / 3.0;
      double rd = 2 * rho_3;
      double cd = ::cos(theta_3) / -2.0;
      double sd = ::sin(theta_3) * sqrt(3.0) / 2.0;
      double a3 = 3 * a;
      if (b < 0) {                              // root with largest magnitude
        r1 = CoolComplex(((-2*rd*cd) - b) / a3, 0); // first
        r2 = CoolComplex((rd * (cd + sd) - b) / a3, 0);
        r3 = CoolComplex((rd * (cd - sd) - b) / a3, 0);
      } else {
        r1 = CoolComplex((rd * (cd - sd) - b) / a3, 0);
        r2 = CoolComplex((rd * (cd + sd) - b) / a3, 0);
        r3 = CoolComplex(((-2*rd*cd) - b) / a3, 0);
      }
    }
    return 3;                                   // number of roots=3
  }
}


// roots of quartic -- a*x^4 + b*x^3 + c*x^2 + d*x + e = 0.
//         Return four roots, largest magnitude first.
//         Decompose quartic into two quadratics for better numerical accuracy
//         than directly solving the 4 roots using Ferrari's formula.

int CoolComplex::roots_of_quartic (const double& a, const double& b, 
                                   const double& c, const double& d, 
                                   const double& e,
                                   CoolComplex& r1, CoolComplex& r2, 
                                   CoolComplex& r3, CoolComplex& r4) {
  if (a < 0) {
    return roots_of_quartic(-a, -b, -c, -d, -e, r1, r2, r3, r4);
  } else if (a == 0) {                          // special cases
    return roots_of_cubic(b, c, d, e, r1, r2, r3);
  } else if (e == 0) {
    r4 = 0;
    return roots_of_cubic(a, b, c, d, r1, r2, r3) + 1;
  } else {                                      // normal case
    double s;                                   // most pos real root of resolvent
    {
      CoolComplex rs1, rs2, rs3;                // roots of resolvent
      roots_of_cubic(1,
                     -c,
                     (b * d) - (4 * a * e),
                     (4 * a * c * e) - ((a * d * d) + (b * b * e)),
                     rs1, rs2, rs3);
      if (rs3.imaginary() != 0)                 // 2 resolvent roots are imaginary
        s = rs1;
      else
      if (rs1.real() > rs3.real())              // most positive/negative real
        s = rs1;                                // root is either rs1 or rs3.
      else
        s = rs3;
    }
    double s1 = sqrt((b * b) - (4 * a * (c - s)));
    double s2 = sqrt((s * s) - (4 * a * e));
    double s1s2 = (b * s) - (2 * a * d);
    double a2 = 2 * a;
    if ((s1 * s2 * s1s2) < 0) {                 // same sign?
      roots_of_quadratic(a2,
                         (b - s1),
                         (s + s2),
                         r1, r2);
      roots_of_quadratic(a2,
                         (b + s1),
                         (s - s2),
                         r3, r4);
    } else {
      roots_of_quadratic(a2,
                         (b - s1),
                         (s - s2),
                         r1, r2);
      roots_of_quadratic(a2,
                         (b + s1),
                         (s + s2),
                         r3, r4);
    }
    return 4;                                   // number of roots=4
  }
}
