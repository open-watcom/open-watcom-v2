:CLFNM.fstream
:CMT.========================================================================
:LIBF fmt='hdr'.fstream
:HFILE.fstream.h
:DVFML.
:DVFM.fstreambase
:DVFM.iostream
:eDVFML.
:CLSS.
The &cls. is used to access files for reading and writing. The file can be
opened and closed, and read, write and seek operations can be performed.
:P.
The &cls. provides very little of its own functionality.
It is derived from both the
:MONO.fstreambase
and
:MONO.iostream
classes. The
:MONO.fstream
constructors, destructor and member function provide simplified
access to the appropriate equivalents in the base classes.
:P.
Of the available I/O stream classes,
creating an &obj. is the preferred method of accessing a file for both input
and output.
:HDG.Public Member Functions
The following public member functions are declared:
:MFNL.
:MFCD cd_idx='c' .fstream();
:MFCD cd_idx='c' .fstream( char const *,
:MFNFLF          .         ios::openmode = ios::in|ios::out,
:MFNFLF          .         int = filebuf::openprot );
:MFCD cd_idx='c' .fstream( filedesc );
:MFCD cd_idx='c' .fstream( filedesc, char *, int );
:MFCD cd_idx='d' .~~fstream();
:MFN index='open'.void open( char const *,
:MFNFLF          .           ios::openmode = ios::in|ios::out,
:MFNFLF          .           int = filebuf::openprot );
:eMFNL.
:eCLSS.
:SALSO.
:SAL typ='cls'.fstreambase
:SAL typ='cls'.ifstream
:SAL typ='cls'.iostream
:SAL typ='cls'.ofstream
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='ctor' prot='public'.fstream
:SNPL.
:SNPFLF                    .#include <fstream.h>
:SNPFLF                    .public:
:SNPCD cd_idx='c'.fstream::fstream();
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
:LIBF fmt='ctor' prot='public'.fstream
:SNPL.
:SNPFLF          .#include <fstream.h>
:SNPFLF          .public:
:SNPCD cd_idx='c'.fstream::fstream( const char *name,
:SNPFLF          .                  ios::openmode mode = ios::in|ios::out,
:SNPFLF          .                  int prot = filebuf::openprot );
:eSNPL.
:SMTICS.
:INCLUDE file='fstr_ct2'.
:SALSO.
:SAL typ='dtor'.
:SAL typ='mfun'.open
:SAL typ='mtyp' ocls='ios'.openmode
:SAL typ='mtyp' ocls='filebuf'.openprot
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='ctor' prot='public'.fstream
:SNPL.
:SNPFLF                    .#include <fstream.h>
:SNPFLF                    .public:
:SNPCD cd_idx='c'.fstream::fstream( filedesc hdl );
:eSNPL.
:SMTICS.
:INCLUDE file='fstr_ct3'.
:SALSO.
:SAL typ='dtor'.
:SAL typ='ofun' ocls='fstreambase'.attach
:SAL typ='ofun' ocls='fstreambase'.fd
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='ctor' prot='public'.fstream
:SNPL.
:SNPFLF          .#include <fstream.h>
:SNPFLF          .public:
:SNPCD cd_idx='c'.fstream::fstream( filedesc hdl, char *buf, int len );
:eSNPL.
:SMTICS.
:INCLUDE file='fstr_ct4'.
:SALSO.
:SAL typ='dtor'.
:SAL typ='ofun' ocls='filebuf'.setbuf
:SAL typ='ofun' ocls='fstreambase'.attach
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='dtor' prot='public'.~~fstream
:SNPL.
:SNPFLF          .#include <fstream.h>
:SNPFLF          .public:
:SNPCD cd_idx='d'.fstream::~~fstream();
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
:SNPF index='open'.void fstream::open( const char *name,
:SNPFLF           .                    ios::openmode mode = ios::in|ios::out,
:SNPFLF           .                    int prot = filebuf::openprot );
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
