:CLFNM.ios
:CMT.========================================================================
:LIBF fmt='hdr'.ios
:HFILE.iostream.h
:DVBYL.
:DVBY.istream
:DVBY.ostream
:eDVBYL.
:CLSS.
The &cls. is used to group together common functionality needed for
other derived stream classes. It is not intended that objects of type
:MONO.ios
be created.
:P.
This class maintains state information about the stream. (the
:MONO.ios
name can be thought of as a short-form for I/O State).
Error flags, formatting flags, and values and the connection to the
buffers used for the input and output are all maintained by the &cls.:PERIOD.
No information about the buffer itself is stored in an &obj., merely the
pointer to the buffer information.
:HDG.Protected Member Functions
The following member functions are declared in the protected interface:
:MFNL.
:MFCD cd_idx='c'          .ios();
:MFN index='init'         .void init( streambuf * );
:MFN index='setstate'     .void setstate( ios::iostate );
:eMFNL.
:HDG.Public Enumerations
The following enumeration typedefs are declared in the public interface:
:MTYPL.
:MTYP index='iostate' .typedef int  iostate;
:MTYP index='fmtflags'.typedef long fmtflags;
:MTYP index='openmode'.typedef int  openmode;
:MTYP index='seekdir' .typedef int  seekdir;
:eMTYPL.
:HDG.Public Member Functions
The following member functions are declared in the public interface:
:MFNL.
:MFCD cd_idx='c'.ios( streambuf * );
:MFCD cd_idx='d' .virtual ~~ios();
:MFN index='tie'            .ostream *tie() const;
:MFN index='tie'            .ostream *tie( ostream * );
:MFN index='rdbuf'          .streambuf *rdbuf() const;
:MFN index='rdstate'        .ios::iostate rdstate() const;
:MFN index='clear'          .ios::iostate clear( ios::iostate = 0 );
:MFN index='good'           .int good() const;
:MFN index='bad'            .int bad() const;
:MFN index='fail'           .int fail() const;
:MFN index='eof'            .int eof() const;
:MFN index='exceptions'     .ios::iostate exceptions( ios::iostate );
:MFN index='exceptions'     .ios::iostate exceptions() const;
:MFN index='setf'           .ios::fmtflags setf( ios::fmtflags, ios::fmtflags );
:MFN index='setf'           .ios::fmtflags setf( ios::fmtflags );
:MFN index='unsetf'         .ios::fmtflags unsetf( ios::fmtflags );
:MFN index='flags'          .ios::fmtflags flags( ios::fmtflags );
:MFN index='flags'          .ios::fmtflags flags() const;
:MFN index='fill'           .char fill( char );
:MFN index='fill'           .char fill() const;
:MFN index='precision'      .int precision( int );
:MFN index='precision'      .int precision() const;
:MFN index='width'          .int width( int );
:MFN index='width'          .int width() const;
:MFN index='iword'          .long &amp.iword( int );
:MFN index='pword'          .void *&amp.pword( int );
:MFN index='sync_with_stdio'.static void sync_with_stdio();
:MFN index='bitalloc'       .static ios::fmtflags bitalloc();
:MFN index='xalloc'         .static int xalloc();
:eMFNL.
:HDG.Public Member Operators
The following member operators are declared in the public interface:
:MFNL.
:MFN index='operator void *'.operator void *() const;
:MFN index='operator !'     .int operator !() const;
:eMFNL.
:eCLSS.
:SALSO.
:SAL typ='cls'.iostream
:SAL typ='cls'.istream
:SAL typ='cls'.ostream
:SAL typ='cls'.streambuf
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.bad
:SNPL.
:SNPFLF          .#include <iostream.h>
:SNPFLF          .public:
:SNPF index='bad'.int ios::bad() const;
:eSNPL.
:SMTICS.
The &fn. queries the state of the &obj.:PERIOD.
:RSLTS.
The &fn. returns a non-zero value if &badbit. is set in the &errstate.,
otherwise zero is returned.
:SALSO.
:SAL typ='mfun'.clear
:SAL typ='mfun'.eof
:SAL typ='mfun'.fail
:SAL typ='mfun'.good
:SAL typ='mtyp'.iostate
:SAL typ='mfun'.operator~b!
:SAL typ='mfun'.operator~bvoid~b*
:SAL typ='mfun'.rdstate
:SAL typ='mfun'.setstate
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public static'.bitalloc
:SNPL.
:SNPFLF               .#include <iostream.h>
:SNPFLF               .public:
:SNPF index='bitalloc'.static ios::fmtflags ios::bitalloc();
:eSNPL.
:SMTICS.
The &fn. is used to allocate a new &fmtflags. bit for use by
user derived classes.
:INCLUDE file='ios_stat'.
:P.
The bit value allocated may be used with the member functions that query
and affect &fmtflags.:PERIOD.
In particular, the bit can be set with the
:MONO.setf
or
:MONO.flags
member functions
or the
:MONO.setiosflags
manipulator, and reset with the
:MONO.unsetf
or
:MONO.flags
member functions or the
:MONO.resetiosflags
manipulator.
:P.
There are two constants defined in
:MONO.<iostream.h>
which indicate the number of bits available when a program starts.
:MONO._LAST_FORMAT_FLAG
indicates the last bit used by the built-in format flags described by
&fmtflags.:PERIOD.
:MONO._LAST_FLAG_BIT
indicates the last bit that is available for the &fn. to allocate.
The difference between the bit positions indicates how many bits are
available.
:RSLTS.
The &fn. returns the next available &fmtflags. bit for use by user
derived classes. If no more bits are available, zero is returned.
:SALSO.
:SAL typ='mtyp'.fmtflags
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.clear
:SNPL.
:SNPFLF            .#include <iostream.h>
:SNPFLF            .public:
:SNPF index='clear'.iostate ios::clear( ios::iostate flags = 0 );
:eSNPL.
:SMTICS.
The &fn. is used to change the current value of &iostate. in the
&obj.:PERIOD.
&iostate. is cleared, all bits specified in
:ARG.flags
are set.
:RSLTS.
The &fn. returns the previous value of &iostate.:PERIOD.
:SALSO.
:SAL typ='mfun'.bad
:SAL typ='mfun'.eof
:SAL typ='mfun'.fail
:SAL typ='mfun'.good
:SAL typ='mtyp'.iostate
:SAL typ='mfun'.operator~b!
:SAL typ='mfun'.operator~bvoid~b*
:SAL typ='mfun'.rdstate
:SAL typ='mfun'.setstate
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.eof
:SNPL.
:SNPFLF          .#include <iostream.h>
:SNPFLF          .public:
:SNPF index='eof'.int ios::eof() const;
:eSNPL.
:SMTICS.
The &fn. queries the state of the &obj.:PERIOD.
:RSLTS.
The &fn. returns a non-zero value if &eofbit. is set in the &errstate.,
otherwise zero is returned.
:SALSO.
:SAL typ='mfun'.bad
:SAL typ='mfun'.clear
:SAL typ='mfun'.fail
:SAL typ='mfun'.good
:SAL typ='mtyp'.iostate
:SAL typ='mfun'.rdstate
:SAL typ='mfun'.setstate
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.exceptions
:SNPL.
:SNPFLF                 .#include <iostream.h>
:SNPFLF                 .public:
:SNPF index='exceptions'.ios::iostate ios::exceptions() const;
:SNPF index='exceptions'.ios::iostate ios::exceptions( int enable );
:eSNPL.
:SMTICS.
The &fn. queries and/or sets the bits that control which exceptions
are enabled. &iostate. within the &obj. is used to enable
and disable exceptions.
:P.
When a condition arises that sets a bit in &iostate., a check is made
to see if the same bit is also set in the exception bits.
If so, an exception is thrown. Otherwise, no exception is thrown.
:P.
The first form of the &fn. looks up the current setting of the exception bits.
The bit values are those described by &iostate.:PERIOD.
:P.
The second form of the &fn. sets the exceptions bits to those specified in the
:ARG.enable
parameter, and returns the current settings.
:P.
:RSLTS.
The &fn. returns the previous setting of the exception bits.
:SALSO.
:SAL typ='mfun'.clear
:SAL typ='mtyp'.iostate
:SAL typ='mfun'.rdstate
:SAL typ='mfun'.setstate
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.fail
:SNPL.
:SNPFLF           .#include <iostream.h>
:SNPFLF           .public:
:SNPF index='fail'.int ios::fail() const;
:eSNPL.
:SMTICS.
The &fn. queries the state of the &obj.:PERIOD.
:RSLTS.
The &fn. returns a non-zero value if &failbit. or &badbit. is set
in the &errstate., otherwise zero is returned.
:SALSO.
:SAL typ='mfun'.bad
:SAL typ='mfun'.clear
:SAL typ='mfun'.eof
:SAL typ='mfun'.good
:SAL typ='mtyp'.iostate
:SAL typ='mfun'.operator~b!
:SAL typ='mfun'.operator~bvoid~b*
:SAL typ='mfun'.rdstate
:SAL typ='mfun'.setstate
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.fill
:SNPL.
:SNPFLF           .#include <iostream.h>
:SNPFLF           .public:
:SNPF index='fill'.char ios::fill() const;
:SNPF index='fill'.char ios::fill( char fillchar );
:eSNPL.
:SMTICS.
The &fn. queries and/or sets the &fillchar. used when the size of a
:I1.fill character
formatted object is smaller than the &fmtwidth. specified.
:P.
The first form of the &fn. looks up the current value of the &fillchar.:PERIOD.
:P.
The second form of the &fn. sets the &fillchar. to
:ARG.fillchar
:PERIOD.
:P.
By default, the &fillchar. is a space.
:RSLTS.
The &fn. returns the previous value of the &fillchar.:PERIOD.
:SALSO.
:SAL typ='mtyp'.fmtflags
:SAL typ='mnp'.setfill
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.flags
:SNPL.
:SNPFLF            .#include <iostream.h>
:SNPFLF            .public:
:SNPF index='flags'.ios::fmtflags ios::flags() const;
:SNPF index='flags'.ios::fmtflags ios::flags( ios::fmtflags setbits );
:eSNPL.
:SMTICS.
The &fn. is used to query and/or set the value of &fmtflags.
in the &obj.:PERIOD.
:P.
The first form of the &fn. looks up the current &fmtflags. value.
:P.
The second form of the &fn. sets &fmtflags. to the value specified in the
:ARG.setbits
parameter.
:P.
Note that the
:MONO.setf
public member function only turns bits on, while the &fn.
turns some bits on and some bits off.
:RSLTS.
The &fn. returns the previous &fmtflags. value.
:SALSO.
:SAL typ='mtyp'.fmtflags
:SAL typ='mfun'.setf
:SAL typ='mfun'.unsetf
:SAL typ='mnp'.dec
:SAL typ='mnp'.hex
:SAL typ='mnp'.oct
:SAL typ='mnp'.resetiosflags
:SAL typ='mnp'.setbase
:SAL typ='mnp'.setiosflags
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mtyp' prot='public'.fmtflags
:SNPL.
:SNPFLF                  .#include <iostream.h>
:SNPFLF                  .public:
:SNPFLF                  .enum fmt_flags {
:SNPT index='skipws'     .  skipws     = 0x0001, // skip whitespace
:SNPT index='left'       .  left       = 0x0002, // align field to left edge
:SNPT index='right'      .  right      = 0x0004, // align field to right edge
:SNPT index='internal'   .  internal   = 0x0008, // sign at left, value at right
:SNPT index='dec'        .  dec        = 0x0010, // decimal conversion for integers
:SNPT index='oct'        .  oct        = 0x0020, // octal conversion for integers
:SNPT index='hex'        .  hex        = 0x0040, // hexadecimal conversion for integers
:SNPT index='showbase'   .  showbase   = 0x0080, // show dec/octal/hex base on output
:SNPT index='showpoint'  .  showpoint  = 0x0100, // show decimal and digits on output
:SNPT index='uppercase'  .  uppercase  = 0x0200, // use uppercase for format characters
:SNPT index='showpos'    .  showpos    = 0x0400, // use + for output positive numbers
:SNPT index='scientific' .  scientific = 0x0800, // use scientific notation for output
:SNPT index='fixed'      .  fixed      = 0x1000, // use floating notation for output
:SNPT index='unitbuf'    .  unitbuf    = 0x2000, // flush stream after output
:SNPT index='stdio'      .  stdio      = 0x4000, // flush stdout/stderr after output
:BLANKLINE.
:SNPT index='basefield'  .  basefield  = dec | oct | hex,
:SNPT index='adjustfield'.  adjustfield= left | right | internal,
:SNPT index='floatfield' .  floatfield = scientific | fixed
:SNPFLF                  .};
:SNPT index='fmtflags'   .typedef long fmtflags;
:eSNPL.
:SMTICS.
The type
:MONO.ios::fmt_flags
is a set of bits representing methods of formatting objects written to
the stream and interpreting objects read from the stream.  The &fn.
represents the same set of bits, but uses a
:MONO.long
to represent the values, thereby avoiding problems made possible by the
compiler's ability to use smaller types for enumerations.  All uses of
these bits should use the &fn.:PERIOD.
:P.
The bit values defined by the &fn. are set and read by the member functions
:MONO.setf
:CONT.,
:MONO.unsetf
and
:MONO.flags
:CONT., as well as the
manipulators
:MONO.setiosflags
and
:MONO.resetiosflags
:PERIOD.
:P.
Because one field is used to store all of these bits, there are three special
values used to mask various groups of bits. These values are named
:MONO.ios::basefield
:CONT.,
:MONO.ios::adjustfield
and
:MONO.ios::floatfield
:CONT.,
and are discussed with the bits that they are used to mask.
:P.
:MONO.ios::skipws
controls whether or not whitespace
characters are automatically skipped when using an
:MONO.operator~b>>
extractor.
If
:MONO.ios::skipws
is on, any use of the
:MONO.operator~b>>
extractor skips whitespace characters
before inputting the next item. Otherwise, skipping of whitespace characters
must be handled by the program.
:P.
:MONO.ios::left
:CONT.,
:MONO.ios::right
and
:MONO.ios::internal
control the alignment of items written using an
:MONO.operator~b<<
inserter.
These bits are usually used in conjunction with the &fmtwidth. and &fillchar.:PERIOD.
:P.
:MONO.ios::adjustfield
can be used to mask the alignment bits returned by the
:MONO.setf
:CONT.,
:MONO.unsetf
and
:MONO.flags
member functions, and for setting new values to ensure that no other bits
are accidentally affected.
:P.
When the item to be written is smaller than the &fmtwidth. specified,
&fillchar.s are written to occupy the additional space. If
:MONO.ios::left
is in effect, the item is written in the left portion of the available
space, and &fillchar.s are written in the right portion. If
:MONO.ios::right
is in effect, the item is written in the right portion of the available
space, and &fillchar.s are written in the left portion. If
:MONO.ios::internal
is in effect, any sign character or base indicator is written in the
left portion, the digits are written in the right portion, and
&fillchar.s are written in between.
:P.
If no alignment is specified,
:MONO.ios::right
is assumed.
:P.
If the item to be written is as big as or bigger than the &fmtwidth. specified,
no &fillchar.s are written and the alignment is ignored.
:P.
:MONO.ios::dec
:CONT.,
:MONO.ios::oct
and
:MONO.ios::hex
control the base used to format integers being written to the stream, and
also control the interpretation of integers being read from the stream.
:P.
:MONO.ios::basefield
can be used to mask the base bits returned by the member functions
:MONO.setf
:CONT.,
:MONO.unsetf
and
:MONO.flags
:CONT., and for setting new values to ensure that no other bits are
accidentally affected.
:P.
When an integer is being read from the stream, these bits control the base
used for the interpretation of the digits. If none of these bits is set,
a number that starts with
:MONO.0x
or
:MONO.0X
is interpreted as hexadecimal (digits
:MONO.0123456789
:CONT., plus the letters
:MONO.abcdef
or
:MONO.ABCDEF
:CONT.), a number that starts with
:MONO.0
(zero) is interpreted as octal (digits
:MONO.01234567
:CONT.), otherwise the number is interpreted as decimal (digits
:MONO.0123456789
:CONT.).
If one of the bits is set, then the prefix is not necessary and the number
is interpreted according to the bit.
:P.
When any one of the integer types is being written to the stream, it can be
written in decimal, octal or hexadecimal. If none of these bits is set,
:MONO.ios::dec
is assumed.
:P.
If
:MONO.ios::dec
is set (or assumed), the integer is written in decimal (digits
:MONO.0123456789
:CONT.). No prefix is included.
:P.
If
:MONO.ios::oct
is set, the integer is written in octal (digits
:MONO.01234567
:CONT.). No sign character is written, as the number is treated as an
unsigned quantity upon conversion to octal.
:P.
If
:MONO.ios::hex
is set, the integer is written in hexadecimal (digits
:MONO.0123456789
:CONT., plus the letters
:MONO.abcdef
or
:MONO.ABCDEF
:CONT., depending on the setting of
:MONO.ios::uppercase
:CONT.).
No sign character is written, as the number is treated as an unsigned
quantity upon conversion to hexadecimal.
:P.
:MONO.ios::showbase
controls whether or not integers written to the stream in octal or
hexadecimal form have a prefix that indicates the base of the number.
If the bit is set, decimal numbers are written without a prefix,
octal numbers are written with the prefix
:MONO.0
(zero) and hexadecimal numbers are written with the prefix
:MONO.0x
or
:MONO.0X
depending on the setting of
:MONO.ios::uppercase
:PERIOD.
If the
:MONO.ios::showbase
is not set, no prefixes are written.
:P.
:MONO.ios::showpoint
is used to control whether or not the decimal point and trailing zeroes are
trimmed when floating-point numbers are written to the stream. If the bit is
set, no trimming is done, causing the number to appear with the
specified &fmtprec.:PERIOD.
If the bit is not set, any trailing zeroes after
the decimal point are trimmed, and if not followed by any digits, the
decimal point is removed as well.
:P.
:MONO.ios::uppercase
is used to force to upper-case all letters used in formatting numbers,
including the letter-digits
:MONO.abcdef
:CONT.,
the
:MONO.x
hexadecimal prefix, and the
:MONO.e
used for the exponents in floating-point numbers.
:P.
:MONO.ios::showpos
controls whether or not a
:MONO.+
is added to the front of positive integers being written to the stream. If
the bit is set, the number is positive and the number is being written in
decimal, a
:MONO.+
is written before the first digit.
:P.
:MONO.ios::scientific
and
:MONO.ios::fixed
controls the form used for writing floating-point numbers to the stream.
Floating-point numbers can be written in scientific notation (also
called exponential notation) or in fixed-point notation.
:P.
:MONO.ios::floatfield
can be used to mask the floating-format bits returned by the member functions
:MONO.setf
:CONT.,
:MONO.unsetf
and
:MONO.flags
:CONT.,
and for setting new values to ensure that no other bits are accidentally
affected.
:P.
If
:MONO.ios::scientific
is set, the floating-point number is written with a leading
:MONO.-
sign (for negative numbers), a digit, a decimal point, more digits, an
:MONO.e
(or
:MONO.E
if
:MONO.ios::uppercase
is set), a
:MONO.+
or
:MONO.-
sign, and two or three digits representing the exponent. The digit
before the decimal is not zero unless the number is zero. The total number of
digits before and after the decimal is equal to the specified &fmtprec.:PERIOD.
If
:MONO.ios::showpoint
is not set, trimming of the decimal and digits following the decimal
may occur.
:P.
If
:MONO.ios::fixed
is set, the floating-point number is written with a
:MONO.-
sign (for negative numbers), at least one digit, the decimal point,
and as many digits following the decimal as specified by the &fmtprec.:PERIOD.
If
:MONO.ios::showpoint
is not set, trimming of the decimal and digits following the decimal
may occur.
:P.
If neither
:MONO.ios::scientific
nor
:MONO.ios::fixed
is specified, the floating-point number is formatted using scientific
notation provided one or both of the following conditions are met:
:UL.
:LI.the exponent is less than -4, or,
:LI.the exponent is greater than the &fmtprec.:PERIOD.
:eUL.
:P.
Otherwise, fixed-point notation is used.
:P.
:MONO.ios::unitbuf
controls whether or not the stream is flushed after each item is written.
If the bit is set, every item that is written to the stream is followed
by a flush operation, which ensures that the I/O stream buffer associated
with the stream is kept empty, immediately transferring the data to its final
destination.
:P.
:MONO.ios::stdio
controls whether or not the stream is synchronized after each item is written.
If the bit is set, every item that is written to the stream causes the
stream to be synchronized, which means any input or output buffers are
flushed so that an I/O operation performed using C (not C++) I/O behaves in
an understandable way. If the output buffer was not flushed, writing
using C++ and then C I/O functions could cause the output from the C
functions to appear before the output from the C++ functions, since the
characters might be sitting in the C++ output buffer. Similarly, after the C
output operations are done, a call should be made to the C library
:MONO.fflush
function on the appropriate stream before resuming C++ output operations.
:SALSO.
:SAL typ='mfun'.flags
:SAL typ='mfun'.setf
:SAL typ='mfun'.unsetf
:SAL typ='mnp'.dec
:SAL typ='mnp'.hex
:SAL typ='mnp'.oct
:SAL typ='mnp'.resetiosflags
:SAL typ='mnp'.setbase
:SAL typ='mnp'.setiosflags
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.good
:SNPL.
:SNPFLF           .#include <iostream.h>
:SNPFLF           .public:
:SNPF index='good'.int ios::good() const;
:eSNPL.
:SMTICS.
The &fn. queries the state of the &obj.:PERIOD.
:RSLTS.
The &fn. returns a non-zero value if none of &iostate. is clear,
otherwise zero is returned.
:SALSO.
:SAL typ='mfun'.bad
:SAL typ='mfun'.clear
:SAL typ='mfun'.eof
:SAL typ='mfun'.fail
:SAL typ='mtyp'.iostate
:SAL typ='mfun'.rdstate
:SAL typ='mfun'.setstate
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public protected'.init
:SNPL.
:SNPFLF           .#include <iostream.h>
:SNPFLF           .protected:
:SNPF index='init'.void ios::init( streambuf *sb );
:eSNPL.
:SMTICS.
The &fn. is used by derived classes to explicitly initialize the
:MONO.ios
portion of the derived object, and to associate a
:MONO.streambuf
with the &obj.:PERIOD.
The &fn. performs the following steps:
:OL.
:LI.The default &fillchar. is set to a space.
:LI.The &fmtprec. is set to six.
:LI.The
:MONO.streambuf
pointer (returned by the
:MONO.rdbuf
member function) is set to
:ARG.sb
:PERIOD.
:LI.The remaining fields of the &obj. are initialized to zero.
:eOL.
:RSLTS.
If
:ARG.sb
is &null.
the &badbit. is set in the &errstate.:PERIOD.
:SALSO.
:SAL typ='ctor'.
:SAL typ='mfun'.rdbuf
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='ctor' prot='protected'.ios
:SNPL.
:SNPFLF                    .#include <iostream.h>
:SNPFLF                    .protected:
:SNPCD cd_idx='c'.ios::ios();
:eSNPL.
:SMTICS.
This form of the &fn. creates a default &obj. that is initialized,
but does not have an associated
:MONO.streambuf
:PERIOD.
Initialization of an &obj. is handled by the
:MONO.init
protected member function.
:RSLTS.
This &fn. creates an &obj. and sets &badbit. in the &errstate.:PERIOD.
:SALSO.
:SAL typ='dtor'.
:SAL typ='mfun'.init
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='ctor' prot='public'.ios
:SNPL.
:SNPFLF                    .#include <iostream.h>
:SNPFLF                    .public:
:SNPCD cd_idx='c'.ios::ios( streambuf *sb );
:eSNPL.
:SMTICS.
This form of the &fn. creates an &obj. that is initialized and has an associated
:MONO.streambuf
:PERIOD.
Initialization of an &obj. is handled by the
:MONO.init
protected member function.
Once the
:MONO.init
protected member function is completed, the &obj.'s
:MONO.streambuf
pointer is set to
:ARG.sb
:PERIOD.
If
:ARG.sb
is not &null., &badbit. is cleared from the &errstate.:PERIOD.
:RSLTS.
This &fn. creates an &obj. and, if
:ARG.sb
is &null.
:CONT., sets &badbit. in the &errstate.:PERIOD.
:SALSO.
:SAL typ='dtor'.
:SAL typ='mfun'.init
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='dtor' prot='public virtual'.~~ios
:SNPL.
:SNPFLF                   .#include <iostream.h>
:SNPFLF                   .public:
:SNPCD cd_idx='d'.virtual ios::~~ios();
:eSNPL.
:SMTICS.
The &fn. destroys an &obj.:PERIOD.
The call to the &fn. is inserted implicitly by the compiler
at the point where the &obj. goes out of scope.
:RSLTS.
The &obj. is destroyed.
:SALSO.
:SAL typ='ctor'.
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mtyp' prot='public'.iostate
:SNPL.
:SNPFLF              .#include <iostream.h>
:SNPFLF              .public:
:SNPFLF              .enum io_state {
:SNPT index='goodbit'.  goodbit = 0x00, // no errors
:SNPT index='badbit' .  badbit  = 0x01, // operation failed, may not proceed
:SNPT index='failbit'.  failbit = 0x02, // operation failed, may proceed
:SNPT index='eofbit' .  eofbit  = 0x04  // end of file encountered
:SNPFLF              .};
:SNPT index='iostate'.typedef int iostate;
:eSNPL.
:SMTICS.
The type
:MONO.ios::io_state
is a set of bits representing the current state of the stream.
The &fn. represents the same set of bits, but uses an
:MONO.int
to represent the values, thereby avoiding problems made possible by the
compiler's ability to use smaller types for enumerations.
All uses of these bits should use the &fn.:PERIOD.
:P.
The bit values defined by the &fn. can be read and set by the
member functions
:MONO.rdstate
and
:MONO.clear
:CONT.,
and can be used to control exception handling with the member function
:MONO.exceptions
:PERIOD.
:P.
&badbit. represents the state where the stream is
no longer usable because of some error condition.
:P.
&failbit.  represents the state where the previous operation on the
stream failed, but the stream is still usable.  Subsequent operations on
the stream are possible, but the state must be cleared using the
:MONO.clear
member function.
:P.
&eofbit. represents the state where the end-of-file condition has been
encountered.
The stream may still be used, but the state must be cleared using the
:MONO.clear
member function.
:P.
Even though &goodbit. is not a bit value (because its value is zero,
which has no bits on), it is provided for completeness.
:SALSO.
:SAL typ='mtyp'.bad
:SAL typ='mtyp'.clear
:SAL typ='mtyp'.eof
:SAL typ='mtyp'.fail
:SAL typ='mtyp'.good
:SAL typ='mfun'.operator~b!
:SAL typ='mfun'.operator~bvoid~b*
:SAL typ='mfun'.rdstate
:SAL typ='mfun'.setstate
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.iword
:SNPL.
:SNPFLF            .#include <iostream.h>
:SNPFLF            .public:
:SNPF index='iword'.long &amp.ios::iword( int index );
:eSNPL.
:SMTICS.
The &fn. creates a reference to a
:MONO.long int
:CONT., which may be used to store and retrieve any suitable integer value.
The
:ARG.index
parameter specifies which
:MONO.long int
is to be referenced and must be obtained from a call to the
:MONO.xalloc
static member function.
:INCLUDE file='ipword'.
:RSLTS.
The &fn. returns a reference to a
:MONO.long int
:PERIOD.
:SALSO.
:SAL typ='mfun'.pword
:SAL typ='mfun'.xalloc
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mtyp' prot='public'.openmode
:SNPL.
:SNPFLF                .#include <iostream.h>
:SNPFLF                .public:
:SNPFLF                .enum open_mode {
:SNPT index='in'       .  in        = 0x0001, // open for input
:SNPT index='out'      .  out       = 0x0002, // open for output
:SNPT index='atend'    .  atend     = 0x0004, // seek to end after opening
:SNPT index='append'   .  append    = 0x0008, // open for output, append to the end
:SNPT index='truncate' .  truncate  = 0x0010, // discard contents after opening
:SNPT index='nocreate' .  nocreate  = 0x0020, // open only an existing file
:SNPT index='noreplace'.  noreplace = 0x0040, // open only a new file
:SNPT index='text'     .  text      = 0x0080, // open as text file
:SNPT index='binary'   .  binary    = 0x0100, // open as binary file
:BLANKLINE.
:SNPT index='app'      .  app       = append,  // synonym
:SNPT index='ate'      .  ate       = atend,   // synonym
:SNPT index='trunc'    .  trunc     = truncate // synonym
:SNPFLF                .};
:SNPT index='openmode' .typedef int openmode;
:eSNPL.
:SMTICS.
The type
:MONO.ios::open_mode
is a set of bits representing ways of opening a stream.
The &fn. represents the same set of bits, but uses an
:MONO.int
to represent the values, thereby avoiding problems made possible by the
compiler's ability to use smaller types for enumerations.
All uses of these bits should use the &fn.:PERIOD.
:P.
The bit values defined by &fn. can be specified in the constructors
for stream objects, as well as in various member functions.
:P.
:MONO.ios::in
is specified in a stream for which input operations may be performed.
:MONO.ios::out
is specified in a stream for which output operations may be performed.
A stream for which only
:MONO.ios::in
is specified is referred to as an
:ITALICS.input
stream. A stream for which only
:MONO.ios::out
is specified is referred to as an
:ITALICS.output
stream.
A stream where both
:MONO.ios::in
and
:MONO.ios::out
are specified is referred to as an
:ITALICS.input/output
stream.
:P.
:MONO.ios::atend
and
:MONO.ios::ate
are equivalent, and either one is specified for streams that are to be
positioned to the end before the first operation takes place.
:MONO.ios:ate
is provided for historical purposes and
compatibility with other implementations of I/O streams.
Note that this bit positions the stream to the end exactly once, when
the stream is opened.
:P.
:MONO.ios::append
and
:MONO.ios::app
are equivalent, and either one is specified for streams that are to be
positioned to the end before any and all output operations take place.
:MONO.ios::app
is provided for historical purposes and
compatibility with other implementations of I/O streams.
Note that this bit causes the stream to be positioned to the end before
each output operation, while
:MONO.ios::atend
causes the stream to be positioned to the end only when first opened.
:P.
:MONO.ios::truncate
and
:MONO.ios::trunc
are equivalent, and either one is specified for streams that are to be
truncated to zero length before the first operation takes place.
:MONO.ios::trunc
is provided for historical purposes and
compatibility with other implementations of I/O streams.
:P.
:MONO.ios::nocreate
is specified if the file must exist before it is opened.
If the file does not exist, an error occurs.
:P.
:MONO.ios::noreplace
is specified if the file must not exist before it is opened. That is,
the file must be a new file. If the file exists, an error occurs.
:P.
:MONO.ios::text
is specified if the file is to be treated as a
:ITALICS.text
file. A text file is divided into records, and each record is terminated by a
:ITALICS.new-line
character, usually represented as
:MONO.'\n'.
The new-line character is translated into a form that is compatible
with the underlying file system's concept of text files.
This conversion happens automatically whenever the new-line is written
to the file, and the inverse conversion (to the new-line character)
happens automatically whenever the end of a record is read from the
file system.
:P.
:MONO.ios::binary
is specified if the file is to be treated as a
:ITALICS.binary
file. Binary files are streams of characters.
No character has a special meaning.
No grouping of characters into records is apparent to the program,
although the underlying file system may cause such a grouping to occur.
:P.
The following default behaviors are defined:
:P.
If
:MONO.ios::out
is specified and none of
:MONO.ios::in
:CONT.,
:MONO.ios::append
or
:MONO.ios::atend
are specified,
:MONO.ios::truncate
is assumed.
:P.
If
:MONO.ios::append
is specified,
:MONO.ios::out
is assumed.
:P.
If
:MONO.ios::truncate
is specified,
:MONO.ios::out
is assumed.
:P.
If neither
:MONO.ios::text
nor
:MONO.ios::binary
is specified,
:MONO.ios::text
is assumed.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.operator !
:SNPL.
:SNPFLF                 .#include <iostream.h>
:SNPFLF                 .public:
:SNPF index='operator !'.int ios::operator !() const;
:eSNPL.
:SMTICS.
The &fn. tests the &errstate. of the &obj.:PERIOD.
:RSLTS.
The &fn. returns a non-zero value if either of &failbit. or &badbit. bits
are set in the &errstate., otherwise zero is returned.
:SALSO.
:SAL typ='mfun'.bad
:SAL typ='mfun'.clear
:SAL typ='mfun'.fail
:SAL typ='mfun'.good
:SAL typ='mtyp'.iostate
:SAL typ='mfun'.operator~bvoid~b*
:SAL typ='mfun'.rdstate
:SAL typ='mfun'.setstate
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.operator void *
:SNPL.
:SNPFLF                      .#include <iostream.h>
:SNPFLF                      .public:
:SNPF index='operator void *'.ios::operator void *() const;
:eSNPL.
:SMTICS.
The &fn. converts the &obj. into a pointer to
:MONO.void
:PERIOD.
The actual pointer value returned is meaningless and intended only for
comparison with &null. to determine the &errstate. of the &obj.:PERIOD.
:RSLTS.
The &fn. returns a &null. pointer
if either of &failbit. or &badbit. bits are set in the
&errstate., otherwise a non-&null. pointer is returned.
:SALSO.
:SAL typ='mfun'.bad
:SAL typ='mfun'.clear
:SAL typ='mfun'.fail
:SAL typ='mfun'.good
:SAL typ='mtyp'.iostate
:SAL typ='mfun'.operator~b!
:SAL typ='mfun'.rdstate
:SAL typ='mfun'.setstate
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.precision
:SNPL.
:SNPFLF                .#include <iostream.h>
:SNPFLF                .public:
:SNPF index='precision'.int ios::precision() const;
:SNPF index='precision'.int ios::precision( int prec );
:eSNPL.
:SMTICS.
The &fn. is used to query and/or set the &fmtprec.:PERIOD.
:I1.format precision
The &fmtprec. is used to control the number of digits of precision used when
formatting floating-point numbers. For scientific notation, the
&fmtprec. describes the total number of
digits before and after the decimal point, but not including the
exponent.
For fixed-point notation, the &fmtprec. describes the number of digits
after the decimal point.
:P.
The first form of the &fn. looks up the current &fmtprec.:PERIOD.
:P.
The second form of the &fn. sets the &fmtprec. to
:ARG.prec
:PERIOD.
:P.
By default, the &fmtprec. is six. If
:ARG.prec
is specified to be less than zero, the &fmtprec. is set to six.
Otherwise, the specified &fmtprec. is used. For scientific notation, a
&fmtprec. of zero is treated as a precision of one.
:RSLTS.
The &fn. returns the previous &fmtprec. setting.
:SALSO.
:SAL typ='mtyp'.fmtflags
:SAL typ='mnp'.setprec
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.pword
:SNPL.
:SNPFLF            .#include <iostream.h>
:SNPFLF            .public:
:SNPF index='pword'.void * &amp.ios::pword( int index );
:eSNPL.
:SMTICS.
The &fn. creates a reference to a
:MONO.void
pointer, which may be used to store and retrieve any suitable pointer value.
The
:ARG.index
parameter specifies which
:MONO.void
pointer is to be referenced and must be obtained from a call to the
:MONO.xalloc
static member function.
:INCLUDE file='ipword'.
:RSLTS.
The &fn. returns a reference to a
:MONO.void
pointer.
:SALSO.
:SAL typ='mfun'.iword
:SAL typ='mfun'.xalloc
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.rdbuf
:SNPL.
:SNPFLF            .#include <iostream.h>
:SNPFLF            .public:
:SNPF index='rdbuf'.streambuf *ios::rdbuf() const;
:eSNPL.
:SMTICS.
The &fn. looks up the pointer to the
:MONO.streambuf
object which maintains the buffer associated with the &obj.:PERIOD.
:RSLTS.
The &fn. returns the pointer to the
:MONO.streambuf
object associated with the &obj.:PERIOD.
If there is no associated
:MONO.streambuf
object, &null. is returned.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.rdstate
:SNPL.
:SNPFLF              .#include <iostream.h>
:SNPFLF              .public:
:SNPF index='rdstate'.iostate ios::rdstate() const;
:eSNPL.
:SMTICS.
The &fn. is used to query the current value of &iostate. in the &obj.
without modifying it.
:RSLTS.
The &fn. returns the current value of &iostate.:PERIOD.
:SALSO.
:SAL typ='mfun'.bad
:SAL typ='mfun'.clear
:SAL typ='mfun'.eof
:SAL typ='mfun'.fail
:SAL typ='mfun'.good
:SAL typ='mtyp'.iostate
:SAL typ='mfun'.operator~b!
:SAL typ='mfun'.operator~bvoid~b*
:SAL typ='mfun'.setstate
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mtyp' prot='public'.seekdir
:SNPL.
:SNPFLF              .#include <iostream.h>
:SNPFLF              .public:
:SNPFLF              .enum seek_dir {
:SNPT index='beg'    .  beg, // seek from beginning
:SNPT index='cur'    .  cur, // seek from current position
:SNPT index='end'    .  end  // seek from end
:SNPFLF              .};
:SNPT index='seekdir'.typedef int seekdir;
:eSNPL.
:SMTICS.
The type
:MONO.ios::seek_dir
is a set of bits representing
different methods of seeking within a stream.
The &fn. represents the same set of bits, but uses an
:MONO.int
to represent the values, thereby avoiding problems made possible by the
compiler's ability to use smaller types for enumerations.
All uses of these bits should use the &fn.:PERIOD.
:P.
The bit values defined by &fn. are used by the member functions
:MONO.seekg
and
:MONO.seekp
:CONT.,
as well the
:MONO.seekoff
and
:MONO.seekpos
member functions in classes derived from the
:MONO.streambuf
class.
:P.
:MONO.ios::beg
causes the seek offset to be interpreted as an offset from the beginning of
the stream. The offset is specified as a positive value.
:P.
:MONO.ios::cur
causes the seek offset to be interpreted as an offset from the current
position of the stream. If the offset is a negative value, the seek is
towards the start of the stream. Otherwise, the seek is towards the end of
the stream.
:P.
:MONO.ios::end
causes the seek offset to be interpreted as an offset from the end of the
stream. The offset is specified as a negative value.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.setf
:SNPL.
:SNPFLF           .#include <iostream.h>
:SNPFLF           .public:
:SNPF index='setf'.ios::fmtflags ios::setf( ios::fmtflags onbits );
:SNPF index='setf'.ios::fmtflags ios::setf( ios::fmtflags setbits,
:SNPFLF           .                         ios::fmtflags mask );
:eSNPL.
:SMTICS.
The &fn. is used to set bits in &fmtflags. in the &obj.:PERIOD.
:P.
The first form is used to turn on the bits that are on in the
:ARG.onbits
parameter. (
:CONT.:ARG.onbits
is or'ed into &fmtflags.).
:P.
The second form is used to turn off the bits specified in the
:ARG.mask
parameter and turn on the bits specified in the
:ARG.setbits
parameter.
This form is particularly useful for setting the bits described by the
:MONO.ios::basefield
:CONT.,
:MONO.ios::adjustfield
and
:MONO.ios::floatfield
values, where only one bit should be on at a time.
:RSLTS.
Both forms of the &fn. return the previous &fmtflags. value.
:SALSO.
:SAL typ='mtyp'.fmtflags
:SAL typ='mfun'.setf
:SAL typ='mfun'.unsetf
:SAL typ='mnp'.dec
:SAL typ='mnp'.hex
:SAL typ='mnp'.oct
:SAL typ='mnp'.setbase
:SAL typ='mnp'.setiosflags
:SAL typ='mnp'.resetiosflags
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='protected'.setstate
:SNPL.
:SNPFLF               .#include <iostream.h>
:SNPFLF               .protected:
:SNPF index='setstate'.void ios::setstate( int or_bits );
:eSNPL.
:SMTICS.
The &fn. is provided as a convenience for classes derived from the &cls.:PERIOD.
It turns on the &errstate. bits that are set in the
:ARG.or_bits
parameter, and leaves the other &errstate. bits unchanged.
:RSLTS.
The &fn. sets the bits specified by
:ARG.or_bits
in the &errstate.:PERIOD.
:SALSO.
:SAL typ='mfun'.bad
:SAL typ='mfun'.clear
:SAL typ='mfun'.eof
:SAL typ='mfun'.fail
:SAL typ='mfun'.good
:SAL typ='mtyp'.iostate
:SAL typ='mfun'.operator~b!
:SAL typ='mfun'.operator~bvoid~b*
:SAL typ='mfun'.rdstate
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public static'.sync_with_stdio
:SNPL.
:SNPFLF                      .#include <iostream.h>
:SNPFLF                      .public:
:SNPF index='sync_with_stdio'.static void ios::sync_with_stdio();
:eSNPL.
:SMTICS.
The &fn. is obsolete.
It is provided for compatibility.
:RSLTS.
The &fn. has no return value.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.tie
:SNPL.
:SNPFLF          .#include <iostream.h>
:SNPFLF          .public:
:SNPF index='tie'.ostream *ios::tie() const;
:SNPF index='tie'.ostream *ios::tie( ostream *ostrm );
:eSNPL.
:SMTICS.
The &fn. is used to query and/or set up a connection between the &obj.
and another stream.
The connection causes the output stream specified by
:ARG.ostrm
to be flushed whenever the &obj. is about to read characters from a
device or is about to write characters to an output buffer or device.
:P.
The first form of the &fn. is used to query the current tie.
:P.
The second form of the &fn. is used to set the tied stream to
:ARG.ostrm
:PERIOD.
:P.
Normally, the predefined streams &cin. and &cerr. set up ties to &cout. so that
any input from the terminal flushes any buffered output, and any writes to
&cerr. flush &cout. before the characters are written. &cout. does not set
up a tie to &cerr. because &cerr. has the flag
:MONO.ios::unitbuf
set, so it flushes itself after every write operation.
:RSLTS.
Both forms of the &fn. return the previous tie value.
:SALSO.
:SAL typ='mtyp'.fmtflags
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.unsetf
:SNPL.
:SNPFLF             .#include <iostream.h>
:SNPFLF             .public:
:SNPF index='unsetf'.ios::fmtflags ios::unsetf( ios::fmtflags offbits );
:eSNPL.
:SMTICS.
The &fn. is used to turn off bits in &fmtflags. that are set in the
:ARG.offbits
parameter. All other bits in &fmtflags. are unchanged.
:RSLTS.
The &fn. returns the old &fmtflags. value.
:SALSO.
:SAL typ='mtyp'.fmtflags
:SAL typ='mfun'.setf
:SAL typ='mfun'.unsetf
:SAL typ='mnp'.dec
:SAL typ='mnp'.hex
:SAL typ='mnp'.oct
:SAL typ='mnp'.setbase
:SAL typ='mnp'.setiosflags
:SAL typ='mnp'.resetiosflags
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.width
:SNPL.
:SNPFLF            .#include <iostream.h>
:SNPFLF            .public:
:SNPF index='width'.int ios::width() const;
:SNPF index='width'.int ios::width( int wid );
:eSNPL.
:SMTICS.
The &fn. is used to query and/or set the &fmtwidth. used to format
:I1.format width
the next item. A &fmtwidth. of zero indicates that the item is to be written
using exactly the number of positions required. Other values indicate
that the item must occupy at least that many positions.
If the formatted item is larger than the specified &fmtwidth., the
&fmtwidth. is ignored and the item is formatted using the required number
of positions.
:P.
The first form of the &fn. is used to query the &fmtwidth. that is to be used
for the next item.
:P.
The second form of the &fn. is used to set the &fmtwidth. to
:ARG.wid
for the next item to be formatted.
:P.
After an item has been formatted, the &fmtwidth. is reset to zero.
Therefore, any non-zero &fmtwidth. must be set before each item that is
to be formatted.
:RSLTS.
The &fn. returns the previous &fmtwidth.:PERIOD.
:SALSO.
:SAL typ='mtyp'.fmtflags
:SAL typ='mnp'.setw
:SAL typ='mnp'.setwidth
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public static'.xalloc
:SNPL.
:SNPFLF             .#include <iostream.h>
:SNPFLF             .public:
:SNPF index='xalloc'.static int ios::xalloc();
:eSNPL.
:SMTICS.
The &fn. returns an index into an array of items that the program may
use for any purpose. Each item can be either a
:MONO.long int
or a pointer to
:MONO.void
:PERIOD.
The index can be used with the
:MONO.iword
and
:MONO.pword
member functions.
:INCLUDE file='ios_stat'.
:RSLTS.
The &fn. returns an index for use with the
:MONO.iword
and
:MONO.pword
member functions.
:SALSO.
:SAL typ='mfun'.iword
:SAL typ='mfun'.pword
:eSALSO.
:eLIBF.
