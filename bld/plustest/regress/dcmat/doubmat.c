#include "doubMat.h"

doubleMatrix doubleArray::operator - () const {
  double* newX = new double [M*N];
  double* t = newX;
  double* u =    X;
  double* v =    X;
  do {
    while (u < v + N)
      *t++ = -(*u++);
  } while ((u = v += L) < &X[M*L]);
  return doubleMatrix(M, N, newX);
  }

doubleMatrix operator * (const double& a, const doubleArray& b) {
  double* newx = new double [b.m()*b.n()];
  double* t = newx;
  double* u =  b.x();
  double* v =  b.x() + b.n();
  do {
    while (u < v)
      *t++ = a * *u++;
    u += b.l() - b.n();
    } while ((v += b.l()) <= &b.x()[b.m()*b.l()]);
  return doubleMatrix(b.m(), b.n(), newx);
  }

doubleMatrix operator * (const doubleArray& a, const double& b) {
  double* newx = new double [a.m()*a.n()];
  double* t = newx;
  double* u =  a.x();
  double* v =  a.x() + a.n();
  do {
    while (u < v)
      *t++ = *u++ * b;
    u += a.l() - a.n();
    } while ((v += a.l()) <= &a.x()[a.m()*a.l()]);
  return doubleMatrix(a.m(), a.n(), newx);
  }

doubleMatrix operator * (const doubleArray& a, const doubleArray& b) {
  if (a.n() == b.n()) {
    if (a.m() == b.m()) {		// a.n() == b.n()
      double* newx = new double [b.m()*b.n()];
      double* s = newx;
      double* t = newx;
      double* u =  a.x();
      double* v =  b.x();
      while ((t += b.n()) <= &newx[b.m()*b.n()]) {
	while (s < t)
	  *s++ = *u++ * *v++;
	u += a.l() - a.n();
	v += b.l() - b.n();
	};
      return doubleMatrix(b.m(), b.n(), newx);
      };
    if (a.m() == 1) {			// a.n() == b.n() && a.m() != b.m()
      double* newx = new double [b.m()*b.n()];
      double* s = newx;
      double* t = newx;
      double* v =  b.x();
      while ((t += b.n()) <= &newx[b.m()*b.n()]) {
	double* u = a.x();
	while (s < t)
	  *s++ = *u++ * *v++;
	v += b.l() - b.n();
	};
      return doubleMatrix(b.m(), b.n(), newx);
      };
    if (b.m() == 1) {	// a.n() == b.n() && a.m() != b.m() && a.m() != 1
      double* newx = new double [a.m()*a.n()];
      double* s = newx;
      double* t = newx;
      double* u =  a.x();
      while ((t += a.n()) <= &newx[a.m()*a.n()]) {
	double* v = b.x();
	while (s < t)
	  *s++ = *u++ * *v++;
	u += a.l() - a.n();
	};
      return doubleMatrix(a.m(), a.n(), newx);
      };
    };
  if (a.n() == 1) {			// b.n() != a.n()
    if (a.m() == b.m()) {		// b.n() != a.n() == 1
      double* newx = new double [b.m()*b.n()];
      double* s = newx;
      double* t = newx;
      double* u =  a.x();
      double* v =  b.x();						
      while ((t += b.n()) <= &newx[b.m()*b.n()]) {
	while (s < t)
	  *s++ = *u * *v++;
	u++;
	v += b.l() - b.n();
	};
      return doubleMatrix(b.m(), b.n(), newx);
      };
    if (a.m() == 1) {			// b.n() != a.n() == 1 && a.m() != b.m()
      double* newx = new double [b.m()*b.n()];
      double* s = newx;
      double* t = newx;
      double* u =  a.x();
      double* v =  b.x();
      while ((t += b.n()) <= &newx[b.m()*b.n()]) {
	while (s < t)
	  *s++ = *u * *v++;
	v += b.l() - b.n();
	};
      return doubleMatrix(b.m(), b.n(), newx);
      };
    };
  if (b.n() == 1) {			// a.n() != b.n() && a.n() != 1
    if (a.m() == b.m()) {		// a.n() != b.n() == 1
      double* newx = new double [a.m()*a.n()];
      double* s = newx;
      double* t = newx;
      double* u =  a.x();
      double* v =  b.x();
      while ((t += a.n()) <= &newx[a.m()*a.n()]) {
	while (s < t)
	  *s++ = *u++ * *v;
	u += a.l() - a.n();
	v++;
	};
      return doubleMatrix(a.m(), a.n(), newx);
      };
    if (b.m() == 1) {			// a.n() != b.n() == 1 && a.m() != b.m()
      double* newx = new double [a.m()*a.n()];
      double* s = newx;
      double* t = newx;
      double* u =  a.x();
      double* v =  b.x();
      while ((t += a.n()) <= &newx[a.m()*a.n()]) {
	while (s < t)
	  *s++ = *u++ * *v;
	u += a.l() - a.n();
	};
      return doubleMatrix(a.m(), a.n(), newx);
      };
    };
  a.error("nonconformant doubleArray * operands.");
  return doubleMatrix();
  }

doubleMatrix operator / (const double& a, const doubleArray& b) {
  double* newx = new double [b.m()*b.n()];
  double* t = newx;
  double* u =  b.x();
  double* v =  b.x() + b.n();
  do {
    while (u < v)
      *t++ = a / *u++;
    u += b.l() - b.n();
    } while ((v += b.l()) <= &b.x()[b.m()*b.l()]);
  return doubleMatrix(b.m(), b.n(), newx);
  }

doubleMatrix operator / (const doubleArray& a, const double& b) {
  double* newx = new double [a.m()*a.n()];
  double* t = newx;
  double* u =  a.x();
  double* v =  a.x() + a.n();
  do {
    while (u < v)
      *t++ = *u++ / b;
    u += a.l() - a.n();
    } while ((v += a.l()) <= &a.x()[a.m()*a.l()]);
  return doubleMatrix(a.m(), a.n(), newx);
  }

doubleMatrix operator / (const doubleArray& a, const doubleArray& b) {
  if (a.n() == b.n()) {
    if (a.m() == b.m()) {		// a.n() == b.n()
      double* newx = new double [b.m()*b.n()];
      double* s = newx;
      double* t = newx;
      double* u =  a.x();
      double* v =  b.x();
      while ((t += b.n()) <= &newx[b.m()*b.n()]) {
	while (s < t)
	  *s++ = *u++ / *v++;
	u += a.l() - a.n();
	v += b.l() - b.n();
	};
      return doubleMatrix(b.m(), b.n(), newx);
      };
    if (a.m() == 1) {			// a.n() == b.n() && a.m() != b.m()
      double* newx = new double [b.m()*b.n()];
      double* s = newx;
      double* t = newx;
      double* v =  b.x();
      while ((t += b.n()) <= &newx[b.m()*b.n()]) {
	double* u = a.x();
	while (s < t)
	  *s++ = *u++ / *v++;
	v += b.l() - b.n();
	};
      return doubleMatrix(b.m(), b.n(), newx);
      };
    if (b.m() == 1) {	// a.n() == b.n() && a.m() != b.m() && a.m() != 1
      double* newx = new double [a.m()*a.n()];
      double* s = newx;
      double* t = newx;
      double* u =  a.x();
      while ((t += a.n()) <= &newx[a.m()*a.n()]) {
	double* v = b.x();
	while (s < t)
	  *s++ = *u++ / *v++;
	u += a.l() - a.n();
	};
      return doubleMatrix(a.m(), a.n(), newx);
      };
    };
  if (a.n() == 1) {			// b.n() != a.n()
    if (a.m() == b.m()) {		// b.n() != a.n() == 1
      double* newx = new double [b.m()*b.n()];
      double* s = newx;
      double* t = newx;
      double* u =  a.x();
      double* v =  b.x();						
      while ((t += b.n()) <= &newx[b.m()*b.n()]) {
	while (s < t)
	  *s++ = *u / *v++;
	u++;
	v += b.l() - b.n();
	};
      return doubleMatrix(b.m(), b.n(), newx);
      };
    if (a.m() == 1) {			// b.n() != a.n() == 1 && a.m() != b.m()
      double* newx = new double [b.m()*b.n()];
      double* s = newx;
      double* t = newx;
      double* u =  a.x();
      double* v =  b.x();
      while ((t += b.n()) <= &newx[b.m()*b.n()]) {
	while (s < t)
	  *s++ = *u / *v++;
	v += b.l() - b.n();
	};
      return doubleMatrix(b.m(), b.n(), newx);
      };
    };
  if (b.n() == 1) {			// a.n() != b.n() && a.n() != 1
    if (a.m() == b.m()) {		// a.n() != b.n() == 1
      double* newx = new double [a.m()*a.n()];
      double* s = newx;
      double* t = newx;
      double* u =  a.x();
      double* v =  b.x();
      while ((t += a.n()) <= &newx[a.m()*a.n()]) {
	while (s < t)
	  *s++ = *u++ / *v;
	u += a.l() - a.n();
	v++;
	};
      return doubleMatrix(a.m(), a.n(), newx);
      };
    if (b.m() == 1) {			// a.n() != b.n() == 1 && a.m() != b.m()
      double* newx = new double [a.m()*a.n()];
      double* s = newx;
      double* t = newx;
      double* u =  a.x();
      double* v =  b.x();
      while ((t += a.n()) <= &newx[a.m()*a.n()]) {
	while (s < t)
	  *s++ = *u++ / *v;
	u += a.l() - a.n();
	};
      return doubleMatrix(a.m(), a.n(), newx);
      };
    };
  a.error("nonconformant doubleArray / operands.");
  return doubleMatrix();
  }

doubleMatrix operator % (const doubleArray& a, const doubleArray& b) {	// inner product
  if (a.n() != b.n())
    a.error("nonconformant doubleArray % operands.");
  double* newx = new double [a.m()*b.m()];
  double* s = newx;
  double* t =  a.x();
  double* u;
  double* v;
  do {
    v = b.x();
    while (v < &b.x()[b.m()*b.l()]) {
      u = t; *s = *u++ * *v++;
      while (u < t + a.n())
	*s += *u++ * *v++;
      ++s;
      v += b.l() - b.n();
      };
    } while((t += a.l()) < &a.x()[a.m()*a.l()]);
  return doubleMatrix(a.m(), b.m(), newx);
  }

doubleMatrix operator + (const double& a, const doubleArray& b) {
  double* newx = new double [b.m()*b.n()];
  double* t = newx;
  double* u =  b.x();
  double* v =  b.x() + b.n();
  do {
    while (u < v)
      *t++ = a + *u++;
    u += b.l() - b.n();
    } while ((v += b.l()) <= &b.x()[b.m()*b.l()]);
  return doubleMatrix(b.m(), b.n(), newx);
  }

doubleMatrix operator + (const doubleArray& a, const double& b) {
  double* newx = new double [a.m()*a.n()];
  double* t = newx;
  double* u =  a.x();
  double* v =  a.x() + a.n();
  do {
    while (u < v)
      *t++ = *u++ + b;
    u += a.l() - a.n();
    } while ((v += a.l()) <= &a.x()[a.m()*a.l()]);
  return doubleMatrix(a.m(), a.n(), newx);
  }

doubleMatrix operator + (const doubleArray& a, const doubleArray& b) {
  if (a.n() == b.n()) {
    if (a.m() == b.m()) {		// a.n() == b.n()
      double* newx = new double [b.m()*b.n()];
      double* s = newx;
      double* t = newx;
      double* u =  a.x();
      double* v =  b.x();
      while ((t += b.n()) <= &newx[b.m()*b.n()]) {
	while (s < t)
	  *s++ = *u++ + *v++;
	u += a.l() - a.n();
	v += b.l() - b.n();
	};
      return doubleMatrix(b.m(), b.n(), newx);
      };
    if (a.m() == 1) {			// a.n() == b.n() && a.m() != b.m()
      double* newx = new double [b.m()*b.n()];
      double* s = newx;
      double* t = newx;
      double* v =  b.x();
      while ((t += b.n()) <= &newx[b.m()*b.n()]) {
	double* u = a.x();
	while (s < t)
	  *s++ = *u++ + *v++;
	v += b.l() - b.n();
	};
      return doubleMatrix(b.m(), b.n(), newx);
      };
    if (b.m() == 1) {	// a.n() == b.n() && a.m() != b.m() && a.m() != 1
      double* newx = new double [a.m()*a.n()];
      double* s = newx;
      double* t = newx;
      double* u =  a.x();
      while ((t += a.n()) <= &newx[a.m()*a.n()]) {
	double* v = b.x();
	while (s < t)
	  *s++ = *u++ + *v++;
	u += a.l() - a.n();
	};
      return doubleMatrix(a.m(), a.n(), newx);
      };
    };
  if (a.n() == 1) {			// b.n() != a.n()
    if (a.m() == b.m()) {		// b.n() != a.n() == 1
      double* newx = new double [b.m()*b.n()];
      double* s = newx;
      double* t = newx;
      double* u =  a.x();
      double* v =  b.x();						
      while ((t += b.n()) <= &newx[b.m()*b.n()]) {
	while (s < t)
	  *s++ = *u + *v++;
	u++;
	v += b.l() - b.n();
	};
      return doubleMatrix(b.m(), b.n(), newx);
      };
    if (a.m() == 1) {			// b.n() != a.n() == 1 && a.m() != b.m()
      double* newx = new double [b.m()*b.n()];
      double* s = newx;
      double* t = newx;
      double* u =  a.x();
      double* v =  b.x();
      while ((t += b.n()) <= &newx[b.m()*b.n()]) {
	while (s < t)
	  *s++ = *u + *v++;
	v += b.l() - b.n();
	};
      return doubleMatrix(b.m(), b.n(), newx);
      };
    };
  if (b.n() == 1) {			// a.n() != b.n() && a.n() != 1
    if (a.m() == b.m()) {		// a.n() != b.n() == 1
      double* newx = new double [a.m()*a.n()];
      double* s = newx;
      double* t = newx;
      double* u =  a.x();
      double* v =  b.x();
      while ((t += a.n()) <= &newx[a.m()*a.n()]) {
	while (s < t)
	  *s++ = *u++ + *v;
	u += a.l() - a.n();
	v++;
	};
      return doubleMatrix(a.m(), a.n(), newx);
      };
    if (b.m() == 1) {			// a.n() != b.n() == 1 && a.m() != b.m()
      double* newx = new double [a.m()*a.n()];
      double* s = newx;
      double* t = newx;
      double* u =  a.x();
      double* v =  b.x();
      while ((t += a.n()) <= &newx[a.m()*a.n()]) {
	while (s < t)
	  *s++ = *u++ + *v;
	u += a.l() - a.n();
	};
      return doubleMatrix(a.m(), a.n(), newx);
      };
    };
  a.error("nonconformant doubleArray + operands.");
  return doubleMatrix();
  }

doubleMatrix operator - (const double& a, const doubleArray& b) {
  double* newx = new double [b.m()*b.n()];
  double* t = newx;
  double* u =  b.x();
  double* v =  b.x() + b.n();
  do {
    while (u < v)
      *t++ = a - *u++;
    u += b.l() - b.n();
    } while ((v += b.l()) <= &b.x()[b.m()*b.l()]);
  return doubleMatrix(b.m(), b.n(), newx);
  }

doubleMatrix operator - (const doubleArray& a, const double& b) {
  double* newx = new double [a.m()*a.n()];
  double* t = newx;
  double* u =  a.x();
  double* v =  a.x() + a.n();
  do {
    while (u < v)
      *t++ = *u++ - b;
    u += a.l() - a.n();
    } while ((v += a.l()) <= &a.x()[a.m()*a.l()]);
  return doubleMatrix(a.m(), a.n(), newx);
  }

doubleMatrix operator - (const doubleArray& a, const doubleArray& b) {
  if (a.n() == b.n()) {
    if (a.m() == b.m()) {		// a.n() == b.n()
      double* newx = new double [b.m()*b.n()];
      double* s = newx;
      double* t = newx;
      double* u =  a.x();
      double* v =  b.x();
      while ((t += b.n()) <= &newx[b.m()*b.n()]) {
	while (s < t)
	  *s++ = *u++ - *v++;
	u += a.l() - a.n();
	v += b.l() - b.n();
	};
      return doubleMatrix(b.m(), b.n(), newx);
      };
    if (a.m() == 1) {			// a.n() == b.n() && a.m() != b.m()
      double* newx = new double [b.m()*b.n()];
      double* s = newx;
      double* t = newx;
      double* v =  b.x();
      while ((t += b.n()) <= &newx[b.m()*b.n()]) {
	double* u = a.x();
	while (s < t)
	  *s++ = *u++ - *v++;
	v += b.l() - b.n();
	};
      return doubleMatrix(b.m(), b.n(), newx);
      };
    if (b.m() == 1) {	// a.n() == b.n() && a.m() != b.m() && a.m() != 1
      double* newx = new double [a.m()*a.n()];
      double* s = newx;
      double* t = newx;
      double* u =  a.x();
      while ((t += a.n()) <= &newx[a.m()*a.n()]) {
	double* v = b.x();
	while (s < t)
	  *s++ = *u++ - *v++;
	u += a.l() - a.n();
	};
      return doubleMatrix(a.m(), a.n(), newx);
      };
    };
  if (a.n() == 1) {			// b.n() != a.n()
    if (a.m() == b.m()) {		// b.n() != a.n() == 1
      double* newx = new double [b.m()*b.n()];
      double* s = newx;
      double* t = newx;
      double* u =  a.x();
      double* v =  b.x();						
      while ((t += b.n()) <= &newx[b.m()*b.n()]) {
	while (s < t)
	  *s++ = *u - *v++;
	u++;
	v += b.l() - b.n();
	};
      return doubleMatrix(b.m(), b.n(), newx);
      };
    if (a.m() == 1) {			// b.n() != a.n() == 1 && a.m() != b.m()
      double* newx = new double [b.m()*b.n()];
      double* s = newx;
      double* t = newx;
      double* u =  a.x();
      double* v =  b.x();
      while ((t += b.n()) <= &newx[b.m()*b.n()]) {
	while (s < t)
	  *s++ = *u - *v++;
	v += b.l() - b.n();
	};
      return doubleMatrix(b.m(), b.n(), newx);
      };
    };
  if (b.n() == 1) {			// a.n() != b.n() && a.n() != 1
    if (a.m() == b.m()) {		// a.n() != b.n() == 1
      double* newx = new double [a.m()*a.n()];
      double* s = newx;
      double* t = newx;
      double* u =  a.x();
      double* v =  b.x();
      while ((t += a.n()) <= &newx[a.m()*a.n()]) {
	while (s < t)
	  *s++ = *u++ - *v;
	u += a.l() - a.n();
	v++;
	};
      return doubleMatrix(a.m(), a.n(), newx);
      };
    if (b.m() == 1) {			// a.n() != b.n() == 1 && a.m() != b.m()
      double* newx = new double [a.m()*a.n()];
      double* s = newx;
      double* t = newx;
      double* u =  a.x();
      double* v =  b.x();
      while ((t += a.n()) <= &newx[a.m()*a.n()]) {
	while (s < t)
	  *s++ = *u++ - *v;
	u += a.l() - a.n();
	};
      return doubleMatrix(a.m(), a.n(), newx);
      };
    };
  a.error("nonconformant doubleArray - operands.");
  return doubleMatrix();
  }


int	Array_number	 = 4;
char*	Array_format	 = "%g";
char*	Array_space	 = " ";
char	empty[1]	 = "";

char* format(char* s,int n,char* w) {
  Array_number	= n;
  Array_format	= s;
  Array_space	= w;
  return empty;
  }
ostream& operator << (ostream& s, const doubleArray& b) {
  double* t = b.x();
  double* u = b.x();
  double* v = b.x() + b.n();
  do {
    do {
      t += (0 < Array_number)? Array_number: b.n();
      t  = (t < v)? t: v;
      while (u < t)
	if (!(s << form(Array_format, *u)
	      && ((++u < t) ? s << Array_space : s << "\n")))
	  return s;
      } while (t < v);
    t = u += b.l() - b.n();
    } while ((v += b.l()) <= &b.x()[b.m()*b.l()]);
  return s;
  }

istream& operator >> (istream& s, const doubleArray& b) {
  double* u = b.x();
  double* v = b.x() + b.n();
  do {
    while (u < v && s >> *u)
      u++;
    if (u < v)
      return s;
    u += b.l() - b.n();
    } while ((v += b.l()) <= &b.x()[b.m()*b.l()]);
  return s;
  }

doubleMatrix operator << (const doubleArray& a, int n) {
  double* newx = new double [a.m()*a.n()];
  double* t =  a.x() + n;
  double* u = newx;
  double* v = newx;
  while ((v += a.n()) <= &newx[a.m()*a.n()]) {
    while (u < v - n)
      *u++ = *t++;
    while (u < v)
      *u++ = 0.0;
    t += a.l() - a.n() + n;
    };
  return doubleMatrix(a.m(), a.n(), newx);
  }

doubleMatrix operator >> (const doubleArray& a, int n) {
  double* newx = new double [a.m()*a.n()];
  double* t =  &a.x()[a.m()*a.l()];
  double* u = &newx[a.m()*a.n()];
  double* v = &newx[a.m()*a.n()];
  while (newx <= (u -= a.n())) {
    t -= a.l() - a.n() + n;
    while (v > u + n)
      *--v = *--t;
    while (v > u)
      *--v = *t;
    };
  return doubleMatrix(a.m(), a.n(), newx);
  }

int operator <  (const double& a, const doubleArray& b) {
  double* u = b.x();
  double* v = b.x() + b.n();
  do {
    while (u < v && a <  *u)
      u++;
    if (u < v)
      return 0;
    u += b.l() - b.n();
    } while ((v += b.l()) <= &b.x()[b.m()*b.l()]);
  return 1;
  }

int operator <  (const doubleArray& a, const double& b) {
  double* u = a.x();
  double* v = a.x() + a.n();
  do {
    while (u < v && *u <  b)
      u++;
    if (u < v)
      return 0;
    u += a.l() - a.n();
    } while ((v += a.l()) <= &a.x()[a.m()*a.l()]);
  return 1;
  }

int operator <  (const doubleArray& a, const doubleArray& b) {
  if (a.n() == b.n()) {
    if (a.m() == b.m()) {		// a.n() == b.n()
      double* t = a.x();
      double* u = b.x();
      double* v = b.x() + b.n();
      do {
	while (u < v && *t <  *u) {
	  t++;
	  u++;
	  };
	if (u < v)
	  return 0;
	t += a.l() - a.n();
	u += b.l() - b.n();
	} while ((v += b.l()) <= &b.x()[b.m()*b.l()]);
      return 1;
      };
    if (a.m() == 1) {			// a.n() == b.n() && a.m() != b.m()
      double* u = b.x();
      double* v = b.x() + b.n();
      do {
	double* t = a.x();
	while (u < v && *t <  *u) {
	  t++;
	  u++;
	  };
	if (u < v)
	  return 0;
	u += b.l() - b.n();
	} while ((v += b.l()) <= &b.x()[b.m()*b.l()]);
      return 1;
      };
    if (b.m() == 1) {			// a.n() == b.n() && a.m() != b.m() && a.m() != 1
      double* u = a.x();
      double* v = a.x() + a.n();
      do {
	double* t = b.x();
	while (u < v && *u <  *t) {
	  t++;
	  u++;
	  };
	if (u < v)
	  return 0;
	t += b.l() - b.n();
	u += a.l() - a.n();
	} while ((v += a.l()) <= &a.x()[a.m()*a.l()]);
      return 1;
      };
    };
  if (a.n() == 1) {			// b.n() != a.n()
    if (a.m() == b.m()) {		// b.n() != a.n() == 1
      double* t = a.x();
      double* u = b.x();
      double* v = b.x() + b.n();						
      do {
	while (u < v && *t <  *u)
	  u++;
	if (u < v)
	  return 0;
	t++;
	u += b.l() - b.n();
	} while ((v += b.l()) <= &b.x()[b.m()*b.l()]);
      return 1;
      };
    if (a.m() == 1) {			// b.n() != a.n() == 1 && a.m() != b.m()
      double* u = b.x();
      double* v = b.x() + b.n();
      do {
	while (u < v && *a.x() <  *u)
	  u++;
	if (u < v)
	  return 0;
	u += b.l() - b.n();
	} while ((v += b.l()) <= &b.x()[b.m()*b.l()]);
      return 1;
      };
    };
  if (b.n() == 1) {			// a.n() != b.n() && a.n() != 1
    if (a.m() == b.m()) {		// a.n() != b.n() == 1
      double* t = b.x();
      double* u = a.x();
      double* v = a.x() + a.n();
      do {
	while (u < v && *u <  *t)
	  u++;
	if (u < v)
	  return 0;
	t++;
	u += a.l() - a.n();
	} while ((v += a.l()) <= &a.x()[a.m()*a.l()]);
      return 1;
      };
    if (b.m() == 1) {			// a.n() != b.n() == 1 && a.m() != b.m()
      double* u = a.x();
      double* v = a.x() + a.n();
      do {
	while (u < v && *u <  *b.x())
	  u++;
	if (u < v)
	  return 0;
	u += a.l() - a.n();
	} while ((v += a.l()) <= &a.x()[a.m()*a.l()]);
      return 1;
      };
    };
  a.error("nonconformant doubleArray <  operands.");
  return 0;
  }

int operator == (const doubleArray& a, const double& b) {
  double* u = a.x();
  double* v = a.x() + a.n();
  do {
    while (u < v && *u == b)
      u++;
    if (u < v)
      return 0;
    u += a.l() - a.n();
    } while ((v += a.l()) <= &a.x()[a.m()*a.l()]);
  return 1;
  }

int operator == (const doubleArray& a, const doubleArray& b) {
  if (a.n() == b.n()) {
    if (a.m() == b.m()) {		// a.n() == b.n()
      double* t = a.x();
      double* u = b.x();
      double* v = b.x() + b.n();
      do {
	while (u < v && *t == *u) {
	  t++;
	  u++;
	  };
	if (u < v)
	  return 0;
	t += a.l() - a.n();
	u += b.l() - b.n();
	} while ((v += b.l()) <= &b.x()[b.m()*b.l()]);
      return 1;
      };
    if (a.m() == 1) {			// a.n() == b.n() && a.m() != b.m()
      double* u = b.x();
      double* v = b.x() + b.n();
      do {
	double* t = a.x();
	while (u < v && *t == *u) {
	  t++;
	  u++;
	  };
	if (u < v)
	  return 0;
	u += b.l() - b.n();
	} while ((v += b.l()) <= &b.x()[b.m()*b.l()]);
      return 1;
      };
    if (b.m() == 1) {			// a.n() == b.n() && a.m() != b.m() && a.m() != 1
      double* u = a.x();
      double* v = a.x() + a.n();
      do {
	double* t = b.x();
	while (u < v && *u == *t) {
	  t++;
	  u++;
	  };
	if (u < v)
	  return 0;
	t += b.l() - b.n();
	u += a.l() - a.n();
	} while ((v += a.l()) <= &a.x()[a.m()*a.l()]);
      return 1;
      };
    };
  if (a.n() == 1) {			// b.n() != a.n()
    if (a.m() == b.m()) {		// b.n() != a.n() == 1
      double* t = a.x();
      double* u = b.x();
      double* v = b.x() + b.n();						
      do {
	while (u < v && *t == *u)
	  u++;
	if (u < v)
	  return 0;
	t++;
	u += b.l() - b.n();
	} while ((v += b.l()) <= &b.x()[b.m()*b.l()]);
      return 1;
      };
    if (a.m() == 1) {			// b.n() != a.n() == 1 && a.m() != b.m()
      double* u = b.x();
      double* v = b.x() + b.n();
      do {
	while (u < v && *a.x() == *u)
	  u++;
	if (u < v)
	  return 0;
	u += b.l() - b.n();
	} while ((v += b.l()) <= &b.x()[b.m()*b.l()]);
      return 1;
      };
    };
  if (b.n() == 1) {			// a.n() != b.n() && a.n() != 1
    if (a.m() == b.m()) {		// a.n() != b.n() == 1
      double* t = b.x();
      double* u = a.x();
      double* v = a.x() + a.n();
      do {
	while (u < v && *u == *t)
	  u++;
	if (u < v)
	  return 0;
	t++;
	u += a.l() - a.n();
	} while ((v += a.l()) <= &a.x()[a.m()*a.l()]);
      return 1;
      };
    if (b.m() == 1) {			// a.n() != b.n() == 1 && a.m() != b.m()
      double* u = a.x();
      double* v = a.x() + a.n();
      do {
	while (u < v && *u == *b.x())
	  u++;
	if (u < v)
	  return 0;
	u += a.l() - a.n();
	} while ((v += a.l()) <= &a.x()[a.m()*a.l()]);
      return 1;
      };
    };
  a.error("nonconformant doubleArray == operands.");
  return 0;
  }

doubleMatrix operator & (const doubleArray& a, const doubleArray& b) {	// outer product
  if (a.m() != b.m())
    a.error("nonconformant doubleArray & operands.");
  double* newx = new double [a.n()*b.n()];
  double* p = newx;
  double* s =  a.x();
  double* t;
  double* u;
  double* v;
  do {
    t = b.x();
    do {
      u = s; v = t; *p = *u * *v;
      while ((u += a.l()), (v += b.l()) < &b.x()[b.m()*b.l()])
	*p += *u * *v;
      p++;
      } while (++t < &b.x()[b.n()]);
    } while (++s < &a.x()[a.n()]);
  return doubleMatrix(a.n(), b.n(), newx);
  }

doubleMatrix operator ^ (const doubleArray& a, const doubleArray& b) {	// stack
  if (a.n() != b.n())
    a.error("nonconformant doubleArray ^ operands.");
  double* newx = new double [(a.m()+b.m())*a.n()];
  double* t = newx;
  double* u =  a.x();
  double* v =  a.x() + a.n();
  do {
    while (u < v)
      *t++ = *u++;
    u += a.l() - a.n();
    } while((v += a.l()) <= &a.x()[a.m()*a.l()]);
  u =  b.x();
  v =  b.x() + b.n();
  do {
    while (u < v)
      *t++ = *u++;
    u += b.l() - b.n();
    } while((v += b.l()) <= &b.x()[b.m()*b.l()]);
  return doubleMatrix(a.m()+b.m(), a.n(), newx);
  }

doubleMatrix operator | (const doubleArray& a, const doubleArray& b) {	// adjoin
  if (a.m() != b.m())
    a.error("nonconformant doubleArray | operands.");
  double* newx = new double [a.m()*(a.n()+b.n())];
  double* t = newx;
  double* u =  a.x();
  double* v =  a.x() + a.n();
  do {
    while (u < v)
      *t++ = *u++;
    t += b.n();
    u += a.l() - a.n();
    } while((v += a.l()) <= &a.x()[a.m()*a.l()]);
  t = newx + a.n();
  u =  b.x();
  v =  b.x() + b.n();
  do {
    while (u < v)
      *t++ = *u++;
    u += b.l() - b.n();
    t += a.n();
    } while((v += b.l()) <= &b.x()[b.m()*b.l()]);
  return doubleMatrix(a.m(), a.n()+b.n(), newx);
  }

doubleArray& doubleArray::operator  = (const double& b) {
  double* s = X;
  double* t = X + N;
  do {
    while (s < t)
      *s++  = b;
    s += L - N;
    } while ((t += L) <= &X[M*L]);
  return *this;
  }

doubleArray& doubleArray::operator  = (const doubleArray& a) {
  if (N == a.N) {
    if (M == a.M) {			// N == a.N
      double* t = a.X;
      double* u =   X;
      double* v =   X + N;
      do {
	while (u < v)
	  *u++  = *t++;
	t += a.L - a.N;
	u +=   L -   N;
	} while ((v += L) <= &X[M*L]);
      return *this;
      };
    if (a.M == 1) {			// N == a.N && M != a.M
      double* t;
      double* u = X;
      double* v = X + N;
      do {
	t = a.X;
	while (u < v)
	  *u++  = *t++;
	u += L - N;
	} while ((v += L) <= &X[M*L]);
      return *this;
      };
    };
  if (a.N == 1) {			// N != a.N
    if (M == a.M) {			// N != a.N == 1
      double* t = a.X;
      double* u =   X;
      double* v =   X + N;
      do {
	while (u < v)
	  *u++  = *t;
	t++;
	u += L - N;
	} while ((v += L) <= &X[M*L]);
      return *this;
      };
    if (a.M == 1) {			// N != a.N == 1 && M != a.M
      double* t = a.X;
      double* u =   X;
      double* v =   X + N;
      do {
	while (u < v)
	  *u++  = *t;
	u += L - N;
	} while ((v += L) <= &X[M*L]);
      return *this;
      };
    };
  error("nonconformant doubleArray  = operands.");
  return *this;
  }

doubleArray& doubleArray::operator *= (const double& b) {
  double* s = X;
  double* t = X + N;
  do {
    while (s < t)
      *s++ *= b;
    s += L - N;
    } while ((t += L) <= &X[M*L]);
  return *this;
  }

doubleArray& doubleArray::operator *= (const doubleArray& a) {
  if (N == a.N) {
    if (M == a.M) {			// N == a.N
      double* t = a.X;
      double* u =   X;
      double* v =   X + N;
      do {
	while (u < v)
	  *u++ *= *t++;
	t += a.L - a.N;
	u +=   L -   N;
	} while ((v += L) <= &X[M*L]);
      return *this;
      };
    if (a.M == 1) {			// N == a.N && M != a.M
      double* t;
      double* u = X;
      double* v = X + N;
      do {
	t = a.X;
	while (u < v)
	  *u++ *= *t++;
	u += L - N;
	} while ((v += L) <= &X[M*L]);
      return *this;
      };
    };
  if (a.N == 1) {			// N != a.N
    if (M == a.M) {			// N != a.N == 1
      double* t = a.X;
      double* u =   X;
      double* v =   X + N;
      do {
	while (u < v)
	  *u++ *= *t;
	t++;
	u += L - N;
	} while ((v += L) <= &X[M*L]);
      return *this;
      };
    if (a.M == 1) {			// N != a.N == 1 && M != a.M
      double* t = a.X;
      double* u =   X;
      double* v =   X + N;
      do {
	while (u < v)
	  *u++ *= *t;
	u += L - N;
	} while ((v += L) <= &X[M*L]);
      return *this;
      };
    };
  error("nonconformant doubleArray *= operands.");
  return *this;
  }

doubleArray& doubleArray::operator /= (const double& b) {
  double* s = X;
  double* t = X + N;
  do {
    while (s < t)
      *s++ /= b;
    s += L - N;
    } while ((t += L) <= &X[M*L]);
  return *this;
  }

doubleArray& doubleArray::operator /= (const doubleArray& a) {
  if (N == a.N) {
    if (M == a.M) {			// N == a.N
      double* t = a.X;
      double* u =   X;
      double* v =   X + N;
      do {
	while (u < v)
	  *u++ /= *t++;
	t += a.L - a.N;
	u +=   L -   N;
	} while ((v += L) <= &X[M*L]);
      return *this;
      };
    if (a.M == 1) {			// N == a.N && M != a.M
      double* t;
      double* u = X;
      double* v = X + N;
      do {
	t = a.X;
	while (u < v)
	  *u++ /= *t++;
	u += L - N;
	} while ((v += L) <= &X[M*L]);
      return *this;
      };
    };
  if (a.N == 1) {			// N != a.N
    if (M == a.M) {			// N != a.N == 1
      double* t = a.X;
      double* u =   X;
      double* v =   X + N;
      do {
	while (u < v)
	  *u++ /= *t;
	t++;
	u += L - N;
	} while ((v += L) <= &X[M*L]);
      return *this;
      };
    if (a.M == 1) {			// N != a.N == 1 && M != a.M
      double* t = a.X;
      double* u =   X;
      double* v =   X + N;
      do {
	while (u < v)
	  *u++ /= *t;
	u += L - N;
	} while ((v += L) <= &X[M*L]);
      return *this;
      };
    };
  error("nonconformant doubleArray /= operands.");
  return *this;
  }

doubleArray& doubleArray::operator += (const double& b) {
  double* s = X;
  double* t = X + N;
  do {
    while (s < t)
      *s++ += b;
    s += L - N;
    } while ((t += L) <= &X[M*L]);
  return *this;
  }

doubleArray& doubleArray::operator += (const doubleArray& a) {
  if (N == a.N) {
    if (M == a.M) {			// N == a.N
      double* t = a.X;
      double* u =   X;
      double* v =   X + N;
      do {
	while (u < v)
	  *u++ += *t++;
	t += a.L - a.N;
	u +=   L -   N;
	} while ((v += L) <= &X[M*L]);
      return *this;
      };
    if (a.M == 1) {			// N == a.N && M != a.M
      double* t;
      double* u = X;
      double* v = X + N;
      do {
	t = a.X;
	while (u < v)
	  *u++ += *t++;
	u += L - N;
	} while ((v += L) <= &X[M*L]);
      return *this;
      };
    };
  if (a.N == 1) {			// N != a.N
    if (M == a.M) {			// N != a.N == 1
      double* t = a.X;
      double* u =   X;
      double* v =   X + N;
      do {
	while (u < v)
	  *u++ += *t;
	t++;
	u += L - N;
	} while ((v += L) <= &X[M*L]);
      return *this;
      };
    if (a.M == 1) {			// N != a.N == 1 && M != a.M
      double* t = a.X;
      double* u =   X;
      double* v =   X + N;
      do {
	while (u < v)
	  *u++ += *t;
	u += L - N;
	} while ((v += L) <= &X[M*L]);
      return *this;
      };
    };
  error("nonconformant doubleArray += operands.");
  return *this;
  }

doubleArray& doubleArray::operator -= (const double& b) {
  double* s = X;
  double* t = X + N;
  do {
    while (s < t)
      *s++ -= b;
    s += L - N;
    } while ((t += L) <= &X[M*L]);
  return *this;
  }

doubleArray& doubleArray::operator -= (const doubleArray& a) {
  if (N == a.N) {
    if (M == a.M) {			// N == a.N
      double* t = a.X;
      double* u =   X;
      double* v =   X + N;
      do {
	while (u < v)
	  *u++ -= *t++;
	t += a.L - a.N;
	u +=   L -   N;
	} while ((v += L) <= &X[M*L]);
      return *this;
      };
    if (a.M == 1) {			// N == a.N && M != a.M
      double* t;
      double* u = X;
      double* v = X + N;
      do {
	t = a.X;
	while (u < v)
	  *u++ -= *t++;
	u += L - N;
	} while ((v += L) <= &X[M*L]);
      return *this;
      };
    };
  if (a.N == 1) {			// N != a.N
    if (M == a.M) {			// N != a.N == 1
      double* t = a.X;
      double* u =   X;
      double* v =   X + N;
      do {
	while (u < v)
	  *u++ -= *t;
	t++;
	u += L - N;
	} while ((v += L) <= &X[M*L]);
      return *this;
      };
    if (a.M == 1) {			// N != a.N == 1 && M != a.M
      double* t = a.X;
      double* u =   X;
      double* v =   X + N;
      do {
	while (u < v)
	  *u++ -= *t;
	u += L - N;
	} while ((v += L) <= &X[M*L]);
      return *this;
      };
    };
  error("nonconformant doubleArray -= operands.");
  return *this;
  }

doubleArray& doubleArray::operator >>= (int n) {
  double* t = &X[M*L];
  double* u = &X[M*L];
  double* v = &X[M*L];
  while (X <= (u -= L)) {
    t -= L - N + n;
    v -= L - N;
    while (t > u)
      *--v = *--t;
    while (v > u)
      *--v = *t;
    };
  return *this;
  }

doubleArray& doubleArray::operator <<= (int n) {
  double* t = X + n;
  double* u = X;
  double* v = X + N;
  do {
    while (t < v)
      *u++ = *t++;
    while (u < v)
      *u++ = 0.0;
    t += L - N + n;
    u += L - N;
    } while ((v += L) <= &X[M*L]);
  return *this;
  }

// Error Handling
void default_doubleArray_error_handler(const char* msg) {
  cerr << "Fatal doubleArray error. " << msg << "\n";
  exit(1);
  return;
  }

one_arg_error_handler_t
	doubleArray_error_handler = default_doubleArray_error_handler;

one_arg_error_handler_t set_doubleArray_error_handler(one_arg_error_handler_t f) {
  one_arg_error_handler_t old = doubleArray_error_handler;
  doubleArray_error_handler = f;
  return old;
  }

void doubleArray::error(const char* msg) const {
  (*doubleArray_error_handler)(msg);
  }


doubleMatrix doubleArray::i(double epsilon) const {	// inverse
  double*  a = new double [N*M];
  double* uu = new double [M*N];
  double* vv = new double [N*N];
  double*  w = new double [N];
  double** u = new double* [M];
  double** v = new double* [N];
  void svdcmp(double**, int, int, double*, double**);
  for (int i = 0; i < M; i++)
    u[i] = &(uu[N*i]);
  for (int j = 0; j < N; j++)
    v[j] = &(vv[N*j]);
  for (i = 0; i < M; i++)
    for (j = 0; j < N; j++)
      u[i][j] = X[i*L+j];
  svdcmp(u, M, N, w, v);	// Singular value decomposition.
  double wmax = 0.0;		// Maximum singular value.
  for (j = 0; j < N; j++)
    if (w[j] > wmax)
      wmax = w[j];
  double wmin = wmax*epsilon;
  for (int k = 0; k < N; k++)
    if (w[k] < wmin)
      w[k] = 0.0;
    else
      w[k] = 1.0/w[k];
  for (i = 0; i < N; i++)
    for (j = 0; j < M; j++) {
      a[M*i+j] = 0.0;
      for (k = 0; k < N; k++)
	a[M*i+j] += v[i][k]*w[k]*u[j][k];
      };

  delete w;
  delete u;
  delete v;
  delete uu;
  delete vv;

  return doubleMatrix(N, M, a);
  }

doubleMatrix doubleArray::t() const {		// transpose
  double* newX = new double [N*M];
  double* t = newX;
  double* u;
  double* v =    X;
  do {
    u = v;
    do
      *t++ = *u;
    while ((u += L) < &X[M*L]);
    } while (++v < &X[N]);
  return doubleMatrix(N, M, newX);
  }

doubleMatrix doubleArray::sum() const {
  double* newX = new double [M];
  double* t = newX;
  double* u =    X;
  double* v =    X + N;
  do {
    *t = *u;
    while (++u < v)
      *t += *u;
    t++;
    u += L - N;
    } while ((v += L) <= &X[M*L]);
  return doubleMatrix(1, M, newX);
  }

doubleMatrix doubleArray::sumsq() const {
  double* newX = new double [M];
  double* t = newX;
  double* u =    X;
  double* v =    X + N;
  do {
    *t  = *u * *u;
    while (++u < v)
      *t += *u * *u;
    t++;
    u += L - N;
    } while ((v += L) <= &X[M*L]);
  return doubleMatrix(1, M, newX);
  }


doubleMatrix doubleArray::map(double (*f)(      double )) const {
  double* newX = new double [M*N];
  double* t = newX;
  double* u =  X;
  double* v =  X + N;
  do {
    while (u < v)
      *t++ = f(*u++);
    u += L - N;
    } while ((v += L) <= &X[M*L]);
  return doubleMatrix(M, N, newX);
  }

doubleMatrix doubleArray::min() const {
  double* newX = new double [M];
  double* t = newX;
  double* u =    X;
  double* v =    X + N;
  do {
    *t = *u;
    while (++u < v)
      if (*t > *u)
	*t = *u;
      t++;
      u += L - N;
    } while ((v += L) <= &X[M*L]);
  return doubleMatrix(1, M, newX);
  }

doubleMatrix doubleArray::max() const {
  double* newX = new double [M];
  double* t = newX;
  double* u =    X;
  double* v =    X + N;
  do {
    *t = *u;
    while (++u < v)
      if (*t < *u)
	*t = *u;
    t++;
    u += L - N;
    } while ((v += L) <= &X[M*L]);
  return doubleMatrix(1, M, newX);
  }

int doubleArray::min_index() const {
  double* t = X;
  double* u = X;
  double* v = X + N;
  do {
    do
      if (*t > *u)
	t = u;
      while (++u < v);
    u += L - N;
    } while ((v += L) <= &X[M*L]);
  return (t - X);
  }

int doubleArray::max_index() const {
  double* t = X;
  double* u = X;
  double* v = X + N;
  do {
    do
      if (*t < *u)
	t = u;
      while (++u < v);
    u += L - N;
    } while ((v += L) <= &X[M*L]);
  return (t - X);
  }

doubleMatrix ffct(const doubleArray& a) {
  extern void cosft(double*, int, int);
  doubleMatrix	y(a);
  for (int i = 0; i < a.m(); i++)
    cosft(y[i] - 1, a.n(), 1);
  return y;
  }

// Constructors
doubleMatrix::doubleMatrix(const doubleMatrix& a) :
doubleArray(a.M, a.N, new double [a.M*a.N]) {
  double* t = a.X;
  double* u =   X;
  double* v =   X;
  while ((v += N) <= &X[M*N]) {
    while (u < v)
      *u++ = *t++;
    t += a.L - a.N;
    };
  }

doubleMatrix::doubleMatrix(const doubleArray& a) :
doubleArray(a.M, a.N, new double [a.M*a.N]) {
  double* t = a.X;
  double* u =   X;
  double* v =   X;
  while ((v += N) <= &X[M*N]) {
    while (u < v)
      *u++ = *t++;
    t += a.L - a.N;
    };
  }


