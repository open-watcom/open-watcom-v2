:CLFNM.ifstream
:CMT.========================================================================
:LIBF fmt='hdr'.ifstream
:HFILE.fstream.h
:DVFML.
:DVFM.fstreambase
:DVFM.istream
:eDVFML.
:CLSS.
The &cls. is used to access existing files for reading.
Such files can be opened and closed, and read and seek operations can be
performed.
:P.
The &cls. provides very little of its own functionality.
Derived from both the
:MONO.fstreambase
and
:MONO.istream
classes, its
constructors, destructor and member functions provide simplified access
to the appropriate equivalents in those base classes.
:P.
Of the available I/O stream classes,
creating an &obj. is the preferred method of accessing a file
for input only operations.
:HDG.Public Member Functions
The following public member functions are declared:
:MFNL.
:MFCD cd_idx='c' .ifstream();
:MFCD cd_idx='c' .ifstream( char const *,
:MFNFLF          .          ios::openmode = ios::in,
:MFNFLF          .          int = filebuf::openprot );
:MFCD cd_idx='c' .ifstream( filedesc );
:MFCD cd_idx='c' .ifstream( filedesc, char *, int );
:MFCD cd_idx='d' .~~ifstream();
:MFN index='open'.void open( char const *,
:MFNFLF          .           ios::openmode = ios::in,
:MFNFLF          .           int = filebuf::openprot );
:eMFNL.
:eCLSS.
:SALSO.
:SAL typ='cls'.fstream
:SAL typ='cls'.fstreambase
:SAL typ='cls'.istream
:SAL typ='cls'.ofstream
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='ctor' prot='public'.ifstream
:SNPL.
:SNPFLF          .#include <fstream.h>
:SNPFLF          .public:
:SNPCD cd_idx='c'.ifstream::ifstream();
:eSNPL.
:SMTICS.
:INCLUDE file='fstr_ct1'.
:SALSO.
:SAL typ='dtor'.
:SAL typ='mfun'.open
:SAL typ='ofun' ocls='fstreambase'.attach
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='ctor' prot='public'.ifstream
:SNPL.
:SNPFLF          .#include <fstream.h>
:SNPFLF          .public:
:SNPCD cd_idx='c'.ifstream::ifstream( const char *name,
:SNPFLF          .                    ios::openmode mode = ios::in,
:SNPFLF          .                    int prot = filebuf::openprot );
:eSNPL.
:SMTICS.
:INCLUDE file='fstr_ct2'.
:SALSO.
:SAL typ='dtor'.
:SAL typ='mfun'.open
:SAL typ='mtyp' ocls='ios'.openmode
:SAL typ='mtyp' ocls='filebuf'.openprot
:SAL typ='ofun' ocls='fstreambase'.attach
:SAL typ='ofun' ocls='fstreambase'.fd
:SAL typ='ofun' ocls='fstreambase'.is_open
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='ctor' prot='public'.ifstream
:SNPL.
:SNPFLF          .#include <fstream.h>
:SNPFLF          .public:
:SNPCD cd_idx='c'.ifstream::ifstream( filedesc hdl );
:eSNPL.
:SMTICS.
:INCLUDE file='fstr_ct3'.
:SALSO.
:SAL typ='ofun' ocls='fstreambase'.attach
:SAL typ='dtor'.
:SAL typ='mfun'.open
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='ctor' prot='public'.ifstream
:SNPL.
:SNPFLF          .#include <fstream.h>
:SNPFLF          .public:
:SNPCD cd_idx='c'.ifstream::ifstream( filedesc hdl, char *buf, int len );
:eSNPL.
:SMTICS.
:INCLUDE file='fstr_ct4'.
:SALSO.
:SAL typ='ofun' ocls='fstreambase'.attach
:SAL typ='ofun' ocls='fstreambase'.setbuf
:SAL typ='dtor'.
:SAL typ='mfun'.open
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='dtor' prot='public'.~~ifstream
:SNPL.
:SNPFLF          .#include <fstream.h>
:SNPFLF          .public:
:SNPCD cd_idx='d'.ifstream::~~ifstream();
:eSNPL.
:SMTICS.
:INCLUDE file='fstr_dt'.
:SALSO.
:SAL typ='ctor'.
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.open
:SNPL.
:SNPFLF           .#include <fstream.h>
:SNPFLF           .public:
:SNPF index='open'.void ifstream::open( const char *name,
:SNPFLF           .                     ios::openmode mode = ios::in,
:SNPFLF           .                     int prot = filebuf::openprot );
:eSNPL.
:SMTICS.
:INCLUDE file='fstr_op'.
:SALSO.
:SAL typ='ofun' ocls='fstreambase'.attach
:SAL typ='ofun' ocls='fstreambase'.close
:SAL typ='ofun' ocls='fstreambase'.fd
:SAL typ='ofun' ocls='fstreambase'.is_open
:SAL typ='mtyp' ocls='ios'.openmode
:SAL typ='mtyp' ocls='filebuf'.openprot
:eSALSO.
:eLIBF.
