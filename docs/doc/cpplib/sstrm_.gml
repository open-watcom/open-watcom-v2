:CLFNM.strstream
:CMT.========================================================================
:LIBF fmt='hdr'.strstream
:HFILE.strstrea.h
:DVFML.
:DVFM.strstreambase
:DVFM.iostream
:eDVFML.
:CLSS.
The &cls. is used to create and write to string stream objects.
:P.
The &cls. provides little of its own functionality. Derived from the
:MONO.strstreambase
and
:MONO.iostream
classes, its constructors and destructor provide simplified access to the
appropriate equivalents in those base classes.  The member functions provide
specialized access to the string stream object.
:P.
Of the available I/O stream classes,
creating a &obj. is the preferred method of performing read and write
operations on a string stream.
:HDG.Public Member Functions
The following member functions are declared in the public interface:
:MFNL.
:MFCD cd_idx='c'.strstream();
:MFCD cd_idx='c'.strstream( char *,
:MFNFLF         .           int,
:MFNFLF         .           ios::openmode = ios::in|ios::out );
:MFCD cd_idx='c'.strstream( signed char *,
:MFNFLF         .           int,
:MFNFLF         .           ios::openmode = ios::in|ios::out );
:MFCD cd_idx='c'.strstream( unsigned char *,
:MFNFLF         .           int,
:MFNFLF         .           ios::openmode = ios::in|ios::out );
:MFCD cd_idx='d'.~~strstream();
:MFN index='str'.char *str();
:eMFNL.
:eCLSS.
:SALSO.
:SAL typ='cls'.istrstream
:SAL typ='cls'.ostrstream
:SAL typ='cls'.strstreambase
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.str
:SNPL.
:SNPFLF          .#include <strstrea.h>
:SNPFLF          .public:
:SNPF index='str'.char *strstream::str();
:eSNPL.
:SMTICS.
:INCLUDE file='sst_str'.
:SALSO.
:SAL typ='ofun' ocls='strstreambuf'.str
:SAL typ='ofun' ocls='strstreambuf'.freeze
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='ctor' prot='public'.strstream
:SNPL.
:SNPFLF                    .#include <strstrea.h>
:SNPFLF                    .public:
:SNPCD cd_idx='c'.strstream::strstream();
:eSNPL.
:SMTICS.
:INCLUDE file='sstr_ct1'.
:SALSO.
:SAL typ='dtor'.
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='ctor' prot='public'.strstream
:SNPL.
:SNPFLF          .#include <strstrea.h>
:SNPFLF          .public:
:SNPCD cd_idx='c'.strstream::strstream( char *str,
:SNPFLF          .                      int len,
:SNPFLF          .                      ios::openmode mode );
:SNPCD cd_idx='c'.strstream::strstream( signed char *str,
:SNPFLF          .                      int len,
:SNPFLF          .                      ios::openmode mode );
:SNPCD cd_idx='c'.strstream::strstream( unsigned char *str,
:SNPFLF          .                      int len,
:SNPFLF          .                      ios::openmode mode );
:eSNPL.
:SMTICS.
:INCLUDE file='sstr_ct2'.
:SALSO.
:SAL typ='dtor'.
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='dtor' prot='public'.~~strstream
:SNPL.
:SNPFLF                   .#include <strstrea.h>
:SNPFLF                   .public:
:SNPCD cd_idx='d'.strstream::~~strstream();
:eSNPL.
:SMTICS.
:INCLUDE file='sstr_dt'.
:SALSO.
:SAL typ='ctor'.
:eSALSO.
:eLIBF.
