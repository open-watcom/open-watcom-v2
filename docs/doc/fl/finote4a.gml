If X is of type COMPLEX, then CMPLX(X) is X.
.xt on
If X is of type COMPLEX*16, then CMPLX(X) is a complex number
whose real part is REAL(real part of X) and imaginary part is
REAL(imaginary part of X).
.xt off
.np
If X is not of type COMPLEX, then CMPLX(X) is the complex number whose
real part is REAL(X) and imaginary part is REAL(0).
CMPLX(X,Y) is the complex number whose real part is REAL(X) and whose
imaginary part is REAL(Y) for X,Y not of type COMPLEX.
