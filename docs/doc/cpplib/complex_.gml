:P.
.ix 'Complex class'
This class is used for the storage and manipulation of complex
numbers, which are often represented by
:ITALICS.real
and
:ITALICS.imaginary
components (Cartesian coordinates), or by
:ITALICS.magnitude
and
:ITALICS.angle
(polar coordinates). Each object stores exactly one complex number.
An object may be used in expressions in the same manner as floating-point
values.
:P.
The class documented here is the Open Watcom legacy complex class. It
is not the
:MONO.std::complex
class template specified by Standard C++.
.*
:CLFNM.Complex
:CMT.========================================================================
:LIBF fmt='hdr'.Complex
:HFILE.complex.h
:CLSS.
The &cls. is used for the storage and
manipulation of complex numbers, which are often represented by
:ITALICS.real
and
:ITALICS.imaginary
components (Cartesian coordinates), or by
:ITALICS.magnitude
and
:ITALICS.angle
(polar coordinates). Each &obj. stores exactly one complex number.
A &obj. may be used in expressions in the same manner as floating-point values.
:HDG.Public Member Functions
The following constructors and destructors are declared:
:MFNL.
:MFCD cd_idx='c'.Complex();
:MFCD cd_idx='c'.Complex( Complex const & );
:MFCD cd_idx='c'.Complex( double, double = 0.0 );
:MFCD cd_idx='d' .~~Complex();
:eMFNL.
:P.
The following arithmetic member functions are declared:
:MFNL.
:MFN index='operator =' .Complex &amp.operator  =( Complex const & );
:MFN index='operator =' .Complex &amp.operator  =( double );
:MFN index='operator +='.Complex &amp.operator +=( Complex const & );
:MFN index='operator +='.Complex &amp.operator +=( double );
:MFN index='operator -='.Complex &amp.operator -=( Complex const & );
:MFN index='operator -='.Complex &amp.operator -=( double );
:MFN index='operator *='.Complex &amp.operator *=( Complex const & );
:MFN index='operator *='.Complex &amp.operator *=( double );
:MFN index='operator /='.Complex &amp.operator /=( Complex const & );
:MFN index='operator /='.Complex &amp.operator /=( double );
:MFN index='operator +' .Complex  operator  +() const;
:MFN index='operator -' .Complex  operator  -() const;
:MFN index='imag'       .double   imag() const;
:MFN index='real'       .double   real() const;
:eMFNL.
:HDG.Friend Functions
The following I/O Stream inserter and extractor friend functions are declared:
:RFNL.
:RFN index='operator >>'.friend istream &amp.operator >>( istream &, Complex & );
:RFN index='operator <<'.friend ostream &amp.operator <<( ostream &, Complex const & );
:eRFNL.
:HDG.Related Operators
The following operators are declared:
:RFNL.
:RFN index='operator +' .Complex operator  +( Complex const &, Complex const & );
:RFN index='operator +' .Complex operator  +( Complex const &, double );
:RFN index='operator +' .Complex operator  +( double         , Complex const & );
:RFN index='operator -' .Complex operator  -( Complex const &, Complex const & );
:RFN index='operator -' .Complex operator  -( Complex const &, double );
:RFN index='operator -' .Complex operator  -( double         , Complex const & );
:RFN index='operator *' .Complex operator  *( Complex const &, Complex const & );
:RFN index='operator *' .Complex operator  *( Complex const &, double );
:RFN index='operator *' .Complex operator  *( double         , Complex const & );
:RFN index='operator /' .Complex operator  /( Complex const &, Complex const & );
:RFN index='operator /' .Complex operator  /( Complex const &, double );
:RFN index='operator /' .Complex operator  /( double         , Complex const & );
:RFN index='operator =='.int     operator ==( Complex const &, Complex const & );
:RFN index='operator =='.int     operator ==( Complex const &, double );
:RFN index='operator =='.int     operator ==( double         , Complex const & );
:RFN index='operator !='.int     operator !=( Complex const &, Complex const & );
:RFN index='operator !='.int     operator !=( Complex const &, double );
:RFN index='operator !='.int     operator !=( double         , Complex const & );
:eRFNL.
:HDG.Related Functions
The following related functions are declared:
:RFNL.
:RFN index='abs'  .double  abs  ( Complex const & );
:RFN index='acos' .Complex acos ( Complex const & );
:RFN index='acosh'.Complex acosh( Complex const & );
:RFN index='arg'  .double  arg  ( Complex const & );
:RFN index='asin' .Complex asin ( Complex const & );
:RFN index='asinh'.Complex asinh( Complex const & );
:RFN index='atan' .Complex atan ( Complex const & );
:RFN index='atanh'.Complex atanh( Complex const & );
:RFN index='conj' .Complex conj ( Complex const & );
:RFN index='cos'  .Complex cos  ( Complex const & );
:RFN index='cosh' .Complex cosh ( Complex const & );
:RFN index='exp'  .Complex exp  ( Complex const & );
:RFN index='imag' .double  imag ( Complex const & );
:RFN index='log'  .Complex log  ( Complex const & );
:RFN index='log10'.Complex log10( Complex const & );
:RFN index='norm' .double  norm ( Complex const & );
:RFN index='polar'.Complex polar( double         , double = 0 );
:RFN index='pow'  .Complex pow  ( Complex const &, Complex const & );
:RFN index='pow'  .Complex pow  ( Complex const &, double );
:RFN index='pow'  .Complex pow  ( double         , Complex const & );
:RFN index='pow'  .Complex pow  ( Complex const &, int );
:RFN index='real' .double  real ( Complex const & );
:RFN index='sin'  .Complex sin  ( Complex const & );
:RFN index='sinh' .Complex sinh ( Complex const & );
:RFN index='sqrt' .Complex sqrt ( Complex const & );
:RFN index='tan'  .Complex tan  ( Complex const & );
:RFN index='tanh' .Complex tanh ( Complex const & );
:eRFNL.
:eCLSS.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='fun'.abs
:SNPL.
:SNPFLF          .#include <complex.h>
:SNPR index='abs'.double abs( Complex const &amp.num );
:eSNPL.
:SMTICS.
The &fn. computes the magnitude of
:ARG.num
:CONT., which is equivalent to the length (magnitude) of the vector when the
:ARG.num
is represented in polar coordinates.
:RSLTS.
The &fn. returns the magnitude of
:ARG.num
:PERIOD.
:SALSO.
:SAL typ='fun'.arg
:SAL typ='fun'.norm
:SAL typ='fun'.polar
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='fun'.acos
:SNPL.
:SNPFLF           .#include <complex.h>
:SNPR index='acos'.Complex acos( Complex const &amp.num );
:eSNPL.
:SMTICS.
The &fn. computes the arccosine of
:ARG.num
:PERIOD.
:RSLTS.
The &fn. returns the arccosine of
:ARG.num
:PERIOD.
:SALSO.
:SAL typ='fun'.asin
:SAL typ='fun'.atan
:SAL typ='fun'.cos
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='fun'.acosh
:SNPL.
:SNPFLF            .#include <complex.h>
:SNPR index='acosh'.Complex acosh( Complex const &amp.num );
:eSNPL.
:SMTICS.
The &fn. computes the inverse hyperbolic cosine of
:ARG.num
:PERIOD.
:RSLTS.
The &fn. returns the inverse hyperbolic cosine of
:ARG.num
:PERIOD.
:SALSO.
:SAL typ='fun'.asinh
:SAL typ='fun'.atanh
:SAL typ='fun'.cosh
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='fun'.arg
:SNPL.
:SNPFLF          .#include <complex.h>
:SNPR index='arg'.double arg( Complex const &amp.num );
:eSNPL.
:SMTICS.
The &fn. computes the angle of the vector when the
:ARG.num
is represented in polar coordinates.
The angle has the same sign as the real component of the
:ARG.num
:PERIOD.
It is positive in the 1st and 2nd quadrants, and negative in the 3rd
and 4th quadrants.
:RSLTS.
The &fn. returns the angle of the vector when the
:ARG.num
is represented in polar coordinates.
:SALSO.
:SAL typ='fun'.abs
:SAL typ='fun'.norm
:SAL typ='fun'.polar
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='fun'.asin
:SNPL.
:SNPFLF           .#include <complex.h>
:SNPR index='asin'.Complex asin( Complex const &amp.num );
:eSNPL.
:SMTICS.
The &fn. computes the arcsine of
:ARG.num
:PERIOD.
:RSLTS.
The &fn. returns the arcsine of
:ARG.num
:PERIOD.
:SALSO.
:SAL typ='fun'.acos
:SAL typ='fun'.atan
:SAL typ='fun'.sin
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='fun'.asinh
:SNPL.
:SNPFLF            .#include <complex.h>
:SNPR index='asinh'.Complex asinh( Complex const &amp.num );
:eSNPL.
:SMTICS.
The &fn. computes the inverse hyperbolic sine of
:ARG.num
:PERIOD.
:RSLTS.
The &fn. returns the inverse hyperbolic sine of
:ARG.num
:PERIOD.
:SALSO.
:SAL typ='fun'.acosh
:SAL typ='fun'.atanh
:SAL typ='fun'.sinh
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='fun'.atan
:SNPL.
:SNPFLF           .#include <complex.h>
:SNPR index='atan'.Complex atan( Complex const &amp.num );
:eSNPL.
:SMTICS.
The &fn. computes the arctangent of
:ARG.num
:PERIOD.
:RSLTS.
The &fn. returns the arctangent of
:ARG.num
:PERIOD.
:SALSO.
:SAL typ='fun'.acos
:SAL typ='fun'.asin
:SAL typ='fun'.tan
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='fun'.atanh
:SNPL.
:SNPFLF            .#include <complex.h>
:SNPR index='atanh'.Complex atanh( Complex const &amp.num );
:eSNPL.
:SMTICS.
The &fn. computes the inverse hyperbolic tangent of
:ARG.num
:PERIOD.
:RSLTS.
The &fn. returns the inverse hyperbolic tangent of
:ARG.num
:PERIOD.
:SALSO.
:SAL typ='fun'.acosh
:SAL typ='fun'.asinh
:SAL typ='fun'.tanh
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='ctor' prot='public'.Complex
:SNPL.
:SNPFLF                    .#include <complex.h>
:SNPFLF                    .public:
:SNPCD cd_idx='c'.Complex::Complex();
:eSNPL.
:SMTICS.
This form of the &fn. creates a default &obj. with value zero for both
the real and imaginary components.
:RSLTS.
This form of the &fn. produces a default &obj.:PERIOD.
:SALSO.
:SAL typ='dtor'.
:SAL typ='mfun'.real
:SAL typ='mfun'.imag
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='ctor' prot='public'.Complex
:SNPL.
:SNPFLF                    .#include <complex.h>
:SNPFLF                    .public:
:SNPCD cd_idx='c'.Complex::Complex( Complex const &amp.num );
:eSNPL.
:SMTICS.
This form of the &fn. creates a &obj. with the same value as
:ARG.num
:PERIOD.
:RSLTS.
This form of the &fn. produces a &obj.:PERIOD.
:SALSO.
:SAL typ='dtor'.
:SAL typ='mfun'.real
:SAL typ='mfun'.imag
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='ctor' prot='public'.Complex
:SNPL.
:SNPFLF                    .#include <complex.h>
:SNPFLF                    .public:
:SNPCD cd_idx='c'.Complex::Complex( double real, double imag = 0.0 );
:eSNPL.
:SMTICS.
This form of the &fn. creates a &obj. with the real component set to
:ARG.real
and the imaginary component set to
:ARG.imag
:PERIOD.
If no imaginary component is specified,
:ARG.imag
takes the default value of zero.
:RSLTS.
This form of the &fn. produces a &obj.:PERIOD.
:SALSO.
:SAL typ='dtor'.
:SAL typ='mfun'.real
:SAL typ='mfun'.imag
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='dtor' prot='public'.~~Complex
:SNPL.
:SNPFLF                   .#include <complex.h>
:SNPFLF                   .public:
:SNPCD cd_idx='d'.Complex::~~Complex();
:eSNPL.
:SMTICS.
The &fn. destroys the &obj.:PERIOD.
The call to the &fn. is inserted implicitly by the compiler
at the point where the &obj. goes out of scope.
:RSLTS.
The &obj. is destroyed.
:SALSO.
:SAL typ='ctor'.
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='fun'.conj
:SNPL.
:SNPFLF           .#include <complex.h>
:SNPR index='conj'.Complex conj( Complex const &amp.num );
:eSNPL.
:SMTICS.
The &fn. computes the conjugate of
:ARG.num
:PERIOD.
The conjugate consists of the unchanged real component, and the negative of
the imaginary component.
:RSLTS.
The &fn. returns the conjugate of
:ARG.num
:PERIOD.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='fun'.cos
:SNPL.
:SNPFLF          .#include <complex.h>
:SNPR index='cos'.Complex cos( Complex const &amp.num );
:eSNPL.
:SMTICS.
The &fn. computes the cosine of
:ARG.num
:PERIOD.
:RSLTS.
The &fn. returns the cosine of
:ARG.num
:PERIOD.
:SALSO.
:SAL typ='fun'.acos
:SAL typ='fun'.sin
:SAL typ='fun'.tan
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='fun'.cosh
:SNPL.
:SNPFLF           .#include <complex.h>
:SNPR index='cosh'.Complex cosh( Complex const &amp.num );
:eSNPL.
:SMTICS.
The &fn. computes the hyperbolic cosine of
:ARG.num
:PERIOD.
:RSLTS.
The &fn. returns the hyperbolic cosine of
:ARG.num
:PERIOD.
:SALSO.
:SAL typ='fun'.acosh
:SAL typ='fun'.sinh
:SAL typ='fun'.tanh
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='fun'.exp
:SNPL.
:SNPFLF          .#include <complex.h>
:SNPR index='exp'.Complex exp( Complex const &amp.num );
:eSNPL.
:SMTICS.
The &fn. computes the value of
:HILITE.e
raised to the power
:ARG.num
:PERIOD.
:RSLTS.
The &fn. returns the value of
:HILITE.e
raised to the power
:ARG.num
:PERIOD.
:SALSO.
:SAL typ='fun'.log
:SAL typ='fun'.log10
:SAL typ='fun'.pow
:SAL typ='fun'.sqrt
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.imag
:SNPL.
:SNPFLF           .#include <complex.h>
:SNPFLF           .public:
:SNPF index='imag'.double Complex::imag();
:eSNPL.
:SMTICS.
The &fn. extracts the imaginary component of the &obj.:PERIOD.
:RSLTS.
The &fn. returns the imaginary component of the &obj.:PERIOD.
:SALSO.
:SAL typ='fun'.imag
:SAL typ='fun'.real
:SAL typ='mfun'.real
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='fun'.imag
:SNPL.
:SNPFLF           .#include <complex.h>
:SNPR index='imag'.double imag( Complex const &amp.num );
:eSNPL.
:SMTICS.
The &fn. extracts the imaginary component of
:ARG.num
:PERIOD.
:RSLTS.
The &fn. returns the imaginary component of
:ARG.num
:PERIOD.
:SALSO.
:SAL typ='fun'.real
:SAL typ='mfun'.imag
:SAL typ='mfun'.real
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='fun'.log
:SNPL.
:SNPFLF          .#include <complex.h>
:SNPR index='num'.Complex log( Complex const &amp.num );
:eSNPL.
:SMTICS.
The &fn. computes the natural, or base
:HILITE.e
:CONT.,
logarithm of
:ARG.num
:PERIOD.
:RSLTS.
The &fn. returns the natural, or base
:HILITE.e
:CONT.,
logarithm of
:ARG.num
:PERIOD.
:SALSO.
:SAL typ='fun'.exp
:SAL typ='fun'.log10
:SAL typ='fun'.pow
:SAL typ='fun'.sqrt
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='fun'.log10
:SNPL.
:SNPFLF            .#include <complex.h>
:SNPR index='log10'.Complex log10( Complex const &amp.num );
:eSNPL.
:SMTICS.
The &fn. computes the base 10 logarithm of
:ARG.num
:PERIOD.
:RSLTS.
The &fn. returns the base 10 logarithm of
:ARG.num
:PERIOD.
:SALSO.
:SAL typ='fun'.exp
:SAL typ='fun'.log
:SAL typ='fun'.pow
:SAL typ='fun'.sqrt
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='fun'.norm
:SNPL.
:SNPFLF           .#include <complex.h>
:SNPR index='norm'.double norm( Complex const &amp.num );
:eSNPL.
:SMTICS.
The &fn. computes the square of the magnitude of
:ARG.num
:CONT.,
which is equivalent to the square of the length (magnitude) of the vector when
:ARG.num
is represented in polar coordinates.
:RSLTS.
The &fn. returns the square of the magnitude of
:ARG.num
:PERIOD.
:SALSO.
:SAL typ='fun'.arg
:SAL typ='fun'.polar
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='fun'.operator !=
:SNPL.
:SNPFLF                  .#include <complex.h>
:SNPR index='operator !='.int operator !=( Complex const &amp.num1, Complex const &amp.num2 );
:SNPR index='operator !='.int operator !=( Complex const &amp.num1, double         num2 );
:SNPR index='operator !='.int operator !=( double         num1, Complex const &amp.num2 );
:eSNPL.
:SMTICS.
The &fn. compares
:ARG.num1
and
:ARG.num2
for inequality.
At least one of the parameters must be a &obj. for this function to be called.
:P.
Two &obj.s are not equal if either of their
corresponding real or imaginary components are not equal.
:INCLUDE file='cpx_op'.
:RSLTS.
The &fn. returns a non-zero value if
:ARG.num1
is not equal to
:ARG.num2
:CONT., otherwise zero is returned.
:SALSO.
:SAL typ='fun'.operator~b==
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='fun'.operator *
:SNPL.
:SNPFLF                 .#include <complex.h>
:SNPR index='operator *'.Complex operator *( Complex const &amp.num1, Complex const &amp.num2 );
:SNPR index='operator *'.Complex operator *( Complex const &amp.num1, double         num2 );
:SNPR index='operator *'.Complex operator *( double         num1,     Complex const &amp.num2 );
:eSNPL.
:SMTICS.
The &fn. is used to multiply
:ARG.num1
by
:ARG.num2
yielding a &obj.:PERIOD.
:P.
The first &fn. multiplies two &obj.s.
:P.
The second &fn. multiplies a &obj. and a floating-point value.
In effect, the real and imaginary components of the &obj.
are multiplied by the floating-point value.
:P.
The third &fn. multiplies a floating-point value and a &obj.:PERIOD.
In effect, the real and imaginary components of the &obj.
are multiplied by the floating-point value.
:INCLUDE file='cpx_op'.
:RSLTS.
The &fn. returns a &obj. that is the product of
:ARG.num1
and
:ARG.num2
:PERIOD.
:SALSO.
:SAL typ='fun'.operator~b+
:SAL typ='fun'.operator~b-
:SAL typ='fun'.operator~b/
:SAL typ='mfun'.operator~b*=
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.operator *=
:SNPL.
:SNPFLF                  .#include <complex.h>
:SNPFLF                  .public:
:SNPF index='operator *='.Complex &amp.Complex::operator *=( Complex const &amp.num );
:SNPF index='operator *='.Complex &amp.Complex::operator *=( double num );
:eSNPL.
:SMTICS.
The &fn. is used to multiply the
:ARG.num
argument into the &obj.:PERIOD.
:P.
The first form of the &fn. multiplies the &obj. by the
:MONO.Complex
parameter.
:P.
The second form of the &fn. multiplies the real and imaginary
components of the &obj. by
:ARG.num
:PERIOD.
:INCLUDE file='cpx_asgn'.
:RSLTS.
The &fn. returns a reference to the target of the assignment.
:SALSO.
:SAL typ='fun'.operator~b*
:SAL typ='mfun'.operator~b+=
:SAL typ='mfun'.operator~b-=
:SAL typ='mfun'.operator~b/=
:SAL typ='mfun'.operator~b=
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.operator +
:SNPL.
:SNPFLF                 .#include <complex.h>
:SNPFLF                 .public:
:SNPF index='operator +'.Complex Complex::operator +();
:eSNPL.
:SMTICS.
The unary &fn. is provided for completeness.
It performs no operation on the &obj.:PERIOD.
:RSLTS.
The unary &fn. returns a &obj. with the same value as the original
&obj.:PERIOD.
:SALSO.
:SAL typ='fun'.operator~b+
:SAL typ='mfun'.operator~b+=
:SAL typ='mfun'.operator~b-
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='fun'.operator +
:SNPL.
:SNPFLF                 .#include <complex.h>
:SNPR index='operator +'.Complex operator +( Complex const &amp.num1, Complex const &amp.num2 );
:SNPR index='operator +'.Complex operator +( Complex const &amp.num1, double         num2 );
:SNPR index='operator +'.Complex operator +( double         num1, Complex const &amp.num2 );
:eSNPL.
:SMTICS.
The &fn. is used to add
:ARG.num1
to
:ARG.num2
yielding a &obj.:PERIOD.
:P.
The first &fn. adds two &obj.s.
:P.
The second &fn. adds a &obj. and a floating-point value.
In effect, the floating-point value is added to the real component
of the &obj.:PERIOD.
:P.
The third &fn. adds a floating-point value and a &obj.:PERIOD.
In effect, the floating-point value is added to the real component
of the &obj.:PERIOD.
:INCLUDE file='cpx_op'.
:RSLTS.
The &fn. returns a &obj. that is the sum of
:ARG.num1
and
:ARG.num2
:PERIOD.
:SALSO.
:SAL typ='fun'.operator~b*
:SAL typ='fun'.operator~b-
:SAL typ='fun'.operator~b/
:SAL typ='mfun'.operator~b+
:SAL typ='mfun'.operator~b+=
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.operator +=
:SNPL.
:SNPFLF                  .#include <complex.h>
:SNPFLF                  .public:
:SNPF index='operator +='.Complex &amp.Complex::operator +=( Complex const &amp.num );
:SNPF index='operator +='.Complex &amp.Complex::operator +=( double num );
:eSNPL.
:SMTICS.
The &fn. is used to add
:ARG.num
to the value of the &obj.:PERIOD.
The second form of the &fn. adds
:ARG.num
to the real component of the &obj.:PERIOD.
:INCLUDE file='cpx_asgn'.
:RSLTS.
The &fn. returns a reference to the target of the assignment.
:SALSO.
:SAL typ='fun'.operator~b+
:SAL typ='mfun'.operator~b*=
:SAL typ='mfun'.operator~b+
:SAL typ='mfun'.operator~b/=
:SAL typ='mfun'.operator~b-=
:SAL typ='mfun'.operator~b=
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.operator -
:SNPL.
:SNPFLF                 .#include <complex.h>
:SNPFLF                 .public:
:SNPF index='operator -'.Complex Complex::operator -();
:eSNPL.
:SMTICS.
The unary &fn. yields a &obj. with the real and imaginary components having
the same magnitude as those of the original object, but with opposite sign.
:RSLTS.
The unary &fn. returns a &obj. with the same magnitude
as the original &obj. and with opposite sign.
:SALSO.
:SAL typ='fun'.operator~b-
:SAL typ='mfun'.operator~b+
:SAL typ='mfun'.operator~b-=
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='fun'.operator -
:SNPL.
:SNPFLF                 .#include <complex.h>
:SNPR index='operator -'.Complex operator -( Complex const &amp.num1, Complex const &amp.num2 );
:SNPR index='operator -'.Complex operator -( Complex const &amp.num1, double         num2 );
:SNPR index='operator -'.Complex operator -( double         num1, Complex const &amp.num2 );
:eSNPL.
:SMTICS.
The &fn. is used to subtract
:ARG.num2
from
:ARG.num1
yielding a &obj.:PERIOD.
:P.
The first &fn. computes the difference between two &obj.s.
:P.
The second &fn. computes the difference between a &obj.
and a floating-point value.
In effect, the floating-point value is subtracted from the real component
of the &obj.:PERIOD.
:P.
The third &fn. computes the difference between a floating-point value and a
&obj.:PERIOD.
In effect, the real component of the result is
:ARG.num1
minus the real component of
:ARG.num2
:CONT,
and the imaginary component of the result is the negative of the imaginary
component of
:ARG.num2
:PERIOD.
:INCLUDE file='cpx_op'.
:RSLTS.
The &fn. returns a &obj. that is the difference between
:ARG.num1
and
:ARG.num2
:PERIOD.
:SALSO.
:SAL typ='fun'.operator~b*
:SAL typ='fun'.operator~b+
:SAL typ='fun'.operator~b/
:SAL typ='mfun'.operator~b-
:SAL typ='mfun'.operator~b-=
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.operator -=
:SNPL.
:SNPFLF                  .#include <complex.h>
:SNPFLF                  .public:
:SNPF index='operator -='.Complex &amp.Complex::operator -=( Complex const &amp.num );
:SNPF index='operator -='.Complex &amp.Complex::operator -=( double num );
:eSNPL.
:SMTICS.
The &fn. is used to subtract
:ARG.num
from the value of the &obj.:PERIOD.
The second form of the &fn. subtracts
:ARG.num
from the real component of the *obj.:PERIOD.
:INCLUDE file='cpx_asgn'.
:RSLTS.
The &fn. returns a reference to the target of the assignment.
:SALSO.
:SAL typ='fun'.operator~b-
:SAL typ='mfun'.operator~b*=
:SAL typ='mfun'.operator~b+=
:SAL typ='mfun'.operator~b-
:SAL typ='mfun'.operator~b/=
:SAL typ='mfun'.operator~b=
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='fun'.operator /
:SNPL.
:SNPFLF                 .#include <complex.h>
:SNPR index='operator /'.Complex operator /( Complex const &amp.num1, Complex const &amp.num2 );
:SNPR index='operator /'.Complex operator /( Complex const &amp.num1, double         num2 );
:SNPR index='operator /'.Complex operator /( double         num1, Complex const &amp.num2 );
:eSNPL.
:SMTICS.
The &fn. is used to divide
:ARG.num1
by
:ARG.num2
yielding a &obj.:PERIOD.
:P.
The first &fn. divides two &obj.s.
:P.
The second &fn. divides a &obj. by a floating-point value. In effect, the
real and imaginary components of the complex number are divided by the
floating-point value.
:P.
The third &fn. divides a floating-point value by a &obj.:PERIOD.
Conceptually, the
floating-point value is converted to a &obj. and then the division is done.
:INCLUDE file='cpx_op'.
:RSLTS.
The &fn. returns a &obj. that is the quotient of
:ARG.num1
divided by
:ARG.num2
:PERIOD.
:SALSO.
:SAL typ='fun'.operator~b*
:SAL typ='fun'.operator~b+
:SAL typ='fun'.operator~b-
:SAL typ='mfun'.operator~b/=
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.operator /=
:SNPL.
:SNPFLF                  .#include <complex.h>
:SNPFLF                  .public:
:SNPF index='operator /='.Complex &amp.Complex::operator /=( Complex const &amp.num );
:SNPF index='operator /='.Complex &amp.Complex::operator /=( double num );
:eSNPL.
:SMTICS.
The &fn. is used to divide the &obj. by
:ARG.num
:PERIOD.
The second form of the &fn. divides the real and imaginary
components of the &obj. by
:ARG.num
:PERIOD.
:INCLUDE file='cpx_asgn'.
:RSLTS.
The &fn. returns a reference to the target of the assignment.
:SALSO.
:SAL typ='fun'.operator~b/
:SAL typ='mfun'.operator~b*=
:SAL typ='mfun'.operator~b+=
:SAL typ='mfun'.operator~b-=
:SAL typ='mfun'.operator~b=
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='fun'.operator <<
:SNPL.
:SNPFLF                  .#include <complex.h>
:SNPR index='operator <<'.friend ostream &amp.operator <<( ostream &amp.strm, Complex &amp.num );
:eSNPL.
:SMTICS.
The &fn. is used to write &obj.s to an I/O stream.
The &obj. is always written in the form:
:P.
:MONO.(real,imag)
:P.
The real and imaginary components are written using the normal rules for
formatting floating-point numbers.
Any formatting options specified prior to inserting the
:ARG.num
apply to both the real and imaginary components.
If the real and imaginary components are to be inserted using different
formats, the
:MONO.real
and
:MONO.imag
member functions should be used to insert each component separately.
:RSLTS.
The &fn. returns a reference to the
:ARG.strm
object.
:SALSO.
:SAL typ='cls'.istream
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.operator =
:SNPL.
:SNPFLF                 .#include <complex.h>
:SNPFLF                 .public:
:SNPF index='operator ='.Complex &amp.Complex::operator =( Complex const &amp.num );
:SNPF index='operator ='.Complex &amp.Complex::operator =( double num );
:eSNPL.
:SMTICS.
The &fn. is used to set the value of the &obj. to
:ARG.num
:PERIOD.
The first assignment operator copies the value of
:ARG.num
into the &obj.:PERIOD.
:P.
The second assignment operator sets the real component of the &obj. to
:ARG.num
and the imaginary component to zero.
:INCLUDE file='cpx_asgn'.
:RSLTS.
The &fn. returns a reference to the target of the assignment.
:SALSO.
:SAL typ='mfun'.operator~b*=
:SAL typ='mfun'.operator~b+=
:SAL typ='mfun'.operator~b-=
:SAL typ='mfun'.operator~b/=
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='fun'.operator ==
:SNPL.
:SNPFLF                  .#include <complex.h>
:SNPR index='operator =='.int operator ==( Complex const &amp.num1, Complex const &amp.num2 );
:SNPR index='operator =='.int operator ==( Complex const &amp.num1, double         num2 );
:SNPR index='operator =='.int operator ==( double         num1, Complex const &amp.num2 );
:eSNPL.
:SMTICS.
The &fn. compares
:ARG.num1
and
:ARG.num2
for equality.
At least one of the arguments must be a &obj. for this function to be called.
:P.
Two &obj.s are equal if their corresponding real and imaginary components
are equal.
:INCLUDE file='cpx_op'.
:RSLTS.
The &fn. returns a non-zero value if
:ARG.num1
is equal to
:ARG.num2
:CONT., otherwise zero is returned.
:SALSO.
:SAL typ='fun'.operator~b!=
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='fun'.operator >>
:SNPL.
:SNPFLF                  .#include <complex.h>
:SNPR index='operator >>'.friend istream &amp.operator >>( istream &amp.strm, Complex &amp.num );
:eSNPL.
:SMTICS.
The &fn. is used to read a &obj. from an I/O stream.
A valid complex value is of one of the following forms:
:P.
:MONO.(real,imag)
:BREAK.
:MONO.real,imag
:BREAK.
:MONO.(real)
:P.
If the imaginary portion is omitted, zero is assumed.
:P.
While reading a &obj., whitespace is ignored before and between the various
components of the number if the
:MONO.ios::skipws
bit is set in &fmtflags.:PERIOD.
:RSLTS.
The &fn. returns a reference to
:ARG.strm
:PERIOD.
:ARG.num
contains the value read from
:ARG.strm
on success, otherwise it is unchanged.
:SALSO.
:SAL typ='cls'.istream
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='fun'.polar
:SNPL.
:SNPFLF            .#include <complex.h>
:SNPR index='polar'.Complex polar( double mag, double angle = 0.0 );
:eSNPL.
:SMTICS.
The &fn. converts
:ARG.mag
and
:ARG.angle
(polar coordinates) into a complex number.
The
:ARG.angle
is optional and defaults to zero if it is unspecified.
:RSLTS.
The &fn. returns a &obj. that is
:ARG.mag
and
:ARG.angle
interpreted as polar coordinates.
:SALSO.
:SAL typ='fun'.abs
:SAL typ='fun'.arg
:SAL typ='fun'.norm
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='fun'.pow
:SNPL.
:SNPFLF          .#include <complex.h>
:SNPR index='pow'.Complex pow( Complex const &amp.num, Complex const &amp.exp );
:SNPR index='pow'.Complex pow( Complex const &amp.num, double         exp );
:SNPR index='pow'.Complex pow( double         num, Complex const &amp.exp );
:SNPR index='pow'.Complex pow( Complex const &amp.num, int            exp );
:eSNPL.
:SMTICS.
The &fn. computes
:ARG.num
raised to the power
:ARG.exp
:PERIOD.
The various forms are provided to minimize the amount of floating-point
calculation performed.
:RSLTS.
The &fn. returns a &obj. that is
:ARG.num
raised to the power a &obj. that is
:ARG.exp
:PERIOD.
:SALSO.
:SAL typ='fun'.exp
:SAL typ='fun'.log
:SAL typ='fun'.log10
:SAL typ='fun'.sqrt
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.real
:SNPL.
:SNPFLF           .#include <complex.h>
:SNPFLF           .public:
:SNPF index='real'.double Complex::real();
:eSNPL.
:SMTICS.
The &fn. extracts the real component of the &obj.:PERIOD.
:RSLTS.
The &fn. returns the real component of the &obj.:PERIOD.
:SALSO.
:SAL typ='fun'.imag
:SAL typ='fun'.real
:SAL typ='mfun'.imag
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='fun'.real
:SNPL.
:SNPFLF           .#include <complex.h>
:SNPR index='real'.double real( Complex const &amp.num );
:eSNPL.
:SMTICS.
The &fn. extracts the real component of
:ARG.num
:PERIOD.
:RSLTS.
The &fn. returns the real component of
:ARG.num
:PERIOD.
:SALSO.
:SAL typ='fun'.imag
:SAL typ='mfun'.imag
:SAL typ='mfun'.real
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='fun'.sin
:SNPL.
:SNPFLF          .#include <complex.h>
:SNPR index='sin'.Complex sin( Complex const &amp.num );
:eSNPL.
:SMTICS.
The &fn. computes the sine of
:ARG.num
:PERIOD.
:RSLTS.
The &fn. returns the sine of
:ARG.num
:PERIOD.
:SALSO.
:SAL typ='fun'.asin
:SAL typ='fun'.cos
:SAL typ='fun'.tan
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='fun'.sinh
:SNPL.
:SNPFLF           .#include <complex.h>
:SNPR index='sinh'.Complex sinh( Complex const &amp.num );
:eSNPL.
:SMTICS.
The &fn. computes the hyperbolic sine of
:ARG.num
:PERIOD.
:RSLTS.
The &fn. returns the hyperbolic sine of
:ARG.num
:PERIOD.
:SALSO.
:SAL typ='fun'.asinh
:SAL typ='fun'.cosh
:SAL typ='fun'.tanh
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='fun'.sqrt
:SNPL.
:SNPFLF           .#include <complex.h>
:SNPR index='sqrt'.Complex sqrt( Complex const &amp.num );
:eSNPL.
:SMTICS.
The &fn. computes the square root of
:ARG.num
:PERIOD.
:RSLTS.
The &fn. returns the square root of
:ARG.num
:PERIOD.
:SALSO.
:SAL typ='fun'.exp
:SAL typ='fun'.log
:SAL typ='fun'.log10
:SAL typ='fun'.pow
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='fun'.tan
:SNPL.
:SNPFLF          .#include <complex.h>
:SNPR index='tan'.Complex tan( Complex const &amp.num );
:eSNPL.
:SMTICS.
The &fn. computes the tangent of
:ARG.num
:PERIOD.
:RSLTS.
The &fn. returns the tangent of
:ARG.num
:PERIOD.
:SALSO.
:SAL typ='fun'.atan
:SAL typ='fun'.cos
:SAL typ='fun'.sin
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='fun'.tanh
:SNPL.
:SNPFLF           .#include <complex.h>
:SNPR index='tanh'.Complex tanh( Complex const &amp.num );
:eSNPL.
:SMTICS.
The &fn. computes the hyperbolic tangent of
:ARG.num
:PERIOD.
:RSLTS.
The &fn. returns the hyperbolic tangent of
:ARG.num
:PERIOD.
:SALSO.
:SAL typ='fun'.atanh
:SAL typ='fun'.cosh
:SAL typ='fun'.sinh
:eSALSO.
:eLIBF.
