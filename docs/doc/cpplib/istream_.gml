:CLFNM.istream
:CMT.========================================================================
:LIBF fmt='hdr'.istream
:HFILE.iostream.h
:DVFML.
:DVFM.ios
:eDVFML.
:DVBYL.
:DVBY.iostream
:DVBY.ifstream
:DVBY.istrstream
:eDVBYL.
:CLSS.
The &cls. supports reading characters from a class derived from
:MONO.streambuf
:CONT., and provides formatted conversion of characters into other types
(such as integers and floating-point numbers). The
:MONO.streambuf
class provides the methods for communicating with the external device
(keyboard, disk), while the &cls. provides the interpretation of the
resulting characters.
:P.
Generally, an &obj.  won't be explicitly created by a program, since
there is no mechanism at this level to open a device.  The only default
&obj.  in a program is &cin., which reads from standard input (usually
the keyboard).
:P.
The &cls. supports two basic concepts of input: formatted and unformatted.
The overloaded
:MONO.operator~b>>
member functions are called
:ITALICS.extractors
:I1.extractor
and they provide the support for formatted input. The rest of the member
functions deal with unformatted input, managing the state of the
:MONO.ios
object and providing a friendlier interface to the associated
:MONO.streambuf
object.
:HDG.Protected Member Functions
The following protected member functions are declared:
:MFNL.
:MFCD cd_idx='c'     .istream();
:MFN index='eatwhite'.eatwhite();
:eMFNL.
:HDG.Public Member Functions
The following public member functions are declared:
:MFNL.
:MFCD cd_idx='c'.istream( istream const & );
:MFCD cd_idx='c'.istream( streambuf * );
:MFCD cd_idx='d' .virtual ~~istream();
:MFN index='ipfx'         .int ipfx( int = 0 );
:MFN index='isfx'         .void isfx();
:MFN index='get'          .int get();
:MFN index='get'          .istream &amp.get(          char *, int, char = '\n' );
:MFN index='get'          .istream &amp.get(   signed char *, int, char = '\n' );
:MFN index='get'          .istream &amp.get( unsigned char *, int, char = '\n' );
:MFN index='get'          .istream &amp.get(          char & );
:MFN index='get'          .istream &amp.get(   signed char & );
:MFN index='get'          .istream &amp.get( unsigned char & );
:MFN index='get'          .istream &amp.get( streambuf &, char = '\n' );
:MFN index='getline'      .istream &amp.getline(          char *, int, char = '\n' );
:MFN index='getline'      .istream &amp.getline(   signed char *, int, char = '\n' );
:MFN index='getline'      .istream &amp.getline( unsigned char *, int, char = '\n' );
:MFN index='ignore'       .istream &amp.ignore( int = 1, int = EOF );
:MFN index='read'         .istream &amp.read(          char *, int );
:MFN index='read'         .istream &amp.read(   signed char *, int );
:MFN index='read'         .istream &amp.read( unsigned char *, int );
:MFN index='seekg'        .istream &amp.seekg( streampos );
:MFN index='seekg'        .istream &amp.seekg( streamoff, ios::seekdir );
:MFN index='putback'      .istream &amp.putback( char );
:MFN index='tellg'        .streampos tellg();
:MFN index='gcount'       .int gcount() const;
:MFN index='peek'         .int peek();
:MFN index='sync'         .int sync();
:eMFNL.
:HDG.Public Member Operators
The following public member operators are declared:
:MFNL.
:MFN index='operator =' .istream &amp.operator  =( streambuf * );
:MFN index='operator =' .istream &amp.operator  =( istream const & );
:MFN index='operator >>'.istream &amp.operator >>(          char * );
:MFN index='operator >>'.istream &amp.operator >>(   signed char * );
:MFN index='operator >>'.istream &amp.operator >>( unsigned char * );
:MFN index='operator >>'.istream &amp.operator >>(          char & );
:MFN index='operator >>'.istream &amp.operator >>(   signed char & );
:MFN index='operator >>'.istream &amp.operator >>( unsigned char & );
:MFN index='operator >>'.istream &amp.operator >>(   signed short & );
:MFN index='operator >>'.istream &amp.operator >>( unsigned short & );
:MFN index='operator >>'.istream &amp.operator >>(   signed int & );
:MFN index='operator >>'.istream &amp.operator >>( unsigned int & );
:MFN index='operator >>'.istream &amp.operator >>(   signed long & );
:MFN index='operator >>'.istream &amp.operator >>( unsigned long & );
:MFN index='operator >>'.istream &amp.operator >>(       float & );
:MFN index='operator >>'.istream &amp.operator >>(      double & );
:MFN index='operator >>'.istream &amp.operator >>( long double & );
:MFN index='operator >>'.istream &amp.operator >>( streambuf & );
:MFN index='operator >>'.istream &amp.operator >>( istream &(*)( istream & ) );
:MFN index='operator >>'.istream &amp.operator >>(     ios &(*)( ios     & ) );
:eMFNL.
:eCLSS.
:SALSO.
:SAL typ='cls'.ios
:SAL typ='cls'.iostream
:SAL typ='cls'.ostream
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='protected'.eatwhite
:SNPL.
:SNPFLF               .#include <iostream.h>
:SNPFLF               .protected:
:SNPF index='eatwhite'.void istream::eatwhite();
:eSNPL.
:SMTICS.
The &fn. extracts and discards whitespace characters from the &obj.,
until a non-whitespace character is found. The non-whitespace character
is not extracted.
:RSLTS.
The &fn. sets &eofbit. in the &errstate. if end-of-file
is encountered as the first character while extracting whitespace characters.
:SALSO.
:SAL typ='mfun'.ignore
:SAL typ='omtyp' ocls='ios'.fmtflags
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.gcount
:SNPL.
:SNPFLF             .#include <iostream.h>
:SNPFLF             .public:
:SNPF index='gcount'.int istream::gcount() const;
:eSNPL.
:SMTICS.
The &fn. determines the number of characters extracted by the last unformatted
input member function.
:RSLTS.
The &fn. returns the number of characters extracted by the last unformatted
input member function.
:SALSO.
:SAL typ='mfun'.get
:SAL typ='mfun'.getline
:SAL typ='mfun'.read
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.get
:SNPL.
:SNPFLF          .#include <iostream.h>
:SNPFLF          .public:
:SNPF index='get'.int istream::get();
:eSNPL.
:SMTICS.
This form of the &fn. performs an unformatted read of a single character from
the &obj.:PERIOD.
:RSLTS.
This form of the &fn. returns the character read from the &obj.:PERIOD.
If the &obj. is positioned at end-of-file before the read, &eof. is returned
and &eofbit. bit is set in the &errstate.:PERIOD.
&failbit. bit is not
set by this form of the &fn.:PERIOD.
:SALSO.
:SAL typ='mfun'.putback
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.get
:SNPL.
:SNPFLF          .#include <iostream.h>
:SNPFLF          .public:
:SNPF index='get'.istream &amp.istream::get(          char &amp.ch );
:SNPF index='get'.istream &amp.istream::get(   signed char &amp.ch );
:SNPF index='get'.istream &amp.istream::get( unsigned char &amp.ch );
:eSNPL.
:SMTICS.
These forms of the &fn. perform an unformatted read of a single character
from the &obj. and store the character in the
:ARG.ch
parameter.
:RSLTS.
These forms of the &fn. return a reference to the &obj.:PERIOD.
&eofbit. is set in the &errstate. if the &obj. is positioned at
end-of-file before the attempt to read the character.
&failbit. is set in the &errstate. if no character is read.
:SALSO.
:SAL typ='mfun'.read
:SAL typ='mfun'.operator~b>>
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.get
:SNPL.
:SNPFLF          .#include <iostream.h>
:SNPFLF          .public:
:SNPF index='get'.istream &amp.istream::get( char *buf, int len,
:SNPFLF          .                       char delim = '\n' );
:SNPF index='get'.istream &amp.istream::get( signed char *buf, int len,
:SNPFLF          .                       char delim = '\n' );
:SNPF index='get'.istream &amp.istream::get( unsigned char *buf, int len,
:SNPFLF          .                       char delim = '\n' );
:eSNPL.
:SMTICS.
These forms of the &fn. perform an unformatted read of at most
:ARG.len
-1 characters from the &obj. and store them starting at the memory
location specified by the
:ARG.buf
parameter. If the character specified by the
:ARG.delim
parameter is encountered in the &obj. before
:ARG.len
-1 characters have been read, the read terminates without
extracting the delimiting character.
:P.
After the read terminates, whether or not an error occurred, a null
character is stored in
:ARG.buf
following the last character read from the &obj.:PERIOD.
:P.
If the
:ARG.delim
parameter is not specified, the new-line character is assumed.
:RSLTS.
These forms of the &fn. return a reference to the &obj.:PERIOD.
If end-of-file is encountered as the first character, &eofbit. is set in
the &errstate.:PERIOD.
If no characters are stored into
:ARG.buf,
&failbit. is set in the &errstate.:PERIOD.
:SALSO.
:SAL typ='mfun'.getline
:SAL typ='mfun'.read
:SAL typ='mfun'.operator~b>>
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.get
:SNPL.
:SNPFLF          .#include <iostream.h>
:SNPFLF          .public:
:SNPF index='get'.istream &amp.istream::get( streambuf &amp.sb, char delim = '\n' );
:eSNPL.
:SMTICS.
This form of the &fn. performs an unformatted read of
characters from the &obj. and transfers them to the
:MONO.streambuf
object specified in the
:ARG.sb
parameter. The transfer stops if end-of-file is encountered, the
delimiting character specified in the
:ARG.delim
parameter is found, or if the store into the
:ARG.sb
parameter fails. If the
:ARG.delim
character is found, it is not extracted from the &obj. and is not transferred
to the
:ARG.sb
object.
:P.
If the
:ARG.delim
parameter is not specified, the new-line character is assumed.
:RSLTS.
The &fn. returns a reference to the &obj.:PERIOD.
&failbit. is set in the &errstate. if the store into the
:MONO.streambuf
object fails.
:SALSO.
:SAL typ='mfun'.getline
:SAL typ='mfun'.read
:SAL typ='mfun'.operator~b>>
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.getline
:SNPL.
:SNPFLF              .#include <iostream.h>
:SNPFLF              .public:
:SNPF index='getline'.istream &amp.istream::getline( char *buf, int len,
:SNPFLF              .                           char delim = '\n' );
:SNPF index='getline'.istream &amp.istream::getline( signed char *buf, int len,
:SNPFLF              .                           char delim = '\n' );
:SNPF index='getline'.istream &amp.istream::getline( unsigned char *buf, int len,
:SNPFLF              .                           char delim = '\n' );
:eSNPL.
:SMTICS.
The &fn. performs an unformatted read of at most
:ARG.len
-1 characters from the &obj. and stores them starting at the memory
location specified by the
:ARG.buf
parameter. If the delimiting character, specified by the
:ARG.delim
parameter, is encountered in the &obj. before
:ARG.len
-1 characters have been read, the read terminates after
extracting the
:ARG.delim
character.
:P.
If
:ARG.len
-1 characters have been read and the next character is the
:ARG.delim
character, it is not extracted.
:P.
After the read terminates, whether or not an error occurred, a null
character is stored in the buffer following the last character
read from the &obj.:PERIOD.
:P.
If the
:ARG.delim
parameter is not specified, the new-line character is assumed.
:RSLTS.
The &fn. returns a reference to the &obj.:PERIOD.
If end-of-file is encountered as the first character, &eofbit. is set in
the &errstate.:PERIOD.
If end-of-file is encountered before
:ARG.len
characters are transferred or the
:ARG.delim
character is reached, &failbit. is set in the &errstate.:PERIOD.
:SALSO.
:SAL typ='mfun'.get
:SAL typ='mfun'.read
:SAL typ='mfun'.operator~b>>
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.ignore
:SNPL.
:SNPFLF             .#include <iostream.h>
:SNPFLF             .public:
:SNPF index='ignore'.istream &amp.istream::ignore( int num = 1, int delim = EOF );
:eSNPL.
:SMTICS.
The &fn. extracts and discards up to
:ARG.num
characters from the &obj.:PERIOD.
If the
:ARG.num
parameter is not specified, the &fn. extracts and discards one character.
If the
:ARG.delim
parameter
is not &eof. and it is encountered before
:ARG.num
characters have been extracted, the extraction ceases after discarding
the delimiting character.
The extraction stops if end-of-file is encountered.
:P.
If the
:ARG.num
parameter is specified as a negative number, no limit is imposed
on the number of characters extracted and discarded.
The operation continues until the delimiting character is found and
discarded, or until end-of-file.  This behavior is a WATCOM extension.
:RSLTS.
The &fn. returns a reference to the &obj.:PERIOD.
If end-of-file is encountered as the first character, &eofbit. is
set in the &errstate.:PERIOD.
:SALSO.
:SAL typ='mfun'.eatwhite
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.ipfx
:SNPL.
:SNPFLF           .#include <iostream.h>
:SNPFLF           .public:
:SNPF index='ipfx'.int istream::ipfx( int noskipws = 0 );
:eSNPL.
:SMTICS.
The &fn. is a prefix function executed before each of the formatted and
unformatted read operations. If any bits are set in &iostate.,
the &fn. immediately returns 0, indicating that the prefix function failed.
Failure in the prefix function causes the input operation to fail.
:P.
If the
:ARG.noskipws
parameter is 0 or unspecified and the
:MONO.ios::skipws
bit is on in &fmtflags., whitespace characters are discarded and
the &obj. is positioned so that the next character read is the first character
after the discarded whitespace. Otherwise, no whitespace skipping takes
place.
:P.
The formatted input functions that read specific types of objects (such as
integers and floating-point numbers) call the &fn. with the
:ARG.noskipws
parameter set to zero, allowing leading whitespaces to be discarded if the
:MONO.ios::skipws
bit is on in &fmtflags.:PERIOD.
The unformatted input functions that read
characters without interpretation call the &fn. with a the
:ARG.noskipws
parameter set to 1 so that no whitespace characters are discarded.
:P.
If the &obj. is tied to an output stream, the output stream is flushed.
:RSLTS.
If the &obj. is not in an &errstate. when the above processing is
completed, the &fn. returns a non-zero value to indicate success.
Otherwise, zero is returned to indicate failure.
:SALSO.
:SAL typ='mfun'.isfx
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.isfx
:SNPL.
:SNPFLF           .#include <iostream.h>
:SNPFLF           .public:
:SNPF index='isfx'.void istream::isfx();
:eSNPL.
:SMTICS.
The &fn. is a suffix function executed just before the end of each
of the formatted and unformatted read operations.
:P.
As currently implemented, the &fn. does not do anything.
:SALSO.
:SAL typ='mfun'.ipfx
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='ctor' prot='protected'.istream
:SNPL.
:SNPFLF                    .#include <iostream.h>
:SNPFLF                    .protected:
:SNPCD cd_idx='c'.istream::istream();
:eSNPL.
:SMTICS.
This form of the &fn. creates an &obj. without an associated
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
:LIBF fmt='ctor' prot='public'.istream
:SNPL.
:SNPFLF                    .#include <iostream.h>
:SNPFLF                    .public:
:SNPCD cd_idx='c'.istream::istream( istream const &amp.istrm );
:eSNPL.
:SMTICS.
This form of the &fn. creates an &obj. associated with the
:MONO.streambuf
object currently associated with the
:ARG.istrm
parameter.
The &obj. is initialized and will use the
:ARG.istrm
:MONO.streambuf
object for subsequent operations.
:ARG.istrm
will continue to use the
:MONO.streambuf
object.
:RSLTS.
This form of the &fn. creates an initialized &obj.:PERIOD.
If there is no
:MONO.streambuf
object currently associated with the
:ARG.istrm
parameter, &badbit. is set in the &errstate.:PERIOD.
:SALSO.
:SAL typ='dtor'.
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='ctor' prot='public'.istream
:SNPL.
:SNPFLF                    .#include <iostream.h>
:SNPFLF                    .public:
:SNPCD cd_idx='c'.istream::istream( streambuf *sb );
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
:LIBF fmt='dtor' prot='public virtual'.~~istream
:SNPL.
:SNPFLF                   .#include <iostream.h>
:SNPFLF                   .public:
:SNPCD cd_idx='d'.virtual istream::~~istream();
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
:LIBF fmt='mfun' prot='public'.operator =
:SNPL.
:SNPFLF                 .#include <iostream.h>
:SNPFLF                 .public:
:SNPF index='operator ='.istream &amp.istream::operator =( streambuf *sb );
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
:SNPF index='operator ='.istream &amp.istream::operator =( istream const &amp.istrm );
:eSNPL.
:SMTICS.
This form of the &fn. is used to associate the &obj. with the
:MONO.streambuf
object currently associated with the
:ARG.istrm
parameter.
The &obj. is initialized and will use the
:ARG.istrm
:CONT.'s
:MONO.streambuf
object for subsequent operations.
The
:ARG.istrm
object will continue to use the
:MONO.streambuf
object.
:RSLTS.
This form of the &fn. returns a reference to the &obj. that is the target of
the assignment. If there is no
:MONO.streambuf
object currently associated with the
:ARG.istrm
parameter, &badbit. is set in the &errstate.:PERIOD.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.operator >>
:SNPL.
:SNPFLF                  .#include <iostream.h>
:SNPFLF                  .public:
:SNPF index='operator >>'.istream &amp.istream::operator >>(          char *buf );
:SNPF index='operator >>'.istream &amp.istream::operator >>(   signed char *buf );
:SNPF index='operator >>'.istream &amp.istream::operator >>( unsigned char *buf );
:eSNPL.
:SMTICS.
These forms of the &fn. perform a formatted read of characters from the &obj.
and place them in the buffer specified by the
:ARG.buf
parameter.  Characters are read until a whitespace character is found or
the maximum size has been read.  If a whitespace character is found, it
is not transferred to the buffer and remains in the &obj.:PERIOD.
:P.
If a non-zero &fmtwidth. has been specified, it is interpreted as the
maximum number of characters that may be placed in
:ARG.buf
:PERIOD.
No more than &fmtwidth.-1 characters are read
from the &obj. and transferred to
:ARG.buf
:PERIOD.
If &fmtwidth. is zero, characters are transferred until a whitespace
character is found.
:P.
Since these forms of the &fn. use &fmtwidth., it is reset to zero after each
use. It must be set before each input operation that requires a
non-zero &fmtwidth.:PERIOD.
:P.
A null character is added following the last transferred character,
even if the transfer fails because of an error.
:RSLTS.
These forms of the &fn. return a reference to the &obj. so that further
extraction operations may be specified in the same statement.
If no characters are transferred to
:ARG.buf
:CONT., &failbit. is set in the &errstate.:PERIOD.
If the first character read yielded end-of-file, &eofbit. is set in the
&errstate.:PERIOD.
:SALSO.
:SAL typ='mfun'.get
:SAL typ='mfun'.getline
:SAL typ='mfun'.read
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.operator >>
:SNPL.
:SNPFLF                  .#include <iostream.h>
:SNPFLF                  .public:
:SNPF index='operator >>'.istream &amp.istream::operator >>(          char &amp.ch );
:SNPF index='operator >>'.istream &amp.istream::operator >>(   signed char &amp.ch );
:SNPF index='operator >>'.istream &amp.istream::operator >>( unsigned char &amp.ch );
:eSNPL.
:SMTICS.
These forms of the &fn. perform a formatted read of a single character from
the &obj. and place it in the
:ARG.ch
parameter.
:RSLTS.
These forms of the &fn. return a reference to the &obj. so that further
extraction operations may be specified in the same statement.
If the character read yielded end-of-file, &eofbit. is set in the
&errstate.:PERIOD.
:SALSO.
:SAL typ='mfun'.get
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.operator >>
:SNPL.
:SNPFLF                  .#include <iostream.h>
:SNPFLF                  .public:
:SNPF index='operator >>'.istream &amp.istream::operator >>(   signed   int &amp.num );
:SNPF index='operator >>'.istream &amp.istream::operator >>( unsigned   int &amp.num );
:SNPF index='operator >>'.istream &amp.istream::operator >>(   signed  long &amp.num );
:SNPF index='operator >>'.istream &amp.istream::operator >>( unsigned  long &amp.num );
:SNPF index='operator >>'.istream &amp.istream::operator >>(   signed short &amp.num );
:SNPF index='operator >>'.istream &amp.istream::operator >>( unsigned short &amp.num );
:eSNPL.
:SMTICS.
These forms the &fn. perform a formatted read of an integral value from
the &obj. and place it in the
:ARG.num
parameter.
:P.
The number may be preceded by a
:MONO.+
or
:MONO.-
sign.
:P.
If
:MONO.ios::dec
is the only bit set in the
:MONO.ios::basefield
bits of &fmtflags., the number is interpreted as a decimal (base 10)
integer, composed of the digits
:MONO.0123456789
:PERIOD.
:P.
If
:MONO.ios::oct
is the only bit set in the
:MONO.ios::basefield
bits of &fmtflags., the number is interpreted as an octal (base 8)
integer, composed of the digits
:MONO.01234567
:PERIOD.
:P.
If
:MONO.ios::hex
is the only bit set in the
:MONO.ios::basefield
bits of &fmtflags., the number is interpreted as a hexadecimal
(base 16) integer, composed of the digits
:MONO.0123456789
and the letters
:MONO.abcdef
or
:MONO.ABCDEF
:PERIOD.
:P.
If no bits are set in the
:MONO.ios::basefield
bits of &fmtflags., the operator looks for a prefix to determine the
base of the number. If the first two characters are
:MONO.0x
or
:MONO.0X
:CONT., the number is interpreted as a hexadecimal number. If the first
character is a
:MONO.0
(and the second is not an
:MONO.x
or
:MONO.X
:CONT.), the number is interpreted as an octal integer. Otherwise, no
prefix is expected and the number is interpreted as a decimal integer.
:P.
If more than one bit is set in the
:MONO.ios::basefield
bits of &fmtflags., the number is interpreted as a decimal integer.
:RSLTS.
These forms of the &fn. return a reference to the &obj. so that further
extraction operations may be specified in the same statement.
If end-of-file is encountered as the first character, &eofbit. is set in
the &errstate.:PERIOD.
If an overflow occurs while converting to the required integer type, the
&failbit. is set in the &errstate.:PERIOD.
:SALSO.
:SAL typ='omtyp' ocls='ios'.fmtflags
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.operator >>
:SNPL.
:SNPFLF                  .#include <iostream.h>
:SNPFLF                  .public:
:SNPF index='operator >>'.istream &amp.istream::operator >>(       float &amp.num );
:SNPF index='operator >>'.istream &amp.istream::operator >>(      double &amp.num );
:SNPF index='operator >>'.istream &amp.istream::operator >>( long double &amp.num );
:eSNPL.
:SMTICS.
These forms of the &fn. perform a formatted read of a floating-point value
from the &obj. and place it in the
:ARG.num
parameter.
:P.
The floating-point value may be specified in any form that is acceptable to
the C++ compiler.
:RSLTS.
These forms of the &fn. return a reference to the &obj. so that further
extraction operations may be specified in the same statement.
If end-of-file is encountered as the first character, &eofbit. is set in
the &errstate.:PERIOD.
If an overflow occurs while converting to the required type, the
&failbit. is set in the &errstate.:PERIOD.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.operator >>
:SNPL.
:SNPFLF                  .#include <iostream.h>
:SNPFLF                  .public:
:SNPF index='operator >>'.istream &amp.istream::operator >>( streambuf &amp.sb );
:eSNPL.
:SMTICS.
This form of the &fn. transfers all the characters from the &obj. into the
:ARG.sb
parameter. Reading continues until end-of-file is encountered.
:RSLTS.
This form of the &fn. return a reference to the &obj. so that further
extraction operations may be specified in the same statement.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.operator >>
:SNPL.
:SNPFLF                  .#include <iostream.h>
:SNPFLF                  .public:
:SNPF index='operator >>'.istream &amp.istream::operator >>( istream &(*fn)( istream & ) );
:SNPF index='operator >>'.istream &amp.istream::operator >>(     ios &(*fn)( ios     & ) );
:eSNPL.
:SMTICS.
These forms of the &fn. are used to implement the non-parameterized
manipulators for the &cls.:PERIOD.
The function specified by the
:ARG.fn
parameter is called with the &obj. as its parameter.
:RSLTS.
These forms of the &fn. return a reference to the &obj. so that further
extraction operations may be specified in the same statement.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.peek
:SNPL.
:SNPFLF           .#include <iostream.h>
:SNPFLF           .public:
:SNPF index='peek'.int istream::peek();
:eSNPL.
:SMTICS.
The &fn. looks up the next character to be extracted from the &obj.,
without extracting the character.
:RSLTS.
The &fn. returns the next character to be extracted from the &obj.:PERIOD.
If the &obj. is positioned at end-of-file, &eof. is returned.
:SALSO.
:SAL typ='mfun'.get
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.putback
:SNPL.
:SNPFLF              .#include <iostream.h>
:SNPFLF              .public:
:SNPF index='putback'.istream &amp.istream::putback( char ch );
:eSNPL.
:SMTICS.
The &fn. attempts to put the extracted character specified by the
:ARG.ch
parameter back into the &obj.:PERIOD.
The
:ARG.ch
character must be the same as the character before the current position of
the &obj., usually the last character extracted from the stream.
If it is not the same character, the result of the next character extraction
is undefined.
:P.
The number of characters that can be put back is defined by the &obj., but
is usually at least 4. Depending on the status of the buffers used for input,
it may be possible to put back more than 4 characters.
:RSLTS.
The &fn. returns a reference to the &obj.:PERIOD.
If the &fn. is unable to put back the
:ARG.ch
parameter, &failbit. is set in the &errstate.:PERIOD.
:SALSO.
:SAL typ='mfun'.get
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.read
:SNPL.
:SNPFLF           .#include <iostream.h>
:SNPFLF           .public:
:SNPF index='read'.istream &amp.istream::read(          char *buf, int len );
:SNPF index='read'.istream &amp.istream::read(   signed char *buf, int len );
:SNPF index='read'.istream &amp.istream::read( unsigned char *buf, int len );
:eSNPL.
:SMTICS.
The &fn. performs an unformatted read of at most
:ARG.len
characters from the &obj. and stores them in the memory locations starting at
:ARG.buf
:PERIOD.
If end-of-file is encountered before
:ARG.len
characters have been transferred, the transfer stops and &failbit.
is set in the &errstate.:PERIOD.
:P.
The number of characters extracted can be determined with the
:MONO.gcount
member function.
:RSLTS.
The &fn. returns a reference to the &obj.:PERIOD.
If end-of-file is encountered as the first character, &eofbit. is set in
the &errstate.:PERIOD.
If end-of-file is encountered before
:ARG.len
characters are transferred, &failbit. is set in the &errstate.:PERIOD.
:SALSO.
:SAL typ='mfun'.gcount
:SAL typ='mfun'.get
:SAL typ='mfun'.getline
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.seekg
:SNPL.
:SNPFLF            .#include <iostream.h>
:SNPFLF            .public:
:SNPF index='seekg'.istream &amp.istream::seekg( streampos pos );
:eSNPL.
:SMTICS.
The &fn. positions the &obj. to the position specified by the
:ARG.pos
parameter so that the next input operation commences from that position.
:RSLTS.
The &fn. returns a reference to the &obj.:PERIOD.
If the seek operation fails, &failbit. is set in the &errstate.:PERIOD.
:SALSO.
:SAL typ='mfun'.tellg
:SAL typ='ofun' ocls='ostream'.tellp
:SAL typ='ofun' ocls='ostream'.seekp
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.seekg
:SNPL.
:SNPFLF            .#include <iostream.h>
:SNPFLF            .public:
:SNPF index='seekg'.istream &amp.istream::seekg( streamoff offset, ios::seekdir dir );
:eSNPL.
:SMTICS.
The &fn. positions the &obj. to the specified position so that the next input
operation commences from that position.
:INCLUDE file='seekdir'.
:RSLTS.
The &fn. returns a reference to the &obj.:PERIOD.
If the seek operation fails, &failbit. is set in the &errstate.:PERIOD.
:SALSO.
:SAL typ='ofun' ocls='ostream'.tellp
:SAL typ='ofun' ocls='ostream'.seekp
:SAL typ='mfun'.tellg
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.sync
:SNPL.
:SNPFLF           .#include <iostream.h>
:SNPFLF           .public:
:SNPF index='sync'.int istream::sync();
:eSNPL.
:SMTICS.
The &fn. synchronizes the input buffer and the &obj. with whatever source of
characters is being used.
The &fn. uses the
:MONO.streambuf
class's
:MONO.sync
virtual member function to carry out the synchronization.
The specific behavior is dependent on what type of
:MONO.streambuf
derived object is associated with the &obj.:PERIOD.
:RSLTS.
The &fn. returns &noteof. on success, otherwise &eof. is returned.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.tellg
:SNPL.
:SNPFLF            .#include <iostream.h>
:SNPFLF            .public:
:SNPF index='tellg'.streampos istream::tellg();
:eSNPL.
:SMTICS.
The &fn. determines the position in the &obj. of the next character available
for reading.
The first character in an &obj. is at offset zero.
:RSLTS.
The &fn. returns the position of the next character available for reading.
:SALSO.
:SAL typ='ofun' ocls='ostream'.tellp
:SAL typ='ofun' ocls='ostream'.seekp
:SAL typ='mfun'.seekg
:eSALSO.
:eLIBF.
