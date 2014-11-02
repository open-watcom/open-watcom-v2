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

#include <cool/M_Vector.h>                      // header of vector and its envelope

//## hack to workaround BC++ 3.1 Envelope bug
#define CoolEnvelope CoolEnvelope_M_Vector

// compare_s -- Function used by == test
template <class Type>
Boolean (*CoolM_Vector<Type>::compare_s)(const Type&, const Type&) = &CoolM_Vector_is_data_equal;

// CoolM_Vector -- constructor specifiying size of CoolM_Vector
// Input:    Length
// Output:   None

template<class Type> 
CoolM_Vector<Type>::CoolM_Vector(unsigned int len)
: CoolBase_M_Vector(len)
{
  this->data = new Type[len];                   // Allocate the elements
}


// CoolM_Vector -- constructor specifiying size of vector and initial value
// Input:    Length and initial value
// Output:   None

template<class Type> 
CoolM_Vector<Type>::CoolM_Vector(unsigned int len, const Type& value)
: CoolBase_M_Vector(len)
{
  this->data = new Type[len];                   // Allocate the elements
  for (int i = 0; i < len; i ++) {              // For each elmt in the CoolM_Vector
    this->data[i] = value;                      // Assign initial value
  }
}

// CoolM_Vector -- constructor specifiying size of vector and initial values
// Input:    Length and initial values
// Output:   None
// Note: Arguments in ... can only be pointers, primitive types like int,
//       and NOT OBJECTS, passed by reference or value, like vectors, matrices;
//       because constructors must be known and called at compile time!!!
//       Cannot have char in ..., because char is 1 instead of 4 bytes, and 
//       va_arg expects sizeof(Type) a multiple of 4 bytes.

template<class Type> 
CoolM_Vector<Type>::CoolM_Vector(unsigned int len, int n, Type v00, ...)
: CoolBase_M_Vector(len)
{
#if ERROR_CHECKING
  if (((sizeof(Type) % 4) != 0) ||              // Cause alignment problems
      (sizeof(Type) > 8))                       // User defined classes?
    this->va_arg_error("Type", sizeof(Type));   // So, cannot use this constructor
#endif
  this->data = new Type[len];                   // Allocate the elements
  if (n > 0) {                                  // If user specified values
    va_list argp;                               // Declare argument list
    va_start (argp, v00);                       // Initialize macro
    for (int i = 0; i < len && n; i++, n--)     // For remaining values given
      if (i == 0)
        this->data[0] = v00;                    // Hack for v00 ...
      else
        this->data[i] = va_arg(argp, Type);     // Extract and assign
    va_end(argp);
  }
}


// CoolM_Vector -- constructor for reference to another CoolM_Vector object
// Input:    CoolM_Vector reference
// Output:   None

template<class Type> 
CoolM_Vector<Type>::CoolM_Vector(const CoolM_Vector<Type>& v)
: CoolBase_M_Vector(v)
{
  this->data = new Type[this->num_elmts];       // Allocate the elements
  for (int i = 0; i < this->num_elmts; i ++) {  // For each element in the CoolM_Vector
    this->data[i] = v.data[i];                  // Copy value
  }
}


// ~CoolM_Vector -- Destructor for CoolM_Vector class that frees up storage
// Input:     *this
// Output:    None

template<class Type> 
CoolM_Vector<Type>::~CoolM_Vector() {
  delete[] this->data;                            // Free up the data space
}

// fill -- Set all elements of a vector to a specified fill value
// Input:  *this, reference to fill value
// Output: None

template<class Type> 
void CoolM_Vector<Type>::fill (const Type& value) {
  for (int i = 0; i < this->num_elmts; i++)     // For each index in the CoolM_Vector
      this->data[i] = value;                    // Assign fill value
}

// operator= -- Overload the assignment operator to assign a single 
//              value to the elements of a CoolM_Vector. 
// Input:       *this, reference to a value
// Output:      Reference to updated CoolM_Vector object

template<class Type> 
CoolM_Vector<Type>& CoolM_Vector<Type>::operator= (const Type& value) {
  for (int i = 0; i < this->num_elmts; i++)     // For each index in CoolM_Vector
      this->data[i] = value;                    // Assign value
  return *this;                                 // Return CoolM_Vector reference
}


// operator= -- Overload the assignment operator to copy the elements
//              in one CoolM_Vector to another. The existing storage for the 
//              destination vector is freed up and new storage of the same 
//              size as the source is allocated.
// Input:       *this, reference to CoolM_Vector
// Output:      Reference to copied CoolM_Vector object

template<class Type> 
CoolM_Vector<Type>& CoolM_Vector<Type>::operator= (const CoolM_Vector<Type>& v) {
  if (this != &v) {                             // make sure *this != m
    if (this->num_elmts != v.num_elmts) {
      delete this->data;                        // Free up the data space
      this->num_elmts = v.num_elmts;            // Copy index specification
      this->data = new Type[this->num_elmts];   // Allocate the elements
    }
    for (int i = 0; i < this->num_elmts; i++)   // For each index 
      this->data[i] = v.data[i];                // Copy value
  }
  return *this;                         // Return CoolM_Vector reference
}

// operator== -- Compare the elements of two Matrices of Type Type using
//               the Compare pointer to funtion (default is ==). If one 
//               CoolM_Vector has more elements than the other, the
//               result is FALSE
// Input:        Reference to CoolM_Vector of Type Type
// Output:       TRUE/FALSE

template<class Type> 
Boolean CoolM_Vector<Type>::operator== (const CoolM_Vector<Type>& v) const {
  if (this->num_elmts != v.num_elmts)           // Size?
    return FALSE;                               // Then not equal
  for (int i = 0; i < this->num_elmts; i++)     // For each index
    if ((*this->compare_s)(this->data[i],v.data[i]) == FALSE) // Same?
      return FALSE;                                           // Then no match
  return TRUE;                                        // Else same, so return TRUE
}

// is_data_equal -- Default data comparison function if user has not provided
//                  another one. Note that this is not inline because we need
//                  to take the address of it for the compare static variable
// Input:           Two Type references
// Output:          TRUE/FALSE

template<class Type>
Boolean CoolM_Vector_is_data_equal (const Type& t1, const Type& t2) {
  return (t1 == t2);
}

// set_compare -- Specify the comparison function to be used
//                in logical tests of vector elements
// Input:         Pointer to a compare function
// Output:        None

template<class Type> 
void CoolM_Vector<Type>::set_compare (register Boolean (*c) (const Type&, const Type&)) {
  if (c == NULL)                                // If no argument supplied
    this->compare_s = &CoolM_Vector_is_data_equal; // Default is_equal
  else
    this->compare_s = c;                        // Else set to user function
}


// operator<< -- Overload the output operator to print a vector
// Input:        ostream reference, CoolM_Vector reference
// Output:       ostream reference

template<class Type>
ostream& operator<< (ostream& s, const CoolM_Vector<Type>& v) {
   for (int i = 0; i < v.num_elmts; i++) {      // For each index in vector
   s << v.data[i] << " ";                       // Output data element
   }
   return (s);                                  // Return ostream reference
}

// operator+= -- Destructive vector addition of a scalar.
// Input:        *this, scalar value
// Output:       New vector reference

template<class Type> 
CoolM_Vector<Type>& CoolM_Vector<Type>::operator+= (const Type& value) {
  for (int i = 0; i < this->num_elmts; i++)     // For each index
    this->data[i] += value;             // Add scalar
  return *this;
}

// operator*= -- Destructive vector multiplication by a scalar.
// Input:        *this, scalar value
// Output:       New vector reference


template<class Type> 
CoolM_Vector<Type>& CoolM_Vector<Type>::operator*= (const Type& value) {
  for (int i = 0; i < this->num_elmts; i++)     // For each index
    this->data[i] *= value;                     // Multiply by scalar
  return *this;
}

// operator/= -- Destructive vector division by a scalar.
// Input:        *this, scalar value
// Output:       New vector reference

template<class Type> 
CoolM_Vector<Type>& CoolM_Vector<Type>::operator/= (const Type& value) {
  for (int i = 0; i < this->num_elmts; i++)     // For each index
    this->data[i] /= value;                     // division by scalar
  return *this;
}


// operator+= -- Destructive vector addition with assignment. Note that the
//               dimensions of each vector must be identical
// Input:        *this, vector reference
// Output:       Updated *this vector reference

template<class Type> 
CoolM_Vector<Type>& CoolM_Vector<Type>::operator+= (const CoolM_Vector<Type>& v) {
  if (this->num_elmts != v.num_elmts) // Size?
    this->dimension_error ("operator+=", "Type", 
                           this->num_elmts, v.num_elmts);
  for (int i = 0; i < this->num_elmts; i++)     // For each index
    this->data[i] += v.data[i];                 // Add elements
  return *this;
}


// operator-= -- Destructive vector subtraction with assignment. Note that the
//               dimensions of each vector must be identical
// Input:        *this, vector reference
// Output:       Updated *this vector reference

template<class Type> 
CoolM_Vector<Type>& CoolM_Vector<Type>::operator-= (const CoolM_Vector<Type>& v) {
  if (this->num_elmts != v.num_elmts)           // Size?
    this->dimension_error ("operator-=", "Type", 
                           this->num_elmts, v.num_elmts);
  for (int i = 0; i < this->num_elmts; i++)
    this->data[i] -= v.data[i];
  return *this;
}



// pre_multiply -- Destructive pre_multiply vector with matrix. v = m * v
//               num_cols of matrix must match num_elmts of vector
// Input:        *this, matrix reference
// Output:       Updated *this vector reference

template<class Type>
CoolM_Vector<Type>& CoolM_Vector<Type>::pre_multiply (const CoolMatrix<Type>& m) {
  if (m.columns() != this->num_elmts)           // dimensions do not match?
    this->dimension_error ("operator*=", "Type", 
                           this->num_elmts, m.columns());
  Type* temp= new Type[m.rows()];               // Temporary
  CoolMatrix<Type>& mm = (CoolMatrix<Type>&) m; // Drop const for get()
  for (int i = 0; i < m.rows(); i++) {  // For each index
    temp[i] = (Type) 0.0;                       // Initialize element value
    for (int k = 0; k < this->num_elmts; k++)   // Loop over column values
      temp[i] += (mm.get(i,k) * this->data[k]); // Multiply
  }
  delete this->data;                            // Free up the data space
  num_elmts = m.rows();                 // Set new num_elmts
  this->data = temp;                            // Pointer to new storage
  return *this;                                 // Return vector reference
}

// post_multiply -- Destructive post_multiply vector with matrix. v = v * m
//               num_elmts of vector must match num_rows of matrix 
// Input:        *this, matrix reference
// Output:       Updated *this vector reference

template<class Type> 
CoolM_Vector<Type>& CoolM_Vector<Type>::post_multiply (const CoolMatrix<Type>& m) {
  if (this->num_elmts != m.rows())              // dimensions do not match?
    this->dimension_error ("operator*=", "Type", 
                           this->num_elmts, m.rows());
  Type* temp= new Type[m.columns()];            // Temporary
  CoolMatrix<Type>& mm = (CoolMatrix<Type>&) m; // Drop const for get()
  for (int i = 0; i < m.columns(); i++) {       // For each index
    temp[i] = (Type) 0.0;                       // Initialize element value
    for (int k = 0; k < this->num_elmts; k++)   // Loop over column values
      temp[i] += (this->data[k] * mm.get(k,i)); // Multiply
  }
  delete this->data;                            // Free up the data space
  num_elmts = m.columns();                      // Set new num_elmts
  this->data = temp;                            // Pointer to new storage
  return *this;                                 // Return vector reference
}


// operator- -- Non-destructive vector negation. a = -b;
// Input:       *this
// Output:      New vector 

template<class Type> 
CoolEnvelope< CoolM_Vector<Type> > CoolM_Vector<Type>::operator- () const {
  CoolM_Vector<Type> temp(this->num_elmts);
  for (int i = 0; i < this->num_elmts; i++)
    temp.data[i] = - this->data[i];             // negate element
  CoolEnvelope< CoolM_Vector<Type> >& result = (CoolEnvelope< CoolM_Vector<Type> >&) temp; // same physical object
  return result;                                 // copy of envelope
}

// operator+ -- Non-destructive vector addition of a scalar.
// Input:       *this, scalar value
// Output:      New vector 

template<class Type> 
CoolEnvelope< CoolM_Vector<Type> > CoolM_Vector<Type>::operator+ (const Type& value) const {
  CoolM_Vector<Type> temp(this->num_elmts);
  for (int i = 0; i < this->num_elmts; i++)     // For each index
    temp.data[i] = (this->data[i] + value);     // Add scalar
  CoolEnvelope< CoolM_Vector<Type> >& result = (CoolEnvelope< CoolM_Vector<Type> >&) temp; // same physical object
  return result;                                 // copy of envelope
}


// operator* -- Non-destructive vector multiply by a scalar.
// Input:       *this, scalar value
// Output:      New vector 

template<class Type> 
CoolEnvelope< CoolM_Vector<Type> > CoolM_Vector<Type>::operator* (const Type& value) const {
  CoolM_Vector<Type> temp(this->num_elmts);
  for (int i = 0; i < this->num_elmts; i++)     // For each index
    temp.data[i] = (this->data[i] * value);     // Multiply
  CoolEnvelope< CoolM_Vector<Type> >& result = (CoolEnvelope< CoolM_Vector<Type> >&) temp; // same physical object
  return result;                                 // copy of envelope
}


// operator/ -- Non-destructive vector division by a scalar.
// Input:       *this, scalar value
// Output:      New vector 

template<class Type> 
CoolEnvelope< CoolM_Vector<Type> > CoolM_Vector<Type>::operator/ (const Type& value) const {
  CoolM_Vector<Type> temp(this->num_elmts);
  for (int i = 0; i < this->num_elmts; i++)     // For each index
    temp.data[i] = (this->data[i] / value);     // Divide
  CoolEnvelope< CoolM_Vector<Type> >& result = (CoolEnvelope< CoolM_Vector<Type> >&) temp; // same physical object
  return result;                                 // copy of envelope
}

// operator* -- Non-destructive multiply matrix with vector: vb = m * va
//              num_cols of first matrix must match num_elmts of second vector.
// Input:       matrix, vector reference
// Output:      New vector 

template<class Type>
CoolEnvelope< CoolM_Vector<Type> > operator* (const CoolMatrix<Type>& m, const CoolM_Vector<Type>& v) {
  if (m.columns() != v.num_elmts)               // dimensions do not match?
    v.dimension_error ("operator*", "Type", 
                       m.columns(), v.num_elmts);
  CoolM_Vector<Type> temp(m.rows());            // Temporary
  CoolMatrix<Type>& mm = (CoolMatrix<Type>&) m; // Drop const for get()
  for (int i = 0; i < m.rows(); i++) {  // For each index
    temp.data[i] = (Type) 0.0;                  // Initialize element value
    for (int k = 0; k < v.num_elmts; k++)       // Loop over column values
      temp.data[i] += (mm.get(i,k) * v.data[k]); // Multiply
  }
  CoolEnvelope< CoolM_Vector<Type> >& result = (CoolEnvelope< CoolM_Vector<Type> >&) temp; // same physical object
  return result;                                 // copy of envelope
}


// operator* -- Non-destructive multiply vector with matrix: vb = va * m
//              num_elmts of first vector must match num_rows of second matrix.
// Input:       vector, matrix reference
// Output:      New vector 

template<class Type>
CoolEnvelope< CoolM_Vector<Type> > operator* (const CoolM_Vector<Type>& v, const CoolMatrix<Type>&m) {
  if (v.num_elmts != m.rows())          // dimensions do not match?
    v.dimension_error ("operator*", "Type", 
                       v.num_elmts, m.rows());
  CoolM_Vector<Type> temp(m.columns());         // Temporary
  CoolMatrix<Type>& mm = (CoolMatrix<Type>&) m; // Drop const for get()
  for (int i = 0; i < m.columns(); i++) {       // For each index
    temp.data[i] = (Type) 0.0;                  // Initialize element value
    for (int k = 0; k < v.num_elmts; k++)       // Loop over column values
      temp.data[i] += (v.data[k] * mm.get(k,i)); // Multiply
  }
  CoolEnvelope< CoolM_Vector<Type> >& result = (CoolEnvelope< CoolM_Vector<Type> >&) temp; // same physical object
  return result;                                 // copy of envelope
}


// update -- replace a subvector of this, by the actual argument.
// Input:       *this, starting corner specified by top and left.
// Ouput:       mutated reference.

template<class Type> 
CoolM_Vector<Type>& CoolM_Vector<Type>::update (const CoolM_Vector<Type>& v, 
                                      unsigned int start) {
  unsigned int end = start + v.num_elmts;
  if (this->num_elmts < end)
    this->dimension_error ("update", "Type", 
                           end-start, v.num_elmts);
  for (int i = start; i < end; i++)
    this->data[i] = v.data[i-start];
  return *this;
}


// extract -- Return a subvector specified by the start-left corner and size.
// Input:       *this, starting corner specified by start and left, and size.
// Ouput:       new vector

template<class Type> 
CoolEnvelope< CoolM_Vector<Type> > CoolM_Vector<Type>::extract (unsigned int len, unsigned int start) const{
  unsigned int end = start + len;
  if (this->num_elmts < end)
    this->dimension_error ("extract", "Type", 
                           end-start, len);
  CoolM_Vector<Type> temp(len);
  for (int i = 0; i < len; i++)
    temp.data[i] = data[start+i];
  CoolEnvelope< CoolM_Vector<Type> >& result = (CoolEnvelope< CoolM_Vector<Type> >&) temp; // same physical object
  return result;                                 // copy of envelope
}

// abs -- Return vector of absolute values.  v[i] = abs(v[i])
// Input:       *this
// Ouput:       new vector

template<class Type> 
CoolEnvelope< CoolM_Vector<Type> > CoolM_Vector<Type>::abs() const {
  CoolM_Vector<Type> temp(this->num_elmts);
  for (int i = 0; i < this->num_elmts; i++)
    if (this->data[i] < 0)
      temp.data[i] = - this->data[i];
    else
      temp.data[i] = this->data[i];
  CoolEnvelope< CoolM_Vector<Type> >& result = (CoolEnvelope< CoolM_Vector<Type> >&) temp; // same physical object
  return result;                                 // copy of envelope
}

// sign -- Mutate *this to store signs, either -1,1 or 0, depending on
//         whether the corresponding values are negative, positive, or 0.
// Input:       *this
// Ouput:       *this mutated.

template<class Type> 
CoolEnvelope< CoolM_Vector<Type> > CoolM_Vector<Type>::sign () const {
  CoolM_Vector<Type> temp(this->num_elmts);
  for (int i = 0; i < this->num_elmts; i++)
    if (this->data[i] == 0)                     // test fuzz equality to 0
      temp.data[i] = 0;                         // first.
    else
      if (this->data[i] < 0)
        temp.data[i] = -1;
      else
        temp.data[i] = 1;
  CoolEnvelope< CoolM_Vector<Type> >& result = (CoolEnvelope< CoolM_Vector<Type> >&) temp; // same physical object
  return result;                                 // copy of envelope
}

// element_product -- return the vector whose elements are the products 
// Input:     2 vectors v1, v2 by reference
// Output:    New vector, whose elements are v1[i]*v2[i].

template<class Type>
CoolEnvelope< CoolM_Vector<Type> > element_product (const CoolM_Vector<Type>& v1, const CoolM_Vector<Type>& v2) {
  if (v1.num_elmts != v2.num_elmts)             // Size?
    v1.dimension_error ("element_product", "Type", 
                        v1.num_elmts, v2.num_elmts);
  CoolM_Vector<Type> temp(v1.num_elmts);
  for (int i = 0; i < v1.num_elmts; i++)
    temp.data[i] = v1.data[i] * v2.data[i];
  CoolEnvelope< CoolM_Vector<Type> >& result = (CoolEnvelope< CoolM_Vector<Type> >&) temp; // same physical object
  return result;                                 // copy of envelope
}

// element_quotient -- return the vector whose elements are the quotients 
// Input:     2 vectors v1, v2 by reference
// Output:    New vector, whose elements are v1[i]/v2[i].

template<class Type>
CoolEnvelope< CoolM_Vector<Type> > element_quotient (const CoolM_Vector<Type>& v1, const CoolM_Vector<Type>& v2) {
  if (v1.num_elmts != v2.num_elmts)             // Size?
    v1.dimension_error ("element_quotient", "Type", 
                        v1.num_elmts, v2.num_elmts);
  CoolM_Vector<Type> temp(v1.num_elmts);
  for (int i = 0; i < v1.num_elmts; i++)
    temp.data[i] = v1.data[i] / v2.data[i];
  CoolEnvelope< CoolM_Vector<Type> >& result = (CoolEnvelope< CoolM_Vector<Type> >&) temp; // same physical object
  return result;                                 // copy of envelope
}


// dot_product -- Return the dot product of the vectors
// Input:       2 vectors by reference
// Ouput:       dot product value

template<class Type>
Type dot_product (const CoolM_Vector<Type>& v1, const CoolM_Vector<Type>& v2) {
  if (v1.num_elmts != v2.num_elmts)             // Size?
    v1.dimension_error ("dot_product", "Type", 
                        v1.num_elmts, v2.num_elmts);
  Type dot = 0;
  for (int i = 0; i < v1.num_elmts; i++)
    dot += v1.data[i] * v2.data[i];     // of vectors
  return dot;
}

// cross_2d -- Return the 2X1 cross-product of 2 2d-vectors
// Input:       2 vectors by reference
// Ouput:       cross product value

template<class Type>
Type cross_2d (const CoolM_Vector<Type>& v1, const CoolM_Vector<Type>& v2) {
  if (v1.num_elmts != 2 || v2.num_elmts != 2)
    v1.dimension_error ("cross_2d", "Type", 
                        v1.num_elmts, v2.num_elmts);

  return (v1.x() * v2.y()                       
          -                                     
          v1.y() * v2.x());
}

// cross_3d -- Return the 3X1 cross-product of 2 3d-vectors
// Input:       2 vectors by reference
// Ouput:       3d cross product vector

template<class Type>
CoolEnvelope< CoolM_Vector<Type> > cross_3d (const CoolM_Vector<Type>& v1, const CoolM_Vector<Type>& v2) {
  if (v1.num_elmts != 3 || v2.num_elmts != 3)
    v1.dimension_error ("cross_3d", "Type", 
                        v1.num_elmts, v2.num_elmts);
  CoolM_Vector<Type> temp(v1.num_elmts);

  temp.x() = v1.y() * v2.z() - v1.z() * v2.y(); // work for both col/row
  temp.y() = v1.z() * v2.x() - v1.x() * v2.z(); // representation
  temp.z() = v1.x() * v2.y() - v1.y() * v2.x();
  CoolEnvelope< CoolM_Vector<Type> >& result = (CoolEnvelope< CoolM_Vector<Type> >&) temp; // same physical object
  return result;                                 // copy of envelope
}

//## hack to workaround BC++ 3.1 Envelope bug
#undef CoolEnvelope

