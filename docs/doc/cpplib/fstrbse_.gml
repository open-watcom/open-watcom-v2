:CLFNM.fstreambase
:CMT.========================================================================
:LIBF fmt='hdr'.fstreambase
:HFILE.fstream.h
:DVFML.
:DVFM.ios
:eDVFML.
:DVBYL.
:DVBY.ifstream
:DVBY.ofstream
:DVBY.fstream
:eDVBYL.
:CLSS.
The &cls. is a base class that provides common
functionality for the three file-based classes,
:MONO.ifstream
:CONT.,
:MONO.ofstream
and
:MONO.fstream
:PERIOD.
The &cls. is derived from the
:MONO.ios
class, providing the stream state information, plus it provides member
functions for opening and closing files.  The actual file manipulation work
is performed by the
:MONO.filebuf
class.
:P.
It is not intended that &obj.s be created.  Instead, the user should create
an
:MONO.ifstream
:CONT.,
:MONO.ofstream
or
:MONO.fstream
object.
:HDG.Protected Member Functions
The following member functions are declared in the protected interface:
:MFNL.
:MFCD cd_idx='c'.fstreambase();
:MFCD cd_idx='c'.fstreambase( char const *,
:MFNFLF         .             ios::openmode,
:MFNFLF         .             int = filebuf::openprot );
:MFCD cd_idx='c'.fstreambase( filedesc );
:MFCD cd_idx='c'.fstreambase( filedesc, char *, int );
:MFCD cd_idx='d' .~~fstreambase();
:eMFNL.
:HDG.Public Member Functions
The following member functions are declared in the public interface:
:MFNL.
:MFN index='attach' .void attach( filedesc );
:MFN index='close'  .void close();
:MFN index='fd'     .filedesc fd() const;
:MFN index='is_open'.int is_open() const;
:MFN index='open'   .void open( char const *,
:MFNFLF             .           ios::openmode,
:MFNFLF             .           int = filebuf::openprot );
:MFN index='rdbuf'  .filebuf *rdbuf() const;
:MFN index='setbuf' .void setbuf( char *, int );
:eMFNL.
:eCLSS.
:SALSO.
:SAL typ='cls'.filebuf
:SAL typ='cls'.fstream
:SAL typ='cls'.ifstream
:SAL typ='cls'.ofstream
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.attach
:SNPL.
:SNPFLF             .#include <fstream.h>
:SNPFLF             .public:
:SNPF index='attach'.void fstreambase::attach( filedesc hdl );
:eSNPL.
:SMTICS.
The &fn. connects the &obj. to the file specified by the
:ARG.hdl
parameter.
:RSLTS.
If the &fn. fails, &failbit. bit is set in the &errstate.:PERIOD.
The &errstate. is cleared on success.
:SALSO.
:SAL typ='mfun'.fd
:SAL typ='mfun'.is_open
:SAL typ='mfun'.open
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.close
:SNPL.
:SNPFLF            .#include <fstream.h>
:SNPFLF            .public:
:SNPF index='close'.void fstreambase::close();
:eSNPL.
:SMTICS.
The &fn. disconnects the &obj. from the file with which it is associated.
If the &obj. is not associated with a file, the &fn. fails.
:RSLTS.
If the &fn. fails, &failbit. is set in the &errstate.:PERIOD.
:SALSO.
:SAL typ='mfun'.fd
:SAL typ='mfun'.is_open
:SAL typ='mfun'.open
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='ctor' prot='protected'.fstreambase
:SNPL.
:SNPFLF                    .#include <fstream.h>
:SNPFLF                    .protected:
:SNPCD cd_idx='c'.fstreambase::fstreambase();
:eSNPL.
:SMTICS.
The &fn. creates an &obj. that is initialized, but not connected to anything.
The
:MONO.open
or
:MONO.attach
member function should be used to connect the &obj. to a file.
:RSLTS.
The &fn. produces an &obj.:PERIOD.
:SALSO.
:SAL typ='dtor'.
:SAL typ='mfun'.attach
:SAL typ='mfun'.open
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='ctor' prot='protected'.fstreambase
:SNPL.
:SNPFLF          .#include <fstream.h>
:SNPFLF          .protected:
:SNPCD cd_idx='c'.fstreambase::fstreambase( char const *name,
:SNPFLF          .                          ios::openmode mode,
:SNPFLF          .                          int prot = filebuf::openprot );
:eSNPL.
:SMTICS.
This &fn. creates an &obj. that is initialized and connected to the file
indicated by
:ARG.name
using the specified
:ARG.mode
and
:ARG.prot
:PERIOD.
The &obj. is connected to the specified file via the
:MONO.open
C library function.
:RSLTS.
The &fn. produces an &obj.:PERIOD.
If the call to
:MONO.open
for the file fails, &failbit. and &badbit. are set in the &errstate.:PERIOD.
:SALSO.
:SAL typ='dtor'.
:SAL typ='mfun'.open
:SAL typ='mtyp' ocls='ios'.openmode
:SAL typ='mtyp' ocls='filebuf'.openprot
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='ctor' prot='protected'.fstreambase
:SNPL.
:SNPFLF                    .#include <fstream.h>
:SNPFLF                    .protected:
:SNPCD cd_idx='c'.fstreambase::fstreambase( filedesc hdl );
:eSNPL.
:SMTICS.
This &fn. creates an &obj. that is initialized and connected to the open file
specified by the
:ARG.hdl
parameter.
:RSLTS.
The &fn. produces an &obj.:PERIOD.
If the attach to the file fails,
&failbit. and &badbit. are set in the &errstate.:PERIOD.
:SALSO.
:SAL typ='dtor'.
:SAL typ='mfun'.attach
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='ctor' prot='protected'.fstreambase
:SNPL.
:SNPFLF                    .#include <fstream.h>
:SNPFLF                    .protected:
:SNPCD cd_idx='c'.fstreambase::fstreambase( filedesc hdl, char *buf, int len );
:eSNPL.
:SMTICS.
This &fn. creates an &obj. that is initialized and connected to the open file
specified by the
:ARG.hdl
parameter. The buffer, specified by the
:ARG.buf
and
:ARG.len
parameters, is offered via the
:MONO.setbuf
virtual member function to be used as the &rsvarea. for the
:MONO.filebuf
associated with the &obj.:PERIOD.
:RSLTS.
The &fn. produces an &obj.:PERIOD.
If the attach to the file fails,
&failbit. and &badbit. are set in the &errstate.:PERIOD.
:SALSO.
:SAL typ='dtor'.
:SAL typ='mfun'.attach
:SAL typ='mfun'.setbuf
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='dtor' prot='protected'.~~fstreambase
:SNPL.
:SNPFLF          .#include <fstream.h>
:SNPFLF          .protected:
:SNPCD cd_idx='d'.fstreambase::~~fstreambase();
:eSNPL.
:SMTICS.
The &fn. does not do anything explicit.
The
:MONO.filebuf
object associated with the &obj. is embedded within the &obj., so the
:MONO.filebuf
destructor is called. The
:MONO.ios
destructor is called for that portion of the &obj.:PERIOD.
The call to the &fn. is inserted implicitly by the compiler
at the point where the &obj. goes out of scope.
:RSLTS.
The &obj. is destroyed.
:SALSO.
:SAL typ='ctor'.
:SAL typ='mfun'.close
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.is_open
:SNPL.
:SNPFLF              .#include <fstream.h>
:SNPFLF              .public:
:SNPF index='is_open'.int fstreambase::is_open() const;
:eSNPL.
:SMTICS.
The &fn. queries the current state of the file associated with the &obj.:PERIOD.
Calling the &fn. is equivalent to calling the
:MONO.fd
member function and testing for &eof.:PERIOD.
:RSLTS.
The &fn. returns a non-zero value if the &obj. is currently
connected to a file, otherwise zero is returned.
:SALSO.
:SAL typ='mfun'.attach
:SAL typ='mfun'.fd
:SAL typ='mfun'.open
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.fd
:SNPL.
:SNPFLF         .#include <fstream.h>
:SNPFLF         .public:
:SNPF index='fd'.filedesc fstreambase::fd() const;
:eSNPL.
:SMTICS.
The &fn. returns the file descriptor for the file to which
the &obj. is connected.
:RSLTS.
The &fn. returns the file descriptor for the file to which the &obj. is
connected.
If the &obj. is not currently connected to a file, &eof. is returned.
:SALSO.
:SAL typ='mfun'.attach
:SAL typ='mfun'.is_open
:SAL typ='mfun'.open
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.open
:SNPL.
:SNPFLF           .#include <fstream.h>
:SNPFLF           .public:
:SNPF index='open'.void fstreambase::open( const char *name,
:SNPFLF           .                        ios::openmode mode,
:SNPFLF           .                        int prot = filebuf::openprot );
:eSNPL.
:SMTICS.
The &fn. connects the &obj. to the file specified by
:ARG.name
:CONT., using the specified
:ARG.mode
and
:ARG.prot
:PERIOD.
The connection is made via the C library
:MONO.open
function.
:RSLTS.
If the open fails, &failbit. is set in the &errstate.:PERIOD.
The &errstate. is cleared on success.
:SALSO.
:SAL typ='mfun'.attach
:SAL typ='mfun'.close
:SAL typ='mfun'.fd
:SAL typ='mfun'.is_open
:SAL typ='mtyp' ocls='ios'.openmode
:SAL typ='mtyp' ocls='filebuf'.openprot
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.rdbuf
:SNPL.
:SNPFLF            .#include <fstream.h>
:SNPFLF            .public:
:SNPF index='rdbuf'.filebuf *fstreambase::rdbuf() const;
:eSNPL.
:SMTICS.
The &fn. returns the address of the
:MONO.filebuf
object currently associated with the &obj.:PERIOD.
:RSLTS.
The &fn. returns a pointer to the
:MONO.filebuf
object currently associated with the &obj. If there is no associated
:MONO.filebuf
:CONT., &null. is returned.
:SALSO.
:SAL typ='ofun' ocls='ios'.rdbuf
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.setbuf
:SNPL.
:SNPFLF             .#include <fstream.h>
:SNPFLF             .public:
:SNPF index='setbuf'.void fstreambase::setbuf( char *buf, int len );
:eSNPL.
:SMTICS.
The &fn. offers the specified buffer to the
:MONO.filebuf
object associated with the &obj.:PERIOD.
The
:MONO.filebuf
may or may not reject the offer, depending upon its state.
:RSLTS.
If the offer is rejected, &failbit. is set in the &errstate.:PERIOD.
:SALSO.
:SAL typ='ofun' ocls='filebuf'.setbuf
:eSALSO.
:eLIBF.
