#ifndef _complexMatrix_h
#define _complexMatrix_h 1

#include "doubmat.h"
#include <complex.h>
typedef Complex complex;

class	 complexMatrix;				// forward declaration

class	 complexArray 
{
public:
	 int		L;			// length of rows
	 int		M;			// number of rows
	 int		N;			// number of columns
	 complex*		X;			// matrix pointer

// Constructors
	 complexArray (int l, int m, int n, complex* d = 0);
	 complexArray (int m, int n, complex* d = 0);
	 complexArray (int n, complex* d = 0);
	 complexArray (const complexArray& a);
	 complexArray ();
	~complexArray ();

// Operator Summary
// subscripting
	 complex*		operator []		(int i)	const;
// unary minus
	 complexMatrix	operator -		()	const;
// unary plus
	 complexMatrix	operator +		()	const;
// caste (type conversion)
			operator complex		()	const;
			operator complex*		()	const;
			operator doubleMatrix	()	const;
// simple assignment
	 complexArray&	operator  = (const complex& a);
	 complexArray&	operator  = (const complexArray& a);
// multiply    and assign
	 complexArray&	operator *= (const complex& b);
	 complexArray&	operator *= (const complexArray& b);
// divide      and assign
	 complexArray&	operator /= (const complex& b);
	 complexArray&	operator /= (const complexArray& b);
// add         and assign
	 complexArray&	operator += (const complex& b);
	 complexArray&	operator += (const complexArray& b);
// subtract    and assign
	 complexArray&	operator -= (const complex& b);
	 complexArray&	operator -= (const complexArray& b);
// shift left  and assign
	 complexArray&	operator <<=(int n);
// shift right and assign
	 complexArray&	operator >>=(int n);

// Function Summary
	 void		error(const char* msg)	const;
// complexArray reconstructors
	 complexArray&	resize(const complexArray& a);
	 complexArray&	resize(int m, int n, complex* d);
	 complexArray&	resize(int n, complex* d);
	 int&		l(int  i);		// change length  of rows
	 int&		m(int  i);		// change number  of rows
	 int&		n(int  i);		// change number  of columns
	 complex*&		x(complex* u);		// change pointer to array
	 int		l()	const;		// return length  of rows
	 int		m()	const;		// return number  of rows
	 int		n()	const;		// return number  of columns
	 complex*		x()	const;		// return pointer to array
// subArray
	 complexArray 	s(int i = 0, int m = 1, int j = 0)	const;
	 complexArray 	s(int i, int m, int j, int n)		const;

	 complexMatrix 	_(int i = 0, int m = 1, int j = 0)	const;
	 complexMatrix 	_(int i, int m, int j, int n)		const;

	 complexArray	invert(double d = 1e-12);	// invert Array
	 complexMatrix	i(double d = 1e-12) const;	// inverse
	 complexMatrix	p()	const;		// prime
	 complexMatrix	t()	const;		// transpose

	 complexMatrix	sum()	const;		// sums of row elements
	 complexMatrix	sumsq()	const;		// sums of row elements^2
// X.map(f) returns the element by element mapping f(X)
#ifdef __ATT_complex__
	 complexMatrix	map(complex (*f)(const complex&)) const;
#else
	 complexMatrix	map(complex (*f)(      complex )) const;
#endif
	 doubleMatrix	map(double (*f)(const complex&)) const;
	 doubleMatrix	map(double (*f)(      complex )) const;
	 doubleMatrix	real()	const;		// real
	 doubleMatrix	imag()	const;		// imaginary

	 complex		f(double x, double y)	const;	// interpolation
};
// Array  Constructors
inline	 complexArray::complexArray(int l, int m, int n, complex* d) :
L(l), M(m), N(n), X(d) { }
inline	 complexArray::complexArray(int m, int n, complex* d) :
L(n), M(m), N(n), X(d) { }
inline	 complexArray::complexArray(int n, complex* d) :
L(n), M(1), N(n), X(d) { }
inline	 complexArray::complexArray(const complexArray& a) :
L(a.L), M(a.M), N(a.N), X(a.X) { }
inline	 complexArray::complexArray() :
L(0), M(0), N(0), X(0) { }
inline	 complexArray::~complexArray()
{ }

class	 complexMatrix : public complexArray
{
public:

// Constructors
	 complexMatrix (int m, int n, complex* d);
	 complexMatrix (const complexMatrix& a);
	 complexMatrix (const complexArray& a);
	 complexMatrix (const doubleArray& r, const doubleArray& i);
	 complexMatrix (const doubleArray& r, const double i = 0.0);
	 complexMatrix (int m, int n, const complex& a);
	 complexMatrix (int n, const complex& a);
	 complexMatrix (int m, int n);
	 complexMatrix (int n);
	 complexMatrix ();
	~complexMatrix ();

// Operator Summary
	 complexMatrix&	operator  = (const complexMatrix& a);

// Function Summary
	 complexMatrix&	resize(const complexArray& a);
	 complexMatrix&	resize(int m, int n, complex* d);
	 complexMatrix&	resize(int n, complex* d);
	 complexMatrix&	resize(int m, int n, const complex& a);
	 complexMatrix&	resize(int n, const complex& a);
	 complexMatrix&	resize(int m, int n);
	 complexMatrix&	resize(int n = 0);

};

// Matrix Constructors
inline	 complexMatrix::complexMatrix(int m, int n, complex* d) :
complexArray(m, n, d) { }
inline	 complexMatrix::complexMatrix(int m, int n, const complex& a) :
complexArray(m, n, new complex [m*n]) { complex* t = X; while (t < &X[m*n]) *t++ = a; }
inline	 complexMatrix::complexMatrix(int n, const complex& a) :
complexArray(n, new complex [n]) { complex* t = X; while (t < &X[n]) *t++ = a; }
inline	 complexMatrix::complexMatrix(int m, int n) :
complexArray(m, n, new complex [m*n]) { }
inline	 complexMatrix::complexMatrix(int n) :
complexArray(n, new complex [n]) { }
inline	 complexMatrix::complexMatrix() :
complexArray() { }
inline	 complexMatrix::~complexMatrix()
{ delete X; }

// Operators
inline	 complex*		complexArray::operator []		(int i)	const
{ return &X[i*L]; }
inline	 complexMatrix	complexArray::operator +		()	const
{ return *this; }
inline			complexArray::operator complex		()	const
{ return X[0]; }
inline			complexArray::operator complex*		()	const
{ return X; }
inline			complexArray::operator doubleMatrix	()	const
{ return map(::real); }// multiply
	 complexMatrix	operator *  (const complexArray& a, const complex& b);
	 complexMatrix	operator *  (const complex& a, const complexArray& b);
	 complexMatrix	operator *  (const complexArray& a, const complexArray& b);
// divide
	 complexMatrix	operator /  (const complexArray& a, const complex& b);
	 complexMatrix	operator /  (const complex& a, const complexArray& b);
	 complexMatrix	operator /  (const complexArray& a, const complexArray& b);
// matrix multiplication (inner product)
	 complexMatrix	operator %  (const complexArray& a, const complexArray& b);
// add (plus)
	 complexMatrix	operator +  (const complexArray& a, const complex& b);
	 complexMatrix	operator +  (const complex& a, const complexArray& b);
	 complexMatrix	operator +  (const complexArray& a, const complexArray& b);
// subtract (minus)
	 complexMatrix	operator -  (const complexArray& a, const complex& b);
	 complexMatrix	operator -  (const complex& a, const complexArray& b);
	 complexMatrix	operator -  (const complexArray& a, const complexArray& b);
// shift left
	 complexMatrix	operator << (const complexArray& a, int n);
// output 
	 ostream&	operator << (ostream& s, const complexArray& a);
// shift right
	 complexMatrix	operator >> (const complexArray& a, int n);
// input
	 istream&	operator >> (istream& s, const complexArray& a);

// equal
	 int		operator == (const complexArray& a, const complex& b);
inline	 int		operator == (const complex& a, const complexArray& b)
{ return  (b == a); }
	 int		operator == (const complexArray& a, const complexArray& b);
// not equal
inline	 int		operator != (const complexArray& a, const complex& b)
{ return !(a == b); }
inline	 int		operator != (const complex& a, const complexArray& b)
{ return !(b == a); }
inline	 int		operator != (const complexArray& a, const complexArray& b)
{ return !(a == b); }
// matrix multiplication (outer product)
	 complexMatrix	operator &  (const complexArray& a, const complexArray& b);
// stack  matrices vertically
	 complexMatrix	operator ^  (const complexArray& a, const complexArray& b);
// adjoin matrices horizontally
	 complexMatrix	operator |  (const complexArray& a, const complexArray& b);

// Functions
inline	 complexArray& complexArray::resize(const complexArray& a) {
  L = a.L; M = a.M; N = a.N; X = a.X;
  return *this; }
inline	 complexArray& complexArray::resize(int m, int n, complex* d) {
  L = n; M = m; N = n; X = d;
  return *this; }
inline	 complexArray& complexArray::resize(int n, complex* d) {
  L = n; M = 1; N = n; X = d;
  return *this; }
inline	 int&		complexArray::l(int  i) { L = i; return L; }
inline	 int&		complexArray::m(int  j) { M = j; return M; }
inline	 int&		complexArray::n(int  k) { N = k; return N; }
inline	 complex*&		complexArray::x(complex* u) { X = u; return X; }
inline	 int		complexArray::l() const { return L; }
inline	 int		complexArray::m() const { return M; }
inline	 int		complexArray::n() const { return N; }
inline	 complex*		complexArray::x() const { return X; }
inline	 complexArray	complexArray::s(int i, int m, int j, int n)	const
{ return complexArray(L, m, n, &X[i*L+j]); }
inline	 complexArray	complexArray::s(int i, int m, int j)	const
{ return complexArray(L, m, N-j, &X[i*L+j]); }
typedef	 void (*one_arg_error_handler_t)(const char*);
extern	 void default_complexArray_error_handler(const char*);
extern	 one_arg_error_handler_t complexArray_error_handler;
extern	 one_arg_error_handler_t 
	 set_complexArray_error_handler(one_arg_error_handler_t f);

inline	 complexMatrix&	complexMatrix::operator  = (const complexMatrix& a)
{ complexArray::operator  = (a); return *this; }
inline	 complexMatrix&	complexMatrix::resize(const complexArray& a)
{ delete X;
  L = a.N; M = a.M; N = a.N; X = new complex [M*N]; *this = a;
  return *this; }
inline	 complexMatrix&	complexMatrix::resize(int m, int n, complex* d)
{ delete X;
  L = n; M = m; N = n; X = d;
  return *this; }
inline	 complexMatrix&	complexMatrix::resize(int n, complex* d)
{ delete X;
  L = n; M = 1; N = n; X = d;
  return *this; }
inline	 complexMatrix&	complexMatrix::resize(int m, int n, const complex& a)
{ delete X;
  L = n; M = m; N = n; X = new complex [m*n];
  complex* t = X; while (t < &X[m*n]) *t++ = a;
  return *this; }
inline	 complexMatrix&	complexMatrix::resize(int n, const complex& a)
{ delete X;
  L = n; M = 1; N = n; X = new complex [n];
  complex* t = X; while (t < &X[n]) *t++ = a;
  return *this; }
inline	 complexMatrix&	complexMatrix::resize(int m, int n)
{ delete X;
  L = n; M = m; N = n; X = new complex [m*n];
  return *this; }
inline	 complexMatrix&	complexMatrix::resize(int n)
{ delete X;
  L = n; M = 1; N = n; X = new complex [n];
  return *this; }
//subMatrix
inline	 complexMatrix	complexArray::_(int i, int m, int j, int n)	const
{ return complexMatrix(s(i, m, j, n)); }
inline	 complexMatrix	complexArray::_(int i, int m, int j)	const
{ return complexMatrix(s(i, m, j, N-j)); }

#ifdef __ATT_complex__
inline	 complex	 tan(const complex& a)	{ return sin(a)/cos(a); }
inline	 complex	tanh(const complex& a)	{ return sinh(a)/cosh(a); }
#else
inline	 complex	 tan(      complex  a)	{ return sin(a)/cos(a); }
inline	 complex	tanh(      complex  a)	{ return sinh(a)/cosh(a); }
#endif
inline	 complexMatrix	 cos(const complexArray& a) { return a.map( cos); }
inline	 complexMatrix	 sin(const complexArray& a) { return a.map( sin); }
inline	 complexMatrix	 tan(const complexArray& a) { return a.map( tan); }
inline	 complexMatrix	cosh(const complexArray& a) { return a.map(cosh); }
inline	 complexMatrix	sinh(const complexArray& a) { return a.map(sinh); }
inline	 complexMatrix	tanh(const complexArray& a) { return a.map(tanh); }
inline	 complexMatrix	 exp(const complexArray& a) { return a.map( exp); }
inline	 complexMatrix	 log(const complexArray& a) { return a.map( log); }
inline	 complexMatrix	sqrt(const complexArray& a) { return a.map(sqrt); }

inline	 complexMatrix	conj(const complexArray& a) { return a.map(conj); }
inline	 doubleMatrix	complexArray::real() const  { return map(::real); }
inline	 doubleMatrix	complexArray::imag() const  { return map(::imag); }
inline	 doubleMatrix	real(const complexArray& a) { return a.map(real); }
inline	 doubleMatrix	imag(const complexArray& a) { return a.map(imag); }
inline	 doubleMatrix	 abs(const complexArray& a) { return a.map( abs); }
inline	 doubleMatrix	norm(const complexArray& a) { return a.map(norm); }
inline	 doubleMatrix	 arg(const complexArray& a) { return a.map( arg); }
inline	 complexMatrix	polar(const doubleArray& a)
{ return complexMatrix(a); }
inline	 complexMatrix	polar(const doubleArray& a, const doubleArray& b)
{ return complexMatrix(a, b); }
	 complexMatrix	 fft(const complexArray& a);   // fast fourier transform
	 complexMatrix	 fft(const doubleArray& a);// fast fourier transform
#endif

