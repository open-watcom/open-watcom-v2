.xt begin
If X is of type COMPLEX*16, then DCMPLX(X) is X.
If X is of type COMPLEX, then DCMPLX(X) is a COMPLEX*16 number whose
real part is DBLE(real part of X) and imaginary part is
DBLE(imaginary part of X).
.np
If X is not of type COMPLEX*16, then DCMPLX(X) is the COMPLEX*16
number whose real part is DBLE(X) and imaginary part is DBLE(0).
DCMPLX(X,Y) is the COMPLEX*16 number whose real part is DBLE(X) and
whose imaginary part is DBLE(Y) for X,Y not of type COMPLEX.
.xt end
