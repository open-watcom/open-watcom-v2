:CLFNM.ostrstream
:CMT.========================================================================
:LIBF fmt='hdr'.ostrstream
:HFILE.strstrea.h
:DVFML.
:DVFM.strstreambase
:DVFM.ostream
:eDVFML.
:CLSS.
The &cls. is used to create and write to string stream objects.
:P.
The &cls. provides little of its own functionality. Derived from the
:MONO.strstreambase
and
:MONO.ostream
classes, its constructors and destructor provide simplified access to the
appropriate equivalents in those base classes.  The member functions provide
specialized access to the string stream object.
:P.
Of the available I/O stream classes,
creating an &obj. is the preferred method of performing write operations
to a string stream.
:HDG.Public Member Functions
The following member functions are declared in the public interface:
:MFNL.
:MFCD cd_idx='c'   .ostrstream();
:MFCD cd_idx='c'   .ostrstream(          char *, int, ios::openmode = ios::out );
:MFCD cd_idx='c'   .ostrstream(   signed char *, int, ios::openmode = ios::out );
:MFCD cd_idx='c'   .ostrstream( unsigned char *, int, ios::openmode = ios::out );
:MFCD cd_idx='d'   .~~ostrstream();
:MFN index='pcount'.int pcount() const;
:MFN index='str'   .char *str();
:eMFNL.
:eCLSS.
:SALSO.
:SAL typ='cls'.istrstream
:SAL typ='cls'.ostream
:SAL typ='cls'.ostrstream
:SAL typ='cls'.strstreambase
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='ctor' prot='public'.ostrstream
:SNPL.
:SNPFLF                    .#include <strstrea.h>
:SNPFLF                    .public:
:SNPCD cd_idx='c'.ostrstream::ostrstream();
:eSNPL.
:SMTICS.
:INCLUDE file='sstr_ct1'.
:SALSO.
:SAL typ='dtor'.
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='ctor' prot='public'.ostrstream
:SNPL.
:SNPFLF          .#include <strstrea.h>
:SNPFLF          .public:
:SNPCD cd_idx='c'.ostrstream::ostrstream( char *str,
:SNPFLF          .                        int len,
:SNPFLF          .                        ios::openmode mode = ios::out );
:SNPCD cd_idx='c'.ostrstream::ostrstream( signed char *str,
:SNPFLF          .                        int len,
:SNPFLF          .                        ios::openmode mode = ios::out );
:SNPCD cd_idx='c'.ostrstream::ostrstream( unsigned char *str,
:SNPFLF          .                        int len,
:SNPFLF          .                        ios::openmode mode = ios::out );
:eSNPL.
:SMTICS.
:INCLUDE file='sstr_ct2'.
:SALSO.
:SAL typ='dtor'.
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='dtor' prot='public'.~~ostrstream
:SNPL.
:SNPFLF                   .#include <strstrea.h>
:SNPFLF                   .public:
:SNPCD cd_idx='d'.ostrstream::~~ostrstream();
:eSNPL.
:SMTICS.
:INCLUDE file='sstr_dt'.
:SALSO.
:SAL typ='ctor'.
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.pcount
:SNPL.
:SNPFLF             .#include <strstrea.h>
:SNPFLF             .public:
:SNPF index='pcount'.int ostrstream::pcount() const;
:eSNPL.
:SMTICS.
The &fn. computes the number of characters that have been written to the
&obj.:PERIOD.
This value is particularly useful if the &obj. does not contain a C
string (terminated by a null character), so that the number of
characters cannot be determined with the C library
:MONO.strlen
function.
If the &obj. was created by appending to a C string in a static buffer,
the length of the original string is included in the character
count.
:RSLTS.
The &fn. returns the number of characters contained in the &obj.:PERIOD.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.str
:SNPL.
:SNPFLF          .#include <strstrea.h>
:SNPFLF          .public:
:SNPF index='str'.char *ostrstream::str();
:eSNPL.
:SMTICS.
:INCLUDE file='sst_str'.
:eLIBF.
