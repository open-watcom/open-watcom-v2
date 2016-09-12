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
// Created: MBN 11/01/89 -- Initial implementation
// Updated: CLJ 04/19/90 -- Finished initial implementation of all functions
// Updated: MJF 05/24/90 -- Added group names to RAISE
// Updated: MJF 07/31/90 -- Added terse print
// Updated: DAN 01/08/91 -- Fixed bug in add and multiply_aux
// Updated: MJF 01/17/91 -- Fixed delete operations
// Updated: MJF 01/21/91 -- Fixed multiply_aux, divide_aux and left_shift
// Updated: DLS 03/22/91 -- New lite version
// Updated: JAM 08/12/92 -- removed DOS specifics, stdized #includes
// Updated: JAM 08/12/92 -- anach. form() replaced with iomanips
// Updated: JAM 08/12/92 -- added DBLRET_HACK in CoolBigNum(double)
// Updated: JAM 08/12/92 -- fixed(?) normalize() bug where 1 was being added
//                          to dividend Data which could cause it to wrap to 0
//
// The Bignum class implements  infinite precision integers  and arithmetic.  A
// Bignum  object storage  will grow and  shrink  in 16-bit chunks as necessary
// based upon  its current value.   Implicit  conversion to the  system defined
// types short, int, long,  float, and  double is supported by virtual operator
// member functions from   the  base Number   class.  Addition  and subtraction
// operations are performed by simple bitwise addition or subtraction on 32-bit
// boundaries with checks for carry flag propagation.   The  multiplication and
// division  operations  utilize  the algorithms   from  Knuth  Volume  2   for
// efficiency. However, the user is  warned that the  Bignum integer arithmetic
// class is still considerably slower than the built-in integer data types.
//

#ifndef BIGNUMH                                 // If no definition for class
#include <cool/Bignum.h>                        // include definition file
#endif

#include <string.h>             // Include standard strings
#define C_STRINGH

#include <math.h>               // Include the standard math library
#include <ctype.h>              // Include character macros
#include <stdlib.h>             // Include standard c library support
#include <limits.h>             // for CHAR_BIT

#include <iomanip.h>          // Include stream maniplutors (eg, hex, setw)

int CoolBignum_Init::count;                     
int    data_bits_s;                             // Initialize to 0 by default
long   radix_s;    

CoolBignum zero_s;                              // There is only 1 set of
CoolBignum one_s;                               // these Bignums
CoolBignum eight_s;                             
CoolBignum ten_s;                               // Constructors are called 
CoolBignum sixteen_s;                           // for static libraries.
CoolBignum max_short_s;
CoolBignum max_int_s;                           // No constructor called
CoolBignum max_long_s;                          // for dynamic libraries,
CoolBignum max_float_s;                         // so these bignums are blank
CoolBignum negmax_float_s;                              // so these bignums are blank
CoolBignum max_double_s;                        
CoolBignum negmax_double_s;

// Reinitialize one more time, if default constructor is called for 
// above globals.

CoolBignum_Init bignum_init2_s;         // Offset static/dynamic difference

CoolBignum_Init::CoolBignum_Init() {
  if ((count++ == 0) ||                         // from bignum_init1_s
      (((double)(max_double_s)) == 0.0)) {              // from bignum_init2_s
    data_bits_s   = BITSIZE(Data);              // This initializer creates
    radix_s       = long(pow(2.0,data_bits_s)); // correct values for
    zero_s       = CoolBignum();                // above global objects,
    one_s        = CoolBignum(long(1));         // as soon as Bignum.h is
    eight_s      = CoolBignum(long(8));         // included as header in
    ten_s        = CoolBignum(long(10));        // main.C
    sixteen_s    = CoolBignum(long(16));
    max_short_s  = CoolBignum(long(MAXSHORT));
    max_int_s    = CoolBignum(long(MAXINT));
    max_long_s   = CoolBignum(MAXLONG);
    max_float_s  = CoolBignum(MAXFLOAT);
    max_double_s = CoolBignum(MAXDOUBLE);
    negmax_float_s  = CoolBignum(-MAXFLOAT);
    negmax_double_s = CoolBignum(-MAXDOUBLE);
  }
}

CoolBignum_Init::~CoolBignum_Init() {}          // no clean up needed





// CoolBignum - simple constructor
// Inputs:  none
// Outputs:  initialized CoolBignum

CoolBignum::CoolBignum () {
  this->data = NULL;                            // No need to allocate yet
  this->count = 0;                              // Size of data is 0
  this->sign = 1;                               // CoolBignum >= 0
  this->state = N_OK;                           // State is ok
}



// ~CoolBignum - destructor
// Inputs:  none
// Outputs:  none

CoolBignum::~CoolBignum () {
  delete [] this->data;                 // Delete any allocated data
}



// CoolBignum - CoolBignum constructor with long argument
// Inputs:  long to be converted to CoolBignum
// Outputs:  new CoolBignum

CoolBignum::CoolBignum (const long inval) {
  long l = inval;
  if (l >= 0)                                   // Get correct sign
    this->sign = 1;
  else {
    l = -l;                                     // Get absolute value of l
    this->sign = -1;
  }
  Data buf[sizeof(l)];                  // Temp buffer to store l in
  Counter i = 0;                        // buffer index
  while (l) {                           // While more bits in l
    if (i >= sizeof(l)) {               // If no more buffer space
      this->state = N_NO_CONVERSION;    //   raise an exception
      no_conversion("CoolBignum(long)");
    }
    buf[i] = Data(l);                           // Peel off lower order bits
    l >>= data_bits_s;                          // Shift next bits into place
    i++;                                
  }
  this->data = ((this->count = i) > 0 ? // Allocate permanent data
    new Data[i] : 0);
  i = 0;                                       
  while (i < this->count) {                     // Save buffer into perm. data
    this->data[i] = buf[i];
    i++;
  }
  this->state = N_OK;                           // Set status to OK
}


// CoolBignum - CoolBignum constructor with double argument
// Inputs:  double to be converted to CoolBignum
// Outputs:  new CoolBignum

CoolBignum::CoolBignum (const double inval) {
  double d = inval;
  if (d >= 0)                                   // Get sign of d
    this->sign = 1;
  else {
    d = -d;                                     // Get absolute value of d
    this->sign = -1;
  }
#if 1
  // need to assign 'double' return value to temporary because of small error
  const double LOG10_MAXDOUBLE = log10(MAXDOUBLE);
  const double LOG10_RADIX_S = log10(double(radix_s));
  const int buf_size_s = int(LOG10_MAXDOUBLE/LOG10_RADIX_S);
  if (buf_size_s==63 && d==MAXDOUBLE) {
    printf("buf_size_s is 63 for MAXDOUBLE!\n");
    cout << setprecision(16) << MAXDOUBLE << ' ' << radix_s << ' ' << LOG10_MAXDOUBLE << ' ' << LOG10_RADIX_S << ' ' << buf_size_s << endl;
    abort();
    }
#else  // 
  static int buf_size_s = 
    int (log10(MAXDOUBLE)/log10(radix_s));      // Size of buffer to convert d
#endif
  Data *buf = new Data[buf_size_s];             // Buffer to convert d into
  Counter i = 0;                                // buffer index
    while (d >= 1.0) {                          
    if (i >= buf_size_s) {                      // If no more buffer space
      this->state = N_NO_CONVERSION;            //   raise an exception
      no_conversion("CoolBignum(double)");
    }
    buf[i] = Data(fmod(d,radix_s));             // Get next data "digit" from d
    d /= radix_s;                               // Shift d right 1 data "digit"
    i++;                                        // Increment buffer index
  }
  this->data = (i > 0 ? new Data[i] : 0);       // Allocate permanent data
  this->count = i;                              // Save count
  i = 0;                                       
  while (i < this->count) {                     // Copy temp buffer into 
    this->data[i] = buf[i];                     //   permanent data
    i++;
  }
  this->state = N_OK;                           // Set status to OK
  delete [] buf;                                // Deallocate buffer
}



// dtoBignum -- convert decimal string to CoolBignum
// Inputs:  string representation of decimal literal to be converted
// Outputs:  number of characters actually converted

int CoolBignum::dtoBignum (const char *s) {
  Counter len = 0;                              // No chars converted yet
  if (s[0] == '-' || s[0] == '+') len++;        // Skip over leading +,-
  while (isdigit(s[len])) {                     // If current char is digit
    (*this) = ((*this) * ten_s) +               // Shift CoolBignum left a decimal
      CoolBignum(long(s[len++] - '0'));         //   digit and add new digit
  }
  if (s[0] == '-') this->sign = -1;             // If s had leading -, note it
  return len;                                   // Return # of chars processed
}



// exptoBignum -- convert exponential string to a CoolBignum
// Inputs:  string representation of exponential literal to be converted
// Outputs:  none

void CoolBignum::exptoBignum (const char *s) {
  Counter pos = this->dtoBignum(s) + 1;         // Convert the base, skip [eE]
  long pow = atol(s + pos);                     // Convert the exponent to long
  while (pow-- > 0)                             // Raise CoolBignum to the given
    *this = (*this) * ten_s;                    //   power
}



// ctox - convert hex character to integer hex value (ASCII or EBCDIC)
// Inputs:  character representation of a hex number
// Outputs:  integer value of the hex number

unsigned int ctox (int c) {
  if ('0' <= c && c <= '9')
    return c - '0';
  if ('a' <= c && c <= 'f')
    return c - 'a' + 10;
  return c - 'A' + 10;
}



// xtoBignum -- convert hex string to CoolBignum value
// Inputs:  string representation of hex number to be converted
// Outputs:  none

void CoolBignum::xtoBignum (const char *s) {
  Counter size = strlen(s);
  Counter len = 2;                              // skip leading "0x"
  while (len < size) {                          // While there are more chars
    (*this) = ((*this) * sixteen_s) +           // Shift CoolBignum left one hex
      CoolBignum(long(ctox(s[len++])));         //   digit and add next digit
  }
}


  
// otoBignum -- convert octal string to CoolBignum
// Inputs:  string representation of octal number to be converted
// Outputs:  none

void CoolBignum::otoBignum (const char *s) {
  Counter size = strlen(s);
  Counter len = 0;                              // No chars converted yet
  while (len < size) {                          // While there are more chars
    (*this) = ((*this) * eight_s) +             // Shift CoolBignum left 1 oct dig.
      CoolBignum(long(s[len++] - '0'));         // Add next character value
  }
}



// CoolBignum -- CoolBignum constructor with string argument
// Inputs:  string representation of number to be converted to CoolBignum
// Outputs:  new CoolBignum with converted value

CoolBignum::CoolBignum (const char *s) {
  static CoolRegexp                                     // Declare CoolRegexp's for
    decimal(" *^[-+]?[1-9][0-9]*$"),                  // decimal
    exponential(" *^[-+]?[1-9][0-9]*[eE][1-9][0-9]*$"), // exponential
    hexadecimal(" *^[0][xX][0-9a-fA-F]+$"),           // hex
    octal(" *^[0][0-7]*$");                           // octal.

  // Init *this to a CoolBignum value of 0
  this->data = 0;                       
  this->count = 0;                              
  this->sign = 1;                               
  this->state = N_OK;

  if (decimal.find(s))                          // If string is decimal
    this->dtoBignum(s);                         // convert decimal to CoolBignum
  else if (exponential.find(s))                 // If string is exponential
    this->exptoBignum(s);                       // convert exp. to CoolBignum
  else if (hexadecimal.find(s))                 // If string is hex,
    this->xtoBignum(s);                         // convert hex to CoolBignum
  else if (octal.find(s))                       // If string is octal
    this->otoBignum(s);                         // convert octal to CoolBignum
  else {                                        // Otherwise
    this->state = N_NO_CONVERSION;              // raise an exception
    no_conversion("CoolBignum(const char *)");
  }
}



// CoolBignum - CoolBignum constructor with CoolBignum reference argument
// Inputs:  reference to CoolBignum
// Outputs:  new CoolBignum with same value as input

CoolBignum::CoolBignum (const CoolBignum& b) {
  this->count = b.count;                        // Copy b's count
  this->data =                                  // Allocate data if necessary
    (b.count > 0 ? new Data[b.count] : 0);
  for (Counter i = 0; i < this->count; i++)     // Copy b's data 
    this->data[i] = b.data[i];  
  this->sign = b.sign;                          // Copy b's sign
  this->state = b.state;                        // Copy b's state
}



// operator= -- overloaded CoolBignum assignment operator
// Inputs:  reference to CoolBignum to be assigned
// Outputs:  reference to modified CoolBignum

CoolBignum& CoolBignum::operator= (const CoolBignum& b) {
  if (this != &b) {                             // So long as b is not "this"
    delete [] this->data;                       // Delete existing data
    this->count = b.count;                      // Copy b's count
    this->data =                                // Allocate data if necessary
      (this->count > 0 ? new Data[this->count] : 0);
    for (Counter i = 0; i < this->count; i++)   // Copy b's data
      this->data[i] = b.data[i];
    this->sign = b.sign;                        // Copy b's sign
    this->state = b.state;                      // Copy b's state
  }
  return *this;                                 // Return reference 
}



// operator- -- overloaded unary minus operator
// Inputs:  none
// Outputs:  CoolBignum, negated

CoolBignumE CoolBignum::operator- () const {
  CoolBignum temp(*this);                       // Temp stores result
  if (temp.count)                               // So long as this is non-zero
    temp.sign *= -1;                            // Flip its sign
  CoolBignumE& result = *((CoolBignumE*) &temp);// same physical object
  return result;                                // shallow swap on return
}


// operator++ overloaded increment operator for CoolBignums
// Inputs:  none
// Outputs:  reference to this CoolBignum, incremented

CoolBignum& CoolBignum::operator++ () {
  static CoolBignum one_s = 1l;                 // static CoolBignum equal to 1
  *this = *this + one_s;;                       // add one to this CoolBignum
  return *this;                                 // return reference to this B.
}



// operator--  overloaded decrement operator for CoolBignums
// Inputs:  none
// Outputs:  this CoolBignum, decremented

CoolBignum& CoolBignum::operator-- () {
  static CoolBignum one_s = 1l;                 // static CoolBignum equal to 1
  *this = *this - one_s;                        // add one to this CoolBignum
  return *this;                                 // return reference to this B.
}



// resize -- change the data allotment for a CoolBignum
// Inputs:  new size of data allotment
// Outputs:  none

void CoolBignum::resize (Counter new_count) {
  if (new_count != this->count) {               // If new size is really new
    Data *new_data =                            // Allocate data if necessary
      (new_count > 0 ? new Data[new_count] : 0);

    if (this->count <= new_count) {             // Copy old data into new
      Counter i;
      for (i = 0; i < this->count; i++)  
        new_data[i] = this->data[i];
      for (; i < new_count; i++)
        new_data[i] = 0;
    }
    else {                                     
      for (Counter i = 0; i < new_count; i++)  
        new_data[i] = this->data[i];
    }

    delete [] this->data;                       // Get rid of old data
    this->data = new_data;                      // Point to new data
    this->count = new_count;                    // Save new count
  }
}



// trim -- trim CoolBignum of excess data allotment
// Inputs:  none
// Outputs:  reference to modified CoolBignum

CoolBignum& CoolBignum::trim () {
  Counter i;
  for (i = this->count; i > 0; i--)             // Skip over high-order words
    if (this->data[i - 1] != 0) break;          //   that are zero
  if (i < this->count) {                        // If there are some such words
    Counter oldcount = this->count;
    this->count = i;                            // Update the count
    Data *new_data = (i > 0 ? new Data[i] : 0); // Allocate data if necessary
    for (; i > 0; i--)                          // Copy old data into new
      new_data[i - 1] = this->data[i - 1];
    delete [] this->data;                       // Delete old data
    this->data = new_data;                      // Point to new data
  }
  return *this;                                 // return reference to CoolBignum
}


// add -- add two CoolBignum values and save their sum
// Inputs:  references to two CoolBignum addends and the resulting sum
// Outputs:  none

void add (const CoolBignum& b1, const CoolBignum& b2, CoolBignum& sum) {
  const CoolBignum *bmax, *bmin;                        // Determine which of the two
  if (b1.count >= b2.count) {                   //   addends has the most
    bmax = &b1;                                 //   data.
    bmin = &b2;
  }
  else {
    bmax = &b2;
    bmin = &b1;
  }
  sum.data = ((sum.count = bmax->count) > 0 ?   // Allocate data for their sum
              new Data[sum.count] : 0);
  unsigned long temp, carry = 0;
  Counter i = 0;
  while (i < bmin->count) {                     // Add, element by element.
    // Add both elements and carry
    temp = (unsigned long)b1.data[i] + (unsigned long)b2.data[i] + carry;
    carry = temp/radix_s;                       // keep track of the carry
    sum.data[i] = Data(temp);                   // store sum
    i++;                                        // go to next element
  }
  while (i < bmax->count) {                     // bmin has no more elements
    temp = bmax->data[i] + carry;               // propagate the carry through
    carry = temp/radix_s;                       // the rest of bmax's elements
    sum.data[i] = Data(temp);                   // store sum
    i++;
  }
  if (carry) {                                  // if carry left over
    sum.resize(bmax->count + 1);                //   allocate another word
    sum.data[bmax->count] = 1;                  //   save the carry in it
  }
}



// subtract - subtract min CoolBignum from max CoolBignum (unsigned), result in diff
// Inputs:  references to CoolBignum
// Outputs:  none

void subtract (const CoolBignum& bmax, const CoolBignum& bmin, CoolBignum& diff) {
  diff.data = new Data[diff.count = bmax.count];// Allocate data for difference
  unsigned long temp;
  int borrow = 0;
  Counter i;
  for (i = 0; i < bmin.count; i++) {    // Subtract word by word.
    
    temp = (unsigned long)bmax.data[i] 
      + (unsigned long)radix_s - borrow;        // Add radix to bmax's data
    temp -= (unsigned long)bmin.data[i];        // Subtract off bmin's data
    borrow = (temp/radix_s == 0);               // Did we have to borrow?
    diff.data[i] = (Data) temp;                 // Reduce modulo radix and save
  }
  for (; i < bmax.count; i++) {                 // No more data for bmin
    temp = (unsigned long)bmax.data[i] 
      + (unsigned long)radix_s - borrow;        // Propagate the borrow through
    borrow = (temp/radix_s == 0);               //   rest of bmax's data
    diff.data[i] = (Data) temp;
  }
  diff.trim();                                  // Done. Now trim excess data
}



// magnitude_cmp - compare absolute values of two CoolBignums
// Inputs:  two CoolBignums
// Outputs:  result of comparison:  -1 if abs(b1) < abs(b2)
//                                   0 if abs(b1) == abs(b2)
//                                  +1 if abs(b1) > abs(b2)

int magnitude_cmp (const CoolBignum& b1, const CoolBignum& b2) {
  if (b1.count > b2.count) return 1;            // If one has more data than
  if (b2.count > b1.count) return -1;           //   the other, it wins
  Counter i = b1.count;                         // Else same number of elmts
  while (i > 0) {                               // Do lexicographic comparison
    if (b1.data[i - 1] > b2.data[i - 1])        
      return 1;                                 
    else if (b1.data[i - 1] < b2.data[i - 1])   
      return -1;
    i--;
  }                                             // No data, or all elmts same
  return 0;                                     //  so must be equal
}



// operator+ -- overloaded CoolBignum addition operator
// Inputs:  two references to CoolBignum addends
// Outputs:  new CoolBignum sum

CoolBignumE operator+ (const CoolBignum& b1, const CoolBignum& b2) {
  CoolBignum sum;                               // Init sum to zero
  if (b1.sign == b2.sign) {                     // If both have same sign
    add(b1,b2,sum);                             //   Do simple addition
    sum.sign = b1.sign;                         // Attach proper sign
  }
  else {                                        // Else different signs
    int mag = magnitude_cmp(b1,b2);             // Determine relative sizes
    if (mag > 0) {                              // If abs(b1) > abs(b2)
      subtract(b1,b2,sum);                      //   sum = b1 - b2
      sum.sign = b1.sign;                       // Sign of sum follows b1
    }
    else if (mag < 0) {                         // Else if abs(b1) < abs(b2)
      subtract(b2,b1,sum);                      //   sum = b2 - b1
      sum.sign = b2.sign;                       // Sign of sum follows b2
    }                                           // (Else abs(b1) == abs(b2)
  }                                             //   so sum must be zero)
  CoolBignumE& result = *((CoolBignumE*) &sum); // same physical object
  return result;                                // shallow swap on return
}


// multiply_aux -- multiply a CoolBignum by a "single digit"
// Inputs:  CoolBignum reference, single word multiplier, reference to the product,
//          and index of starting storage location to use in product
// Outputs:  none

void multiply_aux (const CoolBignum& b, Data d, CoolBignum& prod, Counter i) {
  // this function works just like normal multiplication by hand, in that the
  // top number is multiplied by the first digit of the bottom number, then the
  // second digit, and so on.  The only difference is that instead of doing all
  // of the multiplication before adding the rows, addition is done
  // concurrently.
  Counter j;
  if (i == 0) {                                 // if index is zero
    j = 0;                                      //   then zero out all of
    while (j < prod.count)                      //   prod's data elements
      prod.data[j++] = 0;                  
  }
  if (d != 0) {                                 // if d == 0, nothing to do
    unsigned long temp;
    Data carry = 0;

    Counter j;
    for (j = 0; j < b.count; j++) {     
      // for each of b's data elmts, multiply times d and add running product
      temp = (unsigned long)b.data[j] * (unsigned long)d
        + (unsigned long)prod.data[i + j] + carry;
      prod.data[i + j] = Data(temp % radix_s);  //   store result in product
      carry = Data(temp/radix_s);               //   keep track of carry
    }
    if (i+j < prod.count)
      prod.data[i + j] = carry;                 // Done.  Store the final carry
  }
}



// operator* -- overload * for CoolBignums
// Inputs:  references to two CoolBignum multiplicands
// Outputs:  CoolBignum product

CoolBignumE operator* (const CoolBignum& b1, const CoolBignum& b2) {
  CoolBignum prod;                              //   init product to zero
  if (b1 != zero_s && b2 != zero_s) {           // if neither multiplicand == 0
    prod.data =                                 //   allocate data for product
      new Data[prod.count = b1.count + b2.count];
    for (Counter i = 0; i < b2.count; i++)      //   multiply each b2 "digit" 
      multiply_aux(b1, b2.data[i], prod, i);    //   times b1 and add to total
    prod.sign = b1.sign * b2.sign;              //   determine correct sign
    prod.trim();                                //   trim excess data and ret.
  }
  CoolBignumE& result = *((CoolBignumE*) &prod);// same physical object
  return result;                                // shallow swap on return
}



// normalize -- normalize two CoolBignums  (Refer to Knuth, V.2, Section 4.3.1,
//              Algorithm D for details.  A digit here is one data element in
//              the radix 2**sizeof(Data).)
// Inputs:  references to two CoolBignums b1, b2, and their normalized counterparts
// Outputs:  the integral normalization factor used

Data normalize (const CoolBignum& b1, const CoolBignum& b2, CoolBignum& u, CoolBignum& v) {
  Data d =                                      // Calcualte normalization
    Data(radix_s/(((long)(b2.data[b2.count - 1])) + 1));        //   factor.
                //^^^^- JAM added this cast because Test#115 was overflowing resulting
                // in Divide Error because value was 65535 and wrapping to 0 with +1
  u.data = new Data[u.count = b1.count + 1];    // Get data for u (plus extra)
  v.data = new Data[v.count = b2.count];        // Get data for v
  u.data[b1.count] = 0;                         // Set u's leading digit to 0
  multiply_aux(b1,d,u,0);                       // u = b1 * d
  multiply_aux(b2,d,v,0);                       // v = b2 * d
  return d;                                     // return normalization factor
}



// divide_aux -- divide a CoolBignum by a "single digit" (Refer to Knuth, V.2,
//               Section 4.3.2, exercise 16 for details.  A digit here is one
//               data element in the radix 2**sizeof(Data).)
// Inputs:  reference to CoolBignum dividend, single digit divisor d, CoolBignum
//          quotient, and single digit remainder r
// Outputs:  none

void divide_aux (const CoolBignum& b1, Data d, CoolBignum& q, Data& r) {
  r = 0;                                        // init remainder to zero
  unsigned long temp;
  for (Counter j = b1.count; j > 0; j--) {
    temp = (unsigned long)r*radix_s 
      + (unsigned long)b1.data[j - 1];          // get remainder, append next
    if (j-1 < q.count) 
      q.data[j - 1] = Data(temp/d);             //   digit, then divide
    r = Data(temp % d);                         // calculate new remainder
  }
}


// estimate_q_hat -- estimate next dividend (Refer to Knuth, V.2, Section
//                   4.3.1, Algorithm D for details.  This function estimates
//                   how many times v goes into u, starting at u's jth digit.
//                   A digit here is actually a data element, thought of as
//                   being in the radix 2**sizeof(Data).)
// Inputs:  reference to CoolBignum dividend and divisor and starting digit j
// Outputs:  estimated number of times v goes into u

Data estimate_q_hat (const CoolBignum& u, const CoolBignum& v, Counter j) {
  Data q_hat,
       v1 = v.data[v.count - 1],                // localize frequent data
       v2 = v.data[v.count - 2],
       u0 = u.data[u.count - 1 - j],
       u1 = u.data[u.count - 2 - j],
       u2 = u.data[u.count - 3 - j];

  // Initial Knuth estimate, usually correct
  q_hat = Data(u0 == v1 ?                       
               radix_s - 1 :                    
               (u0*radix_s + u1)/v1);

  // high speed test to determine most of the cases in which initial
  // estimate is too large.  Eliminates most cases in which q_hat is one too
  // large.  Eliminates all cases in which q_hat is two too large.  The test
  // looks hairy because we have to watch out for overflow.  In the book, this
  // test is the simple inequality:
  //     v2*q_hat > (u0*radix_s + u1 - q_hat*v1)*radix_s + u2.
  // If the inequality is true, decrease q_hat by 1.  If inequality is still
  // true, decrease q_hat again.
  unsigned long lhs, rhs;               // lhs, rhs of Knuth inequality
  for (Counter i = 0; i < 2; i++) {     // loop at most twice
    lhs = v2 * q_hat;                   // Calculate left-hand side of ineq.
    rhs = u0 * radix_s + u1;            // Calculate part of right-hand side 
    rhs -= (q_hat * v1);                // Now subtract off part
    
    // DML:  My attempt to fix the overflow testing bug..
    double temp_rhs = double(rhs);
    double temp_radix_s = double(radix_s);
    // OLD WAY: if (rhs > rhs * radix_s)// if multiplication causes overflow
    // NEW WAY: see if result won't fit into a long.
    if ( temp_rhs * temp_radix_s > double(MAXLONG) )
      break;                            //   then rhs > lhs, so test fails
    rhs *= radix_s;                     // No overflow:  ok to multiply

    temp_rhs = double(rhs);
    double temp_u2 = double(u2);
    // OLD WAY: if (rhs > rhs + u2)     // if addition yields overflow
    if ( temp_rhs + temp_u2 > double(MAXLONG) ) // NEW WAY.
      break;                            //   then rhs > lhs, so test fails
    rhs += u2;                          // No overflow: ok to add.
    if (lhs <= rhs)                     // if lhs <= rhs
      break;                            //   test fails
    q_hat--;                            // Test passes:  decrement q_hat
  }                                     // Loop again
  return q_hat;                         // Return estimate
}

// Original version.
// Data estimate_q_hat (const CoolBignum& u, const CoolBignum& v, Counter j) {
//   Data q_hat,
//        v1 = v.data[v.count - 1],             // localize frequent data
//        v2 = v.data[v.count - 2],
//        u0 = u.data[u.count - 1 - j],
//        u1 = u.data[u.count - 2 - j],
//        u2 = u.data[u.count - 3 - j];
// 
//   // Initial Knuth estimate, usually correct
//   q_hat = Data(u0 == v1 ?                    
//             radix_s - 1 :                    
//             (u0*radix_s + u1)/v1);
// 
//   // high speed test to determine most of the cases in which initial
//   // estimate is too large.  Eliminates most cases in which q_hat is one too
//   // large.  Eliminates all cases in which q_hat is two too large.  The test
//   // looks hairy because we have to watch out for overflow.  In the book, this
//   // test is the simple inequality:
//   //     v2*q_hat > (u0*radix_s + u1 - q_hat*v1)*radix_s + u2.
//   // If the inequality is true, decrease q_hat by 1.  If inequality is still
//   // true, decrease q_hat again.
//   unsigned long lhs, rhs;            // lhs, rhs of Knuth inequality
//   for (Counter i = 0; i < 2; i++) {  // loop at most twice
//     lhs = v2 * q_hat;                        // Calculate left-hand side of ineq.
//     rhs = u0 * radix_s + u1;         // Calculate part of right-hand side 
//     rhs -= (q_hat * v1);             // Now subtract off part
//     if (rhs > rhs * radix_s)         // if multiplication causes overflow
//       break;                         //   then rhs > lhs, so test fails
//     rhs *= radix_s;                  // No overflow:  ok to multiply
//     if (rhs > rhs + u2)                      // if addition yields overflow
//       break;                         //   then rhs > lhs, so test fails
//     rhs += u2;                               // No overflow: ok to add.
//     if (lhs <= rhs)                  // if lhs <= rhs
//       break;                         //   test fails
//     q_hat--;                         // Test passes:  decrement q_hat
//   }                                  // Loop again
//   return q_hat;                              // Return estimate
// }



// multiply_subtract -- calculate u - v*q_hat (Refer to Knuth, V. 2, Section
//                      4.3.1, Algorithm D for details.  A digit here is a
//                      data element, thought of as being in the radix
//                      2**sizeof(Data).)
// Inputs:  reference to CoolBignum dividend, divisor, estimated result, and index
//          into jth digit of dividend
// Outputs:  true number of times v goes into u

Data multiply_subtract (CoolBignum& u, const CoolBignum& v, Data q_hat, Counter j) {
  // At this point it has been estimated that v goes into the jth and higher
  // digits of u about q_hat times, and in fact that q_hat is either the
  // correct number of times or one too large.

  if (q_hat == 0) return q_hat;                 // if q_hat 0, nothing to do
  CoolBignum rslt;                                      // create a temporary CoolBignum
  Counter tmpcnt;
  rslt.data =                                   // allocate data for it
     new Data[rslt.count = v.count + 1];

  // simultaneous computation of u - v*q_hat
  unsigned long prod, diff;
  Data carry = 0, borrow = 0;
  Counter i;
  for (i = 0; i < v.count; i++) {       
    // for each digit of v, multiply it by q_hat and subtract the result
    prod = (unsigned long)v.data[i] * (unsigned long)q_hat + carry;
    diff = (unsigned long)u.data[u.count - v.count - 1 - j + i]
           + (unsigned long)radix_s - borrow;
    diff -= (unsigned long)Data(prod);          //   form proper digit of u
    rslt.data[i] = Data(diff);                  //   save the result
    borrow = (diff/radix_s == 0);               //   keep track of any borrows
    carry = Data(prod/radix_s);                 //   keep track of carries
  }
  tmpcnt = u.count - v.count - 1 - j + i;
  diff = (unsigned long)u.data[tmpcnt]          //  special case for the last
         + (unsigned long)radix_s - borrow;     //     digit
  diff -= carry;
  rslt.data[i] = Data(diff);
  borrow = (diff/radix_s == 0);

  // A leftover borrow indicates that u - v*q_hat is negative, i.e., that
  // q_hat was one too large.  So to get correct result, decrement q_hat and
  // add back one multiple of v
  if (borrow) {
    q_hat--;
    carry = 0;
    unsigned long sum;
    for (i = 0; i < v.count; i++) {
      sum = (unsigned long)rslt.data[i] + (unsigned long)v.data[i] + carry;
      carry = Data(sum/radix_s);
      u.data[u.count - v.count - 1 - j + i] = Data(sum);
    }
    u.data[u.count - v.count - 1 - j + i] = rslt.data[i] + carry;
  }
  else {                                        // otherwise, result is ok
    for (i = 0; i < rslt.count; i++)            // store result back into u
      u.data[u.count - v.count - 1 - j + i] = 
        rslt.data[i];
  }
  return q_hat;                                 // return corrected q_hat
}



// divide - divide b2 into b1, getting quotient q and remainder r.  (Refer to
//          Knuth, V.2, Seciton 4.3.1, Algorithm D for details.  This function
//          implements Algorithm D.)
// Inputs:  references to a CoolBignum dividend b1, divisor b2, quotient q, and
//          remainder r.
// Outputs:  none

void divide (const CoolBignum& b1, const CoolBignum& b2, CoolBignum& q, CoolBignum& r) {
  q = r = zero_s;
  if (b1 == zero_s)                  // If divisor is zero
    return;                          //   return zero quotient and remainder
  int mag = magnitude_cmp(b1,b2);    // Compare magnitudes
  if (mag < 0)                       // if abs(b1) < abs(b2)
    r = b1;                          //   return zero quotient, b1 remainder
  else if (mag == 0)                 // if abs(b1) == abs(b2)
    q = one_s;                       //   quotient is 1, remainder is 0
  else {                             // otherwise abs(b1) > abs(b2), so divide
    q.data = new Data[q.count = b1.count - b2.count + 1]; // Allocate quotient
    r.data = new Data[r.count = b2.count];                // Allocate remainder
    if (b2.count == 1) {                        // Single digit divisor?
      divide_aux(b1,b2.data[0],q,r.data[0]);    // Do single digit divide
    }
    else {                                      // Else full-blown divide
      CoolBignum u,v;
      Data d = normalize(b1,b2,u,v);            // Set u = b1/d, v = b2/d
      Data q_hat;                               // Multiplier
      Counter j = 0;
      while (j <= b1.count - b2.count) {        // Main division loop
        q_hat = estimate_q_hat(u,v,j);          // Estimate # times v divides
        q.data[q.count - 1 - j] =               // Do division, get true answ.
          multiply_subtract(u,v,q_hat,j);
        j++;
      }
      static Data dufus;                        // dummy variable
      divide_aux(u,d,r,dufus);                  // Unnormalize u for remainder
    }
    q.trim();                                   // Trim leading zeros of quot.
    r.trim();                                   // Trim leading zeros of rem.
  }
  q.sign = r.sign = b1.sign * b2.sign;          // Calculate signs
}



// operator/ -- overload division operator for CoolBignums
// Inputs:  references to CoolBignum numerator and denominator
// Outputs:  CoolBignum quotient

CoolBignumE operator/ (const CoolBignum& b1, const CoolBignum& b2) {
  CoolBignum q,r;                               // Quotient and remainder
  if (b2.count == 0) {                          // If b2 is zero
    if (b1.count == 0) {                        // If b1 == zero
      q.state = N_DIVIDE_BY_ZERO;               // Both num & den are zero
      b1.divide_by_zero("operator/ ()");        // Raise the exception
    }
    else if (b1.sign > 0) {                     // If b1 > zero
      q.state = N_PLUS_INFINITY;                // Positive infinity
      b1.plus_infinity("operator/ ()");         // Raise the exception
    }
    else if (b1.sign < 0) {                     // If b1 < zero
      q.state = N_MINUS_INFINITY;               // Negative infinity
      b1.minus_infinity("operator/ ()");        // Raise the exception
    }
  }
  else {
    divide(b1,b2,q,r);                          // Call divide fn
  }
  CoolBignumE& result = *((CoolBignumE*) &q);   // same physical object
  return result;                                // shallow swap on return
}



// operator% -- overload modulus operator for CoolBignums
// Inputs:  references to CoolBignum number and modulus
// Outputs:  CoolBignum modulus result

CoolBignumE operator% (const CoolBignum& b1, const CoolBignum& b2) {
  CoolBignum q,r;                             // Temporary quotient and remainder
  if (b2.count == 0) {                // if b2 == 0
    r.state = N_DIVIDE_BY_ZERO;       //   implies division by zero
    b2.divide_by_zero("operator% ()"); //  raise an exception
  }
  else {                              // otherwise,
    divide(b1,b2,q,r);                //   divide b1 by b2 and save remainder
  }                                  
  CoolBignumE& result = *((CoolBignumE*) &r);   // same physical object
  return result;                                // shallow swap on return
}



// left_shift -- left shift (arithmetic) CoolBignum by positive number.  
// Inputs:  reference to CoolBignum, positive shift value
// Outputs:  CoolBignum, multiplied by the corresponding power of two

CoolBignumE left_shift (const CoolBignum& b1, long l) {
  // to carry out this arithmetic left shift, we cheat.  Instead of physically
  // shifting the data array l bits to the left, we shift just enough to get
  // the correct word alignment, and then pad the array on the right with as
  // many zeros as we need.
  CoolBignum rslt;                                      // result of shift
  rslt.sign = b1.sign;                          // result follows sign of input
  Counter growth = Counter(l / data_bits_s);    // # of words rslt will grow by
  Data shift = Data(l % data_bits_s);           // amount to actually shift
  Data rshift = data_bits_s - shift;            // amount to shift next word by
  Data carry =                                  // value that will be shifted
    b1.data[b1.count - 1] >> (data_bits_s - shift); // out end of current array
  rslt.data =                                       // allocate new data array
    new Data[rslt.count = b1.count + growth + (carry != 0 ? 1 : 0)];
  Counter i = 0;                                
  while (i < growth)                            // zero out padded elements
    rslt.data[i++] = 0;
  rslt.data[i++] = b1.data[0] << shift;         // shift first non-zero element
  while (i < rslt.count - 1) {                  // for remaining data words
    rslt.data[i] = (b1.data[i - growth] << shift) + // shift current data word
      (b1.data[i - 1 - growth] >> rshift);          // propagate adjacent
    i++;                                        // carry into current word
  }
  if (i < rslt.count) {
    if (carry)                                  // if last word had overflow
      rslt.data[i] = carry;                     //   store it new data
    else                                                // otherwise,
      rslt.data[i] = (b1.data[i - growth] << shift) +   // do like the rest
        (b1.data[i - 1 - growth] >> rshift);
  }
  CoolBignumE& result = *((CoolBignumE*) &rslt);// same physical object
  return result;                                // shallow swap on return
}



// right_shift -- right shift (arithmetic) CoolBignum by positive number.  
// Inputs:  reference to CoolBignum, positive shift value
// Outputs:  CoolBignum, divided by the corresponding power of two

CoolBignumE right_shift (const CoolBignum& b1, long l) {
  CoolBignum rslt;                                      // result of shift
  Counter shrinkage = Counter(l / data_bits_s); // # of words rslt will shrink
  Data shift = Data(l % data_bits_s);           // amount to actually shift
  Data dregs = (b1.data[b1.count - 1] >> shift); // high end data to save
  if (shrinkage + (dregs == 0) < b1.count) {    // if not all data shifted out
    rslt.sign = b1.sign;                        //   rslt follows sign of input
                                                //   allocate new data
    rslt.data = new Data[rslt.count = b1.count - shrinkage
                 - (dregs == 0 ? 1 : 0)];
    Data lshift = data_bits_s - shift;          //   amount to shift high word
    Counter i = 0;
    while (i < rslt.count - 1) {                //   shift current word
      rslt.data[i] = (b1.data[i + shrinkage] >> shift) + // propagate adjacent
        (b1.data[i + shrinkage + 1] << lshift); //   word into current word
      i++;
    }
    if (dregs)                                  // don't lose dregs
      rslt.data[i] = dregs;
    else {
      rslt.data[i] = (b1.data[i + shrinkage] >> shift) +
        (b1.data[i + shrinkage + 1] << lshift);
    }
  }
  CoolBignumE& result = *((CoolBignumE*) &rslt);// same physical object
  return result;                                // shallow swap on return
}


// operator<< -- overload left shift operator for CoolBignums
// Inputs:  reference to CoolBignum to be shifted, left shift amount
// Outputs:  shifted CoolBignum

CoolBignumE operator<< (const CoolBignum& b1, long l) {
  if (l == 0 || b1 == zero_s) {                 // if either arg is zero
    CoolBignum b(b1);                           // copy b1
    CoolBignumE& result = *((CoolBignumE*) &b); // same physical object
    return result;                              // shallow swap on return
  }
  if (l < 0)                                    // if shift amt is negative
    return (CoolBignumE &) right_shift(b1,-l);                  //   do an actual right shift
  else                                          // otherwise
    return (CoolBignumE &) left_shift(b1,l);                    //   do a left shift
}


// operator>> -- overload right shift operator for CoolBignums
// Inputs:  reference to CoolBignum to be shifted, right shift amount
// Outputs:  shifted CoolBignum

CoolBignumE operator>> (const CoolBignum& b1, long l) {
  if (l == 0 || b1 == zero_s) {                 // if either arg is zero
    CoolBignum b(b1);                           // copy b1
    CoolBignumE& result = *((CoolBignumE*) &b); // same physical object
    return result;                              // shallow swap on return
  }
  if (l < 0)                                    // if shift amt is negative
    return (CoolBignumE &) left_shift(b1,-l);                   //   do an actual left shift
  else                                          // else
    return (CoolBignumE &) right_shift(b1,l);                   //   do a right shift
}



// operator== -- overload equality operator for CoolBignums
// Inputs:  reference to CoolBignum to compare to
// Outputs:  Boolean equality indicator

Boolean CoolBignum::operator== (const CoolBignum& b) const {
  if (this->sign != b.sign) return FALSE;       // Different sign implies !=
  if (this->count != b.count) return FALSE;     // Different size implies !=
  for (Counter i = 0; i < this->count; i++)     // Each data element the same?
    if (this->data[i] != b.data[i]) return FALSE; // No. Return !=
  return TRUE;                                    // Yes. Return ==
}



// operator< -- overload less-than operator for CoolBignums
// Inputs:  reference to CoolBignum to compare to
// Outputs:  Boolean less-than indicator

Boolean CoolBignum::operator< (const CoolBignum& b) const {
  if (this->sign < b.sign) return TRUE;         // Different signs?
  if (this->sign > b.sign) return FALSE;
  if (this->sign == 1)                          // Both signs == 1
    return (magnitude_cmp(*this,b) < 0 ? TRUE: FALSE); // this must be smaller
  else                                          // Both signs == -1
    return (magnitude_cmp(*this,b) > 0 ? TRUE: FALSE); // this must be larger
}



// operator> -- overload greater-than operator for CoolBignums
// Inputs:  reference to CoolBignum to compare to
// Outputs:  Boolean greater-than indicator

Boolean CoolBignum::operator> (const CoolBignum& b) const {
  if (this->sign > b.sign) return TRUE;         // Different signs?
  if (this->sign < b.sign) return FALSE;
  if (this->sign == 1)                          // Both signs == 1
    return (magnitude_cmp(*this,b) > 0 ? TRUE: FALSE); // this must be larger
  else                                          // Both signs == -1
    return (magnitude_cmp(*this,b) < 0 ? TRUE: FALSE); // this must be smaller
}



// operator<< -- overload output operator for CoolBignums
// Inputs:  reference to output stream and CoolBignum
// Outputs:  reference to output stream

ostream& operator<< (ostream& os, const CoolBignum& b) {
  CoolBignum d = b;                                     // Copy the input CoolBignum
  if (d.sign == -1) {                           // If it's negative
    cout.put('-');                              //   Output leading minus sign
    d.sign = 1;                                 //   Make d positive for divide
  }
  CoolBignum q,r;                                       // Temp quotient and remainder
  char *cbuf = new char[5 * b.count];           // Temp character buffer
  Counter i = 0;
  do {                                          // repeat:
    divide(d,ten_s,q,r);                        //   Divide CoolBignum by ten
    cbuf[i++] = char(long(r) + '0');            //   Get one's digit
    d = q;                                      //   Then discard one's digit
    q = r = zero_s;                             //   Prep for next divide
  } while (d != zero_s);                        // until no more one's digits
  do {                                          // repeat;
    os.put(cbuf[--i]);                          //   output char buf in reverse
  } while (i);                                  // until no more chars
  delete [] cbuf;                               // delete temp char buf
  return os;                                    // return output stream
}



// operator short - conversion operator from CoolBignum to short
// Inputs:  none
// Outputs:  CoolBignum converted to short

CoolBignum::operator short () const {
  if (*this > max_short_s) {                    // If short overflow
    overflow("operator short ()");              //   raise exception
  }
  if (*this < -max_short_s) {                   // If short underflow
    underflow("operator short ()");             //   raise exception
  }
  short s = 0;                                  // Default is zero conversion
  for (Counter i = this->count; i > 0; i--)
    s = short(s*radix_s + this->data[i - 1]);
  return this->sign*s;                          // calculate sign
}



// operator int - conversion operator from CoolBignum to int
// Inputs:  none
// Outputs:  CoolBignum converted to int

CoolBignum::operator int () const {
  if (*this > max_int_s) {                      // If int overflow
    overflow("operator int ()");                //   raise exception
  }
  if (*this < -max_int_s) {                     // If int underflow
    underflow("operator int ()");               //   raise exception
  }
  int j = 0;                                    // Otherwise, do conversion
  for (Counter i = this->count; i > 0; i--)     // For each data element
    j = int(j*radix_s + this->data[i - 1]);     //   stick it into j and shift
  return this->sign*j;                          // Attach sign
}



// operator long - conversion operator from CoolBignum to long
// Inputs:  none
// Outputs:  CoolBignum converted to long

CoolBignum::operator long () const {
  if (*this > max_long_s) {                     // If long overflow
    overflow("operator long ()");               //   raise exception
  }
  if (*this < -max_long_s) {                    // If long underflow
    underflow("operator long ()");              //   raise exception
  }
  long l = 0;                                   // Otherwise, do conversion
  for (Counter i = this->count; i > 0; i--)     // For each data element
    l = l*radix_s + this->data[i - 1];          //   stick it into l and shift
  return this->sign*l;                          // Attach sign
}



// operator float - conversion operator from CoolBignum to float
// Inputs:  none
// Outputs:  CoolBignum converted to float

CoolBignum::operator float () const {
  if (*this > max_float_s) {                    // If float overflow
    overflow("operator float ()");              //   raise exception
  }
  if (*this < negmax_float_s) {                 // If float underflow
    underflow("operator float ()");             //   raise exception
  }
  float f = 0.0;                                // Otherwise, do conversion
  for (Counter i = this->count; i > 0; i--)     // For each data element
    f = f*radix_s + this->data[i - 1];          //   stick it into f and shift
  return this->sign*f;                          // Attach sign
}



// operator double - conversion operator from CoolBignum to double
// Inputs:  none
// Outputs:  CoolBignum converted to double

CoolBignum::operator double () const {
  if (*this > max_double_s) {                   // If double overflow
    overflow("operator double ()");             //   raise exception
  }
  if (*this < negmax_double_s) {                        // If double underflow
    underflow("operator double ()");            //   raise exception
  }
  double d = 0.0;                               // Otherwise, do conversion
  for (Counter i = this->count; i > 0; i--)     // For each data element
    d = d*radix_s + this->data[i - 1];          //   stick it into d and shift
  return this->sign*d;                          // Attach sign
}



// print --  terse print function for CoolBignums
// Inputs:   reference to output stream
// Outputs:  none

void CoolBignum::print (ostream& os) {
  os << "/* CoolBignum " << (long)this << " */";
}



// dump -- dump the contents of a CoolBignum to a stream
// Inputs:  stream to dump to (default cout)
// Outputs:  none

void CoolBignum::dump (ostream& os) {
  os << "{count: " << this->count <<            // output count field
        ", sign: " << this->sign  <<            // output sign field
        ", state: " << this->state <<           // output state field
        ", data: " << this->data <<             // output data pointer
        ", {";
#ifdef THE_OLD_WAY_USING_THAT_UGLY_FORM
  // format string == "%04X%s" or "%02X%s", etc.
  static char format_str[10] =
    {'%','0',char(2*sizeof(Data) + '0'),'X','%','s'};
  format_str[2] = char(2*sizeof(Data) + '0');
  if (this->count > 0) {                        // output data array
    for (Counter i = this->count; i > 1; i--)
      os << form(format_str,this->data[i - 1],","); 
    os << form(format_str,this->data[0],"");
  }
#else // the new and improved way which probably no two compilers get alike
  if (this->count > 0) {                        // output data array
    long save_flags = os.flags();         // save format state
    char save_fill = os.fill('0');        // use fill character
    int width = sizeof(Data)*CHAR_BIT/4;  // max width of data in hex
    for (Counter i = this->count; i > 1; i--)
      os << setw(width) << hex << this->data[i - 1] << ","; 
    os << setw(width) << hex << this->data[0];
    os.fill(save_fill);                   // reset fill character
    os.flags(save_flags);                 // rest format state (eg, dec)
  }
#endif
  os << "}}";                                   // close brackets
}



// minus_infinity -- raise a minus infinity error exeption
// Inputs:  name of function in which exception occurred
// Outputs:  none

void CoolBignum::minus_infinity (const char* name) const {
  //RAISE (Error, SYM(CoolBignum), SYM(Minus_Infinity),
  printf ("CoolBignum::%s: Quotient is negative infinity.\n", name);
  abort ();
}



// plus_infinity -- raise a plus infinity error exception
// Inputs:  name of function in which exception occurred
// Outputs:  none

void CoolBignum::plus_infinity (const char* name) const {
  //RAISE (Error, SYM(CoolBignum), SYM(Plus_Infinity),
  printf ("CoolBignum::%s: Quotient is positive infinity.\n", name);
  abort ();
}



// divide_by_zero -- raise a division by zero error exception
// Inputs:  name of function in which exception occurred
// Outputs:  none

void CoolBignum::divide_by_zero (const char* name) const {
  //RAISE (Error, SYM(CoolBignum), SYM(Divide_By_Zero),
  printf ("CoolBignum::%s: Divide by zero.\n", name);
  abort ();
}



// overflow -- raise an overflow exception
// Inputs:  name of function in which exception occurred
// Outputs:  none

void CoolBignum::overflow (const char* name) const {
  //RAISE (Error, SYM(CoolBignum), SYM(Overflow),
  printf ("CoolBignum::%s: Overflow occured during type conversion.\n", name);
  abort ();
}



// underflow -- raise an underflow exception
// Inputs:  name of function in which exception occurred
// Outputs:  none

void CoolBignum::underflow (const char* name) const {
  //RAISE (Error, SYM(CoolBignum), SYM(Underflow),
  printf ("CoolBignum::%s: Underflow occured during type conversion.\n", name);
  abort ();
}



// no_conversion -- raise a no conversion exception
// Inputs:  name of function in which exception occurred
// Outputs:  none

void CoolBignum::no_conversion (const char* name) const {
  //RAISE(Error, SYM(CoolBignum), SYM(No_Conversion),
  printf ("CoolBignum::%s: Can not convert input parameter to CoolBignum.\n", name);
  abort ();
}


// end Bignum.C
