:set symbol="arrow"   value=";.sf7;.ct ~L;.esf;.ct ".
:cmt. - is a minus
.np
The following sections give tables of all generic and specific names of
intrinsic functions and describe how they are used.
The following is a guide to interpreting those tables.
.np
Data types are represented by letter codes.
.autopoint
.point
CHARACTER is represented by CH.
.point
LOGICAL is represented by L.
.point
INTEGER is represented by I.
.point
.xt INTEGER*2 is represented by H.
.point
REAL
.xt (REAL*4)
is represented by R.
.point
DOUBLE PRECISION
.xt (REAL*8)
is represented by D.
.point
Single precision COMPLEX
.xt (COMPLEX*8)
is represented by C.
.point
.xt Double precision COMPLEX (COMPLEX*16) is represented by Z.
.endpoint
.np
There are three columns to each table.
The "Definition" column gives the mathematical definition of the
function performed by the intrinsic function.
The "Name" column lists the specific and generic names of the intrinsic
functions.
The generic names are followed by the word "generic"; all other names
are specific names.
The "Usage" column describes how the intrinsic functions are used.
"R&arrow.ATAN2(R,R)" is a typical entry in this column.
The name of the intrinsic function always follows the "&arrow.".
In this example the name of the intrinsic function is ATAN2.
The data type of the arguments to the intrinsic function are enclosed
in parentheses,
are separated by commas and always follow the name of the intrinsic
function.
In this case, ATAN2 requires two arguments both of type REAL.
The type of the result of the intrinsic function is indicated by the
type preceding the "&arrow.".
In this case, the result of ATAN2 is of type REAL.
.np
&product extensions to the FORTRAN 77 language are flagged by
a dagger (&dagger).
.*
.cp 15
.section Type Conversion
.*
.ix 'generic function' 'INT'
.ix 'intrinsic function' 'INT'
.ix 'intrinsic function' 'IFIX'
.ix 'intrinsic function' 'IDINT'
.ix 'intrinsic function' 'HFIX'
.ix 'INT'
.ix 'IFIX'
.ix 'IDINT'
.ix 'HFIX'
.sr c0=&INDlvl+1
.sr c1=&INDlvl+17
.sr c2=&INDlvl+30
.sr c3=&INDlvl+54
.* .box on 1 17 30 54
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\Conversion to integer:\INT generic\I&arrow.INT(I)
\int(a)                \           \I&arrow.INT(R)
\                      \           \I&arrow.INT(D)
\                      \           \I&arrow.INT(C)
\                      \           \I&arrow.INT(Z) &dagger\
\                      \INT        \I&arrow.INT(R)
\                      \HFIX &dagger     \H&arrow.HFIX(R)
\                      \IFIX       \I&arrow.IFIX(R)
\                      \IDINT      \I&arrow.IDINT(D)
.box off
.im finote1
.ix 'generic function' 'REAL'
.ix 'intrinsic function' 'REAL'
.ix 'intrinsic function' 'FLOAT'
.ix 'intrinsic function' 'SNGL'
.ix 'REAL'
.ix 'FLOAT'
.ix 'SNGL'
.cp 14
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\Conversion to real\REAL generic\R&arrow.REAL(I)
\                  \            \R&arrow.REAL(R)
\                  \            \R&arrow.REAL(D)
\                  \            \R&arrow.REAL(C)
\                  \            \R&arrow.REAL(Z) &dagger\
\                  \REAL        \R&arrow.REAL(I)
\                  \FLOAT       \R&arrow.FLOAT(I)
\                  \SNGL        \R&arrow.SNGL(D)
.box off
.im finote2
.ix 'generic function' 'DBLE'
.ix 'intrinsic function' 'DBLE'
.ix 'intrinsic function' 'DREAL'
.ix 'intrinsic function' 'DFLOAT'
.ix 'DBLE'
.ix 'DREAL'
.ix 'DFLOAT'
.cp 13
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\Conversion to double \DBLE generic\D&arrow.DBLE(I)
\precision            \            \D&arrow.DBLE(R)
\                     \            \D&arrow.DBLE(D)
\                     \            \D&arrow.DBLE(C)
\                     \            \D&arrow.DBLE(Z) &dagger\
\                     \DREAL       \D&arrow.DREAL(Z) &dagger\
\                     \DFLOAT      \D&arrow.DFLOAT(I) &dagger\
.box off
.im finote3
.ix 'generic function' 'CMPLX'
.ix 'intrinsic function' 'CMPLX'
.ix 'CMPLX'
.cp 11
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\Conversion to complex \CMPLX generic \C&arrow.CMPLX(I)|C&arrow.CMPLX(I,I)
\                      \              \C&arrow.CMPLX(R)|C&arrow.CMPLX(R,R)
\                      \              \C&arrow.CMPLX(D)|C&arrow.CMPLX(D,D)
\                      \              \C&arrow.CMPLX(C)
\                      \              \C&arrow.CMPLX(Z) &dagger\
.box off
.im finote4a
.ix 'generic function' 'DCMPLX'
.ix 'intrinsic function' 'DCMPLX'
.ix 'DCMPLX'
.cp 11
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\Conversion to double \DCMPLX generic &dagger\Z&arrow.DCMPLX(I)|Z&arrow.DCMPLX(I,I)
\complex              \                \Z&arrow.DCMPLX(R)|Z&arrow.DCMPLX(R,R)
\                     \                \Z&arrow.DCMPLX(D)|Z&arrow.DCMPLX(D,D)
\                     \                \Z&arrow.DCMPLX(C)
\                     \                \Z&arrow.DCMPLX(Z)
.box off
.im finote4b
.ix 'intrinsic function' 'ICHAR'
.ix 'ICHAR'
.cp 7
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\Conversion to integer \ICHAR \I&arrow.ICHAR(CH)
.box off
.im finote5a
.ix 'intrinsic function' 'CHAR'
.ix 'CHAR'
.cp 7
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\Conversion to character \CHAR \CH&arrow.CHAR(I)
.box off
.im finote5b
.*
.cp 15
.section Truncation
.*
.ix 'generic function' 'AINT'
.ix 'intrinsic function' 'AINT'
.ix 'intrinsic function' 'DINT'
.ix 'AINT'
.ix 'DINT'
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\int(a)     \AINT generic \R&arrow.AINT(R)
\           \             \D&arrow.AINT(D)
\           \AINT         \R&arrow.AINT(R)
\           \DINT         \D&arrow.DINT(D)
.box off
.im finote1
.*
.cp 15
.section Nearest Whole Number
.*
.ix 'generic function' 'ANINT'
.ix 'intrinsic function' 'ANINT'
.ix 'intrinsic function' 'DNINT'
.ix 'ANINT'
.ix 'DNINT'
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\int(a+.5) if a>=0 \ANINT generic \R&arrow.ANINT(R)
\int(a-.5) if a<0  \              \D&arrow.ANINT(D)
\                  \ANINT         \R&arrow.ANINT(R)
\                  \DNINT         \D&arrow.DNINT(D)
.box off
.*
.cp 15
.section Nearest Integer
.*
.ix 'generic function' 'NINT'
.ix 'intrinsic function' 'NINT'
.ix 'intrinsic function' 'IDNINT'
.ix 'NINT'
.ix 'IDNINT'
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\int(a+.5) if a>=0 \NINT generic \I&arrow.NINT(R)
\int(a-.5) if a<0  \             \I&arrow.NINT(D)
\                  \NINT         \I&arrow.NINT(R)
\                  \IDNINT       \I&arrow.IDNINT(D)
.box off
.*
.cp 20
.section Absolute Value
.*
.ix 'generic function' 'ABS'
.ix 'intrinsic function' 'IABS'
.ix 'intrinsic function' 'ABS'
.ix 'intrinsic function' 'DABS'
.ix 'intrinsic function' 'CABS'
.ix 'intrinsic function' 'CDABS'
.ix 'ABS'
.ix 'IABS'
.ix 'DABS'
.ix 'CABS'
.ix 'CDABS'
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\(ar**2+ai**2)**1/2 \ABS generic \I&arrow.ABS(I)
\if a is complex;   \            \R&arrow.ABS(R)
\|a| otherwise      \            \D&arrow.ABS(D)
\                   \            \R&arrow.ABS(C)
\                   \            \D&arrow.ABS(Z) &dagger\
\                   \IABS        \I&arrow.IABS(I)
\                   \ABS         \R&arrow.ABS(R)
\                   \DABS        \D&arrow.DABS(D)
\                   \CABS        \R&arrow.CABS(C)
\                   \CDABS &dagger     \D&arrow.CDABS(Z)
.box off
.im finote6
.*
.cp 15
.section Remainder
.*
.ix 'generic function' 'MOD'
.ix 'intrinsic function' 'MOD'
.ix 'intrinsic function' 'AMOD'
.ix 'intrinsic function' 'DMOD'
.ix 'MOD'
.ix 'AMOD'
.ix 'DMOD'
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\a1-int(a1/a2)*a2 \MOD generic \I&arrow.MOD(I,I)
\                 \            \R&arrow.MOD(R,R)
\                 \            \D&arrow.MOD(D,D)
\                 \MOD         \I&arrow.MOD(I,I)
\                 \AMOD        \R&arrow.AMOD(R,R)
\                 \DMOD        \D&arrow.DMOD(D,D)
.box off
.im finote1
.np
The value of MOD, AMOD and DMOD is undefined if the value of a2 is 0.
.*
.cp 15
.section Transfer of Sign
.*
.ix 'generic function' 'SIGN'
.ix 'intrinsic function' 'ISIGN'
.ix 'intrinsic function' 'SIGN'
.ix 'intrinsic function' 'DSIGN'
.ix 'ISIGN'
.ix 'SIGN'
.ix 'DSIGN'
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\|a1| if a2>=0 \SIGN generic \I&arrow.SIGN(I,I)
\-|a1| if a2<0 \             \R&arrow.SIGN(R,R)
\              \             \D&arrow.SIGN(D,D)
\              \ISIGN        \I&arrow.ISIGN(I,I)
\              \SIGN         \R&arrow.SIGN(R,R)
\              \DSIGN        \D&arrow.DSIGN(D,D)
.box off
.np
If the value of a1 is 0, the result is 0 which has no sign.
.*
.cp 15
.section Positive Difference
.*
.ix 'generic function' 'DIM'
.ix 'intrinsic function' 'IDIM'
.ix 'intrinsic function' 'DIM'
.ix 'intrinsic function' 'DDIM'
.ix 'IDIM'
.ix 'DIM'
.ix 'DDIM'
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\a1-a2 if a1>a2 \DIM generic \I&arrow.DIM(I,I)
\0 if a1<=a2    \            \R&arrow.DIM(R,R)
\               \            \D&arrow.DIM(D,D)
\               \IDIM        \I&arrow.IDIM(I,I)
\               \DIM         \R&arrow.DIM(R,R)
\               \DDIM        \D&arrow.DDIM(D,D)
.box off
.*
.cp 10
.section Double Precision Product
.*
.ix 'intrinsic function' 'DPROD'
.ix 'DPROD'
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\a1*a2 \DPROD \D&arrow.DPROD(R,R)
.box off
.*
.cp 15
.section Choosing Largest Value
.*
.ix 'generic function' 'MAX'
.ix 'intrinsic function' 'MAX0'
.ix 'intrinsic function' 'AMAX1'
.ix 'intrinsic function' 'DMAX1'
.ix 'intrinsic function' 'AMAX0'
.ix 'intrinsic function' 'MAX1'
.ix 'MAX0'
.ix 'AMAX1'
.ix 'DMAX1'
.ix 'AMAX0'
.ix 'MAX1'
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\max(a1,a2,...) \MAX generic \I&arrow.MAX(I,...)
\               \            \R&arrow.MAX(R,...)
\               \            \D&arrow.MAX(D,...)
\               \MAX0        \I&arrow.MAX0(I,...)
\               \AMAX1       \R&arrow.AMAX1(R,...)
\               \DMAX1       \D&arrow.DMAX1(D,...)
\               \AMAX0       \R&arrow.AMAX0(I,...)
\               \MAX1        \I&arrow.MAX1(R,...)
.box off
.*
.cp 15
.section Choosing Smallest Value
.*
.ix 'generic function' 'MIN'
.ix 'intrinsic function' 'MIN0'
.ix 'intrinsic function' 'AMIN1'
.ix 'intrinsic function' 'DMIN1'
.ix 'intrinsic function' 'AMIN0'
.ix 'intrinsic function' 'MIN1'
.ix 'MIN0'
.ix 'AMIN1'
.ix 'DMIN1'
.ix 'AMIN0'
.ix 'MIN1'
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\min(a1,a2,...) \MIN generic \I&arrow.MIN(I,...)
\               \            \R&arrow.MIN(R,...)
\               \            \D&arrow.MIN(D,...)
\               \MIN0        \I&arrow.MIN0(I,...)
\               \AMIN1       \R&arrow.AMIN1(R,...)
\               \DMIN1       \D&arrow.DMIN1(D,...)
\               \AMIN0       \R&arrow.AMIN0(I,...)
\               \MIN1        \I&arrow.MIN1(R,...)
.box off
.*
.cp 10
.section Length
.*
.ix 'intrinsic function' 'LEN'
.ix 'LEN'
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\Length of character \LEN \I&arrow.LEN(CH)
\entity
.box off
.np
The argument to the LEN function need not be defined.
.*
.cp 10
.section Length Without Trailing Blanks
.*
.ix 'intrinsic function' 'LENTRIM'
.ix 'LENTRIM'
.bxt on &c0 &c3 &c1 &c2
\Definition\Name\Usage
.bxt
\Length of character \LENTRIM   \I&arrow.LENTRIM(CH)
\entity excluding    \          \
\trailing blanks     \
.bxt off
.*
.cp 10
.section Index of a Substring
.*
.ix 'intrinsic function' 'INDEX'
.ix 'INDEX'
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\Location of substring \INDEX \I&arrow.INDEX(CH,CH)
\a2 in string a1
.box off
.np
INDEX(x,y) returns the starting position of a substring in x which is
identical to y.
The position of the first such substring is returned.
If y is not contained in x, zero is returned.
.*
.cp 15
.section Imaginary Part of Complex Number
.*
.ix 'generic function' 'IMAG'
.ix 'intrinsic function' 'AIMAG'
.ix 'intrinsic function' 'DIMAG'
.ix 'AIMAG'
.ix 'DIMAG'
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\ai         \IMAG generic &dagger \R&arrow.IMAG(C)
\           \               \D&arrow.IMAG(Z)
\           \AIMAG          \R&arrow.AIMAG(C)
\           \DIMAG &dagger        \D&arrow.DIMAG(Z)
.box off
.im finote6
.*
.cp 15
.section Conjugate of a Complex Number
.*
.ix 'generic function' 'CONJG'
.ix 'intrinsic function' 'CONJG'
.ix 'intrinsic function' 'DCONJG'
.ix 'CONJG'
.ix 'DCONJD'
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\(ar,-ai)   \CONJG generic &dagger \C&arrow.CONJG(C)
\           \                \Z&arrow.CONJG(Z)
\           \CONJG           \C&arrow.CONJG(C)
\           \DCONJG &dagger        \Z&arrow.DCONJG(Z)
.box off
.im finote6
.*
.cp 15
.section Square Root
.*
.ix 'generic function' 'SQRT'
.ix 'intrinsic function' 'SQRT'
.ix 'intrinsic function' 'DSQRT'
.ix 'intrinsic function' 'CSQRT'
.ix 'intrinsic function' 'CDSQRT'
.ix 'SQRT'
.ix 'DSQRT'
.ix 'CSQRT'
.ix 'CDSQRT'
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\a**1/2 \SQRT generic \R&arrow.SQRT(R)
\       \             \D&arrow.SQRT(D)
\       \             \C&arrow.SQRT(C)
\       \             \Z&arrow.SQRT(Z) &dagger\
\       \SQRT         \R&arrow.SQRT(R)
\       \DSQRT        \D&arrow.DSQRT(D)
\       \CSQRT        \C&arrow.CSQRT(C)
\       \CDSQRT &dagger     \Z&arrow.CDSQRT(Z)
.box off
The argument to SQRT must be >= 0.
The result of CSQRT and CDSQRT is the principal value with the
real part >= 0.
When the real part of the result is 0, the imaginary part is >= 0.
.*
.cp 15
.section Exponential
.*
.ix 'generic function' 'EXP'
.ix 'intrinsic function' 'EXP'
.ix 'intrinsic function' 'DEXP'
.ix 'intrinsic function' 'CEXP'
.ix 'intrinsic function' 'CDEXP'
.ix 'EXP'
.ix 'DEXP'
.ix 'CEXP'
.ix 'CDEXP'
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\e**a \EXP generic \R&arrow.EXP(R)
\     \            \D&arrow.EXP(D)
\     \            \C&arrow.EXP(C)
\     \            \Z&arrow.EXP(Z) &dagger\
\     \EXP         \R&arrow.EXP(R)
\     \DEXP        \D&arrow.DEXP(D)
\     \CEXP        \C&arrow.CEXP(C)
\     \CDEXP &dagger     \Z&arrow.CDEXP(Z)
.box off
.im finote8
.*
.cp 15
.section Natural Logarithm
.*
.ix 'generic function' 'LOG'
.ix 'intrinsic function' 'ALOG'
.ix 'intrinsic function' 'DLOG'
.ix 'intrinsic function' 'CLOG'
.ix 'intrinsic function' 'CDLOG'
.ix 'ALOG'
.ix 'DLOG'
.ix 'CLOG'
.ix 'CDLOG'
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\log (a) \LOG generic \R&arrow.LOG(R)
\    e   \            \D&arrow.LOG(D)
\        \            \C&arrow.LOG(C)
\        \            \Z&arrow.LOG(Z) &dagger\
\        \ALOG        \R&arrow.ALOG(R)
\        \DLOG        \D&arrow.DLOG(D)
\        \CLOG        \C&arrow.CLOG(C)
\        \CDLOG &dagger     \Z&arrow.CDLOG(Z)
.box off
.np
The value of
.id a
must be > 0.
The argument of CLOG and CDLOG must not be (0,0).
The result of CLOG and CDLOG is such that -&pi. < imaginary part of
the result <= &pi..
The imaginary part of the result is &pi. only when the real part of the
argument is < 0 and the imaginary part of the argument = 0.
.im finote8
.*
.cp 15
.section Common Logarithm
.*
.ix 'generic function' 'LOG10'
.ix 'intrinsic function' 'ALOG10'
.ix 'intrinsic function' 'DLOG10'
.ix 'ALOG10'
.ix 'DLOG10'
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\log  (a) \LOG10 generic \R&arrow.LOG10(R)
\   10    \              \D&arrow.LOG10(D)
\         \ALOG10        \R&arrow.ALOG10(R)
\         \DLOG10        \D&arrow.DLOG10(D)
.box off
.*
.cp 20
.section Sine
.*
.ix 'generic function' 'SIN'
.ix 'intrinsic function' 'SIN'
.ix 'intrinsic function' 'DSIN'
.ix 'intrinsic function' 'CSIN'
.ix 'intrinsic function' 'CDSIN'
.ix 'SIN'
.ix 'DSIN'
.ix 'CSIN'
.ix 'CDSIN'
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\sin(a) \SIN generic \R&arrow.SIN(R)
\       \            \D&arrow.SIN(D)
\       \            \C&arrow.SIN(C)
\       \            \Z&arrow.SIN(Z) &dagger\
\       \SIN         \R&arrow.SIN(R)
\       \DSIN        \D&arrow.DSIN(D)
\       \CSIN        \C&arrow.CSIN(C)
\       \CDSIN &dagger     \Z&arrow.CDSIN(Z)
.box off
.im finote7
.im finote8
.*
.cp 20
.section Cosine
.*
.ix 'generic function' 'COS'
.ix 'intrinsic function' 'COS'
.ix 'intrinsic function' 'DCOS'
.ix 'intrinsic function' 'CCOS'
.ix 'intrinsic function' 'CDCOS'
.ix 'COS'
.ix 'DCOS'
.ix 'CCOS'
.ix 'CDCOS'
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\cos(a) \COS generic \R&arrow.COS(R)
\       \            \D&arrow.COS(D)
\       \            \C&arrow.COS(C)
\       \            \Z&arrow.COS(Z) &dagger\
\       \COS         \R&arrow.COS(R)
\       \DCOS        \D&arrow.DCOS(D)
\       \CCOS        \C&arrow.CCOS(C)
\       \CDCOS &dagger     \Z&arrow.CDCOS(Z)
.box off
.im finote7
.im finote8
.*
.cp 15
.section Tangent
.*
.ix 'generic function' 'TAN'
.ix 'intrinsic function' 'TAN'
.ix 'intrinsic function' 'DTAN'
.ix 'TAN'
.ix 'DTAN'
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\tan(a)     \TAN generic     \R&arrow.TAN(R)
\           \                \D&arrow.TAN(D)
\           \TAN             \R&arrow.TAN(R)
\           \DTAN            \D&arrow.DTAN(D)
.box off
.im finote7
.*
.cp 15
.section Cotangent
.*
.ix 'generic function' 'COTAN'
.ix 'intrinsic function' 'COTAN'
.ix 'intrinsic function' 'DCOTAN'
.ix 'COTAN'
.ix 'DCOTAN'
.bxt on &c0 &c3 &c1 &c2
\Definition\Name\Usage
.bxt
\cotan(a)   \COTAN generic &dagger \R&arrow.COTAN(R)
\           \                \D&arrow.COTAN(D)
\           \COTAN &dagger         \R&arrow.COTAN(R)
\           \DCOTAN &dagger        \D&arrow.DCOTAN(D)
.bxt off
.im finote7
.*
.cp 15
.section Arcsine
.*
.ix 'generic function' 'ASIN'
.ix 'intrinsic function' 'ASIN'
.ix 'intrinsic function' 'DASIN'
.ix 'ASIN'
.ix 'DASIN'
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\arcsin(a)  \ASIN generic    \R&arrow.ASIN(R)
\           \                \D&arrow.ASIN(D)
\           \ASIN            \R&arrow.ASIN(R)
\           \DASIN           \D&arrow.DASIN(D)
.box off
.np
The absolute value of the argument of ASIN and DASIN must be <= 1.
The result is such that -&pi./2 <= result <= &pi./2.
.*
.cp 15
.section Arccosine
.*
.ix 'generic function' 'ACOS'
.ix 'intrinsic function' 'ACOS'
.ix 'intrinsic function' 'DACOS'
.ix 'ACOS'
.ix 'DACOS'
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\arccos(a)  \ACOS generic    \R&arrow.ACOS(R)
\           \                \D&arrow.ACOS(D)
\           \ACOS            \R&arrow.ACOS(R)
\           \DACOS           \D&arrow.DACOS(D)
.box off
.np
The absolute value of the argument of ACOS and DACOS must be <= 1.
The result is such that 0 <= result <= &pi..
.*
.cp 15
.section Arctangent
.*
.ix 'generic function' 'ATAN'
.ix 'generic function' 'ATAN2'
.ix 'intrinsic function' 'ATAN'
.ix 'intrinsic function' 'DATAN'
.ix 'intrinsic function' 'ATAN2'
.ix 'intrinsic function' 'DATAN2'
.ix 'ATAN'
.ix 'DATAN'
.ix 'ATAN2'
.ix 'DATAN2'
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\arctan(a)     \ATAN generic  \R&arrow.ATAN(R)
\              \              \D&arrow.ATAN(D)
\              \ATAN          \R&arrow.ATAN(R)
\              \DATAN         \D&arrow.DATAN(D)
\arctan(a1/a2) \ATAN2 generic \R&arrow.ATAN2(R,R)
\              \              \D&arrow.ATAN2(D,D)
\              \ATAN2         \R&arrow.ATAN2(R,R)
\              \DATAN2        \D&arrow.DATAN2(D,D)
.box off
.np
The result of ATAN and DATAN is such that -&pi./2 <= result <= &pi./2.
If the value of the first argument of ATAN2 and DATAN2 is positive
then the result is positive.
If the value of the first argument is 0, the result is 0 if the second
argument is positive and &pi. if the second argument is negative.
If the value of the first argument is negative, the result is negative.
If the value of the second argument is 0, the absolute value of the
result is &pi./2.
The arguments must not both be 0.
The result of ATAN2 and DATAN2 is such that -&pi. < result <= &pi..
.*
.cp 15
.section Hyperbolic Sine
.*
.ix 'generic function' 'SINH'
.ix 'intrinsic function' 'SINH'
.ix 'intrinsic function' 'DSINH'
.ix 'SINH'
.ix 'DSINH'
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\sinh(a)    \SINH generic    \R&arrow.SINH(R)
\           \                \D&arrow.SINH(D)
\           \SINH            \R&arrow.SINH(R)
\           \DSINH           \D&arrow.DSINH(D)
.box off
.*
.cp 15
.section Hyperbolic Cosine
.*
.ix 'generic function' 'COSH'
.ix 'intrinsic function' 'COSH'
.ix 'intrinsic function' 'DCOSH'
.ix 'COSH'
.ix 'DCOSH'
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\cosh(a)    \COSH generic    \R&arrow.COSH(R)
\           \                \D&arrow.COSH(D)
\           \COSH            \R&arrow.COSH(R)
\           \DCOSH           \D&arrow.DCOSH(D)
.box off
.*
.cp 15
.section Hyperbolic Tangent
.*
.ix 'generic function' 'TANH'
.ix 'intrinsic function' 'TANH'
.ix 'intrinsic function' 'DTANH'
.ix 'TANH'
.ix 'DTANH'
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\tanh(a)    \TANH generic    \R&arrow.TANH(R)
\           \                \D&arrow.TANH(D)
\           \TANH            \R&arrow.TANH(R)
\           \DTANH           \D&arrow.DTANH(D)
.box off
.*
.cp 15
.section Gamma Function
.*
.ix 'generic function' 'GAMMA'
.ix 'intrinsic function' 'GAMMA'
.ix 'intrinsic function' 'DGAMMA'
.ix 'GAMMA'
.ix 'DGAMMA'
.bxt on &c0 &c3 &c1 &c2
\Definition\Name\Usage
.bxt
\gamma(a)   \GAMMA generic   \R&arrow.GAMMA(R)
\           \                \D&arrow.GAMMA(D)
\           \GAMMA           \R&arrow.GAMMA(R)
\           \DGAMMA          \D&arrow.DGAMMA(D)
.bxt off
.*
.cp 15
.section Natural Log of Gamma Function
.*
.ix 'generic function' 'GAMMA'
.ix 'intrinsic function' 'ALGAMA'
.ix 'intrinsic function' 'DLGAMA'
.ix 'GAMMA'
.ix 'ALGAMA'
.ix 'DLGAMA'
.bxt on &c0 &c3 &c1 &c2
\Definition\Name\Usage
.bxt
\log (gamma(a))     \LGAMMA generic  \R&arrow.LGAMMA(R)
\    e              \                \D&arrow.LGAMMA(D)
\                   \ALGAMA          \R&arrow.ALGAMA(R)
\                   \DLGAMA          \D&arrow.DLGAMA(D)
.bxt off
.*
.cp 15
.section Error Function
.*
.ix 'generic function' 'ERF'
.ix 'intrinsic function' 'ERF'
.ix 'intrinsic function' 'DERF'
.ix 'ERF'
.ix 'DERF'
.bxt on &c0 &c3 &c1 &c2
\Definition\Name\Usage
.bxt
\erf(a)     \ERF generic     \R&arrow.ERF(R)
\           \                \D&arrow.ERF(D)
\           \ERF             \R&arrow.ERF(R)
\           \DERF            \D&arrow.DERF(D)
.bxt off
.*
.cp 15
.section Complement of Error Function
.*
.ix 'generic function' 'ERFC'
.ix 'intrinsic function' 'ERFC'
.ix 'intrinsic function' 'DERFC'
.ix 'ERFC'
.ix 'DERFC'
.bxt on &c0 &c3 &c1 &c2
\Definition\Name\Usage
.bxt
\1-erf(a)   \ERFC generic    \R&arrow.ERFC(R)
\           \                \D&arrow.ERFC(D)
\           \ERFC            \R&arrow.ERFC(R)
\           \DERFC           \D&arrow.DERFC(D)
.bxt off
.*
.cp 9
.section Lexically Greater Than or Equal
.*
.ix 'intrinsic function' 'LGE'
.ix 'LGE'
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\a1>=a2     \LGE     \L&arrow.LGE(CH,CH)
.box off
.im finote9
.*
.cp 9
.section Lexically Greater Than
.*
.ix 'intrinsic function' 'LGT'
.ix 'LGT'
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\a1>a2      \LGT     \L&arrow.LGT(CH,CH)
.box off
.im finote9
.*
.cp 9
.section Lexically Less Than or Equal
.*
.ix 'intrinsic function' 'LLE'
.ix 'LLE'
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\a1<=a2     \LLE     \L&arrow.LLE(CH,CH)
.box off
.im finote9
.*
.cp 9
.section Lexically Less Than
.*
.ix 'intrinsic function' 'LLT'
.ix 'LLT'
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\a1<a2      \LLT     \L&arrow.LLT(CH,CH)
.box off
.im finote9
.*
.cp 17
.section Binary Pattern Processing Functions: Boolean Operations
.*
.ix 'intrinsic function' 'IAND'
.ix 'IAND'
.ix 'intrinsic function' 'IOR'
.ix 'IOR'
.ix 'intrinsic function' 'IEOR'
.ix 'IEOR'
.ix 'intrinsic function' 'NOT'
.ix 'NOT'
.bxt on &c0 &c3 &c1 &c2
\Definition\Name\Usage
.bxt
\Boolean AND        \IAND            \I&arrow.IAND(I,I)
.bxt
\Boolean inclusive OR       \IOR     \I&arrow.IOR(I,I)
.bxt
\Boolean exclusive OR       \IEOR    \I&arrow.IEOR(I,I)
.bxt
\Boolean complement \NOT             \I&arrow.NOT(I)
.bxt off
.*
.cp 15
.section Binary Pattern Processing Functions: Shift Operations
.*
.ix 'intrinsic function' 'ISHL'
.ix 'intrinsic function' 'ISHFT'
.ix 'ISHL'
.ix 'ISHFT'
.ix 'intrinsic function' 'ISHA'
.ix 'ISHA'
.ix 'intrinsic function' 'ISHC'
.ix 'ISHC'
.bxt on &c0 &c3 &c1 &c2
\Definition\Name\Usage
.bxt
\Logical shift      \ISHL            \I&arrow.ISHL(I,I)
\                   \ISHFT           \I&arrow.ISHFT(I,I)
.bxt
\Arithmetic shift   \ISHA            \I&arrow.ISHA(I,I)
.bxt
\Circular shift     \ISHC            \I&arrow.ISHC(I,I)
.bxt off
.im finote10
.*
.cp 15
.section Binary Pattern Processing Functions: Bit Testing
.*
.ix 'intrinsic function' 'BTEST'
.ix 'BTEST'
.bxt on &c0 &c3 &c1 &c2
\Definition\Name\Usage
.bxt
\Test bit           \BTEST           \L&arrow.BTEST(I,I)
\a2'th bit of a1 is tested.\
\If it is 1, .TRUE. is  \
\returned.              \
\If it is 0, .FALSE. is \
\returned.
.bxt off
.*
.cp 10
.section Binary Pattern Processing Functions: Set Bit
.*
.ix 'intrinsic function' 'IBSET'
.ix 'IBSET'
.bxt on &c0 &c3 &c1 &c2
\Definition\Name\Usage
.bxt
\Set bit            \IBSET           \I&arrow.IBSET(I,I)
\Return a1 with a2'th  \
\bit set               \
.bxt off
.*
.cp 10
.section Binary Pattern Processing Functions: Clear Bit
.*
.ix 'intrinsic function' 'IBCLR'
.ix 'IBCLR'
.bxt on &c0 &c3 &c1 &c2
\Definition\Name\Usage
.bxt
\Clear bit          \IBCLR           \I&arrow.IBCLR(I,I)
\Return a1 with a2'th \
\bit cleared          \
.bxt off
.*
.cp 10
.section Binary Pattern Processing Functions: Change Bit
.*
.ix 'intrinsic function' 'IBCHNG'
.ix 'IBCHNG'
.bxt on &c0 &c3 &c1 &c2
\Definition\Name\Usage
.bxt
\Change bit         \IBCHNG          \I&arrow.IBCHNG(I,I)
\Return a1 with a2'th \
\bit complemented     \
.bxt off
.*
.cp 10
.section Allocated Array
.*
.ix 'intrinsic function' 'ALLOCATED'
.ix 'ALLOCATED'
.bxt on &c0 &c3 &c1 &c2
\Definition\Name\Usage
.bxt
\Is array A allocated?      \ALLOCATED       \L&arrow.ALLOCATED(A)
.bxt off
.*
.cp 10
.section Memory Location
.*
.ix 'intrinsic function' 'LOC'
.ix 'LOC'
.bxt on &c0 &c3 &c1 &c2
\Definition\Name\Usage
.bxt
\location of A where      \LOC  \I&arrow.LOC(A)
\A is any variable, array \
\or array element         \
.bxt off
.*
.cp 10
.section Size of Variable or Structure
.*
.ix 'intrinsic function' 'ISIZEOF'
.ix 'ISIZEOF'
.bxt on &c0 &c3 &c1 &c2
\Definition\Name\Usage
.bxt
\size of A in bytes where \ISIZEOF  \I&arrow.ISIZEOF(A)
\A is any constant,       \
\variable, array, or      \
\structure                \
.bxt off
.np
The size reported for a constant or simple variable is based on its
type.
The size of a CHARACTER constant is the number of characters in the
constant.
The size reported for an array is the size of the storage area
required for the array.
The size reported for a structure is the size of the storage area
required for the structure.
An assumed-size CHARACTER variable, assumed-size array, or allocatable
array has size 0.
.*
.cp 15
.section Volatile Reference
.*
.ix 'intrinsic function' 'VOLATILE'
.ix 'VOLATILE'
.bxt on &c0 &c3 &c1 &c2
\Definition\Name\Usage
.bxt
\A is a volatile          \VOLATILE  \A&arrow.VOLATILE(A)
\reference                \
.bxt off
.np
A volatile reference to a symbol indicates that the value of the symbol
may be modified in ways that are unknown to the subprogram.
For example, a symbol in common being referenced in a subprogram may be
modified by another subprogram that is processing an asynchronous interrupt.
Therefore, any subprogram that is referencing the symbol to determine its
value should reference this symbol using the
.id VOLATILE
intrinsic function so that the value currently being evaluated agrees with
the value last stored.
