//
// Copyright (C) 1991 Texas Instruments Incorporated.
// Copyright (C) 1992 General Electric Company.
//
// Permission is granted to any individual or institution to use, copy, modify,
// and distribute this software, provided that this complete copyright and
// permission notice is maintained, intact, in all copies and supporting
// documentation.
//
// Texas Instruments Incorporated, General Electric Company,
// provides this software "as is" without express or implied warranty.
//
// Created: MBN 04/21/89 -- Initial design and implementation
// Updated: MBN 06/22/89 -- Removed non-destructive methods
// Updated: LGO 08/09/89 -- Inherit from Generic
// Updated: MBN 08/20/89 -- Changed template usage to reflect new syntax
// Updated: MBN 09/11/89 -- Added conditional exception handling and base class
// Updated: LGO 10/05/89 -- Don't re-allocate data in operator= when same size
// Updated: LGO 10/19/89 -- Add extra parameter to varargs constructor
// Updated: MBN 10/19/89 -- Added optional argument to set_compare method
// Updated: LGO 12/08/89 -- Allocate column data in one chunk
// Updated: LGO 12/08/89 -- Clean-up get and put, add const everywhere.
// Updated: LGO 12/19/89 -- Remove the map and reduce methods
// Updated: MBN 02/22/90 -- Changed size arguments from int to unsigned int
// Updated: MJF 06/30/90 -- Added base class name to constructor initializer
// Updated: VDN 02/21/92 -- New lite version
// Updated: VDN 05/05/92 -- Use envelope to avoid unecessary copying
// Updated: JAM 08/24/92 -- removed DOS specifics, stdized #includes
// Updated: JAM 08/24/92 -- modernized template syntax, remove macro hacks
//                          non-template classes CoolMatrix=>CoolBase_Matrix
// Updated: JAM 08/24/92 -- removed any reference to envelope classes
// Updated: JAM 08/24/92 -- no default args for operators, so added extra op()
// Updated: JAM 08/24/92 -- made put()'s value a 'const' Type&
// Updated: JAM 09/26/92 -- put envelope back but using modern templates
//
// The parameterized Matrix<Type>  class is publicly   derived from the  Matrix
// class and implements two dimensional arithmetic matrices of a user specified
// type.   This is accompilshed by using  the parameterized  type capability of
// C++.  The only constraint placed on the type  is  that it must  overload the
// following operators: +, -,  *,  and /. Thus, it will  be possible to have  a
// Matrix of  type Complex.  The Matrix<Type> class  is static in size, that is
// once a  Matrix<Type> of  a particular  size has been   declared, there is no
// dynamic growth or resize method available.
//
// Each matrix contains  a protected  data section  that has a  Type** slot  that
// points to the  physical memory allocated  for the two  dimensional array. In
// addition, two integers  specify   the number  of  rows  and columns  for the
// matrix.  These values  are provided in the  constructors. A single protected
// slot  contains a pointer  to a compare  function  to   be used  in  equality
// operations. The default function used is the built-in == operator.
//
// Four  different constructors are provided.  The  first constructor takes two
// integer arguments  specifying the  row  and column  size.   Enough memory is
// allocated to hold row*column elements  of type Type.  The second constructor
// takes the  same two  first arguments, but  also accepts  an additional third
// argument that is  a reference to  an  object of  the appropriate  type whose
// value is used as an initial fill value.  The third constructor is similar to
// the third, except that it accpets a variable number of initialization values
// for the Matrix.  If there are  fewer values than elements,  the rest are set
// to zero. Finally, the last constructor takes a single argument consisting of
// a reference to a Matrix and duplicates its size and element values.
//
// Methods   are  provided   for destructive   scalar   and Matrix    addition,
// multiplication, check for equality  and inequality, fill, reduce, and access
// and set individual elements.  Finally, both  the  input and output operators
// are overloaded to allow for fomatted input and output of matrix elements.

#ifndef MATRIXH                                 // If no Matrix class,
#define MATRIXH                                 // define it

#ifndef STDARGH
#include <stdarg.h>                             // for variable arglists
#define STDARGH
#endif

#ifndef BASE_MATRIXH                            // If no base class definition
#include <cool/Base_Matrix.h>                   // include it
#endif  

//## hack to workaround BC++ 3.1 Envelope bug
#undef CoolEnvelope_H
#define CoolEnvelope CoolEnvelope_Matrix

template<class CoolLetter> class CoolEnvelope;

template<class Type>
class CoolMatrix : public CoolBase_Matrix {
public:
  typedef Boolean (*Compare)(const Type&, const Type&);

  CoolMatrix(unsigned int r=1, unsigned int c=1); // m (r,c);
  CoolMatrix(unsigned int r, unsigned int c, const Type& v0); // m(r,c,init);
  CoolMatrix(unsigned int r, unsigned int c, int n, 
                    Type v00, ...); 
  CoolMatrix(unsigned int r, unsigned int c, // from a block of data
                    const Type* data_block);        // stored row-wise.

  CoolMatrix(const CoolMatrix<Type>&);  // m1 = m2;
  ~CoolMatrix();                                // Destructor
  
  inline void put (unsigned int r, unsigned int, const Type&); // Assign value
  inline Type& get (unsigned int r, unsigned int);         // Get value
  void fill (const Type&);                                 // Set elements to value
  
  inline Type& operator() (unsigned int r, unsigned int c); // Access wo checks
  inline Type& operator() (unsigned int r) { return (*this)(r,0); }
  
  CoolMatrix<Type>& operator= (const Type&);    // Assignment: m = 2;
  CoolMatrix<Type>& operator= (const CoolMatrix<Type>&); // Assignment: m = n;
  inline CoolMatrix<Type>& operator= (CoolEnvelope< CoolMatrix<Type> >&); 
  
  Boolean operator== (const CoolMatrix<Type>&) const; // CoolMatrix equality test
  inline Boolean operator!= (const CoolMatrix<Type>&) const; // inequality test
  void set_compare (Compare = NULL);         // Compare function
  
  template< class U >
  friend ostream& operator<< (ostream&, const CoolMatrix<U>&);
  template< class U >
  inline friend ostream& operator<< (ostream&, const CoolMatrix<U>*);
  
  CoolMatrix<Type>& operator+= (const Type&);   // binary operation and assignment
  CoolMatrix<Type>& operator*= (const Type&);   // Mutate matrix data
  CoolMatrix<Type>& operator/= (const Type&);
  inline CoolMatrix<Type>& operator-= (const Type&);    
  
  CoolMatrix<Type>& operator+= (const CoolMatrix<Type>&);
  CoolMatrix<Type>& operator-= (const CoolMatrix<Type>&);
  inline CoolMatrix<Type>& operator*= (const CoolMatrix<Type>&);
  
  CoolEnvelope< CoolMatrix<Type> > operator- () const;          // negation and 
  CoolEnvelope< CoolMatrix<Type> > operator+ (const Type&) const; // all binary operations 
  CoolEnvelope< CoolMatrix<Type> > operator* (const Type&) const; // return by values.
  CoolEnvelope< CoolMatrix<Type> > operator/ (const Type&) const;
  
  inline CoolEnvelope< CoolMatrix<Type> > operator- (const Type&) const;
  template< class U >
  inline friend CoolEnvelope< CoolMatrix<U> > operator+ (const U&, const CoolMatrix<U>&);
  template< class U >
  inline friend CoolEnvelope< CoolMatrix<U> > operator- (const U&, const CoolMatrix<U>&);
  template< class U >
  inline friend CoolEnvelope< CoolMatrix<U> > operator* (const U&, const CoolMatrix<U>&);
  
// Fewer unnecessary copying with CoolEnvelope
//  friend CoolMatrix<Type> operator+ (const CoolMatrix<Type>&, 
//                                    const CoolMatrix<Type>&);
//  friend CoolMatrix<Type> operator- (const CoolMatrix<Type>&, 
//                                    const CoolMatrix<Type>&);
  template< class U >
  friend CoolEnvelope< CoolMatrix<U> > operator* (const CoolMatrix<U>&,
                                                  const CoolMatrix<U>&);
  
  ////--------------------------- Additions ------------------------------------
  
  CoolEnvelope< CoolMatrix<Type> > transpose () const;          // transpose row/column
  
  CoolEnvelope< CoolMatrix<Type> > abs () const;                // absolute of all elements
  CoolEnvelope< CoolMatrix<Type> > sign () const;
  CoolEnvelope< CoolMatrix<Type> > extract (unsigned int rows, unsigned int cols, // get submatrix
                            unsigned int top=0, unsigned int left=0) const;
  CoolMatrix<Type>& update (const CoolMatrix<Type>&, // update submatrix
                            unsigned int top=0, unsigned int left=0);        
  
  template< class U >
  friend CoolEnvelope< CoolMatrix<U> > element_product (const CoolMatrix<U>&, // a[ij]*b[ij]
                                                        const CoolMatrix<U>&);
  template< class U >
  friend CoolEnvelope< CoolMatrix<U> > element_quotient (const CoolMatrix<U>&, // a[ij]/b[ij]
                                                         const CoolMatrix<U>&);
  Type determinant() const;                     // determinant of square matrix

  
  ////--------------------------- Vector ---------------------------------------
  
  inline Type& x ();                            // using a 2d matrix
  inline Type& y ();                            // to represent a 1d vector
  inline Type& z ();                            // is less efficient 
  inline Type& t ();                            // in time and space
  
  template< class U >
  friend U dot_product (const CoolMatrix<U>&,   // dot-product of n-dim vectors
                        const CoolMatrix<U>&);
  template< class U >
  friend U cross_2d (const CoolMatrix<U>&,      // cross-product of 2d-vectors
                     const CoolMatrix<U>&);
  template< class U >
  friend CoolEnvelope< CoolMatrix<U> > cross_3d (const CoolMatrix<U>&,  // cross-product 
                                                 const CoolMatrix<U>&); // of 3d-vectors 

  inline const Type* data_block ();             // block of data, row-major order.

protected:
  Type** data;                                  // Pointer to the CoolMatrix 
  static Boolean (*compare_s)(const Type&, const Type&);        // Pointer operator== function
  template< class U >
  friend Boolean CoolMatrix_is_data_equal (const U&, const U&);
};


//## BC++ 3.1 bug
void hack(CoolMatrix<int>);
void hack(CoolMatrix<float>);
void hack(CoolMatrix<double>);
//## add your type above
#include <cool/Envelope.h>    //## BC++ 3.1 bug prevents from moving to top

// Use envelope to avoid deep copy on return by value, and mutate in place
template<class Type>
inline CoolEnvelope< CoolMatrix<Type> > operator+ (const CoolMatrix<Type>&arg1,const CoolMatrix<Type>&arg2)
   { return (CoolEnvelope< CoolMatrix<Type> > &) CoolEnvOp(add)(arg1, arg2); }
template<class Type>
inline CoolEnvelope< CoolMatrix<Type> > operator- (const CoolMatrix<Type>&arg1,const CoolMatrix<Type>&arg2)
   { return (CoolEnvelope< CoolMatrix<Type> > &) CoolEnvOp(minus)(arg1, arg2); }


// get -- Get the element at specified index and return value
// Input: this*, row, column
// Output: Element value

template<class Type> 
inline Type& CoolMatrix<Type>::get (unsigned int row, unsigned int column) {
#if ERROR_CHECKING
  if (row >= this->num_rows)                    // If invalid size specified
    this->row_index_error ("get", #Type, row);  // Raise exception
  if (column >= this->num_cols)                 // If invalid size specified
    this->col_index_error ("get", #Type, column); // Raise exception
#endif
  return this->data[row][column];
}

// put -- Put the element value at specified index
// Input: *this, row, column, value
// Output: Element value

template<class Type> 
inline void CoolMatrix<Type>::put (unsigned int row, unsigned int column, const Type& value) {
#if ERROR_CHECKING
  if (row >= this->num_rows)                    // If invalid size specified
    this->row_index_error ("put", #Type, row);  // Raise exception
  if (column >= this->num_cols)                 // If invalid size specified
    this->col_index_error ("put", #Type, column); // Raise exception
#endif
  this->data[row][column] = value;              // Assign data value
}

// operator() -- Overload () to get the element at specified index
//               and return by reference
// Input: this*, row, column
// Output: Element value

template<class Type> 
inline Type& CoolMatrix<Type>::operator() (unsigned int row, unsigned int column) {
  return this->data[row][column];               // fast access without checks.
}


// operator=  -- Assignment from an envelope back to real matrix
// Input:     envelope reference
// Output:    matrix reference with contents in envelope being swapped over

template<class Type>
inline CoolMatrix<Type>& CoolMatrix<Type>::operator= (CoolEnvelope< CoolMatrix<Type> >& env){
  env.shallow_swap((CoolEnvelope< CoolMatrix<Type> >*)this, &env); // same physical layout
  return *this;
}

// operator<< -- Overload the output operator to print a CoolMatrix
// Input:        ostream reference, CoolMatrix pointer
// Output:       ostream reference

template<class Type>
inline ostream& operator<< (ostream& os, const CoolMatrix<Type>* m) {
  if (m) os << *m;
  return os;
}


// operator!= -- Perform not equal comparison test
// Input:        this*, matrix reference
// Output:       TRUE/FALSE

template<class Type> 
inline Boolean CoolMatrix<Type>::operator!= (const CoolMatrix<Type>& m) const {
  return (!operator== (m));
}

// operator-= -- Destructive matrix subtraction of a scalar.
// Input:        this*, scalar value
// Output:       New matrix reference

template<class Type> 
inline CoolMatrix<Type>& CoolMatrix<Type>::operator-= (const Type& value) {
  return *this += (- value);
}


template<class Type>
inline CoolEnvelope< CoolMatrix<Type> > operator+ (const Type& value,
                                    const CoolMatrix<Type>& m) {
  return (CoolEnvelope< CoolMatrix<Type> > &) (m + value);
}

// operator- -- Non-destructive matrix substraction of a scalar.
// Input:       this*, scalar value
// Output:      New matrix 

template<class Type> 
inline CoolEnvelope< CoolMatrix<Type> > CoolMatrix<Type>::operator-(const Type& value) const {
  return (CoolEnvelope< CoolMatrix<Type> > &) ((*this) + (- value));
}

template<class Type>
inline CoolEnvelope< CoolMatrix<Type> > operator- (const Type& value,
                                    const CoolMatrix<Type>& m) {
  return (CoolEnvelope< CoolMatrix<Type> > &) ((- m) + value);
}


template<class Type>
inline CoolEnvelope< CoolMatrix<Type> > operator* (const Type& value,
                                    const CoolMatrix<Type>& m) {
  return (CoolEnvelope< CoolMatrix<Type> > &) (m * value);
}

template<class Type>
inline CoolMatrix<Type>& CoolMatrix<Type>::operator*= (const CoolMatrix<Type>&m) {
  *this = (*this) * m;                          // multiply, then shallow swap
  return *this;
}

////--------------------------  Vector ----------------------------------
//// use a column matrix to represent 1d vector

template<class Type>                            
inline Type& CoolMatrix<Type>::x(){
  return data[0][0];
}               

template<class Type>
inline Type& CoolMatrix<Type>::y() {
  return data[1][0];
}

template<class Type>
inline Type& CoolMatrix<Type>::z() {
  return data[2][0];
}               

template<class Type>
inline Type& CoolMatrix<Type>::t() {
  return data[3][0];
}       


// template<class Type>                         // use a row matrix to
// inline Type& CoolMatrix<Type>::x() {         // represent 1d vector
//   return data[0][0];
// }            
// 
// template<class Type>
// inline Type& CoolMatrix<Type>::y() {
//   return data[0][1];
// }
// 
// template<class Type>
// inline Type& CoolMatrix<Type>::z() {
//   return data[0][2];
// }            
// 
// template<class Type>
// inline Type& CoolMatrix<Type>::t() {
//   return data[0][3];
// }    

// data_block -- Provide access to the contiguous block storing 
//               the elements in the matrix row-wise.
//               Use this only to cast matrix contents,
//               to C array like: Type [rows][columns].

template<class Type>
inline const Type* CoolMatrix<Type>::data_block () {
  return data[0];                               // start of array of data
}

//## hack to workaround BC++ 3.1 Envelope bug
#undef CoolEnvelope

#endif                                          // End of MATRIXH


