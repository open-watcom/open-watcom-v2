:CLFNM.istrstream
:CMT.========================================================================
:LIBF fmt='hdr'.istrstream
:HFILE.strstrea.h
:DVFML.
:DVFM.strstreambase
:DVFM.istream
:eDVFML.
:CLSS.
The &cls. is used to create and read from string stream objects.
:P.
The &cls. provides little of its own functionality. Derived from the
:MONO.strstreambase
and
:MONO.istream
classes, its constructors and destructor provide simplified access to the
appropriate equivalents in those base classes.
:P.
Of the available I/O stream classes,
creating an &obj. is the preferred method of performing read operations
from a string stream.
:HDG.Public Member Functions
The following member functions are declared in the public interface:
:MFNL.
:MFCD cd_idx='c'.istrstream(          char * );
:MFCD cd_idx='c'.istrstream(   signed char * );
:MFCD cd_idx='c'.istrstream( unsigned char * );
:MFCD cd_idx='c'.istrstream(          char *, int );
:MFCD cd_idx='c'.istrstream(   signed char *, int );
:MFCD cd_idx='c'.istrstream( unsigned char *, int );
:MFCD cd_idx='d'.~~istrstream();
:eMFNL.
:eCLSS.
:SALSO.
:SAL typ='cls'.istream
:SAL typ='cls'.ostrstream
:SAL typ='cls'.strstream
:SAL typ='cls'.strstreambase
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='ctor' prot='public'.istrstream
:SNPL.
:SNPFLF                    .#include <strstrea.h>
:SNPFLF                    .public:
:SNPCD cd_idx='c'.istrstream::istrstream( char *str );
:SNPCD cd_idx='c'.istrstream::istrstream( signed char *str );
:SNPCD cd_idx='c'.istrstream::istrstream( unsigned char *str );
:eSNPL.
:SMTICS.
This form of the &fn. creates an &obj. consisting of the null terminated
C string specified by the
:ARG.str
parameter.
The inherited
:MONO.istream
member functions can be used to read from the &obj.:PERIOD.
:RSLTS.
This form of the &fn. creates an initialized &obj.:PERIOD.
:SALSO.
:SAL typ='dtor'.
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='ctor' prot='public'.istrstream
:SNPL.
:SNPFLF                    .#include <strstrea.h>
:SNPFLF                    .public:
:SNPCD cd_idx='c'.istrstream::istrstream( char *str, int len );
:SNPCD cd_idx='c'.istrstream::istrstream( signed char *str, int len );
:SNPCD cd_idx='c'.istrstream::istrstream( unsigned char *str, int len );
:eSNPL.
:SMTICS.
This form of the &fn. creates an &obj. consisting of the characters starting at
:ARG.str
and ending at
:ARG.str
+
:ARG.len
- 1.
The inherited
:MONO.istream
member functions can be used to read from the &obj.:PERIOD.
:RSLTS.
This form of the &fn. creates an initialized &obj.:PERIOD.
:SALSO.
:SAL typ='dtor'.
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='dtor' prot='public'.~~istrstream
:SNPL.
:SNPFLF                   .#include <strstrea.h>
:SNPFLF                   .public:
:SNPCD cd_idx='d'.istrstream::~~istrstream();
:eSNPL.
:SMTICS.
:INCLUDE file='sstr_dt'.
:SALSO.
:SAL typ='ctor'.
:eSALSO.
:eLIBF.
