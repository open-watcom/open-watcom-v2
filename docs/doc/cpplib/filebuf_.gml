:P.
The input/output stream classes provide program access to the file system.
In addition, various options for formatting of output and reading of
input are provided.
.*
:CLFNM.filebuf
:CMT.========================================================================
:LIBF fmt='hdr'.filebuf
:HFILE.fstream.h
:DVFML.
:DVFM.streambuf
:eDVFML.
:CLSS.
The &cls. is derived from the
:MONO.streambuf
class, and provides additional functionality required
to communicate with external files.
Seek operations are supported when the underlying file supports seeking.
Both input and output operations may be performed using a &obj.,
again when the underlying file supports read/write access.
:P.
&obj.s are buffered by default, so the &rsvarea. is allocated automatically
unless one is specified when the &obj. is created.
The &getarea. and &putarea. pointers operate as if they were tied together.
There is only one current position in a &obj.:PERIOD.
:INCLUDE file='flb_desc'.
:P.
C++ programmers who wish to use files without deriving new objects
do not need to explicitly create or use a &obj.:PERIOD.
:HDG.Public Data Members
The following data member is declared in the public interface.
Its value is the default file protection that is used when
creating new files.  It is primarily referenced as a default
argument in member functions.
:MFNL.
:MFN index='openprot'.static int const openprot;
:eMFNL.
:HDG.Public Member Functions
The following member functions are declared in the public interface:
:MFNL.
:MFCD cd_idx='c'.filebuf();
:MFCD cd_idx='c'.filebuf( filedesc );
:MFCD cd_idx='c'.filebuf( filedesc, char *, int );
:MFCD cd_idx='d' .~~filebuf();
:MFN index='is_open'  .int is_open() const;
:MFN index='fd'       .filedesc fd() const;
:MFN index='attach'   .filebuf *attach( filedesc );
:MFN index='open'     .filebuf *open( char const *,
:MFNFLF               .               ios::openmode,
:MFNFLF               .               int = filebuf::openprot );
:MFN index='close'    .filebuf *close();
:MFN index='pbackfail'.virtual int pbackfail( int );
:MFN index='overflow' .virtual int overflow( int = EOF );
:MFN index='underflow'.virtual int underflow();
:MFN index='setbuf'   .virtual streambuf *setbuf( char *, int );
:MFN index='seekoff'  .virtual streampos seekoff( streamoff,
:MFNFLF               .                           ios::seekdir,
:MFNFLF               .                           ios::openmode );
:MFN index='sync'     .virtual int sync();
:eMFNL.
:eCLSS.
:SALSO.
:SAL typ='cls'.fstreambase
:SAL typ='cls'.streambuf
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.attach
:SNPL.
:SNPFLF             .#include <fstream.h>
:SNPFLF             .public:
:SNPF index='attach'.filebuf *filebuf::attach( filedesc hdl );
:eSNPL.
:SMTICS.
The &fn. connects an existing &obj. to an open file via the file's descriptor
or handle specified by
:ARG.hdl
:PERIOD.
If the &obj. is already connected to a file, the &fn. fails. Otherwise,
the &fn. extracts information from the file system to determine the
capabilities of the file and hence the &obj.:PERIOD.
:RSLTS.
The &fn. returns a pointer to the &obj. on success, otherwise &null. is
returned.
:SALSO.
:SAL typ='ctor'.
:SAL typ='mfun'.fd
:SAL typ='mfun'.open
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.close
:SNPL.
:SNPFLF            .#include <fstream.h>
:SNPFLF            .public:
:SNPF index='close'.filebuf *filebuf::close();
:eSNPL.
:SMTICS.
The &fn. disconnects the &obj. from a connected file and closes the file.
Any buffered output is flushed before the file is closed.
:RSLTS.
The &fn. returns a pointer to the &obj. on success, otherwise &null. is
returned.
:SALSO.
:SAL typ='ctor'.
:SAL typ='mfun'.fd
:SAL typ='mfun'.is_open
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.fd
:SNPL.
:SNPFLF         .#include <fstream.h>
:SNPFLF         .public:
:SNPF index='fd'.filedesc filebuf::fd() const;
:eSNPL.
:SMTICS.
The &fn. queries the state of the &obj. file handle.
:RSLTS.
The &fn. returns the file descriptor or handle of the file to which the &obj.
is currently connected. If the &obj. is not currently connected to a file,
&eof. is returned.
:SALSO.
:SAL typ='mfun'.attach
:SAL typ='mfun'.is_open
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='ctor' prot='public'.filebuf
:SNPL.
:SNPFLF          .#include <fstream.h>
:SNPFLF          .public:
:SNPCD cd_idx='c'.filebuf::filebuf();
:eSNPL.
:SMTICS.
This form of the &fn. creates a &obj. that is not currently
connected to any file. A call to the
:MONO.fd
member function for this created &obj. returns &eof., unless a file
is connected using the
:MONO.attach
member function.
:RSLTS.
The &fn. produces a &obj. that is not currently connected to any file.
:SALSO.
:SAL typ='dtor'.
:SAL typ='mfun'.attach
:SAL typ='mfun'.open
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='ctor' prot='public'.filebuf
:SNPL.
:SNPFLF          .#include <fstream.h>
:SNPFLF          .public:
:SNPCD cd_idx='c'.filebuf::filebuf( filedesc hdl );
:eSNPL.
:SMTICS.
This form of the &fn. creates a &obj. that is connected to an open file.
The file is specified via the
:ARG.hdl
parameter, which is a file descriptor or handle.
:P.
This form of the &fn. is similar to using the default
constructor, and calling the
:MONO.attach
member function. A call to the
:MONO.fd
member function for this created &obj. returns
:ARG.hdl
:PERIOD.
:RSLTS.
The &fn. produces a &obj. that is connected to
:ARG.hdl
:PERIOD.
:SALSO.
:SAL typ='dtor'.
:SAL typ='mfun'.attach
:SAL typ='mfun'.open
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='ctor' prot='public'.filebuf
:SNPL.
:SNPFLF          .#include <fstream.h>
:SNPFLF          .public:
:SNPCD cd_idx='c'.filebuf::filebuf( filedesc hdl, char *buf, int len );
:eSNPL.
:SMTICS.
This form of the &fn. creates a &obj. that is
connected to an open file and that uses the buffer specified by
:ARG.buf
and
:ARG.len
:PERIOD.
The file is specified via the
:ARG.hdl
parameter, which is a file descriptor or handle. If
:ARG.buf
is &null. and/or
:ARG.len
is less than or equal to zero, the &obj. is unbuffered,
so that reading and/or writing take place one character at a time.
:P.
This form of the &fn. is similar to using the default constructor, and
calling the
:MONO.attach
and
:MONO.setbuf
member functions.
:RSLTS.
The &fn. constructor produces a &obj. that is connected to
:ARG.hdl
:PERIOD.
:SALSO.
:SAL typ='dtor'.
:SAL typ='mfun'.attach
:SAL typ='mfun'.open
:SAL typ='mfun'.setbuf
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='dtor' prot='public'.~~filebuf
:SNPL.
:SNPFLF          .#include <fstream.h>
:SNPFLF          .public:
:SNPCD cd_idx='d'.filebuf::~~filebuf();
:eSNPL.
:SMTICS.
The &fn.
closes the file if it was explicitly opened using the
:MONO.open
member function. Otherwise, the destructor takes no explicit action.
The
:MONO.streambuf
destructor is called to destroy that portion of the &obj.:PERIOD.
The call to the &fn. is inserted implicitly by the compiler
at the point where the &obj. goes out of scope.
:RSLTS.
The &obj. is destroyed.
:SALSO.
:SAL typ='dtor'.
:SAL typ='mfun'.close
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.is_open
:SNPL.
:SNPFLF              .#include <fstream.h>
:SNPFLF              .public:
:SNPF index='is_open'.int filebuf::is_open();
:eSNPL.
:SMTICS.
The &fn. queries the &obj. state.
:RSLTS.
The &fn. returns a non-zero value if the &obj.
is currently connected to a file. Otherwise, zero is returned.
:SALSO.
:SAL typ='mfun'.attach
:SAL typ='mfun'.close
:SAL typ='mfun'.fd
:SAL typ='mfun'.open
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.open
:SNPL.
:SNPFLF           .#include <fstream.h>
:SNPFLF           .public:
:SNPF index='open'.filebuf *filebuf::open( const char *name,
:SNPFLF           .                        ios::openmode mode,
:SNPFLF           .                        int prot = filebuf::openprot );
:eSNPL.
:SMTICS.
The &fn. is used to connect the &obj. to a file specified by the
:ARG.name
parameter.
The file is opened using the specified
:ARG.mode
:PERIOD.
For details about the
:ARG.mode
parameter, see the description of
:MONO.ios::openmode
:PERIOD.
The
:ARG.prot
parameter specifies the file protection
attributes to use when creating a file.
:RSLTS.
The &fn. returns a pointer to the &obj. on success, otherwise &null. is
returned.
:SALSO.
:SAL typ='ctor'.
:SAL typ='mfun'.close
:SAL typ='mfun'.is_open
:SAL typ='mdata'.openprot
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mdata' prot='public'.openprot
:SNPL.
:SNPFLF               .#include <fstream.h>
:SNPFLF               .public:
:SNPD index='openprot'.static int const filebuf::openprot;
:eSNPL.
:SMTICS.
The &fn. is used to specify the default file protection to be used when
creating new files. This value is used as the default if no user specified
value is provided.
:P.
The default value is octal 0644.  This is generally interpreted as follows:
:UL.
:LI.Owner: read/write
:LI.Group: read
:LI.World: read
:eUL.
:P.
Note that not all operating systems support all bits.
:SALSO.
:SAL typ='ctor'.
:SAL typ='mfun'.open
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public virtual'.overflow
:SNPL.
:SNPFLF               .#include <fstream.h>
:SNPFLF               .public:
:SNPF index='overflow'.virtual int filebuf::overflow( int ch = EOF );
:eSNPL.
:SMTICS.
The &fn. provides the output communication to the file to which the &obj.
is connected. Member functions in the
:MONO.streambuf
class call the &fn. for the derived class when the &putarea. is full.
:INCLUDE file='fs_ovflw'.
:SALSO.
:SAL typ='ofun' ocls='streambuf'.overflow
:SAL typ='mfun'.underflow
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public virtual'.pbackfail
:SNPL.
:SNPFLF                .#include <fstream.h>
:SNPFLF                .public:
:SNPF index='pbackfail'.virtual int filebuf::pbackfail( int ch );
:eSNPL.
:SMTICS.
The &fn. handles an attempt to put back a character when there is no room at
the beginning of the &getarea.:PERIOD.
The &fn. first calls the
:MONO.sync
virtual member function to flush the &putarea. and then it attempts to
seek backwards over
:ARG.ch
in the associated file.
:RSLTS.
The &fn. returns
:ARG.ch
on success, otherwise &eof. is returned.
:SALSO.
:SAL typ='ofun' ocls='streambuf'.pbackfail
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public virtual'.seekoff
:SNPL.
:SNPFLF              .#include <fstream.h>
:SNPFLF              .public:
:SNPF index='seekoff'.virtual streampos filebuf::seekoff( streamoff offset,
:SNPFLF              .                                    ios::seekdir dir,
:SNPFLF              .                                    ios::openmode mode );
:eSNPL.
:SMTICS.
The &fn. is used to position the &obj. (and hence the file) to a particular
offset so that subsequent input or output operations commence from
that point.
The offset is specified by the
:ARG.offset
and
:ARG.dir
parameters.
:P.
Since the &getarea. and &putarea. pointers are tied together for
the &obj., the
:ARG.mode
parameter is ignored.
:P.
Before the actual seek occurs, the &getarea. and &putarea. of the &obj.
are flushed via the
:MONO.sync
virtual member function.
Then, the new position in the file is calculated and the seek takes place.
:INCLUDE file='seekdir'.
:RSLTS.
The &fn. returns the new position in the file on success, otherwise &eof. is
returned.
:SALSO.
:SAL typ='ofun' ocls='streambuf'.seekoff
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public virtual'.setbuf
:SNPL.
:SNPFLF             .#include <fstream.h>
:SNPFLF             .public:
:SNPF index='setbuf'.virtual streambuf *filebuf::setbuf( char *buf, int len );
:eSNPL.
:SMTICS.
The &fn. is used to offer a buffer, specified by
:ARG.buf
and
:ARG.len
to the &obj.:PERIOD.
If the
:ARG.buf
parameter is &null. or the
:ARG.len
is less than or equal to zero, the request is to make the &obj.
unbuffered.
:P.
If the &obj. is already connected to a file and has a buffer, the offer is
rejected. In other words, a call to the &fn. after the &obj. has started to
be used usually fails because the &obj. has set up a buffer.
:P.
If the request is to make the &obj. unbuffered, the offer succeeds.
:P.
If the
:ARG.buf
is too small (less than five characters), the offer is rejected.
Five characters are required to support the default putback area.
:P.
Otherwise, the
:ARG.buf
is acceptable and the offer succeeds.
:P.
If the offer succeeds, the
:MONO.streambuf::setb
member function is called to set up the pointers to the buffer.
The
:MONO.streambuf::setb
member function releases the old buffer (if present),
depending on how that buffer was allocated.
:P.
Calls to the &fn. are usually made by a class derived from the
:MONO.fstream
class, not directly by a user program.
:RSLTS.
The &fn. returns a pointer to the &obj. on success, otherwise &null. is
returned.
:SALSO.
:SAL typ='ofun' ocls='streambuf'.setbuf
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public virtual'.sync
:SNPL.
:SNPFLF           .#include <fstream.h>
:SNPFLF           .public:
:SNPF index='sync'.virtual int filebuf::sync();
:eSNPL.
:SMTICS.
The &fn. synchronizes the &obj. with the external file or device.
If the &putarea. contains characters it is flushed. This leaves the file
positioned after the last written character. If the &getarea.
contains buffered (unread) characters, file is
backed up to be positioned after the last read character.
:P.
Note that the &getarea. and &putarea. never both contain characters.
:RSLTS.
The &fn. returns &noteof. on success, otherwise &eof. is returned.
:SALSO.
:SAL typ='ofun' ocls='streambuf'.sync
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public virtual'.underflow
:SNPL.
:SNPFLF                .#include <fstream.h>
:SNPFLF                .public:
:SNPF index='underflow'.virtual int filebuf::underflow();
:eSNPL.
:SMTICS.
The &fn. provides the input communication from the file to which the &obj.
is connected. Member functions in the
:MONO.streambuf
class call the &fn. for the derived class when the &getarea. is empty.
:INCLUDE file='fs_unflw'.
:SALSO.
:SAL typ='ofun' ocls='streambuf'.underflow
:SAL typ='mfun'.overflow
:eSALSO.
:eLIBF.
