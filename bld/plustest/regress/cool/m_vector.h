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
// Created: VDN 02/21/92 -- new version adapted from Matrix.h
// Updated: JAM 08/15/92 -- removed DOS specifics, stdized #includes
// Updated: JAM 08/24/92 -- modernized template syntax, remove macro hacks
//                          non-template classes CoolM_Vector=>CoolBase_M_Vector
// Updated: JAM 08/24/92 -- removed references to envelope class
// Updated: JAM 08/24/92 -- made some binary member funcs friends instead
// Updated: JAM 08/24/92 -- made put()'s value a 'const' Type&
// Updated: JAM 09/26/92 -- put envelope back but using modern templates
//
// The parameterized M_Vector<Type>  class is publicly   derived from the M_Vector
// class and implements one dimensional arithmetic  vectors  of a user specified
// type.   This is accompilshed by using  the parameterized  type capability of
// C++.  The only constraint placed on the type  is  that it must  overload the
// following operators: +, -,  *,  and /. Thus, it will  be possible to have  a
// M_Vector of  type Complex.  The M_Vector<Type> class  is static in size, that is
// once a  M_Vector<Type> of  a particular  size has been   declared, there is no
// dynamic growth or resize method available.
//
// Each vector contains  a protected  data section  that has a  Type* slot  that
// points to the  physical memory allocated  for the one  dimensional array. In
// addition, an integer  specifies   the number  of  elements  for the
// vector.  These values  are provided in the  constructors. A single protected
// slot  contains a pointer  to a compare  function  to   be used  in  equality
// operations. The default function used is the built-in == operator.
//
// Four  different constructors are provided.  The  first constructor takes an
// integer arguments  specifying the  length. Enough memory is
// allocated to hold [length] elements  of type Type.  The second constructor
// takes the  same first argument, but  also accepts  an additional second
// argument that is  a reference to  an  object of  the appropriate  type whose
// value is used as an initial fill value.  The third constructor is similar to
// the third, except that it accpets a variable number of initialization values
// for the M_Vector.  If there are  fewer values than elements,  the rest are set
// to zero. Finally, the last constructor takes a single argument consisting of
// a reference to a M_Vector and duplicates its size and element values.
//
// Methods   are  provided   for destructive   scalar   and vector  addition,
// multiplication, check for equality  and inequality, fill, reduce, and access
// and set individual elements.  Finally, both  the  input and output operators
// are overloaded to allow for fomatted input and output of vector elements.
//
// M_Vector is a special type of matrix, and is implemented for space and time
// efficiency. When vector is pre_multiplied by/with matrix, m*v, vector is
// implicitly a column matrix. When vector is post_multiplied by/with matrix, v*m,
// vector is implicitly a row matrix.
//

#ifndef M_VECTORH                               // If no M_Vector class,
#define M_VECTORH                               // define it

#ifndef STDARGH
#include <stdarg.h>                             // for variable arglists
#define STDARGH
#endif

#ifndef MATHH
#include <math.h>                               // for sqrt
#define MATHH
#endif

#ifndef BASE_M_VECTORH                          // If no base class definition
#include <cool/Base_M_Vector.h>                 // include it
#endif  

#include <cool/Matrix.h>

//## hack to workaround BC++ 3.1 Envelope bug
#undef CoolEnvelope_H
#define CoolEnvelope CoolEnvelope_M_Vector

template<class CoolLetter> class CoolEnvelope;

template<class Type>
class CoolM_Vector : public CoolBase_M_Vector {
public:
  CoolM_Vector(unsigned int len=1);             // v (n);
  CoolM_Vector(unsigned int len, const Type& v0); // m(n,val);
  CoolM_Vector(unsigned int len, int n, Type v00, ...); // Opt. values
  CoolM_Vector(const CoolM_Vector<Type>&);              // v1(v2);
  ~CoolM_Vector();                                      // Destructor
  
  inline void put (unsigned int i, const Type&);        // Assign value
  inline Type& get (unsigned int i);            // Get value
  void fill (const Type&);                      // set elements to value
  
  inline Type& operator() (unsigned int i);     // Access wo checks
  
  CoolM_Vector<Type>& operator= (const Type&);  // Assignment: m = 2;
  CoolM_Vector<Type>& operator= (const CoolM_Vector<Type>&); // Assignment: m = n;
  inline CoolM_Vector<Type>& operator= (CoolEnvelope< CoolM_Vector<Type> >&); // envelope to vector
  
  Boolean operator== (const CoolM_Vector<Type>&) const; // CoolM_Vector equality test
  inline Boolean operator!= (const CoolM_Vector<Type>&) const; // inequality test
  void set_compare (Boolean (*) (const Type&, const Type&) = NULL);       // Compare function
  
  friend ostream& operator<< (ostream&, const CoolM_Vector<Type>&);
  /*inline##*/ friend ostream& operator<< (ostream&, const CoolM_Vector<Type>*);
  
  CoolM_Vector<Type>& operator+= (const Type&); // binary operation and assignment
  CoolM_Vector<Type>& operator*= (const Type&); // Mutate vector data
  CoolM_Vector<Type>& operator/= (const Type&);
  inline CoolM_Vector<Type>& operator-= (const Type&);  
  
  CoolM_Vector<Type>& operator+= (const CoolM_Vector<Type>&);
  CoolM_Vector<Type>& operator-= (const CoolM_Vector<Type>&); 

  CoolM_Vector<Type>& pre_multiply (const CoolMatrix<Type>&); // v = m * v
  CoolM_Vector<Type>& post_multiply (const CoolMatrix<Type>&); // v = v * m
  inline CoolM_Vector<Type>& operator*= (const CoolMatrix<Type>&); // v = v * m, post-multiply

  CoolEnvelope< CoolM_Vector<Type> > operator- () const;                // negation and 
  CoolEnvelope< CoolM_Vector<Type> > operator+ (const Type&) const;     // all binary operations 
  CoolEnvelope< CoolM_Vector<Type> > operator* (const Type&) const;     // return by values.
  CoolEnvelope< CoolM_Vector<Type> > operator/ (const Type&) const;
  
  inline CoolEnvelope< CoolM_Vector<Type> > operator- (const Type&) const; 
  template< class U >
  inline friend CoolEnvelope< CoolM_Vector<U> > operator+(const U&, const CoolM_Vector<U>&);
  template< class U >
  inline friend CoolEnvelope< CoolM_Vector<U> > operator-(const U&, const CoolM_Vector<U>&);
  template< class U >
  inline friend CoolEnvelope< CoolM_Vector<U> > operator*(const U&, const CoolM_Vector<U>&);

// Fewer unnecessary copying with CoolEnvelope
//   friend CoolM_Vector<Type> operator+ (const CoolM_Vector<Type>&,
//                                      const CoolM_Vector<Type>&);
//   friend CoolM_Vector<Type> operator- (const CoolM_Vector<Type>&,
//                                      const CoolM_Vector<Type>&);
  template< class U >
  friend CoolEnvelope< CoolM_Vector<U> > operator* (const CoolM_Vector<U>&, const CoolMatrix<U>&);
  template< class U >
  friend CoolEnvelope< CoolM_Vector<U> > operator* (const CoolMatrix<U>&, const CoolM_Vector<U>&);

  CoolEnvelope< CoolM_Vector<Type> > abs() const;                       // r[i] = abs(v[i])
  CoolEnvelope< CoolM_Vector<Type> > sign() const;                      // r[i] = sign(v[i])
  CoolEnvelope< CoolM_Vector<Type> > extract (unsigned int len, unsigned int start=0) const; // subvector
  CoolM_Vector<Type>& update (const CoolM_Vector<Type>&, unsigned int start=0);
  
  template< class U >
  friend CoolEnvelope< CoolM_Vector<U> > element_product (const CoolM_Vector<U>&, // v[i] = a[i]*b[i]
                                        const CoolM_Vector<U>&);
  template< class U >
  friend CoolEnvelope< CoolM_Vector<U> > element_quotient (const CoolM_Vector<U>&, // v[i] = a[i]/b[i]
                                               const CoolM_Vector<U>&);

  inline Type squared_magnitude() const;        // dot(v,v)
  inline Type magnitude() const;                // sqrt(dot(v,v))
  inline CoolM_Vector<Type>& normalize();       // v /= sqrt(dot(v,v))
  
  inline Type& x() const;                       // get coordinates along
  inline Type& y() const;                       // 4 axes.
  inline Type& z() const;                       
  inline Type& t() const;                       

  template< class U >
  friend U dot_product(const CoolM_Vector<U>&, // dot-product of n-dim vectors
                       const CoolM_Vector<U>&); 
  template< class U >
  friend U cross_2d(const CoolM_Vector<U>&,      // cross-product of 2d-vectors
                    const CoolM_Vector<U>&);
  template< class U >
  friend CoolEnvelope< CoolM_Vector<U> > cross_3d (const CoolM_Vector<U>&, // cross-product 
                                       const CoolM_Vector<U>&); // of 3d-vectors 

protected:
  Type* data;                                   // Pointer to the CoolM_Vector 
  static Boolean (*compare_s) (const Type&, const Type&);       // Pointer operator== function
  template< class U >
  friend Boolean CoolM_Vector_is_data_equal (const U&, const U&);
};

//## BC++ 3.1 bug
void hack(CoolM_Vector<int>);
void hack(CoolM_Vector<float>);
void hack(CoolM_Vector<double>);
//## add your type above
#include <cool/Envelope.h>    //## BC++ 3.1 bug prevents from moving to top

// Use envelope to avoid deep copy on return by value, and mutate in place
template<class Type>
inline CoolEnvelope< CoolM_Vector<Type> > operator+ (const CoolM_Vector<Type>&arg1,const CoolM_Vector<Type>&arg2)
   { return (CoolEnvelope< CoolM_Vector<Type> > &) CoolEnvOp(add)(arg1, arg2); }
template<class Type>
inline CoolEnvelope< CoolM_Vector<Type> > operator- (const CoolM_Vector<Type>&arg1,const CoolM_Vector<Type>&arg2)
   { return (CoolEnvelope< CoolM_Vector<Type> > &) CoolEnvOp(minus)(arg1, arg2); }


// get -- Get the element at specified index and return value
// Input: *this, index
// Output: Element value

template<class Type> 
inline Type& CoolM_Vector<Type>::get (unsigned int index) {
#if ERROR_CHECKING
  if (index >= this->num_elmts)                 // If invalid index specified
    this->index_error ("get", #Type, index);    // Raise exception
#endif
  return this->data[index];
}

// put -- Put the element value at specified index
// Input: *this, index, value
// Output: Element value

template<class Type> 
inline void CoolM_Vector<Type>::put (unsigned int index, const Type& value) {
#if ERROR_CHECKING
  if (index >= this->num_elmts)                 // If invalid index specified
    this->index_error ("put", #Type, index);    // Raise exception
#endif
  this->data[index] = value;                    // Assign data value
}

// operator() -- Overload () to get the element at specified index and return value
// Input: *this, index
// Output: Element reference

template<class Type> 
inline Type& CoolM_Vector<Type>::operator() (unsigned int index) {
  return this->data[index];                     // fast access without checks.
}

// operator=  -- Assignment from an envelope back to real vector
// Input:     envelope reference
// Output:    vector reference with contents in envelope being swapped over

template<class Type>
inline CoolM_Vector<Type>& CoolM_Vector<Type>::operator= (CoolEnvelope< CoolM_Vector<Type> >& env){
  env.shallow_swap((CoolEnvelope< CoolM_Vector<Type> >*)this, &env); // same physical layout
  return *this;
}

// operator-= -- Destructive vector subtraction of a scalar.
// Input:        *this, scalar value
// Output:       New vector reference

template<class Type> 
inline CoolM_Vector<Type>& CoolM_Vector<Type>::operator-= (const Type& value) {
  return *this += (- value);
}


// operator*= -- Destructive multiply vector with matrix, and assignment. v = v*m
//               num_elmts of vector must match num_rows of matrix 
// Input:        *this, matrix reference
// Output:       Updated *this vector reference

template<class Type>
inline CoolM_Vector<Type>& CoolM_Vector<Type>::operator*= (const CoolMatrix<Type>& m) {
  return this->post_multiply(m);
}


// operator<< -- Overload the output operator to print a CoolM_Vector
// Input:        ostream reference, CoolM_Vector pointer
// Output:       ostream reference

template<class Type> 
inline ostream& operator<< (ostream& os, const CoolM_Vector<Type>* m) {
   return os << *m;
}

// operator!= -- Perform not equal comparison test
// Input:        *this, vector reference
// Output:       TRUE/FALSE

template<class Type> 
inline Boolean CoolM_Vector<Type>::operator!= (const CoolM_Vector<Type>& v) const {
  return (!operator== (v));
}


template<class Type> 
inline CoolEnvelope< CoolM_Vector<Type> > operator+ (const Type& value,
                                      const CoolM_Vector<Type>& v) {
  return (CoolEnvelope< CoolM_Vector<Type> > &) (v + value);
}

// operator- -- Non-destructive vector substraction of a scalar.
// Input:       *this, scalar value
// Output:      New vector 

template<class Type> 
inline CoolEnvelope< CoolM_Vector<Type> > CoolM_Vector<Type>::operator-(const Type& value) const {
  return (CoolEnvelope< CoolM_Vector<Type> > &) ((*this) + (- value));
}

template<class Type> 
inline CoolEnvelope< CoolM_Vector<Type> > operator- (const Type& value,
                                      const CoolM_Vector<Type>& v) {
  return (CoolEnvelope< CoolM_Vector<Type> > &) ((- v) + value);
}


template<class Type> 
inline CoolEnvelope< CoolM_Vector<Type> > operator* (const Type& value,
                                      const CoolM_Vector<Type>& v) {
  return (CoolEnvelope< CoolM_Vector<Type> > &) (v * value);
}


// squared_magnitude -- Return dot-product(*this,*this)
// Input:    *this
// Ouput:    squared of magnitude or dot-product(v,v).

template<class Type>
inline Type CoolM_Vector<Type>::squared_magnitude() const {
  return dot_product(*this,*this);
}

// magnitude -- Return square-root of dot-product(*this,*this)
// Input:    *this
// Ouput:    magnitude of vector.

template<class Type>
inline Type CoolM_Vector<Type>::magnitude() const {
  return (Type) sqrt(double(dot_product(*this,*this)));
}

// normalize -- Mutate vector to have magnitude = 1.
// Input:    *this
// Ouput:    Mutated vector

template<class Type>
inline CoolM_Vector<Type>& CoolM_Vector<Type>::normalize() {
  Type mag = this->magnitude();
  return (*this /= mag);
}


// x,y,z,t -- return the coordinates along the axes
// Input:     *this
// Output:    coordinate by reference

template<class Type>                            
inline Type& CoolM_Vector<Type>::x() const{     
  return data[0];
}               

template<class Type>
inline Type& CoolM_Vector<Type>::y() const {
  return data[1];
}

template<class Type>
inline Type& CoolM_Vector<Type>::z() const {
  return data[2];
}               

template<class Type>
inline Type& CoolM_Vector<Type>::t() const {
  return data[3];
}

//## hack to workaround BC++ 3.1 Envelope bug
#undef CoolEnvelope

#endif                                          // End of M_VECTORH




