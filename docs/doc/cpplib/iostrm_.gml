:CLFNM.iostream
:CMT.========================================================================
:LIBF fmt='hdr'.iostream
:HFILE.iostream.h
:DVFML.
:DVFM.istream
:DVFM.ostream
:eDVFML.
:DVBYL.
:DVBY.fstream
:DVBY.strstream
:eDVBYL.
:CLSS.
The &cls. supports reading and writing of characters from and to the standard
input/output devices, usually the keyboard and screen.
The &cls. provides formatted conversion of characters to and from other types
(e.g. integers and floating-point numbers). The associated
:MONO.streambuf
class provides the methods for communicating with the actual device, while
the &cls. provides the interpretation of the characters.
:P.
Generally, an &obj. won't be created by a program, since there is no mechanism
at this level to "open" a device.
No instance of an &obj. is created by default, since it is usually not
possible to perform both input and output on the standard input/output
devices. The &cls. is provided as a base class for other derived
classes that can provide both input and output capabilities through the
same object. The
:MONO.fstream
and
:MONO.strstream
classes are examples of classes derived from the &cls.:PERIOD.
:HDG.Protected Member Functions
The following protected member functions are declared:
:MFNL.
:MFCD cd_idx='c'.iostream();
:eMFNL.
:HDG.Public Member Functions
The following public member functions are declared:
:MFNL.
:MFCD cd_idx='c'.iostream( ios const & );
:MFCD cd_idx='c'.iostream( streambuf * );
:MFCD cd_idx='d' .virtual ~~iostream();
:eMFNL.
:HDG.Public Member Operators
The following public member operators are declared:
:MFNL.
:MFN index='operator ='.iostream & operator =( streambuf * );
:MFN index='operator ='.iostream & operator =( ios const & );
:eMFNL.
:eCLSS.
:SALSO.
:SAL typ='cls'.ios
:SAL typ='cls'.istream
:SAL typ='cls'.ostream
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='ctor' prot='protected'.iostream
:SNPL.
:SNPFLF                    .#include <iostream.h>
:SNPFLF                    .protected:
:SNPCD cd_idx='c'.iostream::iostream();
:eSNPL.
:SMTICS.
This form of the &fn. creates an &obj. without an attached
:MONO.streambuf
object.
:P.
This form of the &fn. is only used implicitly by the compiler
when it generates a constructor for a derived class.
:RSLTS.
The &fn. produces an initialized &obj.:PERIOD.
&badbit. is set in the &errstate.:PERIOD.
:SALSO.
:SAL typ='dtor'.
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='ctor' prot='public'.iostream
:SNPL.
:SNPFLF                    .#include <iostream.h>
:SNPFLF                    .public:
:SNPCD cd_idx='c'.iostream::iostream( ios const &amp.strm );
:eSNPL.
:SMTICS.
This form of the &fn. creates an &obj. associated with the
:MONO.streambuf
object currently associated with the
:ARG.strm
parameter. The &obj. is initialized and will use the
:ARG.strm
:MONO.streambuf
object for subsequent operations.
:ARG.strm
will continue to use the
:MONO.streambuf
object.
:RSLTS.
The &fn. produces an initialized &obj.:PERIOD.
If there is no
:MONO.streambuf
object currently associated with the
:ARG.strm
parameter, &badbit. is set in the &errstate.:PERIOD.
:SALSO.
:SAL typ='dtor'.
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='ctor' prot='public'.iostream
:SNPL.
:SNPFLF                    .#include <iostream.h>
:SNPFLF                    .public:
:SNPCD cd_idx='c'.iostream::iostream( streambuf *sb );
:eSNPL.
:SMTICS.
This form of the &fn. creates an &obj. with an attached
:MONO.streambuf
object.
:P.
Since a user program usually will not create an &obj., this form of the
&fn. is unlikely to be explicitly used, except in the member initializer
list for the constructor of a derived class. The
:ARG.sb
parameter is a pointer to a
:MONO.streambuf
object, which should be connected to the source and sink of characters for
the stream.
:RSLTS.
The &fn. produces an initialized &obj.:PERIOD.
If the
:ARG.sb
parameter is &null., &badbit. is set in the &errstate.:PERIOD.
:SALSO.
:SAL typ='dtor'.
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='dtor' prot='public'.~~iostream
:SNPL.
:SNPFLF                   .#include <iostream.h>
:SNPFLF                   .public:
:SNPCD cd_idx='d'.virtual iostream::~~iostream();
:eSNPL.
:SMTICS.
The &fn. does not do anything explicit.
The
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
:SNPF index='operator ='.iostream &amp.iostream::operator =( streambuf *sb );
:eSNPL.
:SMTICS.
This form of the &fn. initializes the target &obj. and sets up an association
between the &obj. and the
:MONO.streambuf
object specified by the
:ARG.sb
parameter.
:RSLTS.
The &fn. returns a reference to the &obj. that is the target of the
assignment. If the
:ARG.sb
parameter is &null., &badbit. is set in the &errstate.:PERIOD.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.operator =
:SNPL.
:SNPFLF                 .#include <iostream.h>
:SNPFLF                 .public:
:SNPF index='operator ='.iostream &amp.iostream::operator =( const ios &amp.strm );
:eSNPL.
:SMTICS.
This form of the &fn. initializes the &obj. and sets up an association between
the &obj. and the
:MONO.streambuf
object currently associated with the
:ARG.strm
parameter.
:RSLTS.
The &fn. returns a reference to the &obj. that is the target of the
assignment.
If there is no
:MONO.streambuf
object currently associated with the
:ARG.strm
parameter, &badbit. is set in the &errstate.:PERIOD.
:eLIBF.
