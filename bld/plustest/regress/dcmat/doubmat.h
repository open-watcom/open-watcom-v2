#ifndef _doubleMatrix_h
#define _doubleMatrix_h 1

#include <libc.h>
#include <math.h>
#include <iostream.h>

class	 doubleMatrix;				// forward declaration

class	 doubleArray 
{
public:
	 int		L;			// length of rows
	 int		M;			// number of rows
	 int		N;			// number of columns
	 double*		X;			// matrix pointer

// Constructors
	 doubleArray (int l, int m, int n, double* d = 0);
	 doubleArray (int m, int n, double* d = 0);
	 doubleArray (int n, double* d = 0);
	 doubleArray (const doubleArray& a);
	 doubleArray ();
	~doubleArray ();

// Operator Summary
// subscripting
	 double*		operator []		(int i)	const;
// unary minus
	 doubleMatrix	operator -		()	const;
// unary plus
	 doubleMatrix	operator +		()	const;
// caste (type conversion)
			operator double		()	const;
			operator double*		()	const;
// simple assignment
	 doubleArray&	operator  = (const double& a);
	 doubleArray&	operator  = (const doubleArray& a);
// multiply    and assign
	 doubleArray&	operator *= (const double& b);
	 doubleArray&	operator *= (const doubleArray& b);
// divide      and assign
	 doubleArray&	operator /= (const double& b);
	 doubleArray&	operator /= (const doubleArray& b);
// add         and assign
	 doubleArray&	operator += (const double& b);
	 doubleArray&	operator += (const doubleArray& b);
// subtract    and assign
	 doubleArray&	operator -= (const double& b);
	 doubleArray&	operator -= (const doubleArray& b);
// shift left  and assign
	 doubleArray&	operator <<=(int n);
// shift right and assign
	 doubleArray&	operator >>=(int n);

// Function Summary
	 void		error(const char* msg)	const;
// doubleArray reconstructors
	 doubleArray&	resize(const doubleArray& a);
	 doubleArray&	resize(int m, int n, double* d);
	 doubleArray&	resize(int n, double* d);
	 int&		l(int  i);		// change length  of rows
	 int&		m(int  i);		// change number  of rows
	 int&		n(int  i);		// change number  of columns
	 double*&		x(double* u);		// change pointer to array
	 int		l()	const;		// return length  of rows
	 int		m()	const;		// return number  of rows
	 int		n()	const;		// return number  of columns
	 double*		x()	const;		// return pointer to array
// subArray
	 doubleArray 	s(int i = 0, int m = 1, int j = 0)	const;
	 doubleArray 	s(int i, int m, int j, int n)		const;

	 doubleMatrix 	_(int i = 0, int m = 1, int j = 0)	const;
	 doubleMatrix 	_(int i, int m, int j, int n)		const;

	 doubleArray	invert(double d = 1e-12);	// invert Array
	 doubleMatrix	i(double d = 1e-12) const;	// inverse
	 doubleMatrix	p()	const;		// prime
	 doubleMatrix	t()	const;		// transpose

	 doubleMatrix	sum()	const;		// sums of row elements
	 doubleMatrix	sumsq()	const;		// sums of row elements^2
// X.map(f) returns the element by element mapping f(X)
	 doubleMatrix	map(double (*f)(      double )) const;
	 doubleMatrix	min()	const;		// minimum row elements
	 doubleMatrix	max()	const;		// maximum row elements

	 int		index(const double&a)const;// index of array element == a
	 int		min_index()	const;	// minimum array element index
	 int		max_index()	const;	// maximum array element index

	 double		f(double x, double y)	const;	// interpolation
};
// Array  Constructors
inline	 doubleArray::doubleArray(int l, int m, int n, double* d) :
L(l), M(m), N(n), X(d) { }
inline	 doubleArray::doubleArray(int m, int n, double* d) :
L(n), M(m), N(n), X(d) { }
inline	 doubleArray::doubleArray(int n, double* d) :
L(n), M(1), N(n), X(d) { }
inline	 doubleArray::doubleArray(const doubleArray& a) :
L(a.L), M(a.M), N(a.N), X(a.X) { }
inline	 doubleArray::doubleArray() :
L(0), M(0), N(0), X(0) { }
inline	 doubleArray::~doubleArray()
{ }

class	 doubleMatrix : public doubleArray
{
public:

// Constructors
	 doubleMatrix (int m, int n, double* d);
	 doubleMatrix (const doubleMatrix& a);
	 doubleMatrix (const doubleArray& a);
	 doubleMatrix (int m, int n, const double& a);
	 doubleMatrix (int n, const double& a);
	 doubleMatrix (int m, int n);
	 doubleMatrix (int n);
	 doubleMatrix ();
	~doubleMatrix ();

// Operator Summary
	 doubleMatrix&	operator  = (const doubleMatrix& a);

// Function Summary
	 doubleMatrix&	resize(const doubleArray& a);
	 doubleMatrix&	resize(int m, int n, double* d);
	 doubleMatrix&	resize(int n, double* d);
	 doubleMatrix&	resize(int m, int n, const double& a);
	 doubleMatrix&	resize(int n, const double& a);
	 doubleMatrix&	resize(int m, int n);
	 doubleMatrix&	resize(int n = 0);

};

// Matrix Constructors
inline	 doubleMatrix::doubleMatrix(int m, int n, double* d) :
doubleArray(m, n, d) { }
inline	 doubleMatrix::doubleMatrix(int m, int n, const double& a) :
doubleArray(m, n, new double [m*n]) { double* t = X; while (t < &X[m*n]) *t++ = a; }
inline	 doubleMatrix::doubleMatrix(int n, const double& a) :
doubleArray(n, new double [n]) { double* t = X; while (t < &X[n]) *t++ = a; }
inline	 doubleMatrix::doubleMatrix(int m, int n) :
doubleArray(m, n, new double [m*n]) { }
inline	 doubleMatrix::doubleMatrix(int n) :
doubleArray(n, new double [n]) { }
inline	 doubleMatrix::doubleMatrix() :
doubleArray() { }
inline	 doubleMatrix::~doubleMatrix()
{ delete X; }

// Operators
inline	 double*		doubleArray::operator []		(int i)	const
{ return &X[i*L]; }
inline	 doubleMatrix	doubleArray::operator +		()	const
{ return *this; }
inline			doubleArray::operator double		()	const
{ return X[0]; }
inline			doubleArray::operator double*		()	const
{ return X; }
// multiply
	 doubleMatrix	operator *  (const doubleArray& a, const double& b);
	 doubleMatrix	operator *  (const double& a, const doubleArray& b);
	 doubleMatrix	operator *  (const doubleArray& a, const doubleArray& b);
// divide
	 doubleMatrix	operator /  (const doubleArray& a, const double& b);
	 doubleMatrix	operator /  (const double& a, const doubleArray& b);
	 doubleMatrix	operator /  (const doubleArray& a, const doubleArray& b);
// matrix multiplication (inner product)
	 doubleMatrix	operator %  (const doubleArray& a, const doubleArray& b);
// add (plus)
	 doubleMatrix	operator +  (const doubleArray& a, const double& b);
	 doubleMatrix	operator +  (const double& a, const doubleArray& b);
	 doubleMatrix	operator +  (const doubleArray& a, const doubleArray& b);
// subtract (minus)
	 doubleMatrix	operator -  (const doubleArray& a, const double& b);
	 doubleMatrix	operator -  (const double& a, const doubleArray& b);
	 doubleMatrix	operator -  (const doubleArray& a, const doubleArray& b);
// shift left
	 doubleMatrix	operator << (const doubleArray& a, int n);
// output 
	 ostream&	operator << (ostream& s, const doubleArray& a);
// shift right
	 doubleMatrix	operator >> (const doubleArray& a, int n);
// input
	 istream&	operator >> (istream& s, const doubleArray& a);


// less    than
	 int		operator <  (const doubleArray& a, const double& b);
	 int		operator <  (const double& a, const doubleArray& b);
	 int		operator <  (const doubleArray& a, const doubleArray& b);
// less    than or equal
inline	 int		operator <= (const doubleArray& a, const double& b)
{ return !(b <  a); }
inline	 int		operator <= (const double& a, const doubleArray& b)
{ return !(b <  a); }
inline	 int		operator <= (const doubleArray& a, const doubleArray& b)
{ return !(b <  a); }
// greater than
inline	 int		operator >  (const doubleArray& a, const double& b)
{ return  (b <  a); }
inline	 int		operator >  (const double& a, const doubleArray& b)
{ return  (b <  a); }
inline	 int		operator >  (const doubleArray& a, const doubleArray& b)
{ return  (b <  a); }
// greater than or equal
inline	 int		operator >= (const doubleArray& a, const double& b)
{ return !(a <  b); }
inline	 int		operator >= (const double& a, const doubleArray& b)
{ return !(a <  b); }
inline	 int		operator >= (const doubleArray& a, const doubleArray& b)
{ return !(a <  b); }
// equal
	 int		operator == (const doubleArray& a, const double& b);
inline	 int		operator == (const double& a, const doubleArray& b)
{ return  (b == a); }
	 int		operator == (const doubleArray& a, const doubleArray& b);
// not equal
inline	 int		operator != (const doubleArray& a, const double& b)
{ return !(a == b); }
inline	 int		operator != (const double& a, const doubleArray& b)
{ return !(b == a); }
inline	 int		operator != (const doubleArray& a, const doubleArray& b)
{ return !(a == b); }
// matrix multiplication (outer product)
	 doubleMatrix	operator &  (const doubleArray& a, const doubleArray& b);
// stack  matrices vertically
	 doubleMatrix	operator ^  (const doubleArray& a, const doubleArray& b);
// adjoin matrices horizontally
	 doubleMatrix	operator |  (const doubleArray& a, const doubleArray& b);

// Functions
inline	 doubleArray& doubleArray::resize(const doubleArray& a) {
  L = a.L; M = a.M; N = a.N; X = a.X;
  return *this; }
inline	 doubleArray& doubleArray::resize(int m, int n, double* d) {
  L = n; M = m; N = n; X = d;
  return *this; }
inline	 doubleArray& doubleArray::resize(int n, double* d) {
  L = n; M = 1; N = n; X = d;
  return *this; }
inline	 int&		doubleArray::l(int  i) { L = i; return L; }
inline	 int&		doubleArray::m(int  j) { M = j; return M; }
inline	 int&		doubleArray::n(int  k) { N = k; return N; }
inline	 double*&		doubleArray::x(double* u) { X = u; return X; }
inline	 int		doubleArray::l() const { return L; }
inline	 int		doubleArray::m() const { return M; }
inline	 int		doubleArray::n() const { return N; }
inline	 double*		doubleArray::x() const { return X; }
inline	 doubleArray	doubleArray::s(int i, int m, int j, int n)	const
{ return doubleArray(L, m, n, &X[i*L+j]); }
inline	 doubleArray	doubleArray::s(int i, int m, int j)	const
{ return doubleArray(L, m, N-j, &X[i*L+j]); }

	 char*		format(char* s,int n = 0,char* w = " ");
typedef	 void (*one_arg_error_handler_t)(const char*);
extern	 void default_doubleArray_error_handler(const char*);
extern	 one_arg_error_handler_t doubleArray_error_handler;
extern	 one_arg_error_handler_t 
	 set_doubleArray_error_handler(one_arg_error_handler_t f);

inline	 doubleMatrix&	doubleMatrix::operator  = (const doubleMatrix& a)
{ doubleArray::operator  = (a); return *this; }
inline	 doubleMatrix&	doubleMatrix::resize(const doubleArray& a)
{ delete X;
  L = a.N; M = a.M; N = a.N; X = new double [M*N]; *this = a;
  return *this; }
inline	 doubleMatrix&	doubleMatrix::resize(int m, int n, double* d)
{ delete X;
  L = n; M = m; N = n; X = d;
  return *this; }
inline	 doubleMatrix&	doubleMatrix::resize(int n, double* d)
{ delete X;
  L = n; M = 1; N = n; X = d;
  return *this; }
inline	 doubleMatrix&	doubleMatrix::resize(int m, int n, const double& a)
{ delete X;
  L = n; M = m; N = n; X = new double [m*n];
  double* t = X; while (t < &X[m*n]) *t++ = a;
  return *this; }
inline	 doubleMatrix&	doubleMatrix::resize(int n, const double& a)
{ delete X;
  L = n; M = 1; N = n; X = new double [n];
  double* t = X; while (t < &X[n]) *t++ = a;
  return *this; }
inline	 doubleMatrix&	doubleMatrix::resize(int m, int n)
{ delete X;
  L = n; M = m; N = n; X = new double [m*n];
  return *this; }
inline	 doubleMatrix&	doubleMatrix::resize(int n)
{ delete X;
  L = n; M = 1; N = n; X = new double [n];
  return *this; }
//subMatrix
inline	 doubleMatrix	doubleArray::_(int i, int m, int j, int n)	const
{ return doubleMatrix(s(i, m, j, n)); }
inline	 doubleMatrix	doubleArray::_(int i, int m, int j)	const
{ return doubleMatrix(s(i, m, j, N-j)); }

inline	 double	 sgn(      double  a)	{ return a < 0.0? -1.0: +1.0; }
inline	 doubleMatrix	 cos(const doubleArray& a) { return a.map( cos); }
inline	 doubleMatrix	 sin(const doubleArray& a) { return a.map( sin); }
inline	 doubleMatrix	 tan(const doubleArray& a) { return a.map( tan); }
inline	 doubleMatrix	cosh(const doubleArray& a) { return a.map(cosh); }
inline	 doubleMatrix	sinh(const doubleArray& a) { return a.map(sinh); }
inline	 doubleMatrix	tanh(const doubleArray& a) { return a.map(tanh); }
inline	 doubleMatrix	 exp(const doubleArray& a) { return a.map( exp); }
inline	 doubleMatrix	 log(const doubleArray& a) { return a.map( log); }
inline	 doubleMatrix	sqrt(const doubleArray& a) { return a.map(sqrt); }

inline	 doubleMatrix	 abs(const doubleArray& a) { return a.map(fabs); }
inline	 doubleMatrix	 sgn(const doubleArray& a) { return a.map( sgn); }
	 doubleMatrix	ffct(const doubleArray& a);// fast fourier cosine transform
#endif

