:CLFNM.ostream
:CMT.========================================================================
:LIBF fmt='hdr'.ostream
:HFILE.iostream.h
:DVFML.
:DVFM.ios
:eDVFML.
:DVBYL.
:DVBY.iostream
:DVBY.ofstream
:DVBY.ostrstream
:eDVBYL.
:CLSS.
The &cls. supports writing characters to a class derived from the
:MONO.streambuf
class, and provides formatted conversion of types (such as integers and
floating-point numbers) into characters. The class derived from the
:MONO.streambuf
class provides the methods for communicating with the external device
(screen, disk), while the &cls. provides the conversion of the types into
characters.
:P.
Generally, &obj.s won't be explicitly created by a program, since there
is no mechanism at this level to open a device.  The only default
&obj.s in a program are &cout., &cerr., and &clog. which write to the
standard output and error devices (usually the screen).
:P.
The &cls. supports two basic concepts of output: formatted and unformatted.
The overloaded
:MONO.operator~b<<
member functions are called
:ITALICS.inserters
:I1.inserter
and they provide the support for formatted output. The rest of the member
functions deal with unformatted output, managing the state of the
:MONO.ios
object and providing a friendlier interface to the associated
:MONO.streambuf
object.
:HDG.Protected Member Functions
The following protected member functions are declared:
:MFNL.
:MFCD cd_idx='c'.ostream();
:eMFNL.
:HDG.Public Member Functions
The following public member functions are declared:
:MFNL.
:MFCD cd_idx='c'.ostream( ostream const & );
:MFCD cd_idx='c'.ostream( streambuf * );
:MFCD cd_idx='d' .virtual ~~ostream();
:MFN index='flush'        .ostream &amp.flush();
:MFN index='opfx'         .int opfx();
:MFN index='osfx'         .void osfx();
:MFN index='put'          .ostream &amp.put(          char );
:MFN index='put'          .ostream &amp.put(   signed char );
:MFN index='put'          .ostream &amp.put( unsigned char );
:MFN index='seekp'        .ostream &amp.seekp( streampos );
:MFN index='seekp'        .ostream &amp.seekp( streamoff, ios::seekdir );
:MFN index='tellp'        .streampos tellp();
:MFN index='write'        .ostream &amp.write(          char const *, int );
:MFN index='write'        .ostream &amp.write(   signed char const *, int );
:MFN index='write'        .ostream &amp.write( unsigned char const *, int );
:eMFNL.
:HDG.Public Member Operators
The following public member operators are declared:
:MFNL.
:MFN index='operator =' .ostream &amp.operator =( streambuf * );
:MFN index='operator =' .ostream &amp.operator =( ostream const & );
:MFN index='operator <<'.ostream &amp.operator <<(          char );
:MFN index='operator <<'.ostream &amp.operator <<(   signed char );
:MFN index='operator <<'.ostream &amp.operator <<( unsigned char );
:MFN index='operator <<'.ostream &amp.operator <<(   signed short );
:MFN index='operator <<'.ostream &amp.operator <<( unsigned short );
:MFN index='operator <<'.ostream &amp.operator <<(   signed int );
:MFN index='operator <<'.ostream &amp.operator <<( unsigned int );
:MFN index='operator <<'.ostream &amp.operator <<(   signed long );
:MFN index='operator <<'.ostream &amp.operator <<( unsigned long );
:MFN index='operator <<'.ostream &amp.operator <<(      float );
:MFN index='operator <<'.ostream &amp.operator <<(      double );
:MFN index='operator <<'.ostream &amp.operator <<( long double );
:MFN index='operator <<'.ostream &amp.operator <<( void * );
:MFN index='operator <<'.ostream &amp.operator <<( streambuf & );
:MFN index='operator <<'.ostream &amp.operator <<(          char const * );
:MFN index='operator <<'.ostream &amp.operator <<(   signed char const * );
:MFN index='operator <<'.ostream &amp.operator <<( unsigned char const * );
:MFN index='operator <<'.ostream &amp.operator <<( ostream &(*)( ostream & ) );
:MFN index='operator <<'.ostream &amp.operator <<( ios     &(*)( ios     & ) );
:eMFNL.
:eCLSS.
:SALSO.
:SAL typ='cls'.ios
:SAL typ='cls'.iostream
:SAL typ='cls'.istream
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.flush
:SNPL.
:SNPFLF            .#include <iostream.h>
:SNPFLF            .public:
:SNPF index='flush'.ostream &amp.ostream::flush();
:eSNPL.
:SMTICS.
The &fn. causes the &obj.'s buffers to be flushed, forcing the contents to
be written to the actual device connected to the &obj.:PERIOD.
:RSLTS.
The &fn. returns a reference to the &obj.:PERIOD.
On failure, &failbit. is set in the &errstate.:PERIOD.
:SALSO.
:SAL typ='mfun'.osfx
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.operator <<
:SNPL.
:SNPFLF                  .#include <iostream.h>
:SNPFLF                  .public:
:SNPF index='operator <<'.ostream &amp.ostream::operator <<(          char ch );
:SNPF index='operator <<'.ostream &amp.ostream::operator <<(   signed char ch );
:SNPF index='operator <<'.ostream &amp.ostream::operator <<( unsigned char ch );
:eSNPL.
:SMTICS.
These forms of the &fn. write the
:ARG.ch
character into the &obj.:PERIOD.
:RSLTS.
These forms of the &fn. return a reference to the &obj. so that further
insertion operations may be specified in the same statement.
&failbit. is set in the &errstate. if an error occurs.
:SALSO.
:SAL typ='mfun'.put
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.operator <<
:SNPL.
:SNPFLF                  .#include <iostream.h>
:SNPFLF                  .public:
:SNPF index='operator <<'.ostream &amp.ostream::operator <<(          char const *str );
:SNPF index='operator <<'.ostream &amp.ostream::operator <<(   signed char const *str );
:SNPF index='operator <<'.ostream &amp.ostream::operator <<( unsigned char const *str );
:eSNPL.
:SMTICS.
These forms of the &fn. perform a formatted write of the contents of the C string specified by the
:ARG.str
parameter to the &obj.:PERIOD.
The characters from
:ARG.str
are transferred up to, but not including the terminating null character.
:RSLTS.
These forms of the &fn. return a reference to the &obj. so that further
insertion operations may be specified in the same statement.
&failbit. is set in the &errstate. if an error occurs.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.operator <<
:SNPL.
:SNPFLF                  .#include <iostream.h>
:SNPFLF                  .public:
:SNPF index='operator <<'.ostream &amp.ostream::operator <<(   signed   int num );
:SNPF index='operator <<'.ostream &amp.ostream::operator <<( unsigned   int num );
:SNPF index='operator <<'.ostream &amp.ostream::operator <<(   signed  long num );
:SNPF index='operator <<'.ostream &amp.ostream::operator <<( unsigned  long num );
:SNPF index='operator <<'.ostream &amp.ostream::operator <<(   signed short num );
:SNPF index='operator <<'.ostream &amp.ostream::operator <<( unsigned short num );
:eSNPL.
:SMTICS.
These forms of the &fn. perform a formatted write of the integral value specified by the
:ARG.num
parameter to the &obj.:PERIOD.
The integer value is converted to a string of characters which are
written to the &obj.:PERIOD.
:ARG.num
is converted to a base representation depending on the setting of the
:MONO.ios::basefield
bits in &fmtflags.:PERIOD.
If the
:MONO.ios::oct
bit is the only bit on, the conversion is to an octal (base 8)
representation. If the
:MONO.ios::hex
bit is the only bit on, the conversion is to a hexadecimal (base 16)
representation. Otherwise, the conversion is to a decimal (base 10)
representation.
:P.
For decimal conversions only, a sign may be written in front of the
number. If the number is negative, a
:MONO.-
minus sign is written. If the number is positive and the
:MONO.ios::showpos
bit is on in &fmtflags., a
:MONO.+
plus sign is written. No sign is written for a value of zero.
:P.
If the
:MONO.ios::showbase
bit is on in &fmtflags., and the conversion is to octal or hexadecimal,
the base indicator is written next. The base indicator for a conversion
to octal is a zero. The base indicator for a conversion to hexadecimal is
:MONO.0x
or
:MONO.0X
:CONT., depending on the setting of the
:MONO.ios::uppercase
bit in &fmtflags.:PERIOD.
:P.
If the value being written is zero, the conversion is to octal, and the
:MONO.ios::showbase
bit is on, nothing further is written since a single zero is
sufficient.
:P.
The value of
:ARG.num
is then converted to characters. For conversions to decimal, the magnitude of
the number is converted to a string of decimal digits
:MONO.0123456789
:PERIOD.
For conversions to octal, the number is treated as an unsigned quantity
and converted to a string of octal digits
:MONO.01234567
:PERIOD.
For conversions to hexadecimal, the number is treated as an unsigned
quantity and converted to a string of hexadecimal digits
:MONO.0123456789
and the letters
:MONO.abcdef
or
:MONO.ABCDEF
:CONT., depending on the setting of the
:MONO.ios::uppercase
in &fmtflags.:PERIOD.
The string resulting from the conversion is then written to the &obj.:PERIOD.
:P.
If the
:MONO.ios::internal
bit is set in &fmtflags. and padding is required, the padding characters
are written after the sign and/or base indicator (if present)
and before the digits.
:RSLTS.
These forms of the &fn. return a reference to the &obj. so that further
insertion operations may be specified in the same statement.
&failbit. is set in the &errstate. if an error occurs.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.operator <<
:SNPL.
:SNPFLF                  .#include <iostream.h>
:SNPFLF                  .public:
:SNPF index='operator <<'.ostream &amp.ostream::operator <<(       float num );
:SNPF index='operator <<'.ostream &amp.ostream::operator <<(      double num );
:SNPF index='operator <<'.ostream &amp.ostream::operator <<( long double num );
:eSNPL.
:SMTICS.
These forms of the &fn. perform a formatted write of the floating-point value specified by the
:ARG.num
parameter to the &obj.:PERIOD.
The number is converted to either scientific (exponential) form or
fixed-point form, depending on the setting of the
:MONO.ios::floatfield
bits in &fmtflags.:PERIOD.
If
:MONO.ios::scientific
is the only bit set, the conversion is to scientific form. If
:MONO.ios::fixed
is the only bit set, the conversion is to fixed-point form.
Otherwise (neither or both bits set), the value of the number determines
the conversion used. If the exponent is less than -4 or is greater than or
equal to the &fmtprec., the scientific form is used.
Otherwise, the fixed-point form is used.
:P.
Scientific form consists of a minus sign (for negative numbers), one
digit, a decimal point, &fmtprec.-1 digits, an
:MONO.e
or
:MONO.E
(depending on the setting of the
:MONO.ios::uppercase
bit), a minus sign (for negative exponents) or a plus sign (for zero or
positive exponents), and two or three digits for the exponent.
The digit before the decimal is not zero, unless the number is zero.
If the &fmtprec. is zero (or one), no digits are written following
the decimal point.
:P.
Fixed-point form consists of a minus sign (for negative numbers),
one or more digits, a decimal point, and &fmtprec. digits.
:P.
If the
:MONO.ios::showpoint
bit is not set in &fmtflags., trailing zeroes are trimmed after the
decimal point (and before the exponent for scientific form), and if no digits
remain after the decimal point, the decimal point is discarded as well.
:P.
If the
:MONO.ios::internal
bit is set in &fmtflags. and padding is required, the padding
characters are written after the sign (if present) and before the digits.
:RSLTS.
These forms of the &fn. return a reference to the &obj. so that further
insertion operations may be specified in the same statement.
&failbit. is set in the &errstate. if an error occurs.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.operator <<
:SNPL.
:SNPFLF                  .#include <iostream.h>
:SNPFLF                  .public:
:SNPF index='operator <<'.ostream &amp.ostream::operator <<( void *ptr );
:eSNPL.
:SMTICS.
This form of the &fn. performs a formatted write of the pointer value specified by the
:ARG.ptr
parameter to the &obj.:PERIOD.
The
:ARG.ptr
parameter is converted to an implementation-defined string of characters and
written to the &obj.:PERIOD.
With the &cmppname implementation, the string starts with
:MONO.0x
or
:MONO.0X
(depending on the setting of the
:MONO.ios::uppercase
bit),
followed by
4 hexadecimal digits for 16-bit pointers and 8 hexadecimal
digits for 32-bit pointers. Leading zeroes are added to ensure the correct
number of digits are written.
For far pointers, 4 additional hexadecimal digits and a colon are inserted
immediately after the
:MONO.0x
prefix.
:RSLTS.
This form of the &fn. returns a reference to the &obj. so that further
insertion operations may be specified in the same statement.
&failbit. is set in the &errstate. if an error occurs during the write.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.operator <<
:SNPL.
:SNPFLF                  .#include <iostream.h>
:SNPFLF                  .public:
:SNPF index='operator <<'.ostream &amp.ostream::operator <<( streambuf &amp.sb );
:eSNPL.
:SMTICS.
This form of the &fn. transfers the contents of the
:ARG.sb
:MONO.streambuf
object to the &obj.:PERIOD.
Reading from the
:MONO.streambuf
object stops when the read fails. No padding with the &fillchar. takes place
on output to the &obj.:PERIOD.
:RSLTS.
This form of the &fn. returns a reference to the &obj. so that further
insertion operations may be specified in the same statement.
&failbit. is set in the &errstate. if an error occurs.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.operator <<
:SNPL.
:SNPFLF                  .#include <iostream.h>
:SNPFLF                  .public:
:SNPF index='operator <<'.ostream &amp.ostream::operator <<( ostream &(*fn)( ostream &) );
:SNPF index='operator <<'.ostream &amp.ostream::operator <<( ios     &(*fn)( ios & ) );
:eSNPL.
:SMTICS.
These forms of the &fn. are used to implement the non-parameterized
manipulators for the &cls.:PERIOD.
The function specified by the
:ARG.fn
parameter is called with the &obj. as its parameter.
:RSLTS.
These forms of the &fn. return a reference to the &obj. so that further
insertions operations may be specified in the same statement.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.operator =
:SNPL.
:SNPFLF                 .#include <iostream.h>
:SNPFLF                 .public:
:SNPF index='operator ='.ostream &amp.ostream::operator =( streambuf *sb );
:eSNPL.
:SMTICS.
This form of the &fn. is used to associate a
:MONO.streambuf
object, specified by the
:ARG.sb
parameter, with an existing &obj.:PERIOD.
The &obj. is initialized and will use the specified
:MONO.streambuf
object for subsequent operations.
:RSLTS.
This form of the &fn. returns a reference to the &obj. that is the target of
the assignment.
If the
:ARG.sb
parameter is &null., &badbit. is set in the &errstate.:PERIOD.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.operator =
:SNPL.
:SNPFLF                 .#include <iostream.h>
:SNPFLF                 .public:
:SNPF index='operator ='.ostream &amp.ostream::operator =( const ostream &amp.ostrm );
:eSNPL.
:SMTICS.
This form of the &fn. is used to associate the &obj. with the
:MONO.streambuf
object currently associated with the
:ARG.ostrm
parameter.
The &obj. is initialized and will use the
:ARG.ostrm
:CONT.'s
:MONO.streambuf
object for subsequent operations.
The
:ARG.ostrm
object will continue to use the
:MONO.streambuf
object.
:RSLTS.
This form of the &fn. returns a reference to the &obj. that is the target of
the assignment.
If there is no
:MONO.streambuf
object currently associated with the
:ARG.ostrm
parameter, &badbit. is set in the &errstate.:PERIOD.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.opfx
:SNPL.
:SNPFLF           .#include <iostream.h>
:SNPFLF           .public:
:SNPF index='opfx'.int ostream::opfx();
:eSNPL.
:SMTICS.
If &fn. is a prefix function executed before each of the formatted and
unformatted output operations. If any bits are set in &iostate.,
the &fn. immediately returns zero, indicating that the prefix function failed.
Failure in the prefix function causes the output operation to fail.
:P.
If the &obj. is tied to another &obj., the other &obj. is flushed.
:RSLTS.
The &fn. returns a non-zero value on success, otherwise zero is returned.
:SALSO.
:SAL typ='mfun'.osfx
:SAL typ='mfun'.flush
:SAL typ='ofun' ocls='ios'.tie
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.osfx
:SNPL.
:SNPFLF           .#include <iostream.h>
:SNPFLF           .public:
:SNPF index='osfx'.void ostream::osfx();
:eSNPL.
:SMTICS.
The &fn. is a suffix function executed at the end of each
of the formatted and unformatted output operations.
:P.
If the
:MONO.ios::unitbuf
bit is set in &fmtflags., the
:MONO.flush
member function is called.  If the
:MONO.ios::stdio
bit is set in &fmtflags., the C library
:MONO.fflush
function is invoked on the
:MONO.stdout
and
:MONO.stderr
file streams.
:SALSO.
:SAL typ='mfun'.osfx
:SAL typ='mfun'.flush
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='ctor' prot='protected'.ostream
:SNPL.
:SNPFLF                    .#include <iostream.h>
:SNPFLF                    .protected:
:SNPCD cd_idx='c'.ostream::ostream();
:eSNPL.
:SMTICS.
This form of the &fn. creates an &obj. without an attached
:MONO.streambuf
object.
:P.
This form of the &fn. is only used implicitly by the compiler
when it generates a constructor for a derived class.
:RSLTS.
This form of the &fn. creates an initialized &obj.:PERIOD.
&badbit. is set in the &errstate.:PERIOD.
:SALSO.
:SAL typ='dtor'.
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='ctor' prot='public'.ostream
:SNPL.
:SNPFLF                    .#include <iostream.h>
:SNPFLF                    .public:
:SNPCD cd_idx='c'.ostream::ostream( ostream const &amp.ostrm );
:eSNPL.
:SMTICS.
This form of the &fn. creates an &obj. associated with the
:MONO.streambuf
object currently associated with the
:ARG.ostrm
parameter.
The &obj. is initialized and will use the
:ARG.ostrm
:CONT.'s
:MONO.streambuf
object for subsequent operations.
The
:ARG.ostrm
object will continue to use the
:MONO.streambuf
object.
:RSLTS.
This form of the &fn. creates an initialized &obj.:PERIOD.
If there is no
:MONO.streambuf
object currently associated with the
:ARG.ostrm
parameter, &badbit. is set in the &errstate.:PERIOD.
:SALSO.
:SAL typ='dtor'.
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='ctor' prot='public'.ostream
:SNPL.
:SNPFLF                    .#include <iostream.h>
:SNPFLF                    .public:
:SNPCD cd_idx='c'.ostream::ostream( streambuf *sb );
:eSNPL.
:SMTICS.
This form of the &fn. creates an &obj. with an associated
:MONO.streambuf
object specified by the
:ARG.sb
parameter.
:P.
This function is likely to be used for the creation of an &obj. that is
associated with the same
:MONO.streambuf
object as another &obj.:PERIOD.
:RSLTS.
This form of the &fn. creates an initialized &obj.:PERIOD.
If the
:ARG.sb
parameter is &null., &badbit. is set in the &errstate.:PERIOD.
:SALSO.
:SAL typ='dtor'.
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='dtor' prot='public virtual'.~~ostream
:SNPL.
:SNPFLF                   .#include <iostream.h>
:SNPFLF                   .public:
:SNPCD cd_idx='d'.virtual ostream::~~ostream();
:eSNPL.
:SMTICS.
The &fn. does not do anything explicit. The
:MONO.ios
destructor is called for that portion of the &obj.:PERIOD.
The call to the &fn. is inserted implicitly by the compiler
at the point where the &obj. goes out of scope.
:RSLTS.
The &obj. is destroyed.
:SALSO.
:SAL typ='ctor'.
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.put
:SNPL.
:SNPFLF          .#include <iostream.h>
:SNPFLF          .public:
:SNPF index='put'.ostream &amp.ostream::put(          char ch );
:SNPF index='put'.ostream &amp.ostream::put(   signed char ch );
:SNPF index='put'.ostream &amp.ostream::put( unsigned char ch );
:eSNPL.
:SMTICS.
These forms of the &fn. write the
:ARG.ch
character to the &obj.:PERIOD.
:RSLTS.
These forms of the &fn. return a reference to the &obj.:PERIOD.
If an error occurs, &failbit. is set in the &errstate.:PERIOD.
:SALSO.
:SAL typ='mfun'.operator~b<<
:SAL typ='mfun'.write
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.seekp
:SNPL.
:SNPFLF            .#include <iostream.h>
:SNPFLF            .public:
:SNPF index='seekp'.ostream &amp.ostream::seekp( streampos pos );
:eSNPL.
:SMTICS.
This from of the &fn. positions the &obj. to the position specified by the
:ARG.pos
parameter so that the next output operation commences from that position.
:P.
The
:ARG.pos
value is an absolute position within the stream.  It may be obtained via a
call to the
:MONO.tellp
member function.
:RSLTS.
This from of the &fn. returns a reference to the &obj.:PERIOD.
If the seek operation fails, &failbit. is set in the &errstate.:PERIOD.
:SALSO.
:SAL typ='mfun'.tellp
:SAL typ='ofun' ocls='istream'.tellg
:SAL typ='ofun' ocls='istream'.seekg
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.seekp
:SNPL.
:SNPFLF            .#include <iostream.h>
:SNPFLF            .public:
:SNPF index='seekp'.ostream &amp.ostream::seekp( streamoff offset, ios::seekdir dir );
:eSNPL.
:SMTICS.
This from of the &fn. positions the &obj. to the specified position so that the next output
operation commences from that position.
:INCLUDE file='seekdir'.
:RSLTS.
This from of the &fn. returns a reference to the &obj.:PERIOD.
If the seek operation fails, &failbit. is set in the &errstate.:PERIOD.
:SALSO.
:SAL typ='mfun'.tellp
:SAL typ='ofun' ocls='istream'.tellg
:SAL typ='ofun' ocls='istream'.seekg
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.tellp
:SNPL.
:SNPFLF            .#include <iostream.h>
:SNPFLF            .public:
:SNPF index='tellp'.streampos ostream::tellp();
:eSNPL.
:SMTICS.
The &fn. returns the position in the &obj. at which the next character
will be written.
The first character in an &obj. is at offset zero.
:RSLTS.
The &fn. returns the position in the &obj. at which the next character
will be written.
:SALSO.
:SAL typ='mfun'.seekp
:SAL typ='ofun' ocls='istream'.tellg
:SAL typ='ofun' ocls='istream'.seekg
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.write
:SNPL.
:SNPFLF            .#include <iostream.h>
:SNPFLF            .public:
:SNPF index='write'.ostream &amp.ostream::write(          char const *buf, int len );
:SNPF index='write'.ostream &amp.ostream::write(   signed char const *buf, int len );
:SNPF index='write'.ostream &amp.ostream::write( unsigned char const *buf, int len );
:eSNPL.
:SMTICS.
The &fn. performs an unformatted write of the characters specified by the
:ARG.buf
and
:ARG.len
parameters into the &obj.:PERIOD.
:RSLTS.
These member functions return a reference to the &obj.:PERIOD.
If an error occurs, &failbit. is set in the &errstate.:PERIOD.
:eLIBF.
