:CLFNM.ofstream
:CMT.========================================================================
:LIBF fmt='hdr'.ofstream
:HFILE.fstream.h
:DVFML.
:DVFM.fstreambase
:DVFM.ostream
:eDVFML.
:CLSS.
The &cls. is used to create new files or access existing files for writing.
The file can be opened and closed, and write and seek operations can be
performed.
:P.
The &cls. provides very little of its own functionality.
Derived from both the
:MONO.fstreambase
and
:MONO.ostream
classes, its
constructors, destructor and member function provide simplified access
to the appropriate equivalents in those base classes.
:P.
Of the available I/O stream classes,
creating an &obj. is the preferred method of accessing a file for output
operations.
:HDG.Public Member Functions
The following public member functions are declared:
:MFNL.
:MFCD cd_idx='c' .ofstream();
:MFCD cd_idx='c' .ofstream( char const *,
:MFNFLF          .          ios::openmode = ios::out,
:MFNFLF          .          int = filebuf::openprot );
:MFCD cd_idx='c' .ofstream( filedesc );
:MFCD cd_idx='c' .ofstream( filedesc, char *, int );
:MFCD cd_idx='d' .~~ofstream();
:MFN index='open'.void open( char const *,
:MFNFLF          .           ios::openmode = ios::out,
:MFNFLF          .           int = filebuf::openprot );
:eMFNL.
:eCLSS.
:SALSO.
:SAL typ='cls'.fstream
:SAL typ='cls'.fstreambase
:SAL typ='cls'.ifstream
:SAL typ='cls'.ostream
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='ctor' prot='public'.ofstream
:SNPL.
:SNPFLF                    .#include <fstream.h>
:SNPFLF                    .public:
:SNPCD cd_idx='c'.ofstream::ofstream();
:eSNPL.
:SMTICS.
:INCLUDE file='fstr_ct1'.
:SALSO.
:SAL typ='dtor'.
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='ctor' prot='public'.ofstream
:SNPL.
:SNPFLF          .#include <fstream.h>
:SNPFLF          .public:
:SNPCD cd_idx='c'.ofstream::ofstream( const char *name,
:SNPFLF          .                    ios::openmode mode = ios::out,
:SNPFLF          .                    int prot = filebuf::openprot );
:eSNPL.
:SMTICS.
:INCLUDE file='fstr_ct2'.
:SALSO.
:SAL typ='dtor'.
:SAL typ='mfun'.open
:SAL typ='ofun' ocls='fstreambase'.close
:SAL typ='mtyp' ocls='ios'.openmode
:SAL typ='mtyp' ocls='filebuf'.openprot
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='ctor' prot='public'.ofstream
:SNPL.
:SNPFLF                    .#include <fstream.h>
:SNPFLF                    .public:
:SNPCD cd_idx='c'.ofstream::ofstream( filedesc hdl );
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
:LIBF fmt='ctor' prot='public'.ofstream
:SNPL.
:SNPFLF                    .#include <fstream.h>
:SNPFLF                    .public:
:SNPCD cd_idx='c'.ofstream::ofstream( filedesc hdl, char *buf, int len );
:eSNPL.
:SMTICS.
:INCLUDE file='fstr_ct4'.
:SALSO.
:SAL typ='dtor'.
:SAL typ='ofun' ocls='fstreambase'.attach
:SAL typ='ofun' ocls='fstreambase'.fd
:SAL typ='ofun' ocls='fstreambase'.setbuf
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='dtor' prot='public'.~~ofstream
:SNPL.
:SNPFLF                   .#include <fstream.h>
:SNPFLF                   .public:
:SNPCD cd_idx='d'.ofstream::~~ofstream();
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
:SNPF index='open'.void ofstream::open( const char *name,
:SNPFLF           .                     ios::openmode mode = ios::out,
:SNPFLF           .                     int prot = filebuf::openprot );
:eSNPL.
:SMTICS.
:INCLUDE file='fstr_op'.
:SALSO.
:SAL typ='ctor'.
:SAL typ='mtyp' ocls='ios'.openmode
:SAL typ='mtyp' ocls='filebuf'.openprot
:SAL typ='ofun' ocls='fstreambase'.attach
:SAL typ='ofun' ocls='fstreambase'.close
:SAL typ='ofun' ocls='fstreambase'.fd
:SAL typ='ofun' ocls='fstreambase'.is_open
:eSALSO.
:eLIBF.
