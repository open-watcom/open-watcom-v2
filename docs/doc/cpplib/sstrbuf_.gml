:CLFNM.strstreambuf
:CMT.========================================================================
:LIBF fmt='hdr'.strstreambuf
:HFILE.strstrea.h
:DVFML.
:DVFM.streambuf
:eDVFML.
:CLSS.
The &cls. is derived from the
:MONO.streambuf
class and provides additional functionality required to write characters to
and read characters from a string buffer. Read and write operations can occur
at different positions in the string buffer, since the &getptr. and &putptr.
are not necessarily connected. Seek operations are also supported.
:P.
The &rsvarea. used by the &obj. may be either fixed in size or dynamic.
Generally, input strings are of a fixed size, while output streams are
dynamic, since the final size may not be predictable. For dynamic buffers,
the &obj. automatically grows the buffer when necessary.
:INCLUDE file='ssb_desc'.
:P.
C++ programmers who wish to use string streams without deriving new
objects will probably never explicitly create or use a &obj.:PERIOD.
:HDG.Protected Member Functions
The following member function is declared in the protected interface:
:MFNL.
:MFN index='doallocate'.virtual int doallocate();
:eMFNL.
:HDG.Public Member Functions
The following member functions are declared in the public interface:
:MFNL.
:MFCD cd_idx='c'                 .strstreambuf();
:MFCD cd_idx='c'                 .strstreambuf( int );
:MFCD cd_idx='c'                 .strstreambuf( void *(*)( long ), void (*)( void * ) );
:MFCD cd_idx='c'                 .strstreambuf( char *, int, char * = 0 );
:MFCD cd_idx='d'                 .~~strstreambuf();
:MFN index='alloc_size_increment'.int alloc_size_increment( int );
:MFN index='freeze'              .void freeze( int = 1 );
:MFN index='str'                 .char *str();
:MFN index='overflow'            .virtual int overflow( int = EOF );
:MFN index='underflow'           .virtual int underflow();
:MFN index='setbuf'              .virtual streambuf *setbuf( char *, int );
:MFN index='seekoff'             .virtual streampos seekoff( streamoff,
:MFNFLF                          .                           ios::seekdir,
:MFNFLF                          .                           ios::openmode );
:MFN index='sync'                .virtual int sync();
:eMFNL.
:eCLSS.
:SALSO.
:SAL typ='cls'.streambuf
:SAL typ='cls'.strstreambase
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.alloc_size_increment
:SNPL.
:SNPFLF                           .#include <strstrea.h>
:SNPFLF                           .public:
:SNPF index='alloc_size_increment'.int strstreambuf::alloc_size_increment( int increment );
:eSNPL.
:SMTICS.
The &fn. modifies the allocation size used when the buffer is first allocated
or reallocated by dynamic allocation.
The
:ARG.increment
parameter is added to the previous allocation size for future use.
:P.
This function is a WATCOM extension.
:RSLTS.
The &fn. returns the previous value of the allocation size.
:SALSO.
:SAL typ='mfun'.doallocate
:SAL typ='mfun'.setbuf
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='protected virtual'.doallocate
:SNPL.
:SNPFLF                 .#include <strstrea.h>
:SNPFLF                 .protected:
:SNPF index='doallocate'.virtual int strstreambuf::doallocate();
:eSNPL.
:SMTICS.
The &fn. is called by the
:MONO.allocate
member function when it is determined that the &putarea. is full and needs to
be extended.
:P.
The &fn. performs the following steps:
:OL.
:LI.If dynamic allocation is not being used, the &fn. fails.
:LI.A new size for the buffer is determined.
If the allocation size is bigger than the current size, the allocation
size is used. Otherwise, the buffer size is increased by
:MONO.DEFAULT_MAINBUF_SIZE
:CONT., which is 512.
:LI.A new buffer is allocated.
If an allocation function was specified in the constructor for the
&obj., that allocation function is used, otherwise the
:MONO.operator~bnew
intrinsic function is used. If the allocation fails, the &fn. fails.
:LI.If necessary, the contents of the &getarea. are copied to the newly
allocated buffer and the &getarea. pointers are adjusted accordingly.
:LI.The contents of the &putarea. are copied to the newly allocated buffer
and the &putarea. pointers are adjusted accordingly, extending the &putarea.
to the end of the new buffer.
:LI.The old buffer is freed.
If a free function was specified in the constructor for the
&obj., that free function is used, otherwise the
:MONO.operator~bdelete
intrinsic function is used.
:eOL.
:RSLTS.
The &fn. returns &noteof. on success, otherwise &eof. is returned.
:SALSO.
:SAL typ='mfun'.alloc_size_increment
:SAL typ='mfun'.setbuf
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.freeze
:SNPL.
:SNPFLF             .#include <strstrea.h>
:SNPFLF             .public:
:SNPF index='freeze'.void strstreambuf::freeze( int frozen = 1 );
:eSNPL.
:SMTICS.
The &fn. enables and disables automatic deletion of the &rsvarea.:PERIOD.
If the &fn. is called with no parameter or a non-zero parameter, the
&obj. is frozen.
If the &fn. is called with a zero parameter, the &obj. is unfrozen.
:P.
A frozen &obj. does not free the &rsvarea. in the destructor. If the &obj. is
destroyed while it is frozen, it is the program's responsibility to also
free the &rsvarea.:PERIOD.
:P.
If characters are written to the &obj. while it is frozen, the
effect is undefined since the &rsvarea. may be reallocated and therefore
may move. However, if the &obj. is frozen and then unfrozen, characters
may be written to it.
:RSLTS.
The &fn. returns the previous frozen state.
:SALSO.
:SAL typ='mfun'.str
:SAL typ='dtor'.
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public virtual'.overflow
:SNPL.
:SNPFLF               .#include <strstrea.h>
:SNPFLF               .public:
:SNPF index='overflow'.virtual int strstreambuf::overflow( int ch = EOF );
:eSNPL.
:SMTICS.
The &fn. provides the output communication between the
:MONO.streambuf
member functions and the &obj.:PERIOD.
Member functions in the
:MONO.streambuf
class call the &fn. when the &putarea. is full. The &fn. attempts to grow
the &putarea. so that writing may continue.
:P.
The &fn. performs the following steps:
:OL.
:LI.If dynamic allocation is not being used, the &putarea. cannot be
extended, so the &fn. fails.
:LI.If dynamic allocation is being used, a new buffer is allocated using the
:MONO.doallocate
member function. It handles copying the contents of the old buffer
to the new buffer and discarding the old buffer.
:LI.If the
:ARG.ch
parameter is not &eof., it is added to the end of the extended &putarea. and
the &putptr. is advanced.
:eOL.
:RSLTS.
The &fn. returns &noteof. when it successfully extends the &putarea., otherwise
&eof. is returned.
:SALSO.
:SAL typ='ofun' ocls='streambuf'.overflow
:SAL typ='mfun'.underflow
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public virtual'.seekoff
:SNPL.
:SNPFLF              .#include <strstrea.h>
:SNPFLF              .public:
:SNPF index='seekoff'.virtual streampos strstreambuf::seekoff( streamoff offset,
:SNPFLF              .                                         ios::seekdir dir,
:SNPFLF              .                                         ios::openmode mode );
:eSNPL.
:SMTICS.
The &fn. positions the &getptr. and/or &putptr. to the specified position
in the &rsvarea.:PERIOD.
If the &getptr. is moved, it is moved to a position
relative to the start of the &rsvarea. (which is also the start of the
&getarea.). If a position is specified that is beyond the end of the
&getarea. but is in the &putarea., the &getarea. is extended to include
the &putarea.:PERIOD.
If the &putptr. is moved, it is moved to a position
relative to the start of the &putarea.,
:HILITE.not
relative to the start of the
&rsvarea.:PERIOD.
:P.
The &fn. seeks
:ARG.offset
bytes from the position specified by the
:ARG.dir
parameter.
:INCLUDE file='seekmode'.
:MONO.ios::in|ios::out
is not valid if the
:ARG.dir
parameter is
:MONO.ios::cur
:PERIOD.
:INCLUDE file='seekdir'.
:RSLTS.
The &fn. returns the new position in the file on success, otherwise &eof. is
returned.
If both or
:MONO.ios::in|ios::out
are specified and the
:ARG.dir
parameter is
:MONO.ios::cur
the returned position refers to the &putptr.:PERIOD.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public virtual'.setbuf
:SNPL.
:SNPFLF             .#include <strstrea.h>
:SNPFLF             .public:
:SNPF index='setbuf'.virtual streambuf *strstreambuf::setbuf( char *, int size );
:eSNPL.
:SMTICS.
The &fn. is used to control the size of the allocations when the &obj. is using
dynamic allocation. The first parameter is ignored. The next time an
allocation is required, at least the number of characters specified in the
:ARG.size
parameter is allocated. If the specified size is not sufficient,
the allocation reverts to its default behavior, which is to extend the
buffer by
:MONO.DEFAULT_MAINBUF_SIZE
:CONT., which is 512 characters.
:P.
If a program is going to write a large number of characters to the &obj., it
should call the &fn. to indicate the size of the next allocation, to prevent
multiple allocations as the buffer gets larger.
:RSLTS.
The &fn. returns a pointer to the &obj.:PERIOD.
:SALSO.
:SAL typ='mfun'.alloc_size_increment
:SAL typ='mfun'.doallocate
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.str
:SNPL.
:SNPFLF          .#include <strstrea.h>
:SNPFLF          .public:
:SNPF index='str'.char *strstreambuf::str();
:eSNPL.
:SMTICS.
The &fn. freezes the &obj. and returns a pointer to the &rsvarea.:PERIOD.
This pointer
remains valid after the &obj. is destroyed provided the &obj. remains
frozen, since the destructor does not free the &rsvarea. if it is frozen.
:P.
The returned pointer may be &null. if the &obj. is using dynamic allocation but
has not yet had anything written to it.
:P.
If the &obj. is not using dynamic allocation, the pointer returned by the
&fn. is the same buffer pointer provided to the constructor. For a &obj.
using dynamic allocation, the pointer points to a dynamically allocated area.
:P.
Note that the &rsvarea. does not necessarily end with a null character.
If the pointer returned by the &fn. is to be interpreted as a C string,
it is the program's responsibility to ensure that the null character is
present.
:RSLTS.
The &fn. returns a pointer to the &rsvarea. and freezes the &obj.:PERIOD.
:SALSO.
:SAL typ='mfun'.freeze
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='ctor' prot='public'.strstreambuf
:SNPL.
:SNPFLF                    .#include <strstrea.h>
:SNPFLF                    .public:
:SNPCD cd_idx='c'.strstreambuf::strstreambuf();
:eSNPL.
:SMTICS.
This form of the &fn. creates an empty &obj. that uses dynamic allocation. No
&rsvarea. is allocated to start. Whenever characters are written to extend the
&obj., the &rsvarea. is reallocated and copied as required. The size of
allocation is determined by the &obj. unless the
:MONO.setbuf
or
:MONO.alloc_size_increment
member functions are called to change the allocation size.
The default allocation size is determined by the constant
:MONO.DEFAULT_MAINBUF_SIZE
:CONT., which is 512.
:RSLTS.
This form of the &fn. creates a &obj.:PERIOD.
:SALSO.
:SAL typ='mfun'.doallocate
:SAL typ='dtor'.
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='ctor' prot='public'.strstreambuf
:SNPL.
:SNPFLF                    .#include <strstrea.h>
:SNPFLF                    .public:
:SNPCD cd_idx='c'.strstreambuf::strstreambuf( int alloc_size );
:eSNPL.
:SMTICS.
This form of the &fn. creates an empty &obj. that uses dynamic allocation.
No buffer is allocated to start. Whenever characters are written to extend
the &obj., the &rsvarea. is reallocated and copied as required. The size
of the first allocation is determined by the
:ARG.alloc_size
parameter, unless changed by a call to the
:MONO.setbuf
or
:MONO.alloc_size_increment
member functions.
:P.
Note that the
:ARG.alloc_size
parameter is the starting &rsvarea. size. When the &rsvarea. is reallocated,
the &obj. uses
:MONO.DEFAULT_MAINBUF_SIZE
to increase the &rsvarea. size, unless the
:MONO.setbuf
or
:MONO.alloc_size_increment
member functions have been called to specify a new allocation size.
:RSLTS.
This form of the &fn. creates a &obj.:PERIOD.
:SALSO.
:SAL typ='mfun'.alloc_size_increment
:SAL typ='mfun'.doallocate
:SAL typ='mfun'.setbuf
:SAL typ='dtor'.
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='ctor' prot='public'.strstreambuf
:SNPL.
:SNPFLF          .#include <strstrea.h>
:SNPFLF          .public:
:SNPCD cd_idx='c'.strstreambuf::strstreambuf( void * (*alloc_fn)( long ),
:SNPFLF          .                            void   (*free_fn)( void * ) );
:eSNPL.
:SMTICS.
This form of the &fn. creates an empty &obj. that uses dynamic allocation.
No buffer is allocated to start. Whenever characters are written to extend
the &obj., the &rsvarea. is reallocated and copied as required, using the
specified
:ARG.alloc_fn
and
:ARG.free_fn
functions.
The size of allocation is determined by the class unless the
:MONO.setbuf
or
:MONO.alloc_size_increment
member functions are called to change the allocation size.
The default allocation size is determined by the constant
:MONO.DEFAULT_MAINBUF_SIZE
:CONT., which is 512.
:P.
When a new &rsvarea. is allocated, the function specified by the
:ARG.alloc_fn
parameter is called with a
:MONO.long integer
value indicating the number of bytes to allocate.
If
:ARG.alloc_fn
is &null., the
:MONO.operator~bnew
intrinsic function is used.
Likewise, when the &rsvarea. is freed, the function specified by the
:ARG.free_fn
parameter is called with the pointer returned by the
:ARG.alloc_fn
function as the parameter.
If
:ARG.free_fn
is &null., the
:MONO.operator~bdelete
intrinsic function is used.
:RSLTS.
This form of the &fn. creates a &obj.:PERIOD.
:SALSO.
:SAL typ='mfun'.alloc_size_increment
:SAL typ='mfun'.doallocate
:SAL typ='mfun'.setbuf
:SAL typ='dtor'.
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='ctor' prot='public'.strstreambuf
:SNPL.
:SNPFLF          .#include <strstrea.h>
:SNPFLF          .public:
:SNPCD cd_idx='c'.strstreambuf::strstreambuf( char *str,
:SNPFLF          .                            int len,
:SNPFLF          .                            char *pstart = NULL );
:SNPCD cd_idx='c'.strstreambuf::strstreambuf( signed char *str,
:SNPFLF          .                            int len,
:SNPFLF          .                            signed char *pstart = NULL );
:SNPCD cd_idx='c'.strstreambuf::strstreambuf( unsigned char *str,
:SNPFLF          .                            int len,
:SNPFLF          .                            unsigned char *pstart = NULL );
:eSNPL.
:SMTICS.
This form of the &fn. creates a &obj. that does not use dynamic
allocation (unless
:ARG.str
is &null.). The &obj. is said to be using static allocation. The
:ARG.str
and
:ARG.len
parameters specify the bounds of the &rsvarea.:PERIOD.
:INCLUDE file='str_b'.
:P.
If the &getarea. is exhausted and characters have been written to the
&putarea., the &getarea. is extended to include the &putarea.:PERIOD.
:P.
The &getptr. and &putptr. do not necessarily point at the
same position in the &rsvarea., so a read followed by a write does not
imply that the write stores following the last character read.
The &getptr. is positioned following the last read operation, and
the &putptr. is positioned following the last write operation, unless the
:MONO.seekoff
member function has been used to reposition the pointer(s).
:P.
Note that if
:ARG.str
is &null. the effect is to create an empty dynamic &obj.:PERIOD.
:RSLTS.
This form of the &fn. creates a &obj.:PERIOD.
:SALSO.
:SAL typ='dtor'.
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='dtor' prot='public'.~~strstreambuf
:SNPL.
:SNPFLF                   .#include <strstrea.h>
:SNPFLF                   .public:
:SNPCD cd_idx='d'.strstreambuf::~~strstreambuf();
:eSNPL.
:SMTICS.
The &fn. destroys the &obj. after discarding the &rsvarea.:PERIOD.
The &rsvarea. is
discarded only if the &obj. is using dynamic allocation and is not frozen.
The &rsvarea. is freed using the free function specified by the form of the
constructor that allows specification of the allocate and free functions,
or using the
:MONO.operator~bdelete
intrinsic function.
If the &obj. is frozen or using static allocation, the user of the &obj.
must have a pointer to the &rsvarea. and is responsible for freeing it.
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
:SNPFLF           .#include <strstrea.h>
:SNPFLF           .public:
:SNPF index='sync'.virtual int strstreambuf::sync();
:eSNPL.
:SMTICS.
The &fn. does nothing because there is no external device with which to
synchronize.
:RSLTS.
The &fn. returns &noteof.:PERIOD.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public virtual'.underflow
:SNPL.
:SNPFLF                .#include <strstrea.h>
:SNPFLF                .public:
:SNPF index='underflow'.virtual int strstreambuf::underflow();
:eSNPL.
:SMTICS.
The &fn. provides the input communication between the
:MONO.streambuf
member functions and the &obj.:PERIOD.
Member functions in the
:MONO.streambuf
class call the &fn. when the &getarea. is empty.
:P.
If there is a non-empty &putarea. present following the &getarea.,
the &getarea. is extended to include the &putarea., allowing the
input operation to continue using the &putarea.:PERIOD.
Otherwise the &getarea. cannot be extended.
:RSLTS.
The &fn. returns the first available character in the &getarea. on successful
extension, otherwise &eof. is returned.
:SALSO.
:SAL typ='ofun' ocls='streambuf'.underflow
:SAL typ='mfun'.overflow
:eSALSO.
:eLIBF.
