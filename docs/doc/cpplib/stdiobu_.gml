:CLFNM.stdiobuf
:CMT.========================================================================
:LIBF fmt='hdr'.stdiobuf
:HFILE.stdiobuf.h
:DVFML.
:DVFM.streambuf
:eDVFML.
:CLSS.
The &cls. specializes the
:MONO.streambuf
class and is used to implement the standard input/output buffering required
for the &cin., &cout., &cerr. and &clog. predefined objects.
:INCLUDE file='std_desc'.
:P.
C++ programmers who wish to use the standard input/output streams without
deriving new objects do not need to explicitly create or use a &obj.:PERIOD.
:HDG.Public Member Functions
The following member functions are declared in the public interface:
:MFNL.
:MFCD cd_idx='c'.stdiobuf();
:MFCD cd_idx='c'.stdiobuf( FILE * );
:MFCD cd_idx='d' .~~stdiobuf();
:MFN index='overflow'     .virtual int overflow( int = EOF );
:MFN index='underflow'    .virtual int underflow();
:MFN index='sync'         .virtual int sync();
:eMFNL.
:eCLSS.
:SALSO.
:SAL typ='cls'.streambuf
:SAL typ='cls'.ios
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public virtual'.overflow
:SNPL.
:SNPFLF               .#include <stdiobuf.h>
:SNPFLF               .public:
:SNPF index='overflow'.virtual int stdiobuf::overflow( int ch = EOF );
:eSNPL.
:SMTICS.
The &fn. provides the output communication to the standard output and standard
error devices to which the &obj. is connected. Member functions in the
:MONO.streambuf
class call the &fn. for the derived class when the &putarea. is full.
:INCLUDE file='fs_ovflw'.
:SALSO.
:SAL typ='mfun'.underflow
:SAL typ='ofun' ocls='streambuf'.overflow
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='ctor' prot='public'.stdiobuf
:SNPL.
:SNPFLF               .#include <stdiobuf.h>
:SNPFLF               .public:
:SNPCD cd_idx='c'.stdiobuf::stdiobuf();
:eSNPL.
:SMTICS.
This form of the &fn. creates a &obj. that is initialized but not yet connected
to a file.
:RSLTS.
This form of the &fn. creates a &obj.:PERIOD.
:SALSO.
:SAL typ='dtor'.
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='ctor' prot='public'.stdiobuf
:SNPL.
:SNPFLF               .#include <stdiobuf.h>
:SNPFLF               .public:
:SNPCD cd_idx='c'.stdiobuf::stdiobuf( FILE *fptr );
:eSNPL.
:SMTICS.
This form of the &fn. creates a &obj. that is initialized and connected to a C
library FILE stream.  Usually, one of
:MONO.stdin
:CONT.,
:MONO.stdout
or
:MONO.stderr
is specified for the
:ARG.fptr
parameter.
:RSLTS.
This form of the &fn. creates a &obj. that is initialized and connected to a C
library FILE stream.
:SALSO.
:SAL typ='dtor'.
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='dtor' prot='public'.~~stdiobuf
:SNPL.
:SNPFLF                 .#include <stdiobuf.h>
:SNPFLF                 .public:
:SNPCD cd_idx='d'.stdiobuf::~~stdiobuf();
:eSNPL.
:SMTICS.
The &fn. does not do anything explicit. The
:MONO.streambuf
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
:LIBF fmt='mfun' prot='public virtual'.sync
:SNPL.
:SNPFLF           .#include <stdiobuf.h>
:SNPFLF           .public:
:SNPF index='sync'.virtual int stdiobuf::sync();
:eSNPL.
:SMTICS.
The &fn. synchronizes the &obj. with the associated device.
If the &putarea. contains characters, it is flushed.
If the &getarea. contains buffered characters, the &fn. fails.
:RSLTS.
The &fn. returns &noteof. on success, otherwise, &eof. is returned.
:SALSO.
:SAL typ='ofun' ocls='streambuf'.sync
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public virtual'.underflow
:SNPL.
:SNPFLF                .#include <stdiobuf.h>
:SNPFLF                .public:
:SNPF index='underflow'.virtual int stdiobuf::underflow();
:eSNPL.
:SMTICS.
The &fn. provides the input communication from the standard input device
to which the &obj. is connected. Member functions in the
:MONO.streambuf
class call the &fn. for the derived class when the &getarea. is empty.
:INCLUDE file='fs_unflw'.
:SALSO.
:SAL typ='mfun'.overflow
:SAL typ='ofun' ocls='streambuf'.underflow
:eSALSO.
:eLIBF.
