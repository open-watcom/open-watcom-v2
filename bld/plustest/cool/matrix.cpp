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

#include <cool/Matrix.h>                        // header of matrix and its envelope

//## hack to workaround BC++ 3.1 Envelope bug
#define CoolEnvelope CoolEnvelope_Matrix

// compare_s -- Pointer operator== function
template<class Type> 
Boolean (*CoolMatrix<Type>::compare_s)(const Type&, const Type&);


// Matrix -- constructor specifiying size of Matrix
// Input:    Row, Column parameters
// Output:   None

template<class Type> 
CoolMatrix<Type>::CoolMatrix(unsigned int rows, unsigned int cols)
: CoolBase_Matrix(rows, cols) {
  this->data = new Type*[rows];                 // Allocate the row memory
  Type* columns = new Type[cols*rows];          // Allocate the array of elmts
  for (int i = 0; i < rows; i ++)               // For each row in the Matrix
    this->data[i] = &columns[i*cols];           // Fill in address of row
  if (this->compare_s == NULL)                  // If not set yet
    this->compare_s = &CoolMatrix_is_data_equal; // Default is_equal
}


// Matrix -- constructor specifiying size of matrix and initial value
// Input:    Row, Column parameters and initial value
// Output:   None

template<class Type> 
CoolMatrix<Type>::CoolMatrix(unsigned int rows, unsigned int cols,
                                    const Type& value)
: CoolBase_Matrix(rows, cols) {
  this->data = new Type*[rows];                 // Allocate the row memory
  Type* columns = new Type[cols*rows];          // Allocate the array of elmts
  for (int i = 0; i < rows; i ++) {             // For each row in the Matrix
    this->data[i] = &columns[i*cols];           // Fill in address of row
    for (int j = 0; j < cols; j++)              // For each element in column
      this->data[i][j] = value;                 // Assign initial value
  }
  if (this->compare_s == NULL)                  // If not set yet
    this->compare_s = &CoolMatrix_is_data_equal; // Default is_equal
}

// Matrix -- constructor specifiying size of matrix and initial values
// Input:    Rows, Column parameters and initial values
// Output:   None
// Note: Arguments in ... can only be pointers, primitive types like int, double,
//       and NOT OBJECTS, passed by reference or value, like vectors, matrices;
//       because constructors must be known and called at compile time!!!
//       Cannot have char in ..., because char is 1 instead of 4 bytes, and 
//       va_arg expects sizeof(Type) a multiple of 4 bytes.

template<class Type> 
CoolMatrix<Type>::CoolMatrix(unsigned int rows, unsigned int cols, int n,
                                    Type v00 ...)
: CoolBase_Matrix(rows, cols) {
#if ERROR_CHECKING
  if (((sizeof(Type) % 4) != 0) ||              // Cause alignment problems
      (sizeof(Type) > 8))                       // User defined classes?
    this->va_arg_error(#Type, sizeof(Type));    // So, cannot use this constructor
#endif
  this->data = new Type*[rows];                 // Allocate the row memory
  Type* columns = new Type[cols*rows];          // Allocate the array of elmts
  int i;
  for (i = 0; i < rows; i ++)                   // For each row in the Matrix
    this->data[i] = &columns[i*cols];           // Fill in address of row
  if (n > 0) {                                  // If user specified values
    va_list argp;                               // Declare argument list
    va_start (argp, v00);                       // Initialize macro
    for (i = 0; i < rows && n; i++)             // For remaining values given
      for (int j = 0; j < cols && n; j++, n--)  // Moving sequentially in Matrix
        if ((i == 0) && (j == 0))
          this->data[0][0] = v00;               // Hack for v00 ...
        else
          this->data[i][j] = va_arg(argp, Type); // Extract and assign
    va_end(argp);
  }
  if (this->compare_s == NULL)                  // If not set yet
    this->compare_s = &CoolMatrix_is_data_equal; // Default is_equal
}

// Matrix -- Construct a matrix from a block array of data, stored row-wise.
// Input     number of rows and columns, and array of r*c data.
// Ouput     None

template<class Type>
CoolMatrix<Type>::CoolMatrix(unsigned int rows, unsigned int cols,
                                    const Type* data_block) 
: CoolBase_Matrix(rows,cols) {
  this->data = new Type*[num_rows];             // Allocate the row memory
  int n = num_rows * num_cols;                  // Number of elements 
  Type* columns = new Type[n];                  // Allocate the array of elmts
  for (int d = 0; d < n; d++)                   // Copy all the data elements
    columns[d] = data_block[d];
  for (int i = 0; i < num_rows; i ++)           // For each row in the Matrix
    this->data[i] = &columns[i*num_cols];       // Fill in address of row
}


// Matrix -- Copy constructor
// Input:    Matrix reference
// Output:   None

template<class Type> 
CoolMatrix<Type>::CoolMatrix(const CoolMatrix<Type>& m)
: CoolBase_Matrix(m) {
  this->data = new Type*[this->num_rows];       // Allocate the row memory
  Type* columns = new Type[num_cols*num_rows];  // Allocate the array of elmts
  for (int i = 0; i < num_rows; i ++) {         // For each row in the Matrix
    this->data[i] = &columns[i*num_cols];       // Fill in address of row
    for (int j = 0; j < this->num_cols; j++)    // For each element in column
      this->data[i][j] = m.data[i][j];          // Copy value
  }
}


// ~Matrix -- Destructor for Matrix class that frees up storage
// Input:     *this
// Output:    None

template<class Type> 
CoolMatrix<Type>::~CoolMatrix() {
  delete [] this->data[0];                      // Free up the array of elmts
  delete [] this->data;                         // Free up the row memory
}

// fill -- Set all elements of a matrix to a specified fill value
// Input:  this*, reference to fill value
// Output: None

template<class Type> 
void CoolMatrix<Type>::fill (const Type& value) {
  for (int i = 0; i < this->num_rows; i++)      // For each row in the Matrix
    for (int j = 0; j < this->num_cols; j++)    // For each element in column
      this->data[i][j] = value;                 // Assign fill value
}

// operator= -- Overload the assignment operator to assign a single 
//              value to the elements of a Matrix. 
// Input:       *this, reference to a value
// Output:      Reference to updated Matrix object

template<class Type> 
CoolMatrix<Type>& CoolMatrix<Type>::operator= (const Type& value) {
  for (int i = 0; i < this->num_rows; i++)      // For each row in Matrix
    for (int j = 0; j < this->num_cols; j++)    // For each column in Matrix
      this->data[i][j] = value;                 // Assign value
  return *this;                                 // Return Matrix reference
}


// operator= -- Overload the assignment operator to copy the elements
//              in one Matrix to another. The existing storage for the 
//              destination matrix is freed up and new storage of the same 
//              size as the source is allocated.
// Input:       *this, reference to Matrix
// Output:      Reference to copied Matrix object

template<class Type> 
CoolMatrix<Type>& CoolMatrix<Type>::operator= (const CoolMatrix<Type>& m) {
  if (this != &m) {                             // make sure *this != m
    if (this->num_rows != m.num_rows || this->num_cols != m.num_cols) {
      delete [] this->data[0];                  // Free up the array of elmts
      delete [] this->data;                     // Free up the row memory
      this->num_rows = m.num_rows;              // Copy rows
      this->num_cols = m.num_cols;              // Copy columns 
      this->data = new Type*[this->num_rows];   // Allocate the rows
      Type* columns = new Type[num_cols*num_rows]; // Allocate the columns
      for (int i = 0; i < this->num_rows; i++)     // For each row
        this->data[i] = &columns[i*num_cols];      // Fill in address of row
    }
    for (int i = 0; i < this->num_rows; i++)    // For each row in the Matrix
      for (int j = 0; j < this->num_cols; j++)  // For each element in column
        this->data[i][j] = m.data[i][j];        // Copy value
  }
  return *this;                                 // Return Matrix reference
}

// operator== -- Compare the elements of two Matrices of Type Type using
//               the Compare pointer to funtion (default is ==). If one 
//               Matrix has more rows and/or columns than the other, the
//               result is FALSE
// Input:        Reference to Matrix of Type Type
// Output:       TRUE/FALSE

template<class Type> 
Boolean CoolMatrix<Type>::operator== (const CoolMatrix<Type>& m) const {
  if (this->num_rows != m.num_rows || this->num_cols != m.num_cols) // Size?
    return FALSE;                                                   // Then not equal
  for (int i = 0; i < this->num_rows; i++)                          // For each row
    for (int j = 0; j < this->num_cols; j++)                        // For each columne
      if ((*this->compare_s)(this->data[i][j],m.data[i][j]) == FALSE) // Same?
        return FALSE;                                                 // Then no match
  return TRUE;                                                // Else same, so return TRUE
}


// is_data_equal -- Default data comparison function if user has not provided
//                  another one. Note that this is not inline because we need
    //                  to take the address of it for the compare static variable
// Input:           Two Type references
// Output:          TRUE/FALSE

template<class Type>
  Boolean CoolMatrix_is_data_equal (const Type& t1, const Type& t2) {
    return (t1 == t2);
  }


// set_compare -- Specify the comparison function to be used
//                in logical tests of vector elements
// Input:         Pointer to a compare function
// Output:        None

template<class Type> 
void CoolMatrix<Type>::set_compare ( register /*Compare##*/Boolean (*c)(const Type&, const Type&)) {
  if (c == NULL)                                // If no argument supplied
    this->compare_s = &CoolMatrix_is_data_equal; // Default is_equal
  else
    this->compare_s = c;                        // Else set to user function
}

// operator<< -- Overload the output operator to print a matrix
// Input:        ostream reference, Matrix reference
// Output:       ostream reference

template<class Type>
  ostream& operator<< (ostream& s, const CoolMatrix<Type>& m) {
    for (int i = 0; i < m.rows(); i++) {        // For each row in matrix
      for (int j = 0; j < m.columns(); j++)     // For each column in matrix
        s << m.data[i][j] << " ";               // Output data element
      s << "\n";                                        // Output newline
    }
    return (s);                                 // Return ostream reference
}



// operator+= -- Destructive matrix addition of a scalar.
// Input:        this*, scalar value
// Output:       New matrix reference

template<class Type> 
CoolMatrix<Type>& CoolMatrix<Type>::operator+= (const Type& value) {
  for (int i = 0; i < this->num_rows; i++)      // For each row
    for (int j = 0; j < this->num_cols; j++)    // For each element in column
      this->data[i][j] += value;                // Add scalar
  return *this;
}

// operator*= -- Destructive matrix multiplication by a scalar.
// Input:        this*, scalar value
// Output:       New matrix reference


template<class Type> 
CoolMatrix<Type>& CoolMatrix<Type>::operator*= (const Type& value) {
  for (int i = 0; i < this->num_rows; i++)      // For each row
    for (int j = 0; j < this->num_cols; j++)    // For each element in column
      this->data[i][j] *= value;                // Multiply by scalar
  return *this;
}

// operator/= -- Destructive matrix division by a scalar.
// Input:        this*, scalar value
// Output:       New matrix reference

template<class Type> 
CoolMatrix<Type>& CoolMatrix<Type>::operator/= (const Type& value) {
  for (int i = 0; i < this->num_rows; i++)      // For each row
    for (int j = 0; j < this->num_cols; j++)    // For each element in column
      this->data[i][j] /= value;                // division by scalar
  return *this;
}


// operator+= -- Destructive matrix addition with assignment. Note that the
//               dimensions of each matrix must be identical
// Input:        this*, matrix reference
// Output:       Updated this* matrix reference

template<class Type> 
CoolMatrix<Type>& CoolMatrix<Type>::operator+= (const CoolMatrix<Type>& m) {
  if (this->num_rows != m.num_rows || this->num_cols != m.num_cols) // Size?
    this->dimension_error ("operator+=", "Type", 
                           this->num_rows, this->num_cols, m.num_rows, m.num_cols);
  for (int i = 0; i < this->num_rows; i++)      // For each row
    for (int j = 0; j < this->num_cols; j++)    // For each element in column
      this->data[i][j] += m.data[i][j];         // Add elements
  return *this;
}


// operator-= -- Destructive matrix subtraction with assignment. Note that the
//               dimensions of each matrix must be identical
// Input:        this*, matrix reference
// Output:       Updated this* matrix reference

template<class Type> 
CoolMatrix<Type>& CoolMatrix<Type>::operator-= (const CoolMatrix<Type>& m) {
  if (this->num_rows != m.num_rows || this->num_cols != m.num_cols) // Size?
    this->dimension_error ("operator-=", "Type", 
                           this->num_rows, this->num_cols, m.num_rows, m.num_cols);
  int i, j;
  for (i = 0; i < this->num_rows; i++)
    for (j = 0; j < this->num_cols; j++)
      this->data[i][j] -= m.data[i][j];
  return *this;
}

// operator* -- Non Destructive matrix multiply 
//               num_cols of first matrix must match num_rows of second matrix.
// Input:        two matrix references
// Output:       New matrix containing the product.

template<class Type>
CoolEnvelope< CoolMatrix<Type> > operator* (const CoolMatrix<Type>& m1,
                             const CoolMatrix<Type>& m2) {
  if (m1.num_cols != m2.num_rows)               // dimensions do not match?
    m1.dimension_error ("operator*=", "Type", 
                        m1.num_rows, m1.num_cols, m2.num_rows, m2.num_cols);
  CoolMatrix<Type> temp(m1.num_rows, m2.num_cols); // Temporary to store product
  for (int i = 0; i < m1.num_rows; i++) {       // For each row
    for (int j = 0; j < m2.num_cols; j++) {     // For each element in column
      Type sum = 0;
      for (int k = 0; k < m1.num_cols; k++)     // Loop over column values
        sum += (m1.data[i][k] * m2.data[k][j]); // Multiply
      temp(i,j) = sum;
    }
  }
  CoolEnvelope< CoolMatrix<Type> >& result = (CoolEnvelope< CoolMatrix<Type> >&) temp; // same physical object
  return result;                                         // copy of envelope
}

// operator- -- Non-destructive matrix negation. a = -b;
// Input:       this*
// Output:      New matrix 

template<class Type> 
CoolEnvelope< CoolMatrix<Type> > CoolMatrix<Type>::operator- () const {
  CoolMatrix<Type> temp(this->num_rows, this->num_cols);
  int i, j;
  for (i = 0; i < this->num_rows; i++)
    for (j = 0; j < this->num_cols; j++)
      temp.data[i][j] = - this->data[i][j];
  CoolEnvelope< CoolMatrix<Type> >& result = (CoolEnvelope< CoolMatrix<Type> >&) temp; // same physical object
  return result;                                         // copy of envelope
}

// operator+ -- Non-destructive matrix addition of a scalar.
// Input:       this*, scalar value
// Output:      New matrix 

template<class Type> 
CoolEnvelope< CoolMatrix<Type> > CoolMatrix<Type>::operator+ (const Type& value) const {
  CoolMatrix<Type> temp(this->num_rows, this->num_cols);
  for (int i = 0; i < this->num_rows; i++)      // For each row
    for (int j = 0; j < this->num_cols; j++)    // For each element in column
      temp.data[i][j] = (this->data[i][j] + value); // Add scalar
  CoolEnvelope< CoolMatrix<Type> >& result = (CoolEnvelope< CoolMatrix<Type> >&) temp; // same physical object
  return result;                                         // copy of envelope
}


// operator* -- Non-destructive matrix multiply by a scalar.
// Input:       this*, scalar value
// Output:      New matrix 

template<class Type> 
CoolEnvelope< CoolMatrix<Type> > CoolMatrix<Type>::operator* (const Type& value) const {
  CoolMatrix<Type> temp(this->num_rows, this->num_cols);
  for (int i = 0; i < this->num_rows; i++)      // For each row
    for (int j = 0; j < this->num_cols; j++)    // For each element in column
      temp.data[i][j] = (this->data[i][j] * value); // Multiply
  CoolEnvelope< CoolMatrix<Type> >& result = (CoolEnvelope< CoolMatrix<Type> >&) temp; // same physical object
  return result;                                         // copy of envelope
}


// operator/ -- Non-destructive matrix division by a scalar.
// Input:       this*, scalar value
// Output:      New matrix 

template<class Type> 
CoolEnvelope< CoolMatrix<Type> > CoolMatrix<Type>::operator/ (const Type& value) const {
  CoolMatrix<Type> temp(this->num_rows, this->num_cols);
  for (int i = 0; i < this->num_rows; i++)      // For each row
    for (int j = 0; j < this->num_cols; j++)    // For each element in column
      temp.data[i][j] = (this->data[i][j] / value); // Divide
  CoolEnvelope< CoolMatrix<Type> >& result = (CoolEnvelope< CoolMatrix<Type> >&) temp; // same physical object
  return result;                                         // copy of envelope
}


////--------------------------- Additions ------------------------------------


// transpose -- Return the transpose of this matrix.
// Input:       this*
// Ouput:       New matrix

template<class Type> 
CoolEnvelope< CoolMatrix<Type> > CoolMatrix<Type>::transpose() const {
  CoolMatrix<Type> temp(this->num_cols, this->num_rows);
  int i, j;
  for (i = 0; i < this->num_cols; i++)
    for (j = 0; j < this->num_rows; j++)
      temp.data[i][j] = this->data[j][i];
  CoolEnvelope< CoolMatrix<Type> >& result = (CoolEnvelope< CoolMatrix<Type> >&) temp; // same physical object
  return result;                                         // copy of envelope
}


// abs -- Return the matrix of the absolute values.
// Input:       this*
// Ouput:       New matrix

template<class Type> 
CoolEnvelope< CoolMatrix<Type> > CoolMatrix<Type>::abs() const {
  CoolMatrix<Type> temp(this->num_rows, this->num_cols);
  int i, j;
  for (i = 0; i < this->num_rows; i++)
    for (j = 0; j < this->num_cols; j++)
      if (this->data[i][j] < 0)
        temp.data[i][j] = - this->data[i][j];
      else
        temp.data[i][j] = this->data[i][j];
  CoolEnvelope< CoolMatrix<Type> >& result = (CoolEnvelope< CoolMatrix<Type> >&) temp; // same physical object
  return result;                                         // copy of envelope
}

// sign -- Return the matrix whose elements are either -1,1 or 0
// depending on whether the corresponding values are negative, positive, or 0.
// Input:       this*
// Ouput:       New matrix

template<class Type> 
CoolEnvelope< CoolMatrix<Type> > CoolMatrix<Type>::sign() const {
  CoolMatrix<Type> temp(this->num_rows, this->num_cols);
  int i, j;
  for (i = 0; i < this->num_rows; i++)
    for (j = 0; j < this->num_cols; j++)
      if (this->data[i][j] == 0)                        // test fuzz equality to 0
        temp.data[i][j] = 0;                    // first.
      else
        if (this->data[i][j] < 0)
          temp.data[i][j] = -1;
        else
          temp.data[i][j] = 1;
  CoolEnvelope< CoolMatrix<Type> >& result = (CoolEnvelope< CoolMatrix<Type> >&) temp; // same physical object
  return result;                                         // copy of envelope
}

// element_product -- return the matrix whose elements are the products 
// Input:     2 matrices m1, m2 by reference
// Output:    New matrix, whose elements are m1[ij]*m2[ij].

template<class Type>
CoolEnvelope< CoolMatrix<Type> > element_product (const CoolMatrix<Type>& m1, const CoolMatrix<Type>& m2) {
  if (m1.num_rows != m2.num_rows || m1.num_cols != m2.num_cols) // Size?
    m1.dimension_error ("element_product", "Type", 
                        m1.num_rows, m1.num_cols, m2.num_rows, m2.num_cols);
  CoolMatrix<Type> temp(m1.num_rows, m1.num_cols);
  int i, j;
  for (i = 0; i < m1.num_rows; i++)
    for (j = 0; j < m1.num_cols; j++)
      temp.data[i][j] = m1.data[i][j] * m2.data[i][j];
  CoolEnvelope< CoolMatrix<Type> >& result = (CoolEnvelope< CoolMatrix<Type> >&) temp; // same physical object
  return result;                                         // copy of envelope
}

// element_quotient -- return the matrix whose elements are the quotients 
// Input:     2 matrices m1, m2 by reference
// Output:    New matrix, whose elements are m1[ij]/m2[ij].

template<class Type>
CoolEnvelope< CoolMatrix<Type> > element_quotient (const CoolMatrix<Type>& m1, const CoolMatrix<Type>& m2) {
  if (m1.num_rows != m2.num_rows || m1.num_cols != m2.num_cols) // Size?
    m1.dimension_error ("element_quotient", "Type", 
                        m1.num_rows, m1.num_cols, m2.num_rows, m2.num_cols);
  CoolMatrix<Type> temp(m1.num_rows, m1.num_cols);
  int i, j;
  for (i = 0; i < m1.num_rows; i++)
    for (j = 0; j < m1.num_cols; j++)
      temp.data[i][j] = m1.data[i][j] / m2.data[i][j];
  CoolEnvelope< CoolMatrix<Type> >& result = (CoolEnvelope< CoolMatrix<Type> >&) temp; // same physical object
  return result;                                         // copy of envelope
}

// update -- replace a submatrix of this, by the actual argument.
// Input:       *this, starting corner specified by top and left.
// Ouput:       mutated reference.

template<class Type> 
CoolMatrix<Type>& CoolMatrix<Type>::update (const CoolMatrix<Type>& m, 
                                            unsigned int top, unsigned int left) {
  unsigned int bottom = top + m.num_rows;
  unsigned int right = left + m.num_cols;
  if (this->num_rows < bottom || this->num_cols < right)
    this->dimension_error ("update", "Type", 
                           bottom-top, right-left, m.num_rows, m.num_cols);
  int i, j;
  for (i = top; i < bottom; i++)
    for (j = left; j < right; j++)
      this->data[i][j] = m.data[i-top][j-left];
  return *this;
}


// extract -- Return a submatrix specified by the top-left corner and size.
// Input:       *this, starting corner specified by top and left, and size.
// Ouput:       new matrix

template<class Type> 
CoolEnvelope< CoolMatrix<Type> > CoolMatrix<Type>::extract (unsigned int rows, unsigned int cols, unsigned int top, unsigned int left) const{
  unsigned int bottom = top + rows;
  unsigned int right = left + cols;
  if ((this->num_rows < bottom) || (this->num_cols < right))
    this->dimension_error ("extract", "Type", 
                           bottom-top, right-left, rows, cols);
  CoolMatrix<Type> temp(rows, cols);
  for (int i = 0; i < rows; i++)                // actual copy of all elements
    for (int j = 0; j < cols ; j++)             // in submatrix
      temp.data[i][j] = data[top+i][left+j];
  CoolEnvelope< CoolMatrix<Type> >& result = (CoolEnvelope< CoolMatrix<Type> >&) temp; // same physical object
  return result;                                         // copy of envelope
}

// determinant -- Determinant of a square matrix using Cramer's rule.
//              Signal Error exception if the matrix is not square.

template<class Type>
Type CoolMatrix<Type>::determinant () const {
  if (this->num_rows != this->num_cols || this->num_rows < 2)
    this->dimension_error ("determinant", "Type",
                           this->num_rows, this->num_cols, 
                           this->num_rows, this->num_cols);
  int n = this->num_rows, r, i, j;
  Type det = 0, prod;
  if (n == 2) {
    det = (this->data[0][0] * this->data[1][1] - // border case of 2x2 matrix
           this->data[0][1] * this->data[1][0]);
  } else {
    for (r = 0; r < n; r++) {                   // compute sum of products
      prod = 1;                                 // along diagonals
      for (i = r, j = 0; i < n; i++, j++)       // top-left to bottom-right
        prod *= this->data[i][j];
      for (i = 0; j < n; i++, j++)
        prod *= this->data[i][j];
      det += prod;                              // coeft = +1
    }
    int e = n-1;                                // index of last row/col
    for (r = 0; r < n; r++) {                   // compute sum of products
      prod = 1;                                 // along diagonals
      for (i = r, j = e; i < n; i++, j--)       // top-right to bottom-left
        prod *= this->data[i][j];
      for (i=0; j >= 0; i++, j--)
        prod *= this->data[i][j];
    det -= prod;                                // coeft = -1
    }
  }
  return det;
}

// dot_product -- Return the dot product of the row or column vectors
// Input:       2 vectors by reference
// Ouput:       dot product value

template<class Type>
  Type dot_product (const CoolMatrix<Type>& v1, const CoolMatrix<Type>& v2) {
    if (v1.num_rows != v2.num_rows || v1.num_cols != v2.num_cols) // Size?
      v1.dimension_error ("dot_product", "Type", 
                          v1.num_rows, v1.num_cols, v2.num_rows, v2.num_cols);
    Type dot = 0;
    int i, j;
    for (i = 0; i < v1.num_rows; i++)
      for (j = 0; j < v1.num_cols; j++)         // generalized dot-product
        dot += v1.data[i][j] * v2.data[i][j];   // of matrices
    return dot;
}

// cross_2d -- Return the 2X1 cross-product of 2 2d-vectors
// Input:       2 vectors by reference
// Ouput:       cross product value

template<class Type>
Type cross_2d (const CoolMatrix<Type>& v1, const CoolMatrix<Type>& v2) {
  if (v1.num_rows != v2.num_rows || v1.num_cols != v2.num_cols)
    v1.dimension_error ("cross_2d", "Type", 
                        v1.num_rows, v1.num_cols, v2.num_rows, v2.num_cols);
  CoolMatrix<Type>& m1 = (CoolMatrix<Type>&) v1; // cast away const.
  CoolMatrix<Type>& m2 = (CoolMatrix<Type>&) v2; 
  return (m1.x() * m2.y()                       // work for both col/row
          -                                     // representation.
          m1.y() * m2.x());
}

// cross_3d -- Return the 3X1 cross-product of 2 3d-vectors
// Input:       2 vectors by reference
// Ouput:       3d cross product vector

template<class Type>
CoolEnvelope< CoolMatrix<Type> > cross_3d (const CoolMatrix<Type>& v1, const CoolMatrix<Type>& v2) {
  if (v1.num_rows != v2.num_rows || v1.num_cols != v2.num_cols)
    v1.dimension_error ("cross_3d", "Type", 
                        v1.num_rows, v1.num_cols, v2.num_rows, v2.num_cols);
  CoolMatrix<Type> temp(v1.num_rows, v1.num_cols);
  CoolMatrix<Type>& m1 = (CoolMatrix<Type>&) v1; // cast away const.
  CoolMatrix<Type>& m2 = (CoolMatrix<Type>&) v2; 
  temp.x() = m1.y() * m2.z() - m1.z() * m2.y(); // work for both col/row
  temp.y() = m1.z() * m2.x() - m1.x() * m2.z(); // representation
  temp.z() = m1.x() * m2.y() - m1.y() * m2.x();
  CoolEnvelope< CoolMatrix<Type> >& result = (CoolEnvelope< CoolMatrix<Type> >&) temp; // same physical object
  return result;                                         // copy of envelope
}

//## hack to workaround BC++ 3.1 Envelope bug
#undef CoolEnvelope

