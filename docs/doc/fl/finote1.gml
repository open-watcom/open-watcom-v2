The value of int(X) is X if X is of type INTEGER.
If X is of type REAL or DOUBLE PRECISION, then int(X) is 0 if |X|<1 and
the integer whose magnitude is the largest integer that does not exceed
the magnitude of X and has the same sign of X if |X| > 1.
If X is of type COMPLEX
.xt or COMPLEX*16,
int(X) is int(real part of X).
