#include "compMat.h"

complexMatrix complexArray::operator - () const {
  complex* newX = new complex [M*N];
  complex* t = newX;
  complex* u =    X;
  complex* v =    X;
  do {
    while (u < v + N)
      *t++ = -(*u++);
  } while ((u = v += L) < &X[M*L]);
  return complexMatrix(M, N, newX);
  }

complexMatrix operator * (const complex& a, const complexArray& b) {
  complex* newx = new complex [b.m()*b.n()];
  complex* t = newx;
  complex* u =  b.x();
  complex* v =  b.x() + b.n();
  do {
    while (u < v)
      *t++ = a * *u++;
    u += b.l() - b.n();
    } while ((v += b.l()) <= &b.x()[b.m()*b.l()]);
  return complexMatrix(b.m(), b.n(), newx);
  }

complexMatrix operator * (const complexArray& a, const complex& b) {
  complex* newx = new complex [a.m()*a.n()];
  complex* t = newx;
  complex* u =  a.x();
  complex* v =  a.x() + a.n();
  do {
    while (u < v)
      *t++ = *u++ * b;
    u += a.l() - a.n();
    } while ((v += a.l()) <= &a.x()[a.m()*a.l()]);
  return complexMatrix(a.m(), a.n(), newx);
  }

complexMatrix operator * (const complexArray& a, const complexArray& b) {
  if (a.n() == b.n()) {
    if (a.m() == b.m()) {		// a.n() == b.n()
      complex* newx = new complex [b.m()*b.n()];
      complex* s = newx;
      complex* t = newx;
      complex* u =  a.x();
      complex* v =  b.x();
      while ((t += b.n()) <= &newx[b.m()*b.n()]) {
	while (s < t)
	  *s++ = *u++ * *v++;
	u += a.l() - a.n();
	v += b.l() - b.n();
	};
      return complexMatrix(b.m(), b.n(), newx);
      };
    if (a.m() == 1) {			// a.n() == b.n() && a.m() != b.m()
      complex* newx = new complex [b.m()*b.n()];
      complex* s = newx;
      complex* t = newx;
      complex* v =  b.x();
      while ((t += b.n()) <= &newx[b.m()*b.n()]) {
	complex* u = a.x();
	while (s < t)
	  *s++ = *u++ * *v++;
	v += b.l() - b.n();
	};
      return complexMatrix(b.m(), b.n(), newx);
      };
    if (b.m() == 1) {	// a.n() == b.n() && a.m() != b.m() && a.m() != 1
      complex* newx = new complex [a.m()*a.n()];
      complex* s = newx;
      complex* t = newx;
      complex* u =  a.x();
      while ((t += a.n()) <= &newx[a.m()*a.n()]) {
	complex* v = b.x();
	while (s < t)
	  *s++ = *u++ * *v++;
	u += a.l() - a.n();
	};
      return complexMatrix(a.m(), a.n(), newx);
      };
    };
  if (a.n() == 1) {			// b.n() != a.n()
    if (a.m() == b.m()) {		// b.n() != a.n() == 1
      complex* newx = new complex [b.m()*b.n()];
      complex* s = newx;
      complex* t = newx;
      complex* u =  a.x();
      complex* v =  b.x();						
      while ((t += b.n()) <= &newx[b.m()*b.n()]) {
	while (s < t)
	  *s++ = *u * *v++;
	u++;
	v += b.l() - b.n();
	};
      return complexMatrix(b.m(), b.n(), newx);
      };
    if (a.m() == 1) {			// b.n() != a.n() == 1 && a.m() != b.m()
      complex* newx = new complex [b.m()*b.n()];
      complex* s = newx;
      complex* t = newx;
      complex* u =  a.x();
      complex* v =  b.x();
      while ((t += b.n()) <= &newx[b.m()*b.n()]) {
	while (s < t)
	  *s++ = *u * *v++;
	v += b.l() - b.n();
	};
      return complexMatrix(b.m(), b.n(), newx);
      };
    };
  if (b.n() == 1) {			// a.n() != b.n() && a.n() != 1
    if (a.m() == b.m()) {		// a.n() != b.n() == 1
      complex* newx = new complex [a.m()*a.n()];
      complex* s = newx;
      complex* t = newx;
      complex* u =  a.x();
      complex* v =  b.x();
      while ((t += a.n()) <= &newx[a.m()*a.n()]) {
	while (s < t)
	  *s++ = *u++ * *v;
	u += a.l() - a.n();
	v++;
	};
      return complexMatrix(a.m(), a.n(), newx);
      };
    if (b.m() == 1) {			// a.n() != b.n() == 1 && a.m() != b.m()
      complex* newx = new complex [a.m()*a.n()];
      complex* s = newx;
      complex* t = newx;
      complex* u =  a.x();
      complex* v =  b.x();
      while ((t += a.n()) <= &newx[a.m()*a.n()]) {
	while (s < t)
	  *s++ = *u++ * *v;
	u += a.l() - a.n();
	};
      return complexMatrix(a.m(), a.n(), newx);
      };
    };
  a.error("nonconformant complexArray * operands.");
  return complexMatrix();
  }

complexMatrix operator / (const complex& a, const complexArray& b) {
  complex* newx = new complex [b.m()*b.n()];
  complex* t = newx;
  complex* u =  b.x();
  complex* v =  b.x() + b.n();
  do {
    while (u < v)
      *t++ = a / *u++;
    u += b.l() - b.n();
    } while ((v += b.l()) <= &b.x()[b.m()*b.l()]);
  return complexMatrix(b.m(), b.n(), newx);
  }

complexMatrix operator / (const complexArray& a, const complex& b) {
  complex* newx = new complex [a.m()*a.n()];
  complex* t = newx;
  complex* u =  a.x();
  complex* v =  a.x() + a.n();
  do {
    while (u < v)
      *t++ = *u++ / b;
    u += a.l() - a.n();
    } while ((v += a.l()) <= &a.x()[a.m()*a.l()]);
  return complexMatrix(a.m(), a.n(), newx);
  }

complexMatrix operator / (const complexArray& a, const complexArray& b) {
  if (a.n() == b.n()) {
    if (a.m() == b.m()) {		// a.n() == b.n()
      complex* newx = new complex [b.m()*b.n()];
      complex* s = newx;
      complex* t = newx;
      complex* u =  a.x();
      complex* v =  b.x();
      while ((t += b.n()) <= &newx[b.m()*b.n()]) {
	while (s < t)
	  *s++ = *u++ / *v++;
	u += a.l() - a.n();
	v += b.l() - b.n();
	};
      return complexMatrix(b.m(), b.n(), newx);
      };
    if (a.m() == 1) {			// a.n() == b.n() && a.m() != b.m()
      complex* newx = new complex [b.m()*b.n()];
      complex* s = newx;
      complex* t = newx;
      complex* v =  b.x();
      while ((t += b.n()) <= &newx[b.m()*b.n()]) {
	complex* u = a.x();
	while (s < t)
	  *s++ = *u++ / *v++;
	v += b.l() - b.n();
	};
      return complexMatrix(b.m(), b.n(), newx);
      };
    if (b.m() == 1) {	// a.n() == b.n() && a.m() != b.m() && a.m() != 1
      complex* newx = new complex [a.m()*a.n()];
      complex* s = newx;
      complex* t = newx;
      complex* u =  a.x();
      while ((t += a.n()) <= &newx[a.m()*a.n()]) {
	complex* v = b.x();
	while (s < t)
	  *s++ = *u++ / *v++;
	u += a.l() - a.n();
	};
      return complexMatrix(a.m(), a.n(), newx);
      };
    };
  if (a.n() == 1) {			// b.n() != a.n()
    if (a.m() == b.m()) {		// b.n() != a.n() == 1
      complex* newx = new complex [b.m()*b.n()];
      complex* s = newx;
      complex* t = newx;
      complex* u =  a.x();
      complex* v =  b.x();						
      while ((t += b.n()) <= &newx[b.m()*b.n()]) {
	while (s < t)
	  *s++ = *u / *v++;
	u++;
	v += b.l() - b.n();
	};
      return complexMatrix(b.m(), b.n(), newx);
      };
    if (a.m() == 1) {			// b.n() != a.n() == 1 && a.m() != b.m()
      complex* newx = new complex [b.m()*b.n()];
      complex* s = newx;
      complex* t = newx;
      complex* u =  a.x();
      complex* v =  b.x();
      while ((t += b.n()) <= &newx[b.m()*b.n()]) {
	while (s < t)
	  *s++ = *u / *v++;
	v += b.l() - b.n();
	};
      return complexMatrix(b.m(), b.n(), newx);
      };
    };
  if (b.n() == 1) {			// a.n() != b.n() && a.n() != 1
    if (a.m() == b.m()) {		// a.n() != b.n() == 1
      complex* newx = new complex [a.m()*a.n()];
      complex* s = newx;
      complex* t = newx;
      complex* u =  a.x();
      complex* v =  b.x();
      while ((t += a.n()) <= &newx[a.m()*a.n()]) {
	while (s < t)
	  *s++ = *u++ / *v;
	u += a.l() - a.n();
	v++;
	};
      return complexMatrix(a.m(), a.n(), newx);
      };
    if (b.m() == 1) {			// a.n() != b.n() == 1 && a.m() != b.m()
      complex* newx = new complex [a.m()*a.n()];
      complex* s = newx;
      complex* t = newx;
      complex* u =  a.x();
      complex* v =  b.x();
      while ((t += a.n()) <= &newx[a.m()*a.n()]) {
	while (s < t)
	  *s++ = *u++ / *v;
	u += a.l() - a.n();
	};
      return complexMatrix(a.m(), a.n(), newx);
      };
    };
  a.error("nonconformant complexArray / operands.");
  return complexMatrix();
  }

complexMatrix operator % (const complexArray& a, const complexArray& b) {	// inner product
  if (a.n() != b.n())
    a.error("nonconformant complexArray % operands.");
  complex* newx = new complex [a.m()*b.m()];
  complex* s = newx;
  complex* t =  a.x();
  complex* u;
  complex* v;
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
  return complexMatrix(a.m(), b.m(), newx);
  }

complexMatrix operator + (const complex& a, const complexArray& b) {
  complex* newx = new complex [b.m()*b.n()];
  complex* t = newx;
  complex* u =  b.x();
  complex* v =  b.x() + b.n();
  do {
    while (u < v)
      *t++ = a + *u++;
    u += b.l() - b.n();
    } while ((v += b.l()) <= &b.x()[b.m()*b.l()]);
  return complexMatrix(b.m(), b.n(), newx);
  }

complexMatrix operator + (const complexArray& a, const complex& b) {
  complex* newx = new complex [a.m()*a.n()];
  complex* t = newx;
  complex* u =  a.x();
  complex* v =  a.x() + a.n();
  do {
    while (u < v)
      *t++ = *u++ + b;
    u += a.l() - a.n();
    } while ((v += a.l()) <= &a.x()[a.m()*a.l()]);
  return complexMatrix(a.m(), a.n(), newx);
  }

complexMatrix operator + (const complexArray& a, const complexArray& b) {
  if (a.n() == b.n()) {
    if (a.m() == b.m()) {		// a.n() == b.n()
      complex* newx = new complex [b.m()*b.n()];
      complex* s = newx;
      complex* t = newx;
      complex* u =  a.x();
      complex* v =  b.x();
      while ((t += b.n()) <= &newx[b.m()*b.n()]) {
	while (s < t)
	  *s++ = *u++ + *v++;
	u += a.l() - a.n();
	v += b.l() - b.n();
	};
      return complexMatrix(b.m(), b.n(), newx);
      };
    if (a.m() == 1) {			// a.n() == b.n() && a.m() != b.m()
      complex* newx = new complex [b.m()*b.n()];
      complex* s = newx;
      complex* t = newx;
      complex* v =  b.x();
      while ((t += b.n()) <= &newx[b.m()*b.n()]) {
	complex* u = a.x();
	while (s < t)
	  *s++ = *u++ + *v++;
	v += b.l() - b.n();
	};
      return complexMatrix(b.m(), b.n(), newx);
      };
    if (b.m() == 1) {	// a.n() == b.n() && a.m() != b.m() && a.m() != 1
      complex* newx = new complex [a.m()*a.n()];
      complex* s = newx;
      complex* t = newx;
      complex* u =  a.x();
      while ((t += a.n()) <= &newx[a.m()*a.n()]) {
	complex* v = b.x();
	while (s < t)
	  *s++ = *u++ + *v++;
	u += a.l() - a.n();
	};
      return complexMatrix(a.m(), a.n(), newx);
      };
    };
  if (a.n() == 1) {			// b.n() != a.n()
    if (a.m() == b.m()) {		// b.n() != a.n() == 1
      complex* newx = new complex [b.m()*b.n()];
      complex* s = newx;
      complex* t = newx;
      complex* u =  a.x();
      complex* v =  b.x();						
      while ((t += b.n()) <= &newx[b.m()*b.n()]) {
	while (s < t)
	  *s++ = *u + *v++;
	u++;
	v += b.l() - b.n();
	};
      return complexMatrix(b.m(), b.n(), newx);
      };
    if (a.m() == 1) {			// b.n() != a.n() == 1 && a.m() != b.m()
      complex* newx = new complex [b.m()*b.n()];
      complex* s = newx;
      complex* t = newx;
      complex* u =  a.x();
      complex* v =  b.x();
      while ((t += b.n()) <= &newx[b.m()*b.n()]) {
	while (s < t)
	  *s++ = *u + *v++;
	v += b.l() - b.n();
	};
      return complexMatrix(b.m(), b.n(), newx);
      };
    };
  if (b.n() == 1) {			// a.n() != b.n() && a.n() != 1
    if (a.m() == b.m()) {		// a.n() != b.n() == 1
      complex* newx = new complex [a.m()*a.n()];
      complex* s = newx;
      complex* t = newx;
      complex* u =  a.x();
      complex* v =  b.x();
      while ((t += a.n()) <= &newx[a.m()*a.n()]) {
	while (s < t)
	  *s++ = *u++ + *v;
	u += a.l() - a.n();
	v++;
	};
      return complexMatrix(a.m(), a.n(), newx);
      };
    if (b.m() == 1) {			// a.n() != b.n() == 1 && a.m() != b.m()
      complex* newx = new complex [a.m()*a.n()];
      complex* s = newx;
      complex* t = newx;
      complex* u =  a.x();
      complex* v =  b.x();
      while ((t += a.n()) <= &newx[a.m()*a.n()]) {
	while (s < t)
	  *s++ = *u++ + *v;
	u += a.l() - a.n();
	};
      return complexMatrix(a.m(), a.n(), newx);
      };
    };
  a.error("nonconformant complexArray + operands.");
  return complexMatrix();
  }

complexMatrix operator - (const complex& a, const complexArray& b) {
  complex* newx = new complex [b.m()*b.n()];
  complex* t = newx;
  complex* u =  b.x();
  complex* v =  b.x() + b.n();
  do {
    while (u < v)
      *t++ = a - *u++;
    u += b.l() - b.n();
    } while ((v += b.l()) <= &b.x()[b.m()*b.l()]);
  return complexMatrix(b.m(), b.n(), newx);
  }

complexMatrix operator - (const complexArray& a, const complex& b) {
  complex* newx = new complex [a.m()*a.n()];
  complex* t = newx;
  complex* u =  a.x();
  complex* v =  a.x() + a.n();
  do {
    while (u < v)
      *t++ = *u++ - b;
    u += a.l() - a.n();
    } while ((v += a.l()) <= &a.x()[a.m()*a.l()]);
  return complexMatrix(a.m(), a.n(), newx);
  }

complexMatrix operator - (const complexArray& a, const complexArray& b) {
  if (a.n() == b.n()) {
    if (a.m() == b.m()) {		// a.n() == b.n()
      complex* newx = new complex [b.m()*b.n()];
      complex* s = newx;
      complex* t = newx;
      complex* u =  a.x();
      complex* v =  b.x();
      while ((t += b.n()) <= &newx[b.m()*b.n()]) {
	while (s < t)
	  *s++ = *u++ - *v++;
	u += a.l() - a.n();
	v += b.l() - b.n();
	};
      return complexMatrix(b.m(), b.n(), newx);
      };
    if (a.m() == 1) {			// a.n() == b.n() && a.m() != b.m()
      complex* newx = new complex [b.m()*b.n()];
      complex* s = newx;
      complex* t = newx;
      complex* v =  b.x();
      while ((t += b.n()) <= &newx[b.m()*b.n()]) {
	complex* u = a.x();
	while (s < t)
	  *s++ = *u++ - *v++;
	v += b.l() - b.n();
	};
      return complexMatrix(b.m(), b.n(), newx);
      };
    if (b.m() == 1) {	// a.n() == b.n() && a.m() != b.m() && a.m() != 1
      complex* newx = new complex [a.m()*a.n()];
      complex* s = newx;
      complex* t = newx;
      complex* u =  a.x();
      while ((t += a.n()) <= &newx[a.m()*a.n()]) {
	complex* v = b.x();
	while (s < t)
	  *s++ = *u++ - *v++;
	u += a.l() - a.n();
	};
      return complexMatrix(a.m(), a.n(), newx);
      };
    };
  if (a.n() == 1) {			// b.n() != a.n()
    if (a.m() == b.m()) {		// b.n() != a.n() == 1
      complex* newx = new complex [b.m()*b.n()];
      complex* s = newx;
      complex* t = newx;
      complex* u =  a.x();
      complex* v =  b.x();						
      while ((t += b.n()) <= &newx[b.m()*b.n()]) {
	while (s < t)
	  *s++ = *u - *v++;
	u++;
	v += b.l() - b.n();
	};
      return complexMatrix(b.m(), b.n(), newx);
      };
    if (a.m() == 1) {			// b.n() != a.n() == 1 && a.m() != b.m()
      complex* newx = new complex [b.m()*b.n()];
      complex* s = newx;
      complex* t = newx;
      complex* u =  a.x();
      complex* v =  b.x();
      while ((t += b.n()) <= &newx[b.m()*b.n()]) {
	while (s < t)
	  *s++ = *u - *v++;
	v += b.l() - b.n();
	};
      return complexMatrix(b.m(), b.n(), newx);
      };
    };
  if (b.n() == 1) {			// a.n() != b.n() && a.n() != 1
    if (a.m() == b.m()) {		// a.n() != b.n() == 1
      complex* newx = new complex [a.m()*a.n()];
      complex* s = newx;
      complex* t = newx;
      complex* u =  a.x();
      complex* v =  b.x();
      while ((t += a.n()) <= &newx[a.m()*a.n()]) {
	while (s < t)
	  *s++ = *u++ - *v;
	u += a.l() - a.n();
	v++;
	};
      return complexMatrix(a.m(), a.n(), newx);
      };
    if (b.m() == 1) {			// a.n() != b.n() == 1 && a.m() != b.m()
      complex* newx = new complex [a.m()*a.n()];
      complex* s = newx;
      complex* t = newx;
      complex* u =  a.x();
      complex* v =  b.x();
      while ((t += a.n()) <= &newx[a.m()*a.n()]) {
	while (s < t)
	  *s++ = *u++ - *v;
	u += a.l() - a.n();
	};
      return complexMatrix(a.m(), a.n(), newx);
      };
    };
  a.error("nonconformant complexArray - operands.");
  return complexMatrix();
  }


extern int Array_number; extern char* Array_format; extern char* Array_space;
ostream& operator << (ostream& s, const complexArray& b) {
  complex* t = b.x();
  complex* u = b.x();
  complex* v = b.x() + b.n();
  do {
    do {
      t += (0 < Array_number)? Array_number: b.n();
      t  = (t < v)? t: v;
      while (u < t)
	if (!(s << "("  && s << form(Array_format, real(*u)) &&
	      s << ", " && s << form(Array_format, imag(*u)) &&
	      s << ")"
	      && ((++u < t) ? s << Array_space : s << "\n")))
	  return s;
      } while (t < v);
    t = u += b.l() - b.n();
    } while ((v += b.l()) <= &b.x()[b.m()*b.l()]);
  return s;
  }

istream& operator >> (istream& s, const complexArray& b) {
  complex* u = b.x();
  complex* v = b.x() + b.n();
  do {
    while (u < v && s >> *u)
      u++;
    if (u < v)
      return s;
    u += b.l() - b.n();
    } while ((v += b.l()) <= &b.x()[b.m()*b.l()]);
  return s;
  }

complexMatrix operator << (const complexArray& a, int n) {
  complex* newx = new complex [a.m()*a.n()];
  complex* t =  a.x() + n;
  complex* u = newx;
  complex* v = newx;
  while ((v += a.n()) <= &newx[a.m()*a.n()]) {
    while (u < v - n)
      *u++ = *t++;
    while (u < v)
      *u++ = 0.0;
    t += a.l() - a.n() + n;
    };
  return complexMatrix(a.m(), a.n(), newx);
  }

complexMatrix operator >> (const complexArray& a, int n) {
  complex* newx = new complex [a.m()*a.n()];
  complex* t =  &a.x()[a.m()*a.l()];
  complex* u = &newx[a.m()*a.n()];
  complex* v = &newx[a.m()*a.n()];
  while (newx <= (u -= a.n())) {
    t -= a.l() - a.n() + n;
    while (v > u + n)
      *--v = *--t;
    while (v > u)
      *--v = *t;
    };
  return complexMatrix(a.m(), a.n(), newx);
  }

int operator == (const complexArray& a, const complex& b) {
  complex* u = a.x();
  complex* v = a.x() + a.n();
  do {
    while (u < v && *u == b)
      u++;
    if (u < v)
      return 0;
    u += a.l() - a.n();
    } while ((v += a.l()) <= &a.x()[a.m()*a.l()]);
  return 1;
  }

int operator == (const complexArray& a, const complexArray& b) {
  if (a.n() == b.n()) {
    if (a.m() == b.m()) {		// a.n() == b.n()
      complex* t = a.x();
      complex* u = b.x();
      complex* v = b.x() + b.n();
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
      complex* u = b.x();
      complex* v = b.x() + b.n();
      do {
	complex* t = a.x();
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
      complex* u = a.x();
      complex* v = a.x() + a.n();
      do {
	complex* t = b.x();
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
      complex* t = a.x();
      complex* u = b.x();
      complex* v = b.x() + b.n();						
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
      complex* u = b.x();
      complex* v = b.x() + b.n();
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
      complex* t = b.x();
      complex* u = a.x();
      complex* v = a.x() + a.n();
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
      complex* u = a.x();
      complex* v = a.x() + a.n();
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
  a.error("nonconformant complexArray == operands.");
  return 0;
  }

complexMatrix operator & (const complexArray& a, const complexArray& b) {	// outer product
  if (a.m() != b.m())
    a.error("nonconformant complexArray & operands.");
  complex* newx = new complex [a.n()*b.n()];
  complex* p = newx;
  complex* s =  a.x();
  complex* t;
  complex* u;
  complex* v;
  do {
    t = b.x();
    do {
      u = s; v = t; *p = *u * *v;
      while ((u += a.l()), (v += b.l()) < &b.x()[b.m()*b.l()])
	*p += *u * *v;
      p++;
      } while (++t < &b.x()[b.n()]);
    } while (++s < &a.x()[a.n()]);
  return complexMatrix(a.n(), b.n(), newx);
  }

complexMatrix operator ^ (const complexArray& a, const complexArray& b) {	// stack
  if (a.n() != b.n())
    a.error("nonconformant complexArray ^ operands.");
  complex* newx = new complex [(a.m()+b.m())*a.n()];
  complex* t = newx;
  complex* u =  a.x();
  complex* v =  a.x() + a.n();
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
  return complexMatrix(a.m()+b.m(), a.n(), newx);
  }

complexMatrix operator | (const complexArray& a, const complexArray& b) {	// adjoin
  if (a.m() != b.m())
    a.error("nonconformant complexArray | operands.");
  complex* newx = new complex [a.m()*(a.n()+b.n())];
  complex* t = newx;
  complex* u =  a.x();
  complex* v =  a.x() + a.n();
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
  return complexMatrix(a.m(), a.n()+b.n(), newx);
  }

complexArray& complexArray::operator  = (const complex& b) {
  complex* s = X;
  complex* t = X + N;
  do {
    while (s < t)
      *s++  = b;
    s += L - N;
    } while ((t += L) <= &X[M*L]);
  return *this;
  }

complexArray& complexArray::operator  = (const complexArray& a) {
  if (N == a.N) {
    if (M == a.M) {			// N == a.N
      complex* t = a.X;
      complex* u =   X;
      complex* v =   X + N;
      do {
	while (u < v)
	  *u++  = *t++;
	t += a.L - a.N;
	u +=   L -   N;
	} while ((v += L) <= &X[M*L]);
      return *this;
      };
    if (a.M == 1) {			// N == a.N && M != a.M
      complex* t;
      complex* u = X;
      complex* v = X + N;
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
      complex* t = a.X;
      complex* u =   X;
      complex* v =   X + N;
      do {
	while (u < v)
	  *u++  = *t;
	t++;
	u += L - N;
	} while ((v += L) <= &X[M*L]);
      return *this;
      };
    if (a.M == 1) {			// N != a.N == 1 && M != a.M
      complex* t = a.X;
      complex* u =   X;
      complex* v =   X + N;
      do {
	while (u < v)
	  *u++  = *t;
	u += L - N;
	} while ((v += L) <= &X[M*L]);
      return *this;
      };
    };
  error("nonconformant complexArray  = operands.");
  return *this;
  }

complexArray& complexArray::operator *= (const complex& b) {
  complex* s = X;
  complex* t = X + N;
  do {
    while (s < t)
      *s++ *= b;
    s += L - N;
    } while ((t += L) <= &X[M*L]);
  return *this;
  }

complexArray& complexArray::operator *= (const complexArray& a) {
  if (N == a.N) {
    if (M == a.M) {			// N == a.N
      complex* t = a.X;
      complex* u =   X;
      complex* v =   X + N;
      do {
	while (u < v)
	  *u++ *= *t++;
	t += a.L - a.N;
	u +=   L -   N;
	} while ((v += L) <= &X[M*L]);
      return *this;
      };
    if (a.M == 1) {			// N == a.N && M != a.M
      complex* t;
      complex* u = X;
      complex* v = X + N;
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
      complex* t = a.X;
      complex* u =   X;
      complex* v =   X + N;
      do {
	while (u < v)
	  *u++ *= *t;
	t++;
	u += L - N;
	} while ((v += L) <= &X[M*L]);
      return *this;
      };
    if (a.M == 1) {			// N != a.N == 1 && M != a.M
      complex* t = a.X;
      complex* u =   X;
      complex* v =   X + N;
      do {
	while (u < v)
	  *u++ *= *t;
	u += L - N;
	} while ((v += L) <= &X[M*L]);
      return *this;
      };
    };
  error("nonconformant complexArray *= operands.");
  return *this;
  }

complexArray& complexArray::operator /= (const complex& b) {
  complex* s = X;
  complex* t = X + N;
  do {
    while (s < t)
      *s++ /= b;
    s += L - N;
    } while ((t += L) <= &X[M*L]);
  return *this;
  }

complexArray& complexArray::operator /= (const complexArray& a) {
  if (N == a.N) {
    if (M == a.M) {			// N == a.N
      complex* t = a.X;
      complex* u =   X;
      complex* v =   X + N;
      do {
	while (u < v)
	  *u++ /= *t++;
	t += a.L - a.N;
	u +=   L -   N;
	} while ((v += L) <= &X[M*L]);
      return *this;
      };
    if (a.M == 1) {			// N == a.N && M != a.M
      complex* t;
      complex* u = X;
      complex* v = X + N;
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
      complex* t = a.X;
      complex* u =   X;
      complex* v =   X + N;
      do {
	while (u < v)
	  *u++ /= *t;
	t++;
	u += L - N;
	} while ((v += L) <= &X[M*L]);
      return *this;
      };
    if (a.M == 1) {			// N != a.N == 1 && M != a.M
      complex* t = a.X;
      complex* u =   X;
      complex* v =   X + N;
      do {
	while (u < v)
	  *u++ /= *t;
	u += L - N;
	} while ((v += L) <= &X[M*L]);
      return *this;
      };
    };
  error("nonconformant complexArray /= operands.");
  return *this;
  }

complexArray& complexArray::operator += (const complex& b) {
  complex* s = X;
  complex* t = X + N;
  do {
    while (s < t)
      *s++ += b;
    s += L - N;
    } while ((t += L) <= &X[M*L]);
  return *this;
  }

complexArray& complexArray::operator += (const complexArray& a) {
  if (N == a.N) {
    if (M == a.M) {			// N == a.N
      complex* t = a.X;
      complex* u =   X;
      complex* v =   X + N;
      do {
	while (u < v)
	  *u++ += *t++;
	t += a.L - a.N;
	u +=   L -   N;
	} while ((v += L) <= &X[M*L]);
      return *this;
      };
    if (a.M == 1) {			// N == a.N && M != a.M
      complex* t;
      complex* u = X;
      complex* v = X + N;
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
      complex* t = a.X;
      complex* u =   X;
      complex* v =   X + N;
      do {
	while (u < v)
	  *u++ += *t;
	t++;
	u += L - N;
	} while ((v += L) <= &X[M*L]);
      return *this;
      };
    if (a.M == 1) {			// N != a.N == 1 && M != a.M
      complex* t = a.X;
      complex* u =   X;
      complex* v =   X + N;
      do {
	while (u < v)
	  *u++ += *t;
	u += L - N;
	} while ((v += L) <= &X[M*L]);
      return *this;
      };
    };
  error("nonconformant complexArray += operands.");
  return *this;
  }

complexArray& complexArray::operator -= (const complex& b) {
  complex* s = X;
  complex* t = X + N;
  do {
    while (s < t)
      *s++ -= b;
    s += L - N;
    } while ((t += L) <= &X[M*L]);
  return *this;
  }

complexArray& complexArray::operator -= (const complexArray& a) {
  if (N == a.N) {
    if (M == a.M) {			// N == a.N
      complex* t = a.X;
      complex* u =   X;
      complex* v =   X + N;
      do {
	while (u < v)
	  *u++ -= *t++;
	t += a.L - a.N;
	u +=   L -   N;
	} while ((v += L) <= &X[M*L]);
      return *this;
      };
    if (a.M == 1) {			// N == a.N && M != a.M
      complex* t;
      complex* u = X;
      complex* v = X + N;
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
      complex* t = a.X;
      complex* u =   X;
      complex* v =   X + N;
      do {
	while (u < v)
	  *u++ -= *t;
	t++;
	u += L - N;
	} while ((v += L) <= &X[M*L]);
      return *this;
      };
    if (a.M == 1) {			// N != a.N == 1 && M != a.M
      complex* t = a.X;
      complex* u =   X;
      complex* v =   X + N;
      do {
	while (u < v)
	  *u++ -= *t;
	u += L - N;
	} while ((v += L) <= &X[M*L]);
      return *this;
      };
    };
  error("nonconformant complexArray -= operands.");
  return *this;
  }

complexArray& complexArray::operator >>= (int n) {
  complex* t = &X[M*L];
  complex* u = &X[M*L];
  complex* v = &X[M*L];
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

complexArray& complexArray::operator <<= (int n) {
  complex* t = X + n;
  complex* u = X;
  complex* v = X + N;
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
void default_complexArray_error_handler(const char* msg) {
  cerr << "Fatal complexArray error. " << msg << "\n";
  exit(1);
  return;
  }

one_arg_error_handler_t
	complexArray_error_handler = default_complexArray_error_handler;

one_arg_error_handler_t set_complexArray_error_handler(one_arg_error_handler_t f) {
  one_arg_error_handler_t old = complexArray_error_handler;
  complexArray_error_handler = f;
  return old;
  }

void complexArray::error(const char* msg) const {
  (*complexArray_error_handler)(msg);
  }


complexMatrix complexArray::t() const {		// transpose
  complex* newX = new complex [N*M];
  complex* t = newX;
  complex* u;
  complex* v =    X;
  do {
    u = v;
    do
      *t++ = *u;
    while ((u += L) < &X[M*L]);
    } while (++v < &X[N]);
  return complexMatrix(N, M, newX);
  }

complexMatrix complexArray::sum() const {
  complex* newX = new complex [M];
  complex* t = newX;
  complex* u =    X;
  complex* v =    X + N;
  do {
    *t = *u;
    while (++u < v)
      *t += *u;
    t++;
    u += L - N;
    } while ((v += L) <= &X[M*L]);
  return complexMatrix(1, M, newX);
  }

complexMatrix complexArray::sumsq() const {
  complex* newX = new complex [M];
  complex* t = newX;
  complex* u =    X;
  complex* v =    X + N;
  do {
    *t  = *u * *u;
    while (++u < v)
      *t += *u * *u;
    t++;
    u += L - N;
    } while ((v += L) <= &X[M*L]);
  return complexMatrix(1, M, newX);
  }


#ifdef __ATT_complex__
complexMatrix complexArray::map(complex (*f)(const complex&)) const {
  complex* newX = new complex [M*N];
  complex* t = newX;
  complex* u =  X;
  complex* v =  X + N;
  do {
    while (u < v)
      *t++ = f(*u++);
    u += L - N;
    } while ((v += L) <= &X[M*L]);
  return complexMatrix(M, N, newX);
  }
#else
complexMatrix complexArray::map(const complex (*f)(      complex )) const {
  complex* newX = new complex [M*N];
  complex* t = newX;
  complex* u =  X;
  complex* v =  X + N;
  do {
    while (u < v)
      *t++ = f(*u++);
    u += L - N;
    } while ((v += L) <= &X[M*L]);
  return complexMatrix(M, N, newX);
  }
#endif
doubleMatrix complexArray::map(double (*f)(const complex&)) const {
  double* newX = new double [M*N];
  double* t = newX;
  complex* u =  X;
  complex* v =  X + N;
  do {
    while (u < v)
      *t++ = f(*u++);
    u += L - N;
    } while ((v += L) <= &X[M*L]);
  return doubleMatrix(M, N, newX);
  }

doubleMatrix complexArray::map(double (*f)(      complex )) const {
  double* newX = new double [M*N];
  double* t = newX;
  complex* u =  X;
  complex* v =  X + N;
  do {
    while (u < v)
      *t++ = f(*u++);
    u += L - N;
    } while ((v += L) <= &X[M*L]);
  return doubleMatrix(M, N, newX);
  }

complexMatrix fft(const complexArray& a) {
  extern void four1(double*, int, int);
  complexMatrix	result(a);
  for (int i = 0; i < result.m(); i++)
    four1((double*) result[i] - 1, result.n(), 1);
  return result;
  }

complexMatrix fft(const doubleArray& a) {
  extern void realft(double*, int, int);
  complexMatrix	data(a.m(), a.n());
  doubleArray(2*a.n(), a.m(), a.n(), (double*) data.x()) = a;
  for (int i = 0; i < a.m(); i++) {
    realft((double*) data[i] - 1, a.n()/2, 1);
    for (int j = 1; j < a.n()/2; j++)
      data[i][a.n()-j]	=  conj(data[i][j]);
    data[i][a.n()/2]	=  imag(data[i][0]);
    data[i][0]		=  real(data[i][0]);
    };
  return data;
  }

// Constructors
complexMatrix::complexMatrix(const complexMatrix& a) :
complexArray(a.M, a.N, new complex [a.M*a.N]) {
  complex* t = a.X;
  complex* u =   X;
  complex* v =   X;
  while ((v += N) <= &X[M*N]) {
    while (u < v)
      *u++ = *t++;
    t += a.L - a.N;
    };
  }

complexMatrix::complexMatrix(const complexArray& a) :
complexArray(a.M, a.N, new complex [a.M*a.N]) {
  complex* t = a.X;
  complex* u =   X;
  complex* v =   X;
  while ((v += N) <= &X[M*N]) {
    while (u < v)
      *u++ = *t++;
    t += a.L - a.N;
    };
  }


complexMatrix::complexMatrix(const doubleArray& r, const double i) :
complexArray(r.M, r.N, new complex [r.M*r.N]) {
  complex* u =   X;
  complex* v =   X;
  double* t = r.X;
  while ((v += N) <= &X[M*N]) {
    while (u < v)
      *u++ = complex(*t++, i);
    t += r.L - r.N;
    };
  }

complexMatrix::complexMatrix(const doubleArray& r, const doubleArray& i) :
complexArray(r.M, r.N, new complex [r.M*r.N]) {
  if(i.M == M && i.N == N) {
    complex* u = X;
    complex* v = X;
    double* t =  r.X;
    double* k =  i.X;
    while ((v += N) <= &X[M*N]) {
      while (u < v)
	*u++ = complex(*t++, *k++);
      t += r.L - r.N;
      k += i.L - i.N;
      };
    }
  else
    error("nonconformant complexArray operands.");
  return;
  }

