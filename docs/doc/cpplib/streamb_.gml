:CLFNM.streambuf
:CMT.========================================================================
:LIBF fmt='hdr'.streambuf
:HFILE.streambu.h
:DVBYL.
:DVBY.filebuf
:DVBY.stdiobuf
:DVBY.strstreambuf
:eDVBYL.
:CLSS.
The &cls. is responsible for maintaining the buffer used to create an
efficient implementation of the stream classes. Through its pure virtual
functions, it is also responsible for the actual communication with the
device associated with the stream.
:P.
The &cls. is abstract, due to the presence of pure virtual member functions.
Abstract classes may not be instantiated, only inherited.
Hence, &obj.s will not be created by user programs.
:P.
Stream objects maintain a pointer to an associated &obj. and present the
interface that the user deals with most often. Whenever a stream member
function wishes to read or write characters, it uses the
:MONO.rdbuf
member function to access the associated &obj. and its member functions.
Through judicious use of inline functions, most reads and writes of
characters access the buffer directly without even doing a function call.
Whenever the buffer gets filled (writing) or exhausted (reading), these
inline functions invoke the function required to rectify the situation
so that the proper action can take place.
:P.
:I1.reserve area
:I1.get area
:I1.put area
A &obj. can be unbuffered, but most often has one buffer which can be used
for both input and output operations. The buffer (called the &rsvarea.)
is divided into two areas, called the &getarea. and the &putarea.:PERIOD.
For a &obj. being used exclusively to write, the &getarea. is empty or not
present. Likewise, a &obj. being used exclusively for reading has an empty or
non-existent &putarea.:PERIOD.
:P.
The use of the &getarea. and &putarea. differs among the various classes
derived from the &cls.:PERIOD.
:INCLUDE file='flb_desc'.
:INCLUDE file='std_desc'.
:INCLUDE file='ssb_desc'.
:P.
The &rsvarea. is marked by two pointer values. The
:MONO.base
member function returns the pointer to the start of the buffer. The
:MONO.ebuf
member function returns the pointer to the end of the buffer
(last character + 1). The
:MONO.setb
protected member function is used to set both pointers.
:P.
Within the &rsvarea., the &getarea. is marked by three pointer values. The
:MONO.eback
member function returns a pointer to the start of the &getarea.:PERIOD.
The
:MONO.egptr
member function returns a pointer to the end of the &getarea.
(last character + 1). The
:MONO.gptr
member function returns the &getptr.:PERIOD.
The &getptr. is a pointer to the next character to be extracted from the
&getarea.:PERIOD.
Characters before the &getptr.
have already been consumed by the program, while characters at and after the
&getptr. have been read from their source and are buffered and waiting to be
read by the program. The
:MONO.setg
member function is used to set all three pointer values. If any of these
pointers are &null., there is no &getarea.:PERIOD.
:P.
Also within the &rsvarea., the &putarea. is marked by three pointer values.
The
:MONO.pbase
member function returns a pointer to the start of the &putarea.:PERIOD.
The
:MONO.epptr
member function returns a pointer to the end of the &putarea. (last
character + 1 ).
The
:MONO.pptr
member function returns the &putptr.:PERIOD.
The &putptr. is a pointer to the next available position into which a
character may be stored.
Characters before the &putptr. are buffered and waiting to be written to their
final destination, while character positions at and after the &putptr. have
yet to be written by the program. The
:MONO.setp
member function is used to set all three pointer values. If any of these
pointers are &null., there is no &putarea.:PERIOD.
:P.
Unbuffered I/O is also possible. If unbuffered, the
:MONO.overflow
virtual member function is used to write single characters directly to their
final destination without using the &putarea.:PERIOD.
Similarly, the
:MONO.underflow
virtual member function is used to read single characters directly from their
source without using the &getarea.:PERIOD.
:HDG.Protected Member Functions
The following member functions are declared in the protected interface:
:MFNL.
:MFCD cd_idx='c'.streambuf();
:MFCD cd_idx='c'.streambuf( char *, int );
:MFCD cd_idx='d' .virtual ~~streambuf();
:MFN index='allocate'     .int allocate();
:MFN index='base'         .char *base() const;
:MFN index='ebuf'         .char *ebuf() const;
:MFN index='blen'         .int blen() const;
:MFN index='setb'         .void setb( char *, char *, int );
:MFN index='eback'        .char *eback() const;
:MFN index='gptr'         .char *gptr() const;
:MFN index='egptr'        .char *egptr() const;
:MFN index='gbump'        .void gbump( streamoff );
:MFN index='setg'         .void setg( char *, char *, char *);
:MFN index='pbase'        .char *pbase() const;
:MFN index='pptr'         .char *pptr() const;
:MFN index='epptr'        .char *epptr() const;
:MFN index='pbump'        .void pbump( streamoff );
:MFN index='setp'         .void setp( char *, char *);
:MFN index='unbuffered'   .int unbuffered( int );
:MFN index='unbuffered'   .int unbuffered() const;
:MFN index='doallocate'   .virtual int doallocate();
:eMFNL.
:HDG.Public Member Functions
The following member functions are declared in the public interface:
:MFNL.
:MFN index='in_avail'   .int in_avail() const;
:MFN index='out_waiting'.int out_waiting() const;
:MFN index='snextc'     .int snextc();
:MFN index='sgetn'      .int sgetn( char *, int );
:MFN index='speekc'     .int speekc();
:MFN index='sgetc'      .int sgetc();
:MFN index='sgetchar'   .int sgetchar();
:MFN index='sbumpc'     .int sbumpc();
:MFN index='stossc'     .void stossc();
:MFN index='sputbackc'  .int sputbackc( char );
:MFN index='sputc'      .int sputc( int );
:MFN index='sputn'      .int sputn( char const *, int );
:MFN index='dbp'        .void dbp();
:eMFNL.
:MFNL.
:MFN index='do_sgetn'   .virtual int do_sgetn( char *, int );
:MFN index='do_sputn'   .virtual int do_sputn( char const *, int );
:MFN index='pbackfail'  .virtual int pbackfail( int );
:MFN index='overflow'   .virtual int overflow( int = EOF ) = 0;
:MFN index='underflow'  .virtual int underflow() = 0;
:MFN index='setbuf'     .virtual streambuf *setbuf( char *, int );
:MFN index='seekoff'    .virtual streampos seekoff( streamoff, ios::seekdir,
:MFNFLF                 .                           ios::openmode = ios::in|ios::out );
:MFN index='seekpos'    .virtual streampos seekpos( streampos,
:MFNFLF                 .                           ios::openmode = ios::in|ios::out );
:MFN index='sync'       .virtual int sync();
:eMFNL.
:eCLSS.
:SALSO.
:SAL typ='cls'.filebuf
:SAL typ='cls'.stdiobuf
:SAL typ='cls'.strstreambuf
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='protected'.allocate
:SNPL.
:SNPFLF               .#include <streambu.h>
:SNPFLF               .protected:
:SNPF index='allocate'.int streambuf::allocate();
:eSNPL.
:SMTICS.
The &fn. works in tandem with the
:MONO.doallocate
protected virtual member function to manage allocation of the &obj.
&rsvarea.:PERIOD.
Classes derived from the &cls. should call the &fn., rather than the
:MONO.doallocate
protected virtual member function. The &fn. determines whether or not the &obj.
is allowed to allocate a buffer for use as the &rsvarea.:PERIOD.
If a &rsvarea.
already exists or if the &obj. unbuffering state is non-zero, the &fn.
fails. Otherwise, it calls the
:MONO.doallocate
protected virtual member function.
:RSLTS.
The &fn. returns &noteof. on success, otherwise &eof. is returned.
:SALSO.
:SAL typ='mfun'.doallocate
:SAL typ='mfun'.underflow
:SAL typ='mfun'.overflow
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='protected'.base
:SNPL.
:SNPFLF           .#include <streambu.h>
:SNPFLF           .protected:
:SNPF index='base'.char *streambuf::base() const;
:eSNPL.
:SMTICS.
The &fn. returns a pointer to the start of the &rsvarea. that the &obj.
is using.
:INCLUDE file='sb_rgp'.
:RSLTS.
The &fn. returns a pointer to the start of the &rsvarea. that the &obj.
is using. If the &obj. currently does not have a &rsvarea., &null. is
returned.
:SALSO.
:SAL typ='mfun'.blen
:SAL typ='mfun'.ebuf
:SAL typ='mfun'.setb
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='protected'.blen
:SNPL.
:SNPFLF           .#include <streambu.h>
:SNPFLF           .protected:
:SNPF index='blen'.int streambuf::blen() const;
:eSNPL.
:SMTICS.
The &fn. reports the length of the &rsvarea. that the &obj. is using.
:INCLUDE file='sb_rgp'.
:RSLTS.
The &fn. returns the length of the &rsvarea. that the &obj. is using.
If the &obj. currently does not have a &rsvarea., zero is returned.
:SALSO.
:SAL typ='mfun'.base
:SAL typ='mfun'.ebuf
:SAL typ='mfun'.setb
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.dbp
:SNPL.
:SNPFLF          .#include <streambu.h>
:SNPFLF          .public:
:SNPF index='dbp'.void streambuf::dbp();
:eSNPL.
:SMTICS.
The &fn. dumps information about the &obj. directly to
:MONO.stdout
:CONT., and is used for
debugging classes derived from the &cls.:PERIOD.
:P.
The following is an example of what the &fn. dumps:
:eLIBF.
:XMP.
STREAMBUF Debug Info:
this  = 00030679, unbuffered = 0, delete_reserve = 1
base  = 00070010, ebuf = 00070094
eback = 00000000, gptr = 00000000, egptr = 00000000
pbase = 00070010, pptr = 00070010, epptr = 00070094
:eXMP.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public virtual'.do_sgetn
:SNPL.
:SNPFLF               .#include <streambu.h>
:SNPFLF               .public:
:SNPF index='do_sgetn'.virtual int do_sgetn( char *buf, int len );
:eSNPL.
:SMTICS.
The &fn. works in tandem with the
:MONO.sgetn
member function to transfer
:ARG.len
characters from the &getarea. into
:ARG.buf
:PERIOD.
:P.
Classes derived from the &cls. should call the
:MONO.sgetn
member function, rather than the &fn.:PERIOD.
:PRTCL.
Classes derived from the &cls. that implement the &fn. should support copying
up to
:ARG.len
characters from the source through the &getarea. and into
:ARG.buf
:PERIOD.
:DEFIMPL.
The default &fn. provided with the &cls. calls the
:MONO.underflow
virtual member function to fetch more characters and then copies
the characters from the &getarea. into
:ARG.buf
:PERIOD.
:RSLTS.
The &fn. returns the number of characters successfully transferred.
:SALSO.
:SAL typ='mfun'.sgetn
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public virtual'.do_sputn
:SNPL.
:SNPFLF               .#include <streambu.h>
:SNPFLF               .public:
:SNPF index='do_sputn'.virtual int do_sputn( char const *buf, int len );
:eSNPL.
:SMTICS.
The &fn. works in tandem with the
:MONO.sputn
member function to transfer
:ARG.len
characters from
:ARG.buf
to the end of the &putarea. and advances the &putptr.:PERIOD.
:P.
Classes derived from the &cls. should call the
:MONO.sputn
member function, rather than the &fn.:PERIOD.
:PRTCL.
Classes derived from the &cls. that implement the &fn. should support copying
up to
:ARG.len
characters from
:ARG.buf
through the &putarea. and out to the destination device.
:DEFIMPL.
The default &fn. provided with the &cls. calls the
:MONO.overflow
virtual member function to flush the &putarea. and then copies the rest of
the characters from
:ARG.buf
into the &putarea.:PERIOD.
:RSLTS.
The &fn. returns the number of characters successfully written.
If an error occurs, this number may be less than
:ARG.len
:PERIOD.
:SALSO.
:SAL typ='mfun'.sputn
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='protected virtual'.doallocate
:SNPL.
:SNPFLF                 .#include <streambu.h>
:SNPFLF                 .protected:
:SNPF index='doallocate'.virtual int streambuf::doallocate();
:eSNPL.
:SMTICS.
The &fn. manages allocation of the &obj.'s &rsvarea. in tandem with the
:MONO.allocate
protected member function.
:P.
Classes derived from the &cls. should call the
:MONO.allocate
protected member function rather than the &fn.:PERIOD.
:P.
The &fn. does the actual memory allocation, and can be defined for each
class derived from the &cls.:PERIOD.
:PRTCL.
Classes derived from the &cls. should implement the &fn. such that it does
the following:
:OL.
:LI.attempts to allocate an area of memory,
:LI.calls the
:MONO.setb
protected member function to initialize the &rsvarea. pointers,
:LI.performs any class specific operations required.
:eOL.
:DEFIMPL.
The default &fn. provided with the &cls. attempts to allocate a buffer area
with the
:MONO.operator~bnew
intrinsic function. It then calls the
:MONO.setb
protected member function to set up the pointers to the &rsvarea.:PERIOD.
:RSLTS.
The &fn. returns &noteof. on success, otherwise &eof. is returned.
:SALSO.
:SAL typ='mfun'.allocate
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='protected'.eback
:SNPL.
:SNPFLF            .#include <streambu.h>
:SNPFLF            .protected:
:SNPF index='eback'.char *streambuf::eback() const;
:eSNPL.
:SMTICS.
The &fn. returns a pointer to the start of the &getarea. within the &rsvarea.
used by the &obj.:PERIOD.
:INCLUDE file='sb_rgp'.
:RSLTS.
The &fn. returns a pointer to the start of the &getarea.:PERIOD.
If the &obj.
currently does not have a &getarea., &null. is returned.
:SALSO.
:SAL typ='mfun'.egptr
:SAL typ='mfun'.gptr
:SAL typ='mfun'.setg
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='protected'.ebuf
:SNPL.
:SNPFLF           .#include <streambu.h>
:SNPFLF           .protected:
:SNPF index='ebuf'.char *streambuf::ebuf() const;
:eSNPL.
:SMTICS.
The &fn. returns a pointer to the end of the &rsvarea. that the &obj.
is using.  The character pointed at is actually the first character past
the end of the &rsvarea.:PERIOD.
:INCLUDE file='sb_rgp'.
:RSLTS.
The &fn. returns a pointer to the end of the &rsvarea.:PERIOD.
If the &obj. currently does not have a &rsvarea., &null. is returned.
:SALSO.
:SAL typ='mfun'.base
:SAL typ='mfun'.blen
:SAL typ='mfun'.setb
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='protected'.egptr
:SNPL.
:SNPFLF            .#include <streambu.h>
:SNPFLF            .protected:
:SNPF index='egptr'.char *streambuf::egptr() const;
:eSNPL.
:SMTICS.
The &fn. returns a pointer to the end of the &getarea. within the &rsvarea.
used by the &obj.:PERIOD.
The character pointed at is actually the first character
past the end of the &getarea.:PERIOD.
:INCLUDE file='sb_rgp'.
:RSLTS.
The &fn. returns a pointer to the end of the &getarea.:PERIOD.
If the &obj.
currently does not have a &getarea., &null. is returned.
:SALSO.
:SAL typ='mfun'.eback
:SAL typ='mfun'.gptr
:SAL typ='mfun'.setg
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='protected'.epptr
:SNPL.
:SNPFLF            .#include <streambu.h>
:SNPFLF            .protected:
:SNPF index='epptr'.char *streambuf::epptr() const;
:eSNPL.
:SMTICS.
The &fn. returns a pointer to the end of the &putarea. within the &rsvarea.
used by the &obj.:PERIOD.
The character pointed at is actually the first character
past the end of the &putarea.:PERIOD.
:INCLUDE file='sb_rgp'.
:RSLTS.
The &fn. returns a pointer to the end of the &putarea.:PERIOD.
If the &obj.
currently does not have a &putarea., &null. is returned.
:SALSO.
:SAL typ='mfun'.pbase
:SAL typ='mfun'.pptr
:SAL typ='mfun'.setp
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='protected'.gbump
:SNPL.
:SNPFLF            .#include <streambu.h>
:SNPFLF            .protected:
:SNPF index='gbump'.void streambuf::gbump( streamoff offset );
:eSNPL.
:SMTICS.
The &fn. increments the &getptr. by the specified
:ARG.offset
:CONT., without regard for the boundaries of the &getarea.:PERIOD.
The
:ARG.offset
parameter may be positive or negative.
:RSLTS.
The &fn. returns nothing.
:SALSO.
:SAL typ='mfun'.gptr
:SAL typ='mfun'.pbump
:SAL typ='mfun'.sbumpc
:SAL typ='mfun'.sputbackc
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='protected'.gptr
:SNPL.
:SNPFLF           .#include <streambu.h>
:SNPFLF           .protected:
:SNPF index='gptr'.char *streambuf::gptr() const;
:eSNPL.
:SMTICS.
The &fn. returns a pointer to the next available character in the &getarea.
within the &rsvarea. used by the &obj.:PERIOD.
This pointer is called the &getptr.:PERIOD.
:I1.get pointer
:P.
If the &getptr. points beyond the end of the &getarea., all characters in
the &getarea. have been read by the program and a subsequent read causes the
:MONO.underflow
virtual member function to be called to fetch more characters from the source
to which the &obj. is attached.
:INCLUDE file='sb_rgp'.
:RSLTS.
The &fn. returns a pointer to the next available character in the &getarea.:PERIOD.
If the &obj. currently does not have a &getarea., &null. is returned.
:SALSO.
:SAL typ='mfun'.eback
:SAL typ='mfun'.egptr
:SAL typ='mfun'.setg
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.in_avail
:SNPL.
:SNPFLF               .#include <streambu.h>
:SNPFLF               .public:
:SNPF index='in_avail'.int streambuf::in_avail() const;
:eSNPL.
:SMTICS.
The &fn. computes the number of input characters buffered in the &getarea.
that have not yet been read by the program.
These characters can be read with a guarantee that no errors will occur.
:RSLTS.
The &fn. returns the number of buffered input characters.
:SALSO.
:SAL typ='mfun'.egptr
:SAL typ='mfun'.gptr
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.out_waiting
:SNPL.
:SNPFLF                  .#include <streambu.h>
:SNPFLF                  .public:
:SNPF index='out_waiting'.int streambuf::out_waiting() const;
:eSNPL.
:SMTICS.
The &fn. computes the number of characters that have been buffered in the
&putarea. and not yet been written to the output device.
:RSLTS.
The &fn. returns the number of buffered output characters.
:SALSO.
:SAL typ='mfun'.pbase
:SAL typ='mfun'.pptr
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public virtual'.overflow
:SNPL.
:SNPFLF               .#include <streambu.h>
:SNPFLF               .public:
:SNPF index='overflow'.virtual int streambuf::overflow( int ch = EOF ) = 0;
:eSNPL.
:SMTICS.
The &fn. is used to flush the &putarea. when it is full.
:PRTCL.
Classes derived from the &cls. should implement the &fn. so that it performs
the following:
:OL.
:LI.if no &rsvarea. is present and the &obj. is not unbuffered, allocate a
&rsvarea. using the
:MONO.allocate
member function and set up the &rsvarea. pointers using the
:MONO.setb
protected member function,
:LI.flush any other uses of the &rsvarea.,
:LI.write any characters in the &putarea. to the &obj.'s destination,
:LI.set up the &putarea. pointers to reflect the characters that were written,
:LI.return &noteof. on success, otherwise return &eof.:PERIOD.
:eOL.
:DEFIMPL.
There is no default &cls. implementation of the &fn.:PERIOD.
The &fn. must be defined for all classes derived from the &cls.:PERIOD.
:RSLTS.
The &fn. returns &noteof. on success, otherwise &eof. is returned.
:SALSO.
:SAL typ='ofun' ocls='filebuf'.overflow
:SAL typ='ofun' ocls='stdiobuf'.overflow
:SAL typ='ofun' ocls='strstreambuf'.overflow
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public virtual'.pbackfail
:SNPL.
:SNPFLF                .#include <streambu.h>
:SNPFLF                .public:
:SNPF index='pbackfail'.virtual int streambuf::pbackfail( int ch );
:eSNPL.
:SMTICS.
The &fn. is called by the
:MONO.sputbackc
member function when the &getptr. is at the beginning of the &getarea.,
and so there is no place to put the
:ARG.ch
parameter.
:PRTCL.
Classes derived from the &cls. should implement the &fn. such that it attempts
to put
:ARG.ch
back into the source of the stream.
:DEFIMPL.
The default &cls. implementation of the &fn. is to return &eof.:PERIOD.
:RSLTS.
If the &fn. succeeds, it returns
:ARG.ch
:PERIOD.
Otherwise, &eof. is returned.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='protected'.pbase
:SNPL.
:SNPFLF            .#include <streambu.h>
:SNPFLF            .protected:
:SNPF index='pbase'.char *streambuf::pbase() const;
:eSNPL.
:SMTICS.
The &fn. returns a pointer to the start of the &putarea. within the &rsvarea.
used by the &obj.:PERIOD.
:INCLUDE file='sb_rgp'.
:RSLTS.
The &fn. returns a pointer to the start of the &putarea.:PERIOD.
If the &obj.
currently does not have a &putarea., &null. is returned.
:SALSO.
:SAL typ='mfun'.epptr
:SAL typ='mfun'.pptr
:SAL typ='mfun'.setp
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='protected'.pbump
:SNPL.
:SNPFLF            .#include <streambu.h>
:SNPFLF            .protected:
:SNPF index='pbump'.void streambuf::pbump( streamoff offset );
:eSNPL.
:SMTICS.
The &fn. increments the &putptr. by the specified
:ARG.offset
:CONT., without regard for the boundaries of the &putarea.:PERIOD.
The
:ARG.offset
parameter may be positive or negative.
:RSLTS.
The &fn. returns nothing.
:SALSO.
:SAL typ='mfun'.gbump
:SAL typ='mfun'.pbase
:SAL typ='mfun'.pptr
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='protected'.pptr
:SNPL.
:SNPFLF           .#include <streambu.h>
:SNPFLF           .protected:
:SNPF index='pptr'.char *streambuf::pptr() const;
:eSNPL.
:SMTICS.
The &fn. returns a pointer to the next available space in the &putarea.
within the &rsvarea. used by the &obj.:PERIOD.
This pointer is called the &putptr.:PERIOD.
:I1.put pointer
:P.
If the &putptr. points beyond the end of the &putarea., the &putarea.
is full and a subsequent write causes the
:MONO.overflow
virtual member function to be called to empty the &putarea. to the device
to which the &obj. is attached.
:INCLUDE file='sb_rgp'.
:RSLTS.
The &fn. returns a pointer to the next available space in the &putarea.:PERIOD.
If the &obj. currently does not have a &putarea., &null. is returned.
:SALSO.
:SAL typ='mfun'.epptr
:SAL typ='mfun'.pbase
:SAL typ='mfun'.setp
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.sbumpc
:SNPL.
:SNPFLF               .#include <streambu.h>
:SNPFLF               .public:
:SNPF index='sbumpc'.int streambuf::sbumpc();
:eSNPL.
:SMTICS.
:INCLUDE file='sb_sbmpc'.
:SALSO.
:SAL typ='mfun'.gbump
:SAL typ='mfun'.sgetc
:SAL typ='mfun'.sgetchar
:SAL typ='mfun'.sgetn
:SAL typ='mfun'.snextc
:SAL typ='mfun'.sputbackc
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public virtual'.seekoff
:SNPL.
:SNPFLF              .#include <streambu.h>
:SNPFLF              .public:
:SNPF index='seekoff'.virtual streampos streambuf::seekoff( streamoff offset,
:SNPFLF              .                                      ios::seekdir dir,
:SNPFLF              .                                      ios::openmode mode );
:eSNPL.
:SMTICS.
The &fn. is used for positioning to a relative location within the &obj.,
and hence within the device that is connected to the &obj.:PERIOD.
The
:ARG.offset
and
:ARG.dir
parameters specify the relative change in position.  The
:ARG.mode
parameter controls whether the &getptr. and/or the &putptr. are repositioned.
:PRTCL.
Classes derived from the &cls. should implement the
:MONO.seekoff
virtual member function so that it uses its parameters in the following way.
:INCLUDE file='seekmode'.
:INCLUDE file='seekdir'.
:DEFIMPL.
The default implementation of the &fn. provided by the &cls. returns
&eof.:PERIOD.
:RSLTS.
The &fn. returns the new position in the stream on success, otherwise &eof.
is returned.
:SALSO.
:SAL typ='mfun'.seekpos
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public virtual'.seekpos
:SNPL.
:SNPFLF              .#include <streambu.h>
:SNPFLF              .public:
:SNPF index='seekpos'.virtual streampos streambuf::seekpos( streampos pos,
:SNPFLF              .                    ios::openmode mode = ios::in|ios::out );
:eSNPL.
:SMTICS.
The &fn. is used for positioning to an absolute location within the &obj.,
and hence within the device that is connected to the &obj.:PERIOD.
The
:ARG.pos
parameter specifies the absolute position.  The
:ARG.mode
parameter controls whether the &getptr. and/or the &putptr. are repositioned.
:PRTCL.
Classes derived from the &cls. should implement the &fn.
so that it uses its parameters in the following way.
:INCLUDE file='seekmode'.
:P.
In general the &fn. is equivalent to calling the
:MONO.seekoff
virtual member function with the offset set to
:ARG.pos
:CONT.,
the direction set to
:MONO.ios::beg
and the mode set to
:ARG.mode
:PERIOD.
:DEFIMPL.
The default implementation of the &fn. provided by the &cls. calls the
:MONO.seekoff
virtual member function with the offset set to
:ARG.pos
:CONT.,
the direction set to
:MONO.ios::beg
:CONT., and the mode set to
:ARG.mode
:PERIOD.
:RSLTS.
The &fn. returns the new position in the stream on success, otherwise &eof.
is returned.
:SALSO.
:SAL typ='mfun'.seekoff
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='protected'.setb
:SNPL.
:SNPFLF           .#include <streambu.h>
:SNPFLF           .protected:
:SNPF index='setb'.void streambuf::setb( char *base, char *ebuf, int autodel );
:eSNPL.
:SMTICS.
The &fn. is used to set the pointers to the &rsvarea. that the &obj. is using.
:P.
The
:ARG.base
parameter is a pointer to the start of the &rsvarea. and
corresponds to the value that the
:MONO.base
member function returns.
:P.
The
:ARG.ebuf
parameter is a pointer to the end of the &rsvarea. and corresponds to the
value that the
:MONO.ebuf
member function returns.
:P.
The
:ARG.autodel
parameter indicates whether or not the &obj. can free the &rsvarea. when the
&obj. is destroyed or when a new &rsvarea. is set up in a subsequent call to
the &fn.:PERIOD.
If the
:ARG.autodel
parameter is non-zero, the &obj. can delete the &rsvarea., using the
:MONO.operator~bdelete
intrinsic function. Otherwise, a zero value indicates that the buffer will be
deleted elsewhere.
:P.
If either of the
:ARG.base
or
:ARG.ebuf
parameters are &null. or if
:ARG.ebuf
<=
:ARG.base
:CONT., the &obj. does not have a buffer and input/output operations
are unbuffered, unless another buffer is set up.
:P.
Note that the &fn. is used to set the &rsvarea. pointers, while the
:MONO.setbuf
protected member function is used to offer a buffer to the &obj.:PERIOD.
:SALSO.
:SAL typ='mfun'.base
:SAL typ='mfun'.blen
:SAL typ='mfun'.ebuf
:SAL typ='mfun'.setbuf
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public virtual'.setbuf
:SNPL.
:SNPFLF             .#include <streambu.h>
:SNPFLF             .public:
:SNPF index='setbuf'.virtual streambuf *streambuf::setbuf( char *buf, int len );
:eSNPL.
:SMTICS.
The &fn. is used to offer a buffer specified by the
:ARG.buf
and
:ARG.len
parameters to the &obj. for use as its &rsvarea.:PERIOD.
Note that the &fn. is used to offer a buffer, while the
:MONO.setb
protected member function is used to set the &rsvarea. pointers once a
buffer has been accepted.
:PRTCL.
Classes derived from the &cls. may implement the &fn. if the default behavior
is not suitable.
:P.
Derived classes that provide their own implementations of the &fn.
may accept or reject the offered buffer. Often, if a buffer is already
allocated, the offered buffer is rejected, as it may be difficult to
transfer the information from the current buffer.
:DEFIMPL.
The default &fn. provided by the &cls. rejects the buffer if one is already
present.
:P.
If no buffer is present and either
:ARG.buf
is &null. or
:ARG.len
is zero, the offer is accepted and the &obj. is unbuffered.
:P.
Otherwise, no buffer is present and one is specified.
If
:ARG.len
is less than five characters the buffer is too small and it is rejected.
Otherwise, the buffer is accepted.
:RSLTS.
The &fn. returns the address of the &obj. if the offered buffer is accepted,
otherwise &null. is returned.
:SALSO.
:SAL typ='mfun'.setb
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='protected'.setg
:SNPL.
:SNPFLF           .#include <streambu.h>
:SNPFLF           .protected:
:SNPF index='setg'.void streambuf::setg( char *eback, char *gptr, char *egptr );
:eSNPL.
:SMTICS.
The &fn. is used to set the three &getarea. pointers.
:P.
The
:ARG.eback
parameter is a pointer to the start of the &getarea.
and corresponds to the value that the
:MONO.eback
member function returns.
:P.
The
:ARG.gptr
parameter is a pointer to the first available character in the &getarea.,
that is, the &getptr., and usually is greater than the
:MONO.eback
parameter in order to accommodate a putback area.
The
:ARG.gptr
parameter corresponds to the value that the
:MONO.gptr
member function returns.
:P.
The
:ARG.egptr
parameter is a pointer to the end of the &getarea.
and corresponds to the value that the
:MONO.egptr
member function returns.
:P.
If any of the three parameters are &null., there is no &getarea.:PERIOD.
:SALSO.
:SAL typ='mfun'.eback
:SAL typ='mfun'.egptr
:SAL typ='mfun'.gptr
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='protected'.setp
:SNPL.
:SNPFLF           .#include <streambu.h>
:SNPFLF           .protected:
:SNPF index='setp'.void streambuf::setp( char *pbase, char *epptr );
:eSNPL.
:SMTICS.
The &fn. is used to set the three &putarea. pointers.
:P.
The
:ARG.pbase
parameter is a pointer to the start of the &putarea. and corresponds to the
value that the
:MONO.pbase
member function returns.
:P.
The
:ARG.epptr
parameter is a pointer to the end of the &putarea.
and corresponds to the value that the
:MONO.epptr
member function returns.
:P.
The &putptr. is set to the
:ARG.pbase
parameter value and corresponds to the value that the
:MONO.pptr
member function returns.
:P.
If either parameter is &null., there is no &putarea.:PERIOD.
:SALSO.
:SAL typ='mfun'.epptr
:SAL typ='mfun'.pbase
:SAL typ='mfun'.pptr
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.sgetc
:SNPL.
:SNPFLF            .#include <streambu.h>
:SNPFLF            .public:
:SNPF index='sgetc'.int streambuf::sgetc();
:eSNPL.
:SMTICS.
:INCLUDE file='sb_spkc'.
:SALSO.
:SAL typ='mfun'.sbumpc
:SAL typ='mfun'.sgetchar
:SAL typ='mfun'.sgetn
:SAL typ='mfun'.snextc
:SAL typ='mfun'.speekc
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.sgetchar
:SNPL.
:SNPFLF               .#include <streambu.h>
:SNPFLF               .public:
:SNPF index='sgetchar'.int streambuf::sgetchar();
:eSNPL.
:SMTICS.
:INCLUDE file='sb_sbmpc'.
:SALSO.
:SAL typ='mfun'.gbump
:SAL typ='mfun'.sgetc
:SAL typ='mfun'.sgetchar
:SAL typ='mfun'.sgetn
:SAL typ='mfun'.snextc
:SAL typ='mfun'.speekc
:SAL typ='mfun'.sputbackc
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.sgetn
:SNPL.
:SNPFLF            .#include <streambu.h>
:SNPFLF            .public:
:SNPF index='sgetn'.int streambuf::sgetn( char *buf, int len );
:eSNPL.
:SMTICS.
The &fn. transfers up to
:ARG.len
characters from the &getarea. into
:ARG.buf
:PERIOD.
If there are not enough characters in the &getarea., the
:MONO.do_sgetn
virtual member function is called to fetch more.
:P.
Classes derived from the &cls. should call the &fn., rather than the
:MONO.do_sgetn
virtual member function.
:RSLTS.
The &fn. returns the number of characters transferred from the &getarea. into
:ARG.buf
:PERIOD.
:SALSO.
:SAL typ='mfun'.do_sgetn
:SAL typ='mfun'.sbumpc
:SAL typ='mfun'.sgetc
:SAL typ='mfun'.sgetchar
:SAL typ='mfun'.speekc
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.snextc
:SNPL.
:SNPFLF             .#include <streambu.h>
:SNPFLF             .public:
:SNPF index='snextc'.int streambuf::snextc();
:eSNPL.
:SMTICS.
The &fn. advances the &getptr. and then returns the character following the
&getptr.:PERIOD.
The &getptr. is left pointing at the returned character.
:P.
If the &getptr. cannot be advanced, the
:MONO.underflow
virtual member function is called to fetch more characters from the source
into the &getarea.:PERIOD.
:RSLTS.
The &fn. advances the &getptr. and returns the next available character in the
&getarea.:PERIOD.
If there is no next available character, &eof. is returned.
:SALSO.
:SAL typ='mfun'.sbumpc
:SAL typ='mfun'.sgetc
:SAL typ='mfun'.sgetchar
:SAL typ='mfun'.sgetn
:SAL typ='mfun'.speekc
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.speekc
:SNPL.
:SNPFLF             .#include <streambu.h>
:SNPFLF             .public:
:SNPF index='speekc'.int streambuf::speekc();
:eSNPL.
:SMTICS.
:INCLUDE file='sb_spkc'.
:SALSO.
:SAL typ='mfun'.sbumpc
:SAL typ='mfun'.sgetc
:SAL typ='mfun'.sgetchar
:SAL typ='mfun'.sgetn
:SAL typ='mfun'.snextc
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.sputbackc
:SNPL.
:SNPFLF                .#include <streambu.h>
:SNPFLF                .public:
:SNPF index='sputbackc'.int streambuf::sputbackc( char ch );
:eSNPL.
:SMTICS.
The &fn. is used to put a character back into the &getarea.:PERIOD.
The
:ARG.ch
character specified must be the same as the character before the &getptr.,
otherwise the behavior is undefined.
The &getptr. is backed up by one position. At least four characters may be put
back without any intervening reads.
:RSLTS.
The &fn. returns
:ARG.ch
on success, otherwise &eof. is returned.
:SALSO.
:SAL typ='mfun'.gbump
:SAL typ='mfun'.sbumpc
:SAL typ='mfun'.sgetchar
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.sputc
:SNPL.
:SNPFLF            .#include <streambu.h>
:SNPFLF            .public:
:SNPF index='sputc'.int streambuf::sputc( int ch );
:eSNPL.
:SMTICS.
The &fn. adds the character
:ARG.ch
to the end of the &putarea. and advances the &putptr.:PERIOD.
If the &putarea. is full before the character is added, the
:MONO.overflow
virtual member function is called to empty the &putarea. and write the
character.
:RSLTS.
The &fn. returns
:ARG.ch
on success, otherwise &eof. is returned.
:SALSO.
:SAL typ='mfun'.sgetc
:SAL typ='mfun'.sputn
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.sputn
:SNPL.
:SNPFLF            .#include <streambu.h>
:SNPFLF            .public:
:SNPF index='sputn'.int streambuf::sputn( char const *buf, int len );
:eSNPL.
:SMTICS.
The &fn. transfers up to
:ARG.len
characters from
:ARG.buf
to the end of the &putarea. and advance the &putptr.:PERIOD.
If the &putarea. is full or becomes full and more characters are to be
written, the
:MONO.do_sputn
virtual member function is called to empty the &putarea. and finish writing
the characters.
:P.
Classes derived from the &cls. should call the &fn., rather than the
:MONO.do_sputn
virtual member function.
:RSLTS.
The &fn. returns the number of characters successfully written.
If an error occurs, this number may be less than
:ARG.len
:PERIOD.
:SALSO.
:SAL typ='mfun'.do_sputn
:SAL typ='mfun'.sputc
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.stossc
:SNPL.
:SNPFLF             .#include <streambu.h>
:SNPFLF             .public:
:SNPF index='stossc'.void streambuf::stossc();
:eSNPL.
:SMTICS.
The &fn. advances the &getptr. by one without returning a character.
If the &getarea. is empty, the
:MONO.underflow
virtual member function is called to fetch more characters and then the
&getptr. is advanced.
:SALSO.
:SAL typ='mfun'.gbump
:SAL typ='mfun'.sbumpc
:SAL typ='mfun'.sgetchar
:SAL typ='mfun'.snextc
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='ctor' prot='protected'.streambuf
:SNPL.
:SNPFLF                    .#include <streambu.h>
:SNPFLF                    .protected:
:SNPCD cd_idx='c'.streambuf::streambuf();
:eSNPL.
:SMTICS.
This form of the &fn. creates an empty &obj. with all fields initialized to
zero. No &rsvarea. is yet allocated, but the &obj. is buffered unless a
subsequent call to the
:MONO.setbuf
or
:MONO.unbuffered
member functions dictate otherwise.
:RSLTS.
This form of the &fn. creates an initialized &obj. with no
associated &rsvarea.:PERIOD.
:SALSO.
:SAL typ='dtor'.
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='ctor' prot='protected'.streambuf
:SNPL.
:SNPFLF                    .#include <streambu.h>
:SNPFLF                    .protected:
:SNPCD cd_idx='c'.streambuf::streambuf( char *buf, int len );
:eSNPL.
:SMTICS.
This form of the &fn. creates an empty &obj. with all fields initialized to zero.
The
:ARG.buf
and
:ARG.len
parameters are passed to the
:MONO.setbuf
member function, which sets up the buffer (if specified), or makes the
&obj. unbuffered (if the
:ARG.buf
parameter is &null. or the
:ARG.len
parameter is not positive).
:RSLTS.
This form of the &fn. creates an initialized &obj. with an associated &rsvarea.:PERIOD.
:SALSO.
:SAL typ='dtor'.
:SAL typ='mfun'.setbuf
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='dtor' prot='protected'.~~streambuf
:SNPL.
:SNPFLF                   .#include <streambu.h>
:SNPFLF                   .protected:
:SNPCD cd_idx='d'.virtual streambuf::~~streambuf();
:eSNPL.
:SMTICS.
The &obj. is destroyed.
If the buffer was allocated by the &obj., it is freed.
Otherwise, the buffer is not freed and must be freed by the user of the &obj.:PERIOD.
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
:SNPFLF           .#include <streambu.h>
:SNPFLF           .public:
:SNPF index='sync'.virtual int streambuf::sync();
:eSNPL.
:SMTICS.
The &fn. is used to synchronize the &obj.'s &getarea. and &putarea. with the
associated device.
:PRTCL.
Classes derived from the &cls. should implement the &fn. such that it attempts
to perform the following:
:OL.
:LI.flush the &putarea.,
:LI.discard the contents of the &getarea. and reposition the stream device so
that the discarded characters may be read again.
:eOL.
:DEFIMPL.
The default implementation of the &fn. provided by the &cls. takes no action.
It succeeds if the &getarea. and the &putarea. are empty, otherwise it fails.
:RSLTS.
The &fn. returns &noteof. on success, otherwise &eof. is returned.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='protected'.unbuffered
:SNPL.
:SNPFLF                 .#include <streambu.h>
:SNPFLF                 .protected:
:SNPF index='unbuffered'.int ios::unbuffered() const;
:SNPF index='unbuffered'.int ios::unbuffered( int unbuf );
:eSNPL.
:SMTICS.
The &fn. is used to query and/or set the unbuffering state of the &obj.:PERIOD.
A non-zero unbuffered state indicates that the &obj. is unbuffered.  An
unbuffered state of zero indicates that the &obj. is buffered.
:P.
The first form of the &fn. is used to query the current unbuffering state.
:P.
The second form of the &fn. is used to set the unbuffering state to
:ARG.unbuf
:PERIOD.
:P.
Note that the unbuffering state only affects the
:MONO.allocate
protected member function, which does nothing if the unbuffering state is
non-zero. Setting the unbuffering state to a non-zero value does not mean
that future I/O operations will be unbuffered.
:P.
To determine if current I/O operations are unbuffered, use the
:MONO.base
protected member function. A return value of &null. from the
:MONO.base
protected member function indicates that unbuffered I/O operations will be
used.
:RSLTS.
The &fn. returns the previous unbuffered state.
:SALSO.
:SAL typ='mfun'.allocate
:SAL typ='mfun'.pbase
:SAL typ='mfun'.setbuf
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public virtual'.underflow
:SNPL.
:SNPFLF                .#include <streambu.h>
:SNPFLF                .public:
:SNPF index='underflow'.virtual int streambuf::underflow() = 0;
:eSNPL.
:SMTICS.
The &fn. is used to fill the &getarea. when it is empty.
:PRTCL.
Classes derived from the &cls. should implement the &fn. so that it performs
the following:
:OL.
:LI.if no &rsvarea. is present and the &obj. is buffered, allocate the
&rsvarea. using the
:MONO.allocate
member function and set up the &rsvarea. pointers using the
:MONO.setb
protected member function,
:LI.flush any other uses of the &rsvarea.,
:LI.read some characters from the &obj.'s source into the &getarea.,
:LI.set up the &getarea. pointers to reflect the characters that were read,
:LI.return the first character of the &getarea., or &eof. if no characters
could be read.
:eOL.
:DEFIMPL.
There is no default &cls. implementation of the &fn.:PERIOD.
The &fn. must be defined for all classes derived from the &cls.:PERIOD.
:RSLTS.
The &fn. returns the first character read into the &getarea., or &eof.
if no characters could be read.
:SALSO.
:SAL typ='ofun' ocls='filebuf'.underflow
:SAL typ='ofun' ocls='stdiobuf'.underflow
:SAL typ='ofun' ocls='strstreambuf'.underflow
:eSALSO.
:eLIBF.
