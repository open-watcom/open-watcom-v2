.if &e'&dohelp eq 0 .do begin
:set symbol="arrow"   value=";.sf7;.ct ~L;.esf;.ct ".
:set symbol="dagref"  value="a dagger".
:set symbol="generic" value="(generic)".
.dm nameuse begin
.   .note Name:
.   .bi Usage:
.dm nameuse end
.do end
.el .do begin
:set symbol="arrow"   value="<-".
:set symbol="dagger"  value="!".
:set symbol="dagref"  value="an exclamation mark".
:set symbol="pi"      value="pi".
:set symbol="generic" value="(g)".
.dm nameuse begin
.   .note Name / Usage:
.dm nameuse end
.do end
:cmt. - is a minus
.np
The following sections present all generic and specific names of
intrinsic functions and describe how they are used.
The following is a guide to interpreting the information presented.
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
.xt INTEGER*1 is represented by I1.
.point
.xt INTEGER*2 is represented by I2.
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
The "Definition" description gives the mathematical definition of the
function performed by the intrinsic function.
There are two fields for each intrinsic function.
The "Name" field lists the specific and generic names of the intrinsic
functions.
When the name of an intrinsic function is a generic name, it is
indicated by
.if &e'&dohelp eq 0 .do begin
the word "generic"
.do end
.el .do begin
the letter "g"
.do end
in parentheses;
all other names are specific names.
The "Usage" field describes how the intrinsic functions are used.
"R&arrow.ATAN2(R,R)" is a typical entry in this field.
The name of the intrinsic function always follows the "&arrow.".
In this example the name of the intrinsic function is ATAN2.
The data type of the arguments to the intrinsic function are enclosed
in parentheses, are separated by commas, and always follow the name of
the intrinsic function.
In this case, ATAN2 requires two arguments both of type REAL.
The type of the result of the intrinsic function is indicated by the
type preceding the "&arrow.".
In this case, the result of ATAN2 is of type REAL.
.np
&product extensions to the FORTRAN 77 language are flagged by
&dagref. (&dagger.).
.*
.cp 18
.section Type Conversion: Conversion to integer
.*
.begnote $setptnt 12
.note Definition:
.mono int(a)
.nameuse
.ix 'generic function' 'INT'
.ix 'intrinsic function' 'INT'
.ix 'intrinsic function' 'IFIX'
.ix 'intrinsic function' 'IDINT'
.ix 'intrinsic function' 'HFIX'
.ix 'INT'
.ix 'IFIX'
.ix 'IDINT'
.ix 'HFIX'
.note INT &generic.
I&arrow.INT(I),
I&arrow.INT(R),
I&arrow.INT(D),
I&arrow.INT(C),
I&arrow.INT(Z) &dagger.
.note INT
I&arrow.INT(R)
.note HFIX
I2&arrow.HFIX(R) &dagger.
.note IFIX
I&arrow.IFIX(R)
.note IDINT
I&arrow.IDINT(D)
.note Notes:
.im finote1
.np
&dagger. is an extension to FORTRAN 77.
.endnote
.*
.cp 18
.section Type Conversion: Conversion to real
.*
.begnote $setptnt 12
.nameuse
.ix 'generic function' 'REAL'
.ix 'intrinsic function' 'REAL'
.ix 'intrinsic function' 'FLOAT'
.ix 'intrinsic function' 'SNGL'
.ix 'REAL'
.ix 'FLOAT'
.ix 'SNGL'
.note REAL &generic.
R&arrow.REAL(I),
R&arrow.REAL(R),
R&arrow.REAL(D),
R&arrow.REAL(C),
R&arrow.REAL(Z) &dagger.
.note REAL
R&arrow.REAL(I)
.note FLOAT
R&arrow.FLOAT(I)
.note SNGL
R&arrow.SNGL(D)
.note Notes:
.im finote2
.np
&dagger. is an extension to FORTRAN 77.
.endnote
.*
.cp 18
.section Type Conversion: Conversion to double precision
.*
.begnote $setptnt 12
.nameuse
.ix 'generic function' 'DBLE'
.ix 'intrinsic function' 'DBLE'
.ix 'intrinsic function' 'DREAL'
.ix 'intrinsic function' 'DFLOAT'
.ix 'DBLE'
.ix 'DREAL'
.ix 'DFLOAT'
.note DBLE &generic.
D&arrow.DBLE(I),
D&arrow.DBLE(R),
D&arrow.DBLE(D),
D&arrow.DBLE(C),
D&arrow.DBLE(Z) &dagger.
.note DREAL
D&arrow.DREAL(Z) &dagger.
.note DFLOAT
D&arrow.DFLOAT(I) &dagger.
.note Notes:
.im finote3
.np
&dagger. is an extension to FORTRAN 77.
.endnote
.*
.cp 18
.section Type Conversion: Conversion to complex
.*
.begnote $setptnt 12
.nameuse
.ix 'generic function' 'CMPLX'
.ix 'intrinsic function' 'CMPLX'
.ix 'CMPLX'
.note CMPLX &generic.
C&arrow.CMPLX(I), C&arrow.CMPLX(I,I),
C&arrow.CMPLX(R), C&arrow.CMPLX(R,R),
C&arrow.CMPLX(D), C&arrow.CMPLX(D,D),
C&arrow.CMPLX(C),
C&arrow.CMPLX(Z) &dagger.
.note Notes:
.im finote4a
.np
&dagger. is an extension to FORTRAN 77.
.endnote
.*
.cp 18
.section Type Conversion: Conversion to double complex
.*
.begnote $setptnt 12
.nameuse
.ix 'generic function' 'DCMPLX'
.ix 'intrinsic function' 'DCMPLX'
.ix 'DCMPLX'
.note DCMPLX &generic. &dagger.
Z&arrow.DCMPLX(I), Z&arrow.DCMPLX(I,I),
Z&arrow.DCMPLX(R), Z&arrow.DCMPLX(R,R),
Z&arrow.DCMPLX(D), Z&arrow.DCMPLX(D,D),
Z&arrow.DCMPLX(C),
Z&arrow.DCMPLX(Z)
.note Notes:
.im finote4b
.np
&dagger. is an extension to FORTRAN 77.
.endnote
.*
.cp 18
.section Type Conversion: Character conversion to integer
.*
.begnote $setptnt 12
.nameuse
.ix 'intrinsic function' 'ICHAR'
.ix 'ICHAR'
.note ICHAR
I&arrow.ICHAR(CH)
.note Notes:
.im finote5a
.endnote
.*
.cp 18
.section Type Conversion: Conversion to character
.*
.begnote $setptnt 12
.nameuse
.ix 'intrinsic function' 'CHAR'
.ix 'CHAR'
.note CHAR
CH&arrow.CHAR(I)
.note Notes:
.im finote5b
.endnote
.*
.cp 18
.section Truncation
.*
.begnote $setptnt 12
.note Definition:
.mono int(a)
.nameuse
.ix 'generic function' 'AINT'
.ix 'intrinsic function' 'AINT'
.ix 'intrinsic function' 'DINT'
.ix 'AINT'
.ix 'DINT'
.note AINT &generic.
R&arrow.AINT(R),
D&arrow.AINT(D)
.note AINT
R&arrow.AINT(R)
.note DINT
D&arrow.DINT(D)
.note Notes:
.im finote1
.endnote
.*
.cp 18
.section Nearest Whole Number
.*
.begnote $setptnt 12
.note Definition:
.mono int(a+.5)
if a>=0;
.mono int(a-.5)
if a<0
.nameuse
.ix 'generic function' 'ANINT'
.ix 'intrinsic function' 'ANINT'
.ix 'intrinsic function' 'DNINT'
.ix 'ANINT'
.ix 'DNINT'
.note ANINT &generic.
R&arrow.ANINT(R),
D&arrow.ANINT(D)
.note ANINT
R&arrow.ANINT(R)
.note DNINT
D&arrow.DNINT(D)
.endnote
.*
.cp 18
.section Nearest Integer
.*
.begnote $setptnt 12
.note Definition:
.mono int(a+.5)
if a>=0;
.mono int(a-.5)
if a<0
.nameuse
.ix 'generic function' 'NINT'
.ix 'intrinsic function' 'NINT'
.ix 'intrinsic function' 'IDNINT'
.ix 'NINT'
.ix 'IDNINT'
.note NINT &generic.
I&arrow.NINT(R),
I&arrow.NINT(D)
.note NINT
I&arrow.NINT(R)
.note IDNINT
I&arrow.IDNINT(D)
.endnote
.*
.cp 18
.section Absolute Value
.*
.begnote $setptnt 12
.note Definition:
.mono (ar**2+ai**2)**1/2
if a is complex;
.mono |a|
otherwise
.nameuse
.ix 'generic function' 'ABS'
.ix 'intrinsic function' 'IABS'
.ix 'intrinsic function' 'I1ABS'
.ix 'intrinsic function' 'I2ABS'
.ix 'intrinsic function' 'ABS'
.ix 'intrinsic function' 'DABS'
.ix 'intrinsic function' 'CABS'
.ix 'intrinsic function' 'CDABS'
.ix 'ABS'
.ix 'IABS'
.ix 'I1ABS'
.ix 'I2ABS'
.ix 'DABS'
.ix 'CABS'
.ix 'CDABS'
.note ABS &generic.
I&arrow.ABS(I),
I1&arrow.ABS(I1) &dagger.,
I2&arrow.ABS(I2) &dagger.,
R&arrow.ABS(R),
D&arrow.ABS(D),
R&arrow.ABS(C),
D&arrow.ABS(Z) &dagger.
.note IABS
I&arrow.IABS(I)
.note I1ABS
I1&arrow.I1ABS(I1) &dagger.
.note I2ABS
I2&arrow.I2ABS(I2) &dagger.
.note ABS
R&arrow.ABS(R)
.note DABS
D&arrow.DABS(D)
.note CABS
R&arrow.CABS(C)
.note CDABS &dagger.
D&arrow.CDABS(Z)
.note Notes:
.im finote6
.np
&dagger. is an extension to FORTRAN 77.
.endnote
.*
.cp 18
.section Remainder
.*
.begnote $setptnt 12
.note Definition:
.mono mod(a1,a2) = a1-int(a1/a2)*a2
.nameuse
.ix 'generic function' 'MOD'
.ix 'intrinsic function' 'MOD'
.ix 'intrinsic function' 'I1MOD'
.ix 'intrinsic function' 'I2MOD'
.ix 'intrinsic function' 'AMOD'
.ix 'intrinsic function' 'DMOD'
.ix 'MOD'
.ix 'I1MOD'
.ix 'I2MOD'
.ix 'AMOD'
.ix 'DMOD'
.note MOD &generic.
I&arrow.MOD(I,I),
I1&arrow.MOD(I1,I1) &dagger.,
I2&arrow.MOD(I2,I2) &dagger.,
R&arrow.MOD(R,R),
D&arrow.MOD(D,D),
.note MOD
I&arrow.MOD(I,I)
.note I1MOD
I1&arrow.I1MOD(I1,I1) &dagger.
.note I2MOD
I2&arrow.I2MOD(I2,I2) &dagger.
.note AMOD
R&arrow.AMOD(R,R)
.note DMOD
D&arrow.DMOD(D,D)
.note Notes:
.im finote1
.np
The value of MOD, I1MOD, I2MOD, AMOD or DMOD is undefined if the
value of a2 is 0.
.endnote
.*
.cp 18
.section Transfer of Sign
.*
.begnote $setptnt 12
.note Definition:
.mono sign(a1,a2) = |a1|
if a2>=0;
.mono -|a1|
if a2<0
.nameuse
.ix 'generic function' 'SIGN'
.ix 'intrinsic function' 'ISIGN'
.ix 'intrinsic function' 'I1SIGN'
.ix 'intrinsic function' 'I2SIGN'
.ix 'intrinsic function' 'SIGN'
.ix 'intrinsic function' 'DSIGN'
.ix 'ISIGN'
.ix 'I1SIGN'
.ix 'I2SIGN'
.ix 'SIGN'
.ix 'DSIGN'
.note SIGN &generic.
I&arrow.SIGN(I,I),
I1&arrow.SIGN(I1,I1) &dagger.,
I2&arrow.SIGN(I2,I2) &dagger.,
R&arrow.SIGN(R,R),
D&arrow.SIGN(D,D)
.note ISIGN
I&arrow.ISIGN(I,I)
.note I1SIGN
I1&arrow.I1SIGN(I1,I1) &dagger.
.note I2SIGN
I2&arrow.I2SIGN(I2,I2) &dagger.
.note SIGN
R&arrow.SIGN(R,R)
.note DSIGN
D&arrow.DSIGN(D,D)
.note Notes:
If the value of a1 is 0, the result is 0 which has no sign.
.endnote
.*
.cp 18
.section Positive Difference
.*
.begnote $setptnt 12
.note Definition:
.mono a1-a2
if a1>a2;
.mono 0
if a1<=a2
.nameuse
.ix 'generic function' 'DIM'
.ix 'intrinsic function' 'IDIM'
.ix 'intrinsic function' 'I1DIM'
.ix 'intrinsic function' 'I2DIM'
.ix 'intrinsic function' 'DIM'
.ix 'intrinsic function' 'DDIM'
.ix 'IDIM'
.ix 'I1DIM'
.ix 'I2DIM'
.ix 'DIM'
.ix 'DDIM'
.note DIM &generic.
I&arrow.DIM(I,I),
I1&arrow.DIM(I1,I1) &dagger.,
I2&arrow.DIM(I2,I2) &dagger.,
R&arrow.DIM(R,R),
D&arrow.DIM(D,D)
.note IDIM
I&arrow.IDIM(I,I)
.note I1IDIM
I1&arrow.I1DIM(I1,I1) &dagger.
.note I2IDIM
I2&arrow.I2DIM(I2,I2) &dagger.
.note DIM
R&arrow.DIM(R,R)
.note DDIM
D&arrow.DDIM(D,D)
.endnote
.*
.cp 18
.section Double Precision Product
.*
.begnote $setptnt 12
.note Definition:
.mono a1*a2
.nameuse
.ix 'intrinsic function' 'DPROD'
.ix 'DPROD'
.note DPROD
D&arrow.DPROD(R,R)
.endnote
.*
.cp 18
.section Choosing Largest Value
.*
.begnote $setptnt 12
.note Definition:
.mono max(a1,a2,...)
.nameuse
.ix 'generic function' 'MAX'
.ix 'intrinsic function' 'MAX0'
.ix 'intrinsic function' 'I1MAX0'
.ix 'intrinsic function' 'I2MAX0'
.ix 'intrinsic function' 'AMAX1'
.ix 'intrinsic function' 'DMAX1'
.ix 'intrinsic function' 'AMAX0'
.ix 'intrinsic function' 'MAX1'
.ix 'MAX0'
.ix 'I1MAX0'
.ix 'I2MAX0'
.ix 'AMAX1'
.ix 'DMAX1'
.ix 'AMAX0'
.ix 'MAX1'
.note MAX &generic.
I&arrow.MAX(I,...),
I1&arrow.MAX(I1,...) &dagger.,
I2&arrow.MAX(I2,...) &dagger.,
R&arrow.MAX(R,...),
D&arrow.MAX(D,...)
.note MAX0
I&arrow.MAX0(I,...)
.note I1MAX0
I1&arrow.I1MAX0(I1,...) &dagger.
.note I2MAX0
I2&arrow.I2MAX0(I2,...) &dagger.
.note AMAX1
R&arrow.AMAX1(R,...)
.note DMAX1
D&arrow.DMAX1(D,...)
.note AMAX0
R&arrow.AMAX0(I,...)
.note MAX1
I&arrow.MAX1(R,...)
.endnote
.*
.cp 18
.section Choosing Smallest Value
.*
.begnote $setptnt 12
.note Definition:
.mono min(a1,a2,...)
.nameuse
.ix 'generic function' 'MIN'
.ix 'intrinsic function' 'MIN0'
.ix 'intrinsic function' 'I1MIN0'
.ix 'intrinsic function' 'I2MIN0'
.ix 'intrinsic function' 'AMIN1'
.ix 'intrinsic function' 'DMIN1'
.ix 'intrinsic function' 'AMIN0'
.ix 'intrinsic function' 'MIN1'
.ix 'MIN0'
.ix 'I1MIN0'
.ix 'I2MIN0'
.ix 'AMIN1'
.ix 'DMIN1'
.ix 'AMIN0'
.ix 'MIN1'
.note MIN &generic.
I&arrow.MIN(I,...),
I1&arrow.MIN(I1,...) &dagger.,
I2&arrow.MIN(I2,...) &dagger.,
R&arrow.MIN(R,...),
D&arrow.MIN(D,...)
.note MIN0
I&arrow.MIN0(I,...)
.note I1MIN0
I1&arrow.I1MIN0(I1,...) &dagger.
.note I2MIN0
I2&arrow.I2MIN0(I2,...) &dagger.
.note AMIN1
R&arrow.AMIN1(R,...)
.note DMIN1
D&arrow.DMIN1(D,...)
.note AMIN0
R&arrow.AMIN0(I,...)
.note MIN1
I&arrow.MIN1(R,...)
.endnote
.*
.cp 18
.section Length
.*
.begnote $setptnt 12
.note Definition:
Length of character entity
.nameuse
.ix 'intrinsic function' 'LEN'
.ix 'LEN'
.note LEN
I&arrow.LEN(CH)
.note Notes:
The argument to the LEN function need not be defined.
.endnote
.*
.cp 18
.section Length Without Trailing Blanks
.*
.begnote $setptnt 12
.note Definition:
Length of character entity excluding trailing blanks
.nameuse
.ix 'intrinsic function' 'LENTRIM'
.ix 'LENTRIM'
.note LENTRIM
I&arrow.LENTRIM(CH)
.endnote
.*
.cp 18
.section Index of a Substring
.*
.begnote $setptnt 12
.note Definition:
.mono index(a1,a2)
is location of substring
.mono a2
in string
.mono a1
.nameuse
.ix 'intrinsic function' 'INDEX'
.ix 'INDEX'
.note INDEX
I&arrow.INDEX(CH,CH)
.note Notes:
INDEX(x,y) returns the starting position of a substring in x which is
identical to y.
The position of the first such substring is returned.
If y is not contained in x, zero is returned.
.endnote
.*
.cp 18
.section Imaginary Part of Complex Number
.*
.begnote $setptnt 12
.note Definition:
.mono ai
.nameuse
.ix 'generic function' 'IMAG'
.ix 'intrinsic function' 'AIMAG'
.ix 'intrinsic function' 'DIMAG'
.ix 'AIMAG'
.ix 'DIMAG'
.note IMAG &generic. &dagger.
R&arrow.IMAG(C),
D&arrow.IMAG(Z)
.note AIMAG
R&arrow.AIMAG(C)
.note DIMAG
D&arrow.DIMAG(Z) &dagger.
.note Notes:
.im finote6
.np
&dagger. is an extension to FORTRAN 77.
.endnote
.*
.cp 18
.section Conjugate of a Complex Number
.*
.begnote $setptnt 12
.note Definition:
.mono (ar,-ai)
.nameuse
.ix 'generic function' 'CONJG'
.ix 'intrinsic function' 'CONJG'
.ix 'intrinsic function' 'DCONJG'
.ix 'CONJG'
.ix 'DCONJD'
.note CONJG &generic. &dagger.
C&arrow.CONJG(C),
Z&arrow.CONJG(Z)
.note CONJG
C&arrow.CONJG(C)
.note DCONJG
Z&arrow.DCONJG(Z) &dagger.
.note Notes:
.im finote6
.np
&dagger. is an extension to FORTRAN 77.
.endnote
.*
.cp 18
.section Square Root
.*
.begnote $setptnt 12
.note Definition:
.mono a**1/2
.nameuse
.ix 'generic function' 'SQRT'
.ix 'intrinsic function' 'SQRT'
.ix 'intrinsic function' 'DSQRT'
.ix 'intrinsic function' 'CSQRT'
.ix 'intrinsic function' 'CDSQRT'
.ix 'SQRT'
.ix 'DSQRT'
.ix 'CSQRT'
.ix 'CDSQRT'
.note SQRT &generic.
R&arrow.SQRT(R),
D&arrow.SQRT(D),
C&arrow.SQRT(C),
Z&arrow.SQRT(Z) &dagger.
.note SQRT
R&arrow.SQRT(R)
.note DSQRT
D&arrow.DSQRT(D)
.note CSQRT
C&arrow.CSQRT(C)
.note CDSQRT
Z&arrow.CDSQRT(Z) &dagger.
.note Notes:
The argument to SQRT must be >= 0.
The result of CSQRT and CDSQRT is the principal value with the
real part >= 0.
When the real part of the result is 0, the imaginary part is >= 0.
.np
&dagger. is an extension to FORTRAN 77.
.endnote
.*
.cp 18
.section Exponential
.*
.begnote $setptnt 12
.note Definition:
.mono e**a
.nameuse
.ix 'generic function' 'EXP'
.ix 'intrinsic function' 'EXP'
.ix 'intrinsic function' 'DEXP'
.ix 'intrinsic function' 'CEXP'
.ix 'intrinsic function' 'CDEXP'
.ix 'EXP'
.ix 'DEXP'
.ix 'CEXP'
.ix 'CDEXP'
.note EXP &generic.
R&arrow.EXP(R),
D&arrow.EXP(D),
C&arrow.EXP(C),
Z&arrow.EXP(Z) &dagger.
.note EXP
R&arrow.EXP(R)
.note DEXP
D&arrow.DEXP(D)
.note CEXP
C&arrow.CEXP(C)
.note CDEXP
Z&arrow.CDEXP(Z) &dagger.
.note Notes:
.im finote8
.np
&dagger. is an extension to FORTRAN 77.
.endnote
.*
.cp 18
.section Natural Logarithm
.*
.begnote $setptnt 12
.note Definition:
.millust begin
log (a)
   e
.millust end
.nameuse
.ix 'generic function' 'LOG'
.ix 'intrinsic function' 'ALOG'
.ix 'intrinsic function' 'DLOG'
.ix 'intrinsic function' 'CLOG'
.ix 'intrinsic function' 'CDLOG'
.ix 'ALOG'
.ix 'DLOG'
.ix 'CLOG'
.ix 'CDLOG'
.note LOG &generic.
R&arrow.LOG(R),
D&arrow.LOG(D),
C&arrow.LOG(C),
Z&arrow.LOG(Z) &dagger.
.note ALOG
R&arrow.ALOG(R)
.note DLOG
D&arrow.DLOG(D)
.note CLOG
C&arrow.CLOG(C)
.note CDLOG
Z&arrow.CDLOG(Z) &dagger.
.note Notes:
The value of
.id a
must be > 0.
The argument of CLOG and CDLOG must not be (0,0).
The result of CLOG and CDLOG is such that -&pi. < imaginary part of
the result <= &pi..
The imaginary part of the result is &pi. only when the real part of the
argument is < 0 and the imaginary part of the argument = 0.
.np
.im finote8
.np
&dagger. is an extension to FORTRAN 77.
.endnote
.*
.cp 18
.section Common Logarithm
.*
.begnote $setptnt 12
.note Definition:
.millust begin
log  (a)
   10
.millust end
.nameuse
.ix 'generic function' 'LOG10'
.ix 'intrinsic function' 'ALOG10'
.ix 'intrinsic function' 'DLOG10'
.ix 'ALOG10'
.ix 'DLOG10'
.note LOG10 &generic.
R&arrow.LOG10(R),
D&arrow.LOG10(D)
.note ALOG10
R&arrow.ALOG10(R)
.note DLOG10
D&arrow.DLOG10(D)
.endnote
.*
.cp 18
.section Sine
.*
.begnote $setptnt 12
.note Definition:
.mono sin(a)
.nameuse
.ix 'generic function' 'SIN'
.ix 'intrinsic function' 'SIN'
.ix 'intrinsic function' 'DSIN'
.ix 'intrinsic function' 'CSIN'
.ix 'intrinsic function' 'CDSIN'
.ix 'SIN'
.ix 'DSIN'
.ix 'CSIN'
.ix 'CDSIN'
.note SIN &generic.
R&arrow.SIN(R),
D&arrow.SIN(D),
C&arrow.SIN(C),
Z&arrow.SIN(Z) &dagger.
.note SIN
R&arrow.SIN(R)
.note DSIN
D&arrow.DSIN(D)
.note CSIN
C&arrow.CSIN(C)
.note CDSIN
Z&arrow.CDSIN(Z) &dagger.
.note Notes:
.im finote7
.np
.im finote8
.np
&dagger. is an extension to FORTRAN 77.
.endnote
.*
.cp 18
.section Cosine
.*
.begnote $setptnt 12
.note Definition:
.mono cos(a)
.nameuse
.ix 'generic function' 'COS'
.ix 'intrinsic function' 'COS'
.ix 'intrinsic function' 'DCOS'
.ix 'intrinsic function' 'CCOS'
.ix 'intrinsic function' 'CDCOS'
.ix 'COS'
.ix 'DCOS'
.ix 'CCOS'
.ix 'CDCOS'
.note COS &generic.
R&arrow.COS(R),
D&arrow.COS(D),
C&arrow.COS(C),
Z&arrow.COS(Z) &dagger.
.note COS
R&arrow.COS(R)
.note DCOS
D&arrow.DCOS(D)
.note CCOS
C&arrow.CCOS(C)
.note CDCOS
Z&arrow.CDCOS(Z) &dagger.
.note Notes:
.im finote7
.np
.im finote8
.np
&dagger. is an extension to FORTRAN 77.
.endnote
.*
.cp 18
.section Tangent
.*
.begnote $setptnt 12
.note Definition:
.mono tan(a)
.nameuse
.ix 'generic function' 'TAN'
.ix 'intrinsic function' 'TAN'
.ix 'intrinsic function' 'DTAN'
.ix 'TAN'
.ix 'DTAN'
.note TAN &generic.
R&arrow.TAN(R),
D&arrow.TAN(D)
.note TAN
R&arrow.TAN(R)
.note DTAN
D&arrow.DTAN(D)
.note Notes:
.im finote7
.endnote
.*
.cp 18
.section Cotangent
.*
.begnote $setptnt 12
.note Definition:
.mono cotan(a)
.nameuse
.ix 'generic function' 'COTAN'
.ix 'intrinsic function' 'COTAN'
.ix 'intrinsic function' 'DCOTAN'
.ix 'COTAN'
.ix 'DCOTAN'
.note COTAN &generic. &dagger.
R&arrow.COTAN(R),
D&arrow.COTAN(D)
.note COTAN
R&arrow.COTAN(R) &dagger.
.note DCOTAN
D&arrow.DCOTAN(D) &dagger.
.note Notes:
.im finote7
.np
&dagger. is an extension to FORTRAN 77.
.endnote
.*
.cp 18
.section Arcsine
.*
.begnote $setptnt 12
.note Definition:
.mono arcsin(a)
.nameuse
.ix 'generic function' 'ASIN'
.ix 'intrinsic function' 'ASIN'
.ix 'intrinsic function' 'DASIN'
.ix 'ASIN'
.ix 'DASIN'
.note ASIN &generic.
R&arrow.ASIN(R),
D&arrow.ASIN(D)
.note ASIN
R&arrow.ASIN(R)
.note DASIN
D&arrow.DASIN(D)
.note Notes:
The absolute value of the argument of ASIN and DASIN must be <= 1.
The result is such that -&pi./2 <= result <= &pi./2.
.endnote
.*
.cp 18
.section Arccosine
.*
.begnote $setptnt 12
.note Definition:
.mono arccos(a)
.nameuse
.ix 'generic function' 'ACOS'
.ix 'intrinsic function' 'ACOS'
.ix 'intrinsic function' 'DACOS'
.ix 'ACOS'
.ix 'DACOS'
.note ACOS &generic.
R&arrow.ACOS(R),
D&arrow.ACOS(D)
.note ACOS
R&arrow.ACOS(R)
.note DACOS
D&arrow.DACOS(D)
.note Notes:
The absolute value of the argument of ACOS and DACOS must be <= 1.
The result is such that 0 <= result <= &pi..
.endnote
.*
.cp 18
.section Arctangent
.*
.begnote $setptnt 12
.note Definition:
.mono arctan(a)
.nameuse
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
.note ATAN &generic.
R&arrow.ATAN(R),
D&arrow.ATAN(D)
.note ATAN
R&arrow.ATAN(R)
.note DATAN
D&arrow.DATAN(D)
.note Definition:
.mono arctan(a1/a2)
.nameuse
.note ATAN2 &generic.
R&arrow.ATAN2(R,R),
D&arrow.ATAN2(D,D)
.note ATAN2
R&arrow.ATAN2(R,R)
.note DATAN2
D&arrow.DATAN2(D,D)
.note Notes:
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
.endnote
.*
.cp 18
.section Hyperbolic Sine
.*
.begnote $setptnt 12
.note Definition:
.mono sinh(a)
.nameuse
.ix 'generic function' 'SINH'
.ix 'intrinsic function' 'SINH'
.ix 'intrinsic function' 'DSINH'
.ix 'SINH'
.ix 'DSINH'
.note SINH &generic.
R&arrow.SINH(R)
D&arrow.SINH(D)
.note SINH
R&arrow.SINH(R)
.note DSINH
D&arrow.DSINH(D)
.endnote
.*
.cp 18
.section Hyperbolic Cosine
.*
.begnote $setptnt 12
.note Definition:
.mono cosh(a)
.nameuse
.ix 'generic function' 'COSH'
.ix 'intrinsic function' 'COSH'
.ix 'intrinsic function' 'DCOSH'
.ix 'COSH'
.ix 'DCOSH'
.note COSH &generic.
R&arrow.COSH(R),
D&arrow.COSH(D)
.note COSH
R&arrow.COSH(R)
.note DCOSH
D&arrow.DCOSH(D)
.endnote
.*
.cp 18
.section Hyperbolic Tangent
.*
.begnote $setptnt 12
.note Definition:
.mono tanh(a)
.nameuse
.ix 'generic function' 'TANH'
.ix 'intrinsic function' 'TANH'
.ix 'intrinsic function' 'DTANH'
.ix 'TANH'
.ix 'DTANH'
.note TANH &generic.
R&arrow.TANH(R),
D&arrow.TANH(D)
.note TANH
R&arrow.TANH(R)
.note DTANH
D&arrow.DTANH(D)
.endnote
.*
.cp 18
.section Gamma Function
.*
.begnote $setptnt 12
.note Definition:
.mono gamma(a)
.nameuse
.ix 'generic function' 'GAMMA'
.ix 'intrinsic function' 'GAMMA'
.ix 'intrinsic function' 'DGAMMA'
.ix 'GAMMA'
.ix 'DGAMMA'
.note GAMMA &generic.
R&arrow.GAMMA(R),
D&arrow.GAMMA(D)
.note GAMMA
R&arrow.GAMMA(R)
.note DGAMMA
D&arrow.DGAMMA(D)
.endnote
.*
.cp 18
.section Natural Log of Gamma Function
.*
.begnote $setptnt 12
.note Definition:
.millust begin
log (gamma(a))
   e
.millust end
.nameuse
.ix 'generic function' 'GAMMA'
.ix 'intrinsic function' 'ALGAMA'
.ix 'intrinsic function' 'DLGAMA'
.ix 'GAMMA'
.ix 'ALGAMA'
.ix 'DLGAMA'
.note LGAMMA &generic.
R&arrow.LGAMMA(R),
D&arrow.LGAMMA(D)
.note ALGAMA
R&arrow.ALGAMA(R)
.note DLGAMA
D&arrow.DLGAMA(D)
.endnote
.*
.cp 18
.section Error Function
.*
.begnote $setptnt 12
.note Definition:
.mono erf(a)
.nameuse
.ix 'generic function' 'ERF'
.ix 'intrinsic function' 'ERF'
.ix 'intrinsic function' 'DERF'
.ix 'ERF'
.ix 'DERF'
.note ERF &generic.
R&arrow.ERF(R),
D&arrow.ERF(D)
.note ERF
R&arrow.ERF(R)
.note DERF
D&arrow.DERF(D)
.endnote
.*
.cp 18
.section Complement of Error Function
.*
.begnote $setptnt 12
.note Definition:
.mono 1-erf(a)
.nameuse
.ix 'generic function' 'ERFC'
.ix 'intrinsic function' 'ERFC'
.ix 'intrinsic function' 'DERFC'
.ix 'ERFC'
.ix 'DERFC'
.note ERFC &generic.
R&arrow.ERFC(R),
D&arrow.ERFC(D)
.note ERFC
R&arrow.ERFC(R)
.note DERFC
D&arrow.DERFC(D)
.endnote
.*
.cp 18
.section Lexically Greater Than or Equal
.*
.begnote $setptnt 12
.note Definition:
a1>=a2
.nameuse
.ix 'intrinsic function' 'LGE'
.ix 'LGE'
.note LGE
L&arrow.LGE(CH,CH)
.note Notes:
.im finote9
.endnote
.*
.cp 18
.section Lexically Greater Than
.*
.begnote $setptnt 12
.note Definition:
a1>a2
.nameuse
.ix 'intrinsic function' 'LGT'
.ix 'LGT'
.note LGT
L&arrow.LGT(CH,CH)
.note Notes:
.im finote9
.endnote
.*
.cp 18
.section Lexically Less Than or Equal
.*
.begnote $setptnt 12
.note Definition:
a1<=a2
.nameuse
.ix 'intrinsic function' 'LLE'
.ix 'LLE'
.note LLE
L&arrow.LLE(CH,CH)
.note Notes:
.im finote9
.endnote
.*
.cp 18
.section Lexically Less Than
.*
.begnote $setptnt 12
.note Definition:
a1<a2
.nameuse
.ix 'intrinsic function' 'LLT'
.ix 'LLT'
.note LLT
L&arrow.LLT(CH,CH)
.note Notes:
.im finote9
.endnote
.*
.cp 18
.section Binary Pattern Processing Functions: Boolean AND
.*
.begnote $setptnt 12
.note Definition:
.mono iand(i,j)
Boolean AND
.nameuse
.ix 'generic function' 'IAND'
.ix 'IAND'
.ix 'intrinsic function' 'IAND'
.ix 'IAND'
.ix 'intrinsic function' 'I1AND'
.ix 'I1AND'
.ix 'intrinsic function' 'I2AND'
.ix 'I2AND'
.note IAND &generic
I&arrow.IAND(I,I),
I1&arrow.IAND(I1,I1),
I2&arrow.IAND(I2,I2)
.note IAND
I&arrow.IAND(I,I)
.note I1AND
I1&arrow.I1AND(I1,I1)
.note I2AND
I2&arrow.I2AND(I2,I2)
.endnote
.*
.cp 18
.section Binary Pattern Processing Functions: Boolean Inclusive OR
.*
.begnote $setptnt 12
.note Definition:
.mono ior(i,j)
Boolean inclusive OR
.nameuse
.ix 'generic function' 'IOR'
.ix 'IOR'
.ix 'intrinsic function' 'IOR'
.ix 'IOR'
.ix 'intrinsic function' 'I1OR'
.ix 'I1OR'
.ix 'intrinsic function' 'I2OR'
.ix 'I2OR'
.note IOR &generic
I&arrow.IOR(I,I),
I1&arrow.IOR(I1,I1),
I2&arrow.IOR(I2,I2)
.note IOR
I&arrow.IOR(I,I)
.note I1OR
I1&arrow.I1OR(I1,I1)
.note I2OR
I2&arrow.I2OR(I2,I2)
.endnote
.*
.cp 18
.section Binary Pattern Processing Functions: Boolean Exclusive OR
.*
.begnote $setptnt 12
.note Definition:
.mono ieor(i,j)
Boolean exclusive OR
.nameuse
.ix 'generic function' 'IEOR'
.ix 'IEOR'
.ix 'intrinsic function' 'IEOR'
.ix 'IEOR'
.ix 'intrinsic function' 'I1EOR'
.ix 'I1EOR'
.ix 'intrinsic function' 'I2EOR'
.ix 'I2EOR'
.note IEOR &generic
I&arrow.IEOR(I,I),
I1&arrow.IEOR(I1,I1),
I2&arrow.IEOR(I2,I2)
.note IEOR
I&arrow.IEOR(I,I)
.note I1EOR
I1&arrow.I1EOR(I1,I1)
.note I2EOR
I2&arrow.I2EOR(I2,I2)
.endnote
.*
.cp 18
.section Binary Pattern Processing Functions: Boolean Complement
.*
.begnote $setptnt 12
.note Definition:
.mono not(i)
Boolean complement
.nameuse
.ix 'generic function' 'NOT'
.ix 'NOT'
.ix 'intrinsic function' 'NOT'
.ix 'NOT'
.ix 'intrinsic function' 'I1NOT'
.ix 'I1NOT'
.ix 'intrinsic function' 'I2NOT'
.ix 'I2NOT'
.note NOT &generic
I&arrow.NOT(I),
I1&arrow.NOT(I1),
I2&arrow.NOT(I2)
.note NOT
I&arrow.NOT(I)
.note I1NOT
I1&arrow.I1NOT(I1)
.note I2NOT
I2&arrow.I2NOT(I2)
.endnote
.*
.cp 18
.section Binary Pattern Processing Functions: Logical Shift
.*
.begnote $setptnt 12
.note Definition:
.mono ishl(j,n)
Logical shift
.nameuse
.ix 'generic function' 'ISHL'
.ix 'intrinsic function' 'ISHL'
.ix 'ISHL'
.ix 'intrinsic function' 'I1SHL'
.ix 'I1SHL'
.ix 'intrinsic function' 'I2SHL'
.ix 'I2SHL'
.note ISHL &generic.
I&arrow.ISHL(I,I),
I1&arrow.ISHL(I1,I1),
I2&arrow.ISHL(I2,I2)
.note ISHL
I&arrow.ISHL(I,I)
.note I1ISHL
I1&arrow.I1SHL(I1,I1)
.note I2ISHL
I2&arrow.I2SHL(I2,I2)
.endnote
.*
.cp 18
.*
.begnote $setptnt 12
.note Definition:
.mono ishft(j,n)
Logical shift
.nameuse
.ix 'generic function' 'ISHFT'
.ix 'intrinsic function' 'ISHFT'
.ix 'ISHFT'
.ix 'intrinsic function' 'I1SHFT'
.ix 'I1SHFT'
.ix 'intrinsic function' 'I2SHFT'
.ix 'I2SHFT'
.note ISHFT &generic.
I&arrow.ISHFT(I,I),
I1&arrow.ISHFT(I1,I1),
I2&arrow.ISHFT(I2,I2)
.note ISHFT
I&arrow.ISHFT(I,I)
.note I1ISHFT
I1&arrow.I1SHFT(I1,I1)
.note I2ISHFT
I2&arrow.I2SHFT(I2,I2)
.note Notes:
.im finote10
.endnote
.*
.cp 18
.section Binary Pattern Processing Functions: Arithmetic Shift
.*
.begnote $setptnt 12
.note Definition:
.mono isha(j,n)
Arithmetic shift
.nameuse
.ix 'generic function' 'ISHA'
.ix 'intrinsic function' 'ISHA'
.ix 'ISHA'
.ix 'intrinsic function' 'I1SHA'
.ix 'I1SHA'
.ix 'intrinsic function' 'I2SHA'
.ix 'I2SHA'
.note ISHA &generic.
I&arrow.ISHA(I,I),
I1&arrow.ISHA(I1,I1),
I2&arrow.ISHA(I2,I2)
.note ISHA
I&arrow.ISHA(I,I)
.note I1ISHA
I1&arrow.I1SHA(I1,I1)
.note I2ISHA
I2&arrow.I2SHA(I2,I2)
.note Notes:
.im finote10
.endnote
.*
.cp 18
.section Binary Pattern Processing Functions: Circular Shift
.*
.begnote $setptnt 12
.note Definition:
.mono ishc(j,n)
Circular shift
.nameuse
.ix 'generic function' 'ISHC'
.ix 'intrinsic function' 'ISHC'
.ix 'ISHC'
.ix 'intrinsic function' 'I1SHC'
.ix 'I1SHC'
.ix 'intrinsic function' 'I2SHC'
.ix 'I2SHC'
.note ISHC &generic.
I&arrow.ISHC(I,I),
I1&arrow.ISHC(I1,I1),
I2&arrow.ISHC(I2,I2)
.note ISHC
I&arrow.ISHC(I,I)
.note I1ISHC
I1&arrow.I1SHC(I1,I1)
.note I2ISHC
I2&arrow.I2SHC(I2,I2)
.note Notes:
.im finote10
.endnote
.*
.cp 18
.section Binary Pattern Processing Functions: Bit Testing
.*
.begnote $setptnt 12
.note Definition:
Test bit - a2'th bit of a1 is tested.
If it is 1, .TRUE. is returned.
If it is 0, .FALSE. is returned.
.nameuse
.ix 'generic function' 'BTEST'
.ix 'intrinsic function' 'BTEST'
.ix 'BTEST'
.ix 'intrinsic function' 'I1BTEST'
.ix 'I1BTEST'
.ix 'intrinsic function' 'I2BTEST'
.ix 'I2BTEST'
.note BTEST &generic.
L&arrow.BTEST(I,I),
L&arrow.BTEST(I1,I1),
L&arrow.BTEST(I2,I2)
.note BTEST
L&arrow.BTEST(I,I)
.note I1BTEST
L&arrow.I1BTEST(I1,I1)
.note I2BTEST
L&arrow.I2BTEST(I2,I2)
.endnote
.*
.cp 18
.section Binary Pattern Processing Functions: Set Bit
.*
.begnote $setptnt 12
.note Definition:
Set bit - Return a1 with a2'th bit set.
.nameuse
.ix 'generic function' 'IBSET'
.ix 'intrinsic function' 'IBSET'
.ix 'IBSET'
.ix 'intrinsic function' 'I1BSET'
.ix 'I1BSET'
.ix 'intrinsic function' 'I2BSET'
.ix 'I2BSET'
.note IBSET &generic.
I&arrow.IBSET(I,I),
I1&arrow.IBSET(I1,I1),
I2&arrow.IBSET(I2,I2)
.note IBSET
I&arrow.IBSET(I,I)
.note I1IBSET
I1&arrow.I1BSET(I1,I1)
.note I2IBSET
I2&arrow.I2BSET(I2,I2)
.endnote
.*
.cp 18
.section Binary Pattern Processing Functions: Clear Bit
.*
.begnote $setptnt 12
.note Definition:
Clear bit - Return a1 with a2'th bit cleared.
.nameuse
.ix 'generic function' 'IBCLR'
.ix 'intrinsic function' 'IBCLR'
.ix 'IBCLR'
.ix 'intrinsic function' 'I1BCLR'
.ix 'I1BCLR'
.ix 'intrinsic function' 'I2BCLR'
.ix 'I2BCLR'
.note IBCLR &generic.
I&arrow.IBCLR(I,I),
I1&arrow.IBCLR(I1,I1),
I2&arrow.IBCLR(I2,I2)
.note IBCLR
I&arrow.IBCLR(I,I)
.note I1IBCLR
I1&arrow.I1BCLR(I1,I1)
.note I2IBCLR
I2&arrow.I2BCLR(I2,I2)
.endnote
.*
.cp 18
.section Binary Pattern Processing Functions: Change Bit
.*
.begnote $setptnt 12
.note Definition:
Change bit - Return a1 with a2'th bit complemented.
.nameuse
.ix 'generic function' 'IBCHNG'
.ix 'intrinsic function' 'IBCHNG'
.ix 'IBCHNG'
.ix 'intrinsic function' 'I1BCHNG'
.ix 'I1BCHNG'
.ix 'intrinsic function' 'I2BCHNG'
.ix 'I2BCHNG'
.note IBCHNG &generic.
I&arrow.IBCHNG(I,I),
I1&arrow.IBCHNG(I1,I1),
I2&arrow.IBCHNG(I2,I2)
.note IBCHNG
I&arrow.IBCHNG(I,I)
.note I1IBCHNG
I1&arrow.I1BCHNG(I1,I1)
.note I2IBCHNG
I2&arrow.I2BCHNG(I2,I2)
.endnote
.*
.cp 18
.section Binary Pattern Processing Functions: Arithmetic Shifts
.*
.begnote $setptnt 12
.note Definition:
.mono lshift(j,n)
Arithmetic left shift
.nameuse
.ix 'generic function' 'LSHIFT'
.ix 'intrinsic function' 'LSHIFT'
.ix 'LSHIFT'
.ix 'intrinsic function' 'I1LSHIFT'
.ix 'I1LSHIFT'
.ix 'intrinsic function' 'I2LSHIFT'
.ix 'I2LSHIFT'
.note LSHIFT &generic.
I&arrow.LSHIFT(I,I),
I1&arrow.LSHIFT(I1,I1),
I2&arrow.LSHIFT(I2,I2)
.note LSHIFT
I&arrow.LSHIFT(I,I)
.note I1LSHIFT
I1&arrow.I1LSHIFT(I1,I1)
.note I2LSHIFT
I2&arrow.I2LSHIFT(I2,I2)
.endnote
.*
.begnote $setptnt 12
.note Definition:
.mono rshift(j,n)
Arithmetic right shift
.nameuse
.ix 'generic function' 'RSHIFT'
.ix 'intrinsic function' 'RSHIFT'
.ix 'RSHIFT'
.ix 'intrinsic function' 'I1RSHIFT'
.ix 'I1RSHIFT'
.ix 'intrinsic function' 'I2RSHIFT'
.ix 'I2RSHIFT'
.note RSHIFT &generic.
I&arrow.RSHIFT(I,I),
I1&arrow.RSHIFT(I1,I1),
I2&arrow.RSHIFT(I2,I2)
.note RSHIFT
I&arrow.RSHIFT(I,I)
.note I1RSHIFT
I1&arrow.I1RSHIFT(I1,I1)
.note I2RSHIFT
I2&arrow.I2RSHIFT(I2,I2)
.note Notes:
.im finote11
.endnote
.*
.cp 18
.section Allocated Array
.*
.begnote $setptnt 12
.note Definition:
Is array A allocated?
.nameuse
.ix 'intrinsic function' 'ALLOCATED'
.ix 'ALLOCATED'
.note ALLOCATED
L&arrow.ALLOCATED(A)
.endnote
.*
.cp 18
.section Memory Location
.*
.begnote $setptnt 12
.note Definition:
Location of A where A is any variable, array or array element
.nameuse
.ix 'intrinsic function' 'LOC'
.ix 'LOC'
.note LOC
I&arrow.LOC(A)
.endnote
.*
.cp 18
.section Size of Variable or Structure
.*
.begnote $setptnt 12
.note Definition:
Size of A in bytes where A is any constant, variable, array, or
structure
.nameuse
.ix 'intrinsic function' 'ISIZEOF'
.ix 'ISIZEOF'
.note ISIZEOF
I&arrow.ISIZEOF(A)
.note Notes:
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
.endnote
.*
.cp 18
.section Volatile Reference
.*
.begnote $setptnt 12
.note Definition:
A is a volatile reference
.nameuse
.ix 'intrinsic function' 'VOLATILE'
.ix 'VOLATILE'
.note VOLATILE
A&arrow.VOLATILE(A)
.note Notes:
A volatile reference to a symbol indicates that the value of the symbol
may be modified in ways that are unknown to the subprogram.
For example, a symbol in common being referenced in a subprogram may be
modified by another subprogram that is processing an asynchronous interrupt.
Therefore, any subprogram that is referencing the symbol to determine its
value should reference this symbol using the
.id VOLATILE
intrinsic function so that the value currently being evaluated agrees with
the value last stored.
.endnote
