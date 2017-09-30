// @(#)field.h	1.5   Release Date: 2/26/93
//	 Author:  Kent G. Budge, 
//		  Computational Physics Research and Development (1431)
//		  Sandia National Laboratories 

#ifndef fieldH
#define fieldH

// Compilation macro flags:

//	CRAY		Compile for Cray mainframe
//	SUN		Compile for Sun workstations
//      HP_UX		Compile for HP workstations
//	NDEBUG		Turn off computationally expensive checks

/******************************** Macros *************************************/

/***************************** Include files *********************************/

#include <assert.h>
#include <stddef.h>		// Needed for size_t
#include <stdio.h>		// Needed for FILE* 
#include <string.h>		// Needed for memcpy

#ifdef __WATCOM_INT64__
typedef __int64 INT;
#else
typedef long INT;
#endif

/**************************** Predeclarations ********************************/

// Classes defined in this library are:

class field;			// A field of double values
class ifield;			// A field of integer values

/*************************** External Variables ******************************/

/***************************** Helper classes ********************************/

struct __field_node {
  size_t length;		// Data array count
  size_t ref_count;		// Reference count
  double data[1];		// Start of data array

  //void *operator new(size_t, size_t=1);
  void *operator new(size_t, size_t);
  void *operator new(size_t);
  void operator delete(void*);
  __field_node(size_t);
  __field_node(size_t, double);
  __field_node(const __field_node&);
  __field_node& operator=(const __field_node&);
  __field_node *NewCopy(void) const;
};

struct __field_inode {
  size_t length;		// Data array count
  size_t ref_count;		// Reference count
  INT data[1];			// Start of data array

  //void *operator new(size_t, size_t=1);
  void *operator new(size_t, size_t);
  void *operator new(size_t);
  void operator delete(void*);
  __field_inode(size_t);
  __field_inode(size_t, INT);
  __field_inode(const __field_inode&);
  __field_inode& operator=(const __field_inode&);
  __field_inode *NewCopy(void) const;
};

/********************************* field *************************************
******************************************************************************/

class field {
  public:

		/**** Constructors, destructors, assigns ****/

    field(void);			// Create an unsized field.
    field(size_t);			// Create an uninitialized field
    field(size_t, double);		// Create a field of given size and
					// initialize to a given value.
    field(size_t, const double*);

    field(const field&);
    field(const ifield&);

    virtual ~field(void);

    field& operator=(const field&);
    field& operator=(const ifield&);
    field& operator=(double);

		/**** Operator overloads ****/

// Access functions

    size_t Length(void) const;		// Return the length of the allocated
					// data array (1 for a constant field)

    double operator[](size_t) const;
    double& operator[](size_t);

    operator double*(void);
    operator const double*(void) const;	// Direct access to data array.

// Unary operators
    
    field operator+(void) const;
    field operator-(void) const;

// Operators with scalar constants

    friend field operator*(const field&, double);
    friend field operator*(double, const field&);
    field& operator*=(double);
    friend field operator/(const field&, double);
    friend field operator/(double, const field&);
    field& operator/=(double);
    friend field operator+(const field&, double);
    friend field operator+(double, const field&);
    field& operator+=(double);
    friend field operator-(const field&, double);
    friend field operator-(double, const field&);
    field& operator-=(double);

// Operators with other double fields

    friend field  operator*(const field&, const field&);
    field& operator*=(const field& ab);
    friend field  operator/(const field&, const field&);
    field& operator/=(const field& ab);
    friend field  operator+(const field&, const field&);
    field& operator+=(const field& ab);
    friend field  operator-(const field&, const field&);
    field& operator-=(const field& ab);

// operators returning ifield

    friend ifield operator==(const field&, double);
    friend ifield operator==(double, const field&);
    friend ifield operator==(const field&, const field&);
    friend ifield operator!=(const field&, double);
    friend ifield operator!=(double, const field&);
    friend ifield operator!=(const field&, const field&);
    friend ifield operator<(const field&, double);
    friend ifield operator<(double, const field&);
    friend ifield operator<(const field&, const field&);
    friend ifield operator>(const field&, double);
    friend ifield operator>(double, const field&);
    friend ifield operator>(const field&, const field&);
    friend ifield operator<=(const field&, double);
    friend ifield operator<=(double, const field&);
    friend ifield operator<=(const field&, const field&);
    friend ifield operator>=(const field&, double);
    friend ifield operator>=(double, const field&);
    friend ifield operator>=(const field&, const field&);

 				// Methods

// find the sum of all the components

    double Sum(void) const;

// Perform the indicated function element-by-element

    friend field acos(const field&);
    friend field asin(const field&);
    friend field atan(const field&);
    friend field atan2(const field& y, double x);
    friend field atan2(double y, const field& x);
    friend field atan2(const field& y, const field& x);
    friend field ceil(const field&);
    friend field cos(const field&);
    friend field cosh(const field&);
    friend field exp(const field&);
    friend field fabs(const field&);
    friend field floor(const field&);
    friend field frexp(const field&, ifield&);
    friend field fmod(const field&, double);
    friend field fmod(double, const field&);
    friend field fmod(const field&, const field&);
    friend field ldexp(const field&, const ifield&);
    friend field log(const field&);
    friend field log10(const field&);
    friend field modf(const field&, field&);
    friend field pow(const field&, double);
    friend field pow(double, const field&);
    friend field pow(const field&, const field&);
    friend field sin(const field&);
    friend field sinh(const field&);
    friend field sqrt(const field&);
    friend field tan(const field&);
    friend field tanh(const field&);

// FORTRAN-like functions

    friend field dim(const field&, double);
    friend field dim(double, const field&);
    friend field dim(const field&, const field&);
    friend field min(double, const field&);
    friend field max(double, const field&);
    friend field min(const field&, double);
    friend field max(const field&, double);
    friend field min(const field&, const field&);
    friend field max(const field&, const field&);
    friend double min(const field&);
    friend double max(const field&);
    friend field sign(double, const field&);
    friend field sign(const field&, double);
    friend field sign(const field&, const field&);

// Topological methods

    field Shift(int) const;

    field& Assemble(const field& a, const ifield& ia, const ifield& ib);
		// Add selected elements from a to this.  ib lists the 
		// elements selected from a and ia lists their destination
		// in this. ia and ib must be the same length; this and a
		// need not be the same length.  To function reliably, it
		// is necessary that each value in ia be unique. 

    field& Assemble(const field& a, const ifield& ia);
		// Add all elements from a to selected elements of this.
		// ia lists the destination of each element of a.  ia and
		// a must be the same size; a and this need not be the same
		// size. To function reliably, each value in ia should be
		// unique.

    field& Assemble(double, const ifield& ia);
		// Like the previous function, but a constant value is
		// being assembled into selected elements of this.

    field& Scatter(const field& a, const ifield& ia, const ifield& ib);
		// Scatter selected elements from a to this (replacing the
		// old values.)  ib lists the elements selected from a and ia 
		// lists their destination in this. ia and ib must be the same
		// length; this and a need not be the same length.  To function
		// reliably, it is necessary that each value in ia be unique. 

    field& Scatter(const field& a, const ifield& ia);
		// Scatter the elements of a into this (replacing the old
		// values).  ia lists the destination of each element
		// in this.  a and ia must be the same length. To function 
		// reliably, each value in ia should be unique.

    field& Scatter(double fill, const ifield& ia);
		// Like the previous function, but a constant value is
		// being scattered into selected elements of this.
 
    field Gather(const ifield& ia) const;
		// Gather selected elements from this into a new field.
		// ia lists the elements to select.  the result will have
		// the same length as ia.  The mapping need NOT be one-to-one
		// to function reliably.

    field& Scatter(const field& a, size_t start, size_t end, size_t stride=1);
		// Scatter the elements of a into those elements of the field
		// whose indices are n*stride+start.  The length of the field
		// must be stride times the length of a and start must be less
		// than stride.

    field Gather(size_t start, size_t end, size_t stride=1) const;
		// Gather the elements whose indices are given by n*stride+start
		// into a new field. The value of start must be less than 
		// the value of stride.
 
// I/O

    friend size_t fwrite(const field&, FILE*);
    friend size_t fread(field&, FILE*);

// Miscellaneous

    void Free(void);

  private:

    friend class ifield;

    // Data

    int ref_flag;			// Number of references given out
    struct __field_node *root;

    // Methods

    void Private(void);
};

/********************************* ifield ************************************
******************************************************************************/

class ifield {
  public:

		/**** Constructors, destructors, assigns ****/

    ifield(void);			// Create an unsized field
    ifield(size_t);			// Create an uninitialized field.
    ifield(size_t size, INT fill);	// Create a sized field and initialize
					// to the fill value.
    ifield(size_t, const INT*);

    ifield(const ifield&);
    ifield(const field&);

    virtual ~ifield(void);

    ifield& operator=(const ifield&);
    ifield& operator=(const field&);
    ifield& operator=(INT);

		/**** Operator overloads ****/

// Access functions

    size_t Length(void) const;
    INT operator[](size_t) const;
    INT& operator[](size_t);
    operator const INT*(void) const;
    operator INT*(void);

// Unary operators
    
    ifield operator+(void) const;
    ifield operator-(void) const;
    ifield operator~(void) const;
    ifield operator!(void) const;

// Operators with scalar constants

    friend ifield operator*(const ifield&, INT);
    friend ifield operator*(INT, const ifield&);
    ifield& operator*=(INT);
    friend ifield operator/(const ifield&, INT);
    friend ifield operator/(INT, const ifield&);
    ifield& operator/=(INT);
    friend ifield operator%(const ifield&, INT);
    friend ifield operator%(INT, const ifield&);
    ifield& operator%=(INT);
    friend ifield operator+(const ifield&, INT);
    friend ifield operator+(INT, const ifield&);
    ifield& operator+=(INT);
    friend ifield operator-(const ifield&, INT);
    friend ifield operator-(INT, const ifield&);
    ifield& operator-=(INT);
    friend ifield operator^(const ifield&, INT);
    friend ifield operator^(INT, const ifield&);
    ifield& operator^=(INT);
    friend ifield operator&(const ifield&, INT);
    friend ifield operator&(INT, const ifield&);
    ifield& operator&=(INT);
    friend ifield operator|(const ifield&, INT);
    friend ifield operator|(INT, const ifield&);
    ifield& operator|=(INT);
    friend ifield operator<(const ifield&, INT);
    friend ifield operator<(INT, const ifield&);
    friend ifield operator>(const ifield&, INT);
    friend ifield operator>(INT, const ifield&);
    friend ifield operator<=(const ifield&, INT);
    friend ifield operator<=(INT, const ifield&);
    friend ifield operator>=(const ifield&, INT);
    friend ifield operator>=(INT, const ifield&);
    friend ifield operator<<(const ifield&, INT);
    friend ifield operator<<(INT, const ifield&);
    ifield& operator<<=(INT);
    friend ifield operator>>(const ifield&, INT);
    friend ifield operator>>(INT, const ifield&);
    ifield& operator>>=(INT);
    friend ifield operator&&(const ifield&, INT);
    friend ifield operator&&(INT, const ifield&);
    friend ifield operator||(const ifield&, INT);
    friend ifield operator||(INT, const ifield&);
    friend ifield operator==(const ifield&, INT);
    friend ifield operator==(INT, const ifield&);
    friend ifield operator!=(const ifield&, INT);
    friend ifield operator!=(INT, const ifield&);

// Operators with other integer fields

    friend ifield  operator*(const ifield&, const ifield&);
    ifield& operator*=(const ifield& ab);
    friend ifield  operator/(const ifield&, const ifield&);
    ifield& operator/=(const ifield& ab);
    friend ifield  operator%(const ifield&, const ifield&);
    ifield& operator%=(const ifield& ab);
    friend ifield  operator+(const ifield&, const ifield&);
    ifield& operator+=(const ifield& ab);
    friend ifield  operator-(const ifield&, const ifield&);
    ifield& operator-=(const ifield& ab);
    friend ifield operator^(const ifield&, const ifield&);
    ifield& operator^=(const ifield&);
    friend ifield operator|(const ifield&, const ifield&);
    ifield& operator|=(const ifield&);
    friend ifield operator&(const ifield&, const ifield&);
    ifield& operator&=(const ifield&);
    friend ifield operator<<(const ifield&, const ifield&);
    ifield& operator<<=(const ifield&);
    friend ifield operator>>(const ifield&, const ifield&);
    ifield& operator>>=(const ifield&);

    friend ifield operator==(const ifield&, const ifield&);
    friend ifield operator!=(const ifield&, const ifield&);
    friend ifield operator<( const ifield&, const ifield&);
    friend ifield operator>( const ifield&, const ifield&);
    friend ifield operator<=(const ifield&, const ifield&);
    friend ifield operator>=(const ifield&, const ifield&);
    friend ifield operator||(const ifield&, const ifield&);
    friend ifield operator&&(const ifield&, const ifield&);

// operators returning ifield

    friend ifield operator==(const field&, double);
    friend ifield operator==(double, const field&);
    friend ifield operator==(const field&, const field&);
    friend ifield operator!=(const field&, double);
    friend ifield operator!=(double, const field&);
    friend ifield operator!=(const field&, const field&);
    friend ifield operator<(const field&, double);
    friend ifield operator<(double, const field&);
    friend ifield operator<(const field&, const field&);
    friend ifield operator>(const field&, double);
    friend ifield operator>(double, const field&);
    friend ifield operator>(const field&, const field&);
    friend ifield operator<=(const field&, double);
    friend ifield operator<=(double, const field&);
    friend ifield operator<=(const field&, const field&);
    friend ifield operator>=(const field&, double);
    friend ifield operator>=(double, const field&);
    friend ifield operator>=(const field&, const field&);

 				// Methods

// find the sum of all the components

    INT Sum(void) const;

// Perform the indicated function element-by-element

    friend ifield abs(const ifield&);
    friend void div(const ifield&, const ifield&, ifield&, ifield&);
    void rand(void);

// FORTRAN-like functions

    friend ifield min(INT, const ifield&);
    friend ifield max(INT, const ifield&);
    friend ifield min(const ifield&, INT);
    friend ifield max(const ifield&, INT);
    friend ifield min(const ifield&, const ifield&);
    friend ifield max(const ifield&, const ifield&);
    friend INT min(const ifield&);
    friend INT max(const ifield&);
    friend ifield sign(INT, const ifield&);
    friend ifield sign(const ifield&, INT);
    friend ifield sign(const ifield&, const ifield&);
    friend ifield dim(INT, const ifield&);
    friend ifield dim(const ifield&, INT);
    friend ifield dim(const ifield&, const ifield&);

// perform a gather operation

    ifield Shift(int) const;

    ifield& Assemble(const ifield& a, const ifield& ia, const ifield& ib);
		// Add selected elements from a to this.  ib lists the 
		// elements selected from a and ia lists their destination
		// in this. ia and ib must be the same length; this and a
		// need not be the same length.  To function reliably, it
		// is necessary that each value in ia be unique. 

    ifield& Assemble(const ifield& a, const ifield& ia);
		// Add all elements from a to selected elements of this.
		// ia lists the destination of each element of a.  ia and
		// a must be the same size; a and this need not be the same
		// size. To function reliably, each value in ia should be
		// unique.

    ifield& Assemble(INT, const ifield& ia);
		// Like the previous function, but a constant value is
		// being assembled into selected elements of this.

    ifield& Scatter(const ifield& a, const ifield& ia, const ifield& ib);
		// Scatter selected elements from a to this (replacing the
		// old values.)  ib lists the elements selected from a and ia 
		// lists their destination in this. ia and ib must be the same
		// length; this and a need not be the same length.  To function
		// reliably, it is necessary that each value in ia be unique. 

    ifield& Scatter(const ifield& a, const ifield& ia);
		// Scatter the elements of a into this (replacing the old
		// values).  ia lists the destination of each element
		// in this.  a and ia must be the same length. To function 
		// reliably, each value in ia should be unique.

    ifield& Scatter(INT fill, const ifield& ia);
		// Like the previous function, but a constant value is
		// being scattered into selected elements of this.
 
    ifield Gather(const ifield& ia) const;
		// Gather selected elements from this into a new ifield.
		// ia lists the elements to select.  the result will have
		// the same length as ia.  The mapping need NOT be one-to-one
		// to function reliably.

    ifield& Scatter(const ifield& a, size_t start, size_t end, size_t stride=1);
		// Scatter the elements of a into those elements of the ifield
		// whose indices are n*stride+start.  The length of the ifield
		// must be stride times the length of a and start must be less
		// than stride.

    ifield Gather(size_t start, size_t end, size_t stride=1) const;
		// Gather the elements whose indices are given by n*stride+start
		// into a new ifield. The value of start must be less than 
		// the value of stride.

// I/O

    friend size_t fwrite(const ifield&, FILE*);
    friend size_t fread(ifield&, FILE*);

// Miscellaneous

    void Free(void);

  private:

    friend class field;

    // Data

    int ref_flag;
    struct __field_inode *root;

    // Methods

    void Private(void);
};

/******************************* Inlines *************************************/

/**** __field_node ****/

inline __field_node::__field_node(size_t size) :
  length(size),
  ref_count(1)
{}

inline __field_node* __field_node::NewCopy(void) const {
  return new(length) __field_node(*this);
}

/**** __field_inode ****/

inline __field_inode::__field_inode(size_t size) :
  length(size),
  ref_count(1)
{}

inline __field_inode* __field_inode::NewCopy(void) const {
  return new(length) __field_inode(*this);
}

/**** field ****/

inline field::field(void) :
  ref_flag(0),
  root(NULL)
{}

inline field::field(size_t size) :
  ref_flag(0),
  root(new(size) __field_node(size))
{}

inline field::field(size_t size, double fill) :
  ref_flag(0),
  root(new(size) __field_node(size, fill))
{}

inline field::~field(void){ Free(); }

inline size_t field::Length(void) const {
  return root? root->length : 0;
}

inline double field::operator[](size_t n) const{
  assert(n<root->length);
  return root->data[n];
}
inline double& field::operator[](size_t n) {
  assert(n<root->length);
  if (root->ref_count>1) Private();
  ref_flag = 1;
  return root->data[n];
}

inline field::operator const double*(void) const{ 
  assert(root!=NULL);
  return root->data; 
}
inline field::operator double *(void){ 
  assert(root!=NULL);
  ref_flag = 1;
  if (root->ref_count>1) Private();
  return root->data; 
}

inline field field::operator+(void) const{
  return *this;
}

/**** ifield ****/

inline ifield::ifield(void) :
  ref_flag(0),
  root(NULL)
{}

inline ifield::ifield(size_t size) :
  ref_flag(0),
  root(new(size) __field_inode(size))
{}

inline ifield::ifield(size_t size, INT fill) :
  ref_flag(0),
  root(new(size) __field_inode(size, fill))
{}

inline ifield::~ifield(void){ Free(); }

inline size_t ifield::Length(void) const {
  return root? root->length : 0;
}

inline INT ifield::operator[](size_t n) const{
  assert(n<root->length);
  return root->data[n];
}

inline INT& ifield::operator[](size_t n) {
  assert(n<root->length);
  if (root->ref_count>1) Private();
  ref_flag = 1;
  return root->data[n];
}

inline ifield::operator const INT *(void) const{
  assert(root!=NULL);
  return root->data; 
}
inline ifield::operator INT *(void){
  assert(root!=NULL);
  if (root->ref_count>1) Private();
  ref_flag = 1;
  return root->data; 
}

inline ifield ifield::operator+(void) const{
  return *this;
}

#endif
