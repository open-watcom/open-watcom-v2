:CLFNM.strstreambase
:CMT.========================================================================
:LIBF fmt='hdr'.strstreambase
:HFILE.strstrea.h
:DVFML.
:DVFM.ios
:eDVFML.
:DVBYL.
:DVBY.istrstream
:DVBY.ostrstream
:DVBY.strstream
:eDVBYL.
:CLSS.
The &cls. is a base class that provides common
functionality for the three string stream-based classes,
:MONO.istrstream
:CONT.,
:MONO.ostrstream
and
:MONO.strstream
:PERIOD.
The &cls. is derived from the
:MONO.ios
class which provides the stream state information.
The &cls. provides constructors for string stream objects and one member
function.
:HDG.Protected Member Functions
The following member functions are declared in the protected interface:
:MFNL.
:MFCD cd_idx='c'.strstreambase();
:MFCD cd_idx='c'.strstreambase( char *, int, char * = 0 );
:MFCD cd_idx='d'.~~strstreambase();
:eMFNL.
:HDG.Public Member Functions
The following member function is declared in the public interface:
:MFNL.
:MFN index='rdbuf'.strstreambuf *rdbuf() const;
:eMFNL.
:eCLSS.
:SALSO.
:SAL typ='cls'.istrstream
:SAL typ='cls'.ostrstream
:SAL typ='cls'.strstream
:SAL typ='cls'.strstreambuf
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.rdbuf
:SNPL.
:SNPFLF            .#include <strstrea.h>
:SNPFLF            .public:
:SNPF index='rdbuf'.strstreambuf *strstreambase::rdbuf() const;
:eSNPL.
:SMTICS.
The &fn. creates a pointer to the
:MONO.strstreambuf
associated with the &obj.:PERIOD.
Since the
:MONO.strstreambuf
object is embedded within the &obj.,
this function never returns &null.:PERIOD.
:RSLTS.
The &fn. returns a pointer to the
:MONO.strstreambuf
associated with the &obj.:PERIOD.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='ctor' prot='protected'.strstreambase
:SNPL.
:SNPFLF                    .#include <strstrea.h>
:SNPFLF                    .protected:
:SNPCD cd_idx='c'.strstreambase::strstreambase();
:eSNPL.
:SMTICS.
This form of the &fn. creates a &obj. that is initialized, but empty. Dynamic
allocation is used to store characters. No buffer is allocated. A buffer is
be allocated when data is first written to the &obj.:PERIOD.
:P.
This form of the &fn. is only used implicitly by the compiler
when it generates a constructor for a derived class.
:RSLTS.
The &fn. creates an initialized &obj.:PERIOD.
:SALSO.
:SAL typ='dtor'.
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='ctor' prot='protected'.strstreambase
:SNPL.
:SNPFLF          .#include <strstrea.h>
:SNPFLF          .protected:
:SNPCD cd_idx='c'.strstreambase::strstreambase( char *str,
:SNPFLF          .                              int len,
:SNPFLF          .                              char *pstart );
:eSNPL.
:SMTICS.
This form of the &fn. creates a &obj. that is initialized and uses the
buffer specified by the
:ARG.str
and
:ARG.len
parameters as its &rsvarea. within the associated
:MONO.strstreambuf
object. Dynamic allocation is not used.
:P.
This form of the &fn. is unlikely to be explicitly used, except in the
member initializer list for the constructor of a derived class.
:INCLUDE file='str_b'.
:RSLTS.
The &fn. creates an initialized &obj.:PERIOD.
:SALSO.
:SAL typ='dtor'.
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='dtor' prot='protected'.~~strstreambase
:SNPL.
:SNPFLF                   .#include <strstrea.h>
:SNPFLF                   .protected:
:SNPCD cd_idx='d'.strstreambase::~~strstreambase();
:eSNPL.
:SMTICS.
The &fn. does not do anything explicit.
The call to the &fn. is inserted implicitly by the compiler
at the point where the &obj. goes out of scope.
:RSLTS.
The &obj. is destroyed.
:SALSO.
:SAL typ='ctor'.
:eSALSO.
:eLIBF.
