.chap *refid=Introd1 Trap File Interface
The &company debugger consists of a number of separate pieces of code. The
main executable, WD.EXE (wd on UNIX systems), provides a debugging `engine'
and user interface.
When the engine wishes to perform an operation upon the program being
debugged such as reading memory or setting a breakpoint, it creates
a request structure and sends it to the `trap file' (so called because
under DOS, it contains the first level trap handlers). The trap file
examines the request structure, performs the indicated action and returns
a result structure to the debugger. The debugger and trap files also use
Machine Architecture Description (MAD) files which abstract the CPU
architecture.
This design has the following benefits:
:OL.
:LI. OS debugging interfaces tend to be wildly varying in how they are accessed.
By moving all the OS specific interface code into the trap file and having
a defined interface to access it, porting the debugger becomes much easier.
:LI. By abstracting the machine architecture specifics through MAD files, it
becomes possible to use one debugger for several target CPU architectures
(such as x86 and Alpha AXP). Unlike most other debuggers, the &company
debugger is not tied to a single host/target combination and if appropriate
trap and MAD files are available, the debugger running on any host can
remotely debug any target.
:LI. The trap file does not have to actually perform the operation. Instead
it could send the request out to a remote server by a communication link
such as a serial line or LAN. The remote server can retrieve the request,
perform the operation on the remote machine and send the results back
via the link. This enables the debugger to debug applications
in cases where there are memory constraints or other considerations which
prevent the debugger proper from running on the remote system (such as
Novell Netware 386).
:eOL.
:P.
This document describes the interface initially used by version 4.0 of the
WATCOM debugger (shipped with the 10.0 C/C++ and FORTRAN releases). It has
been revised to describe changes incorporated in Watcom 11.0 release, as
well as subsequent Open Watcom releases. It
is expected to be modified in future releases. Where possible, notification
of expected changes are given in the document, but all aspects are subject
to revision.
.section Some Definitions
.np
Next follow some general trap definitions.
.beglevel
.section Byte Order
.np
The trap file interface is defined to use little endian byte order. That is,
the least significant byte is stored at the lowest address. Little endian
byte order was chosen for compatibility with existing trap files and
tools. Fixed byte order also eases network communication between
debuggers and trap files running on machines with different byte order.
.section Pointer Sizes
.np
In a 16-bit hosted environment such as DOS, all pointers used by the trap
file are "far" 16:16 pointers. In a 32-bit environment such as Windows NT
the pointers are "near" 0:32 pointers.
.section Base Types
.np
A number of basic types are used in the interface. They are defined
as follows:
:DL.
:DTHD.Type
:DDHD.Definition
:DT.unsigned_8
:DD.1 byte unsigned quantity
:DT.unsigned_16
:DD.2 byte unsigned quantity
:DT.unsigned_32
:DD.4 byte unsigned quantity
:DT.access_req
:DD.The first field of every request is of this type. It is a 1 byte field
which identifies the request to be performed.
:DT.addr48_ptr
:DD.This type encapsulates the concept of a 16:32 pointer.
All addresses in the debuggee memory
are described with these. The debugger
always acts as if the debuggee were in a 32-bit large model environment since
the 32-bit flat model and all 16-bit memory models are subsets.
The structure is defined as follows:
:XMP.
    typedef struct {
        unsigned_32    offset;
        unsigned_16    segment;
    } addr48_ptr;
:eXMP.
The :F.segment:eF. field contains the segment of the address and the :F.offset:eF.
field stores the offset of the address.
:DT.bytes
:DD.The type :F.bytes:eF. is an array of unsigned_8.
The length is provided
by other means. Typically a field of type :F.bytes:eF. is the last one in
a request and the length is calculated from the total length of the request.
:DT.string
:DD.The type :F.string:eF. is actually an array of characters.
The array is terminated by a null ('\0') character.
The length is provided
by other means. Typically a field of type :F.string:eF. is the last one in
a request and the length is calculated from the total length of the request.
:DT.trap_error
:DD.
Some trap file requests return debuggee operating system error codes, notably
the requests to perform file I/O on the remote system. These error codes are
returned as an unsigned_32. The debugger considers the value zero to indicate
no error.
:DT.trap_phandle
:DD.This is an :F.unsigned_32:eF. which holds process (task) handle. A task
handle is used to uniquely identify a debuggee process.
:DT.trap_mhandle
:DD.This is an :F.unsigned_32:eF. which holds a module handle. Typically the
main executable will be one module, and on systems which support DLLs or
shared libraries, each library will be identified by a unique module handle.
:eDL.
.endlevel
.chap The Request Interface
Next follow detailed description of interface elements.
.section Request Structure.
.np
Each request is a composed of two sequences of bytes provided by the
debugger called messages. The first set contains the actual request
code and whatever
parameters that are required by the request. The second sequence is where
the result of the operation is to be stored by the trap file.
:P.
The two sequences need not be contiguous. The sequences are described
to the trap file through two arrays of message entry structures.
This allows the debugger to avoid unnecessary packing and unpacking of
messages, since :F.mx_entry:eF.'s can be set to point directly at
parameter/result buffers.
:P.
Multiple requests are :HP2.not:eHP2. allowed in a single message. The
:F.mx_entry:eF.'s are only used to provide scatter/gather capabilities for
one request at a time.
:P.
The message entry structure is as follows (defined in :F.trptypes.h:eF.):
:XMP.
    typedef struct {
        void            *ptr;
        unsigned        len;
    } mx_entry;
:eXMP.
:PC.
The :F.ptr:eF. is pointing to a block of data for that message entry.
The :F.len:eF. field gives the length of that block.
One array of :F.mx_entry:eF.'s describes the request message. The second array
describes the return message.
:P.
It is not legal to split a message into arbitrary pieces with mx_entries.
Each request documents where an :F.mx_entry:eF. is allowed to start with a
line of dashes.
.section The Interface Routines
.np
The trap file interface must provide three routines: :F.TrapInit:eF.,
:F.TrapRequest:eF., and :F.TrapFini:eF.. How the debugger determines the
address of these routines after loading a trap file, as well as the calling
convention used, is system dependent and described later. These functions are
prototyped in :F.trpimp.h:eF..
.beglevel
.section TrapInit
.np
This function initializes the environment for proper operation of
:F.TrapRequest:eF..
:XMP.
    trap_version TRAPENTRY TrapInit( char       *parm,
                                     char       *error,
                                     unsigned_8 remote
    );
:eXMP.
:PC.
The :F.parm:eF. is a string that the user passes to the trap file. Its
interpretation is completely up to the trap file. In the case of the
&company debugger,
all the characters following the semicolon in the :F./TRAP:eF. option are
passed as the :F.parm:eF.. For example:
:XMP.
    wd /trap=nov;testing program
:eXMP.
:P.
The :F.parm:eF. would be "testing".
Any error message
will be returned in :F.error:eF.. The :F.remote:eF. field
is a zero if the &company debugger is loading the trap file and a one if a
remote server is loading it.
This function returns a structure :F.trap_version:eF. of the following form
(defined in :F.trptypes.h:eF.):
:XMP.
    typedef struct {
        unsigned_8  major;
        unsigned_8  minor;
        unsigned_8  remote;
    } trap_version;
:eXMP.
:PC.
The :F.major:eF. field contains the major version number of the trap file while
the :F.minor:eF. field tells the minor version number of the trap file.
:F.Major:eF. is changed whenever there is a modification made to the trap file
that is not upwardly compatable with previous versions. :F.Minor:eF.
increments by one whenever a change is made to the trap file that is upwardly
compatible with previous versions.
The current major verion is &trp_majver., the current minor version is &trp_minver..
The :F.remote:eF. field informs the
debugger whether the trap file communicates with a remote machine.
:P.
:F.TrapInit:eF. must be called before using :F.TrapRequest:eF. to send a request.
Failure to do so may result in unpredictable operation of :F.TrapRequest:eF..
.section TrapRequest
.np
All requests between the server and the remote trap file are handled by
TrapRequest.
:XMP.
    unsigned TRAPENTRY TrapRequest( unsigned num_in_mx,
                                    mx_entry *mx_in,
                                    unsigned num_out_mx,
                                    mx_entry *mx_out
    );
:eXMP.
:PC.
The :F.mx_in:eF. points to an array of request mx_entry's.
The :F.num_in_mx:eF.
field contains the number of elements of the array. Similarly,
the :F.mx_out:eF.
will point to an array of return mx_entry's. The number of elements
will be
given by the :F.num_out_mx:eF. field. The total number of bytes actually
filled in to the return message by the trap file is returned by the
function (this may be less than the total number of bytes described by
the :F.mx_out:eF. array).
:P.Since every request must start with an :F.access_req:eF. field, the
minimum size of a request message is one byte.
:P.
Some requests do not require a return message. In this case, the program
invoking TrapRequest :HP2.must:eHP2. pass zero for :F.num_out_mx:eF. and NULL for
:F.mx_out:eF..
.beglevel
.section Request Example
.np
The request REQ_READ_MEM needs the memory address and length
of memory to read as input and will return the memory block in the
output message. To read 30 bytes of memory from address 0x0010:0x8000 into
a buffer, we can write:
:XMP.
    mx_entry        in[1];
    mx_entry        out[1];
    unsigned char   buffer[30];
    struct in_msg_def {
        access_req          req;
        addr48_ptr          addr;
        unsigned_16         len;
    } in_msg = { REQ_READ_MEM, { 0x8000, 0x0010 }, sizeof( buffer ) };

    unsigned_16 mem_blk_len;

    in[0].ptr = &in_msg;
    in[0].len = sizeof( in_msg );
    out[0].ptr = &buffer;
    out[0].len = sizeof( buffer );

    mem_blk_len = TrapRequest( 1, in, 1, out );

    if( mem_blk_length != sizeof( buffer ) ) {
        printf( "Error in reading memory\n" );
    } else {
        printf( "OK\n" );
    }
:eXMP.
:PC.
The program will print "OK" if it has transferred 30 bytes of data from the
debuggee's address space to the :F.buffer:eF. variable. If less than 30
bytes is transfered, an error message is printed out.
.endlevel
.section TrapFini
.np
The function terminates the link between the debugger and the trap file.
It should be called
after finishing all access requests.
:XMP.
    void TRAPENTRY TrapFini( void );
:eXMP.
:PC.
After calling :F.TrapFini:eF., it is illegal to call :F.TrapRequest:eF.
without calling :F.TrapInit:eF. again.
.endlevel
.chap The Requests
This section descibes the individual requests, their parameters, and
their return values. A line of dashes indicates where an :F.mx_entry:eF. is
allowed (but not required) to start.
The debugger allows
(via REQ_GET_SUPPLEMENTARY_SERVICE/REQ_PERFORM_SUPPLEMENTARY_SERVICE) optional
components to be implemented only on specific systems.
:P.
The numeric value of the request which is placed in the :F.req:eF. field
follows the symbolic name in parentheses.
.section Core Requests
.np
These requests need to be implemented in all versions of the trap file,
although some of them may only be stub implementations in some environments.
Note that structures suitable for individual requests are declared in
:F.trpcore.h:eF..
.beglevel
.section REQ_CONNECT (0)
.np
Request to connect to the remote machine. This must be the first request made.
:P.
Request message:
:XMP.
    access_req      req
    unsigned_8      major;   <-+- struct trap_version
    unsigned_8      minor;     |
    unsigned_8      remote;  <-+
:eXMP.
:PC.
The :F.req:eF. field contains the request.
The :F.trap_version:eF.
structure tells the version of the program making the request. The :F.major:eF.
field contains the major version number of the trap file while the :F.minor:eF.
field tells the minor version number of the trap file. The :F.major:eF. is
changed whenever there is a modification made to the trap file that is not
upwardly compatable with previous versions. The :F.minor:eF. increments by one
whenever a change is made to the trap file that is upwardly compatable with
previous versions. The current major version is &trp_majver., the current minor version
is &trp_minver..
The :F.remote:eF. field informs the trap file whether a
remote server is between the &company debugger and the trap file.
:P.
Return message:
:XMP.
    unsigned_16 max_msg_size
    ----------------------------
    string      err_msg
:eXMP.
:PC.
If error has occurred, the :F.err_msg:eF. field will returns the error
message string. If there is no error, :F.error_msg:eF. returns a null character
and the field
:F.max_msg_size:eF. will contain the allowed maximum size of a message in
bytes. Any message
(typically reading/writing memory or files)
which would require more than the maximum number of bytes
to transmit or receive must be broken up into multiple requests.
The minimum acceptable value for this
field is 256.
.section REQ_DISCONNECT (1)
.np
Request to terminate the link between the local and remote machine.
After this request, a REQ_CONNECT must be the next one made.
:P.
Request message:
:XMP.
    access_req      req
:eXMP.
:PC.
The :F.req:eF. field contains the request.
:P.
Return message:
:XMP.
    NONE
:eXMP.
.section REQ_SUSPEND (2)
.np
Request to suspend the link between the server and the remote trap file.
The debugger issues this message just before it spawns a sub-shell (the
"system" command). This allows a remote server to enter a state where
it allows other trap files to connect to it (normally, once a remote server
has connected to a trap file, the remote link will fail any other attempts
to connect to it). This allows the user for instance to start up an RFX
process and transfer any missing files to the remote machine before
continuing the debugging process.
:P.
Request message:
:XMP.
    access_req      req
:eXMP.
:PC.
The :F.req:eF. field contains the request.
:P.
Return message:
:XMP.
    NONE
:eXMP.
.section REQ_RESUME (3)
.np
Request to resume the link between the server and the remote trap file.
The debugger issues this request when the spawned sub-shell exits.
:P.
Request message:
:XMP.
    access_req      req
:eXMP.
:PC.
The :F.req:eF. field contains the request.
:P.
Return message:
:XMP.
    NONE
:eXMP.
.section REQ_GET_SUPPLEMENTARY_SERVICE (4)
.np
Request to obtain a supplementary service id.
:P.
Request message:
:XMP.
    access_req  req
    ------------------------
    string      service_name
:eXMP.
:PC.
The :F.req:eF. field contains the request. The :F.service_name:eF field
contains a string identifying the supplementary service. This string is
case insensitive.
:P.
Return message:
:XMP.
    trap_error      err;
    trap_shandle    id;
:eXMP.
:PC.
The :F.err:eF. field is non-zero if something went wrong in obtaining
or initializing the service.
:F.Id:eF. is the identifier for a particular supplementary service.
It need not be the same from one invocation of the trap file to another.
If both it and the :F.err:eF. field are zero, it means that the
service is not available from this trap file.
:NOTE. In the future, we might allow for user developed add-ons to be
integrated with the debugger. There would be two components, one to be
added to the debugger and one to be added to the trap file. The two
pieces could communicate with each other via the supplementary services
mechanism.
.section REQ_PERFORM_SUPPLEMENTARY_SERVICE (5)
.np
Request to perform a supplementary service.
:P.
Request message:
:XMP.
    access_req  req
    unsigned_32 service_id
    ------------------------
    unspecified
:eXMP.
:PC.
The :F.req:eF. field contains the request. The :F.service_id:eF field
indicates which service is being requested. The remainder of the request
is specified by the individual supplementary service provider.
:P.
Return message:
:XMP.
    unspecified
:eXMP.
:PC.
The return message is specified by the individual supplementary service
provider.
.section REQ_GET_SYS_CONFIG (6)
.np
Request to get system information from the remote machine.
:P.
Request message:
:XMP.
    access_req      req
:eXMP.
:PC.
The :F.req:eF. field contains the request.
:P.
Return message:
:XMP.
    unsigned_8  cpu;
    unsigned_8  fpu;
    unsigned_8  osmajor;
    unsigned_8  osminor;
    unsigned_8  os;
    unsigned_8  huge_shift;
    mad_handle  mad;
:eXMP.
:PC.
The :F.mad:eF. field specifies the MAD (Machine Architecture Description) in use
and determines how the other fields will be interpreted. Currently the following
MADs are used:
:XMP.
    MAD_X86  - Intel Architecture IA-32 compatible
    MAD_AXP  - Alpha Architecture
    MAD_PPC  - PowerPC Architecture
    MAD_MIPS - MIPS Architecture
    MAD_MSJ  - Java Virtual Machine (Microsoft)
:eXMP.
:PC.
The :F.cpu:eF. fields returns the type of the remote CPU. The size of that field
is unsigned_8. Possible CPU types for MAD_X86 are:
:XMP.
    bits 0-3
       X86_86  = 0   - 8086
       X86_186 = 1   - 80186
       X86_286 = 2   - 80286
       X86_386 = 3   - 80386
       X86_486 = 4   - 80486
       X86_586 = 5   - Pentium
       X86_686 = 6   - Pentium Pro/II/III
       X86_P4  = 15  - Pentium 4
    bit  4           - MM registers
    bit  5           - XMM registers
    bits 6,7         - unused
:eXMP.
:PC.
The :F.fpu:eF. fields tells the type of FPU. The size of the field is unsigned_8.
FPU types for MAD_X86 include:
:XMP.
    X86_EMU = -1     - Software emulated FPU
    X86_NO  =  0     - No FPU
    X86_87  =  1     - 8087
    X86_287 =  2     - 80287
    X86_387 =  3     - 80387
    X86_487 =  4     - 486 integrated FPU
    X86_587 =  5     - Pentium integrated FPU
    X86_587 =  6     - Pentium Pro/II/III integrated FPU
    X86_P47 =  15    - Pentium 4 integrated FPU
:eXMP.
:PC.
The :F.osmajor:eF. and :F.osminor:eF. contains the major and minor version
number for the operating system of the remote machine. The type of operating
system can be found in
:F.os:eF. field. The size of this field is unsigned_8. The OS can be :
:XMP.
    OS_IDUNNO    =  0   - Unknown operating system
    OS_DOS       =  1   - DOS
    OS_OS2       =  2   - OS/2
    OS_PHAR      =  3   - Phar Lap 386 DOS Extender
    OS_ECLIPSE   =  4   - Eclipse 386 DOS Extender (obsolete)
    OS_NW386     =  5   - NetWare 386
    OS_QNX       =  6   - QNX 4.x
    OS_RATIONAL  =  7   - DOS/4G or compatible
    OS_WINDOWS   =  8   - Windows 3.x
    OS_PENPOINT  =  9   - PenPoint (obsolete)
    OS_NT        = 10   - Win32
    OS_AUTOCAD   = 11   - ADS/ADI development (obsolete)
    OS_NEUTRINO  = 12   - QNX 6.x
    OS_LINUX     = 13   - Linux
    OS_FREEBSD   = 14   - Free BSD
:eXMP.
:PC.
The :F.huge_shift:eF. field is used to determine the shift needed for huge
arithmetic in that system. It stores the number of left shifts required in order
to calculate the next segment correctly. It is 12 for real mode programs. The
value in a protect mode environment must be obtained from the OS of the debuggee
machine. This field is only relevant for 16-bit segmented architectures.
.section REQ_MAP_ADDR (7)
.np
Request to map the input address to the actual address of the remote machine.
The addresses in the symbolic information provided by the linker do not
reflect any relocation performed on the executable by the system loader. This
request obtains that relocation information so that the debugger can update
its addresses.
:P.
Request message:
:XMP.
    access_req      req;
    addr48_ptr      in_addr;
    trap_mhandle    handle;
:eXMP.
:PC.
The :F.req:eF. field contains the request. The :F.in_addr:eF. tells the address
to map. The :F.handle:eF. field identifies the module which the address is
from. The value from this field is obtained by REQ_PROG_LOAD or
REQ_GET_LIB_NAME. There are two magical values for the :F.in_addr.segment:eF.
field.
:XMP.
    MAP_FLAT_CODE_SELECTOR  = -1
    MAP_FLAT_DATA_SELECTOR  = -2
:eXMP.
:PC.
When the :F.in_addr.segment:eF. equals one of these values, the debugger
does not have a map segment value and is requesting that the trap file
performs the mapping as if the given offset was in the flat address space.
:P.
Return message:
:XMP.
    addr48_ptr      out_addr
    addr48_off      lo_bound;
    addr48_off      hi_bound;
:eXMP.
:PC.
The mapped address is returned in :F.out_addr:eF.. Note that in addition
to the segment portion being modified, the offset of the portion of the
address may be adjusted as well if the loader performs offset relocations
(like OS/2 2.x or Windows NT). The :F.lo_bound:eF. and :F.hi_bound:eF. fields
identify the lowest and highest input offsets for which this mapping is
valid. If the debugger needs to map another address whose input segment value
is the same as a previous request, and the input offset falls within the
valid range identified by the return of that previous request, it can
perform the mapping itself and not bother sending the request to the trap
file.
.section REQ_ADDR_INFO (8)
.np
This request is x86 specific and obsolete; REQ_MACHINE_DATA should be used
instead. It needs to be provided only for backwards compatibility.
:P.
Request to check if a given address is using 32-bit addressing (the
386 compatible CPU's current selector's B-bit is on) by
default. The debugger requires this information to properly disassemble
instructions.
:P.
Request message:
:XMP.
    access_req      req
    addr48_ptr      in_addr
:eXMP.
:PC.
The :F.req:eF. field contains the request and the :F.in_addr:eF. tells the input
address.
:P.
Return message:
:XMP.
    unsigned_8      is_32
:eXMP.
:PC.
The field returns one if the address is a USE32 segment, zero otherwise.
.section REQ_CHECKSUM_MEM (9)
.np
Request to calculate the checksum for a block of memory in the debuggee's
address space.
This is used by the debugger to determine if the contents of the memory block
have changed since the last time it was read. Since only a four byte
checksum has to be transmitted back, it is more efficient than actually
reading the memory again. The debugger does not care how the checksum
is calculated.
:P.
Request message:
:XMP.
    access_req      req;
    addr48_ptr      in_addr;
    unsigned_16     len;
:eXMP.
:PC.
The :F.req:eF. field stores the request. The :F.in_addr:eF. contains the
starting address and the :F.len:eF. field tells how large the block of memory is.
:P.
Return message:
:XMP.
    unsigned_32     result
:eXMP.
:PC.
The checksum will be returned in :F.result:eF..
.section REQ_READ_MEM (10)
.np
Request to read a block of memory.
:P.
Request message:
:XMP.
    access_req      req;
    addr48_ptr      mem_addr;
    unsigned_16     len;
:eXMP.
:PC.
The :F.mem_addr:eF. contains the address of the memory block to read from the
remote machine. The length of the block is determined by :F.len:eF.. The
memory data will be copied to output message.
:P.
Return message:
:XMP.
    bytes       data
:eXMP.
:PC.
The :F.data:eF. field stores the memory block read in. The length of this memory
block is given by the return value from TrapRequest. If error has occurred in reading
memory, the length of the data returns will not be equal to the number of bytes
requested.
.section REQ_WRITE_MEM (11)
.np
Request to write a block of memory.
:P.
Request message:
:XMP.
    access_req      req
    addr48_ptr      mem_addr
    ------------------------
    bytes           data
:eXMP.
:PC.
The :F.data:eF. field stores the memory data to be transferred. The data will be
stored in the debuggee's address space starting at the address in the
:F.mem_addr:eF. field.
:P.
Return message:
:XMP.
    unsigned_16 len
:eXMP.
:PC.
The :F.len:eF. field tells the length of memory block actually written to the
debuggee machine. If error has occurred in writing the memory, the length
returned will not be equal to the number of bytes requested.
.section REQ_READ_IO (12)
.np
Request to read data from I/O address space of the debuggee.
:P.
Request message:
:XMP.
    access_req      req
    unsigned_32     IO_offset
    unsigned_8      len
:eXMP.
:PC.
The :F.IO_offset:eF. contains the I/O address of the debuggee machine. The length
of the block is determined by :F.len:eF.. It must be 1, 2 or 4 bytes. The
data will be copied from :F.IO_offset:eF. to the return message.
:P.
Return message:
:XMP.
    bytes       data
:eXMP.
:PC.
The :F.data:eF. field stores the memory block read in. The length of this memory
block is given by the return value from TrapRequest. If an error has occurred in
reading, the length returned will not be equal to the number of bytes
requested.
.section REQ_WRITE_IO (13)
.np
Request to write data to the I/O address space of the debuggee.
:P.
Request message:
:XMP.
    access_req      req
    unsigned_32     IO_offset
    -------------------------
    bytes           data
:eXMP.
:PC.
The :F.IO_offset:eF. contains the I/O address of the debuggee machine.
The data stored in :F.data:eF. field will be copied to :F.IO_offset:eF. on
the debuggee machine.
:P.
Return message:
:XMP.
    unsigned_8  len
:eXMP.
:PC.
The :F.len:eF. field tells the number of bytes actually written out. If an error
has occurred in writing, the length returned will not be equal
to the number of bytes requested.
.section REQ_READ_CPU (14)
.np
This request is x86 specific and obsolete; REQ_READ_REGS should be used
instead. It needs to be provided only for backwards compatibility.
:P.
Request to read the CPU registers.
:P.
Request message:
:XMP.
    access_req      req
:eXMP.
:P.
Return message:
:XMP.
    bytes           data
:eXMP.
:PC.
The :F.data:eF.
field contains the register information requested. It contains the following
structure:
:XMP.
    struct cpu_regs {
        unsigned_32 EAX;
        unsigned_32 EBX;
        unsigned_32 ECX;
        unsigned_32 EDX;
        unsigned_32 ESI;
        unsigned_32 EDI;
        unsigned_32 EBP;
        unsigned_32 ESP;
        unsigned_32 EIP;
        unsigned_32 EFL;
        unsigned_32 CR0;
        unsigned_32 CR2;
        unsigned_32 CR3;
        unsigned_16 DS;
        unsigned_16 ES;
        unsigned_16 SS;
        unsigned_16 CS;
        unsigned_16 FS;
        unsigned_16 GS;
        };
:eXMP.
.section REQ_READ_FPU (15)
.np
This request is x86 specific and obsolete; REQ_READ_REGS should be used
instead. It needs to be provided only for backwards compatibility.
:P.
Request to read the FPU registers.
:P.
Request message:
:XMP.
    access_req      req
:eXMP.
:P.
Return message:
:XMP.
    bytes           data
:eXMP.
:PC.
The :F.data:eF. field contains the register information requested. Its format
is the same as the result of a "fsave" instruction in a 32-bit segment (the
instruction pointer and operand pointer fields take up 8 bytes each).
Implementations of trap files in 16-bit environments should expand the
instruction pointer and operand pointer fields from 4 bytes to 8 (shuffling
the data register fields down in memory) before returning the result to
the debugger.
.section REQ_WRITE_CPU (16)/REQ_WRITE_FPU (17)
.np
These requests are x86 specific and obsolete; REQ_WRITE_REGS should be used
instead. They needs to be provided only for backwards compatibility.
:P.
Requests to write to the CPU or FPU state.
:P.
Request message:
:XMP.
    access_req          req
    ------------------------
    bytes               data
:eXMP.
:P.
Information in :F.data:eF. field will be transfered to the debuggee's
registers. The formats of data can be found in
REQ_READ_CPU/REQ_READ_FPU
:NOTE.
For the REQ_WRITE_FPU case, the data will be in a 32-bit "fsave" instruction
format,
so 16-bit environments will have to squish the instruction and operand
pointer fields back to their 4 byte forms.
:P.
Return message:
:XMP.
    NONE
:eXMP.
:PC.
.section REQ_PROG_GO (18)/REQ_PROG_STEP (19)
.np
Requests to execute the debuggee. REQ_PROG_GO causes the debuggee to
resume execution, while REQ_PROG_STEP requests only a single machine
instruction to be executed before returning. In either case, this
request will return when a breakpoint, watchpoint, machine exception
or other significant event has been encountered. While executing, a
trap file is allowed to return spurious COND_WATCH indications.
The debugger always checks its own watchpoint table for changes before
reporting to the user. This means that a legal implementation of a trap
file (but :HP2.very:eHP2. inefficient) can just single step the program and return
COND_WATCH for every instruction when there are active watchpoints
present.
:P.
Request message:
:XMP.
    access_req      req
:eXMP.
:PC.
The request is in :F.req:eF. field.
:P.
Return message:
:XMP.
    addr48_ptr      stack_pointer
    addr48_ptr      program_counter
    unsigned_16     conditions
:eXMP.
:PC.
The :F.stack_pointer:eF. and :F.program_counter:eF. fields store the latest
values of SS:ESP and CS:EIP (or their non-x86 equivalents) respectively.
The :F.conditions:eF. informs the debugger what conditions have changed since
execution began. It contains the following flags:
:XMP.
    Bit 0   : COND_CONFIG           - Configurations change
    Bit 1   : COND_SECTIONS         - Program overlays change
    Bit 2   : COND_LIBRARIES        - Libraries (DLL) change
    Bit 3   : COND_ALIASING         - Alias change
    Bit 4   : COND_THREAD           - Thread change
    Bit 5   : COND_THREAD_EXTRA     - Thread extra change
    Bit 6   : COND_TRACE            - Trace point occurred
    Bit 7   : COND_BREAK            - Break point occurred
    Bit 8   : COND_WATCH            - Watch point occurred
    Bit 9   : COND_USER             - User interrupt
    Bit 10  : COND_TERMINATE        - Program terminated
    Bit 11  : COND_EXCEPTION        - Machine exception
    Bit 12  : COND_MESSAGE          - Message to be displayed
    Bit 13  : COND_STOP             - Debuggee wants to stop
    Bit 14-15  :  not used
:eXMP.
When a bit is off, the debugger avoids having to make additional requests
to determine the new state of the debuggee. If the trap file is not sure
that a particular item has changed, or if it is expensive to find out, it
should just turn the bit on.
.section REQ_PROG_LOAD (20)
.np
Request to load a program.
:P.
Request message:
:XMP.
    access_req      req
    unsigned_8      true_argv
    -------------------------
    bytes           argv
:eXMP.
:PC.
The :F.true_argv:eF. field indicates whether the argument consists of a
single string, or a true C-style argument vector. This field is set to
be one for a true argument vector and zero otherwise.
The :F.argv:eF. is a set of zero-terminated strings, one following each
other. The first string gives the name of the
program to be loaded. The remainder of the :F.argv:eF. field contains
the program's arguments. The arguments can be a single string or an array
of strings.
:P.
Return message:
:XMP.
    trap_error      err
    trap_phandle    task_id
    trap_mhandle    mod_handle
    unsigned_8      flags
:eXMP.
:PC.
The :F.err:eF. field returns the error code while loading the program.
The :F.task_id:eF. shows the task (process) ID for the program loaded.
The :F.mod_handle:eF. is the system module identification for the executable
image. It is used as input to the REQ_MAP_ADDR request.
The :F.flags:eF. field contains the following
information:
:XMP.
    Bit 0      :  LD_FLAG_IS_32             - 32-bit program (obsolete)
    Bit 1      :  LD_FLAG_IS_PROT           - Protected mode (obsolete)
    Bit 2      :  LD_FLAG_IS_STARTED        - Program already started
    Bit 3      :  LD_FLAG_IGNORE_SEGMENTS   - Ignore segments (flat)
    Bit 4      :  LD_FLAG_HAVE_RUNTIME_DLLS - DLL load breaks supported
    Bit 5      :  LD_FLAG_DISPLAY_DAMAGED   - Debugger must repaint screen
    Bit 6 - 7  :  not used
:eXMP.
.section REQ_PROG_KILL (21)
.np
Request to kill the program.
:P.
Request message:
:XMP.
    access_req      req
    trap_phandle    task_id
:eXMP.
:PC.
The :F.req:eF. field contains the request. The :F.task_id:eF. field (obtained
from REQ_PROG_LOAD) identifies the program to be killed.
:P.
Return message:
:XMP.
    trap_error      err
:eXMP.
:PC.
The :F.err:eF. field returns the error code of the OS kill program
operation.
.section REQ_SET_WATCH (22)
.np
Request to set a watchpoint at the address given.
:P.
Request message:
:XMP.
    access_req      req
    addr48_ptr      watch_addr
    unsigned_8      size
:eXMP.
:PC.
The address of the watchpoint is given by the :F.watch_addr:eF. field. The
:F.size:eF. field gives the number of bytes to be watched.
:P.
Return message:
:XMP.
    trap_error      err
    unsigned_32     multiplier
:eXMP.
The :F.err:eF. field returns the error code if the setting failed.
If the setting of the watchpoint worked, the 31 low order bits of
:F.multiplier:eF. indicate the expected slow down of the program when it's
placed into execution. The top bit of the field is set to one if a debug
register is being used for the watchpoint, and zero if the watchpoint is
being done by software.
.section REQ_CLEAR_WATCH (23)
.np
Request to clear a watchpoint at the address given. The trap file may
assume all watch points are cleared at once.
:P.
Request message:
:XMP.
    access_req      req
    addr48_ptr      watch_addr
    unsigned_8      size
:eXMP.
:PC.
The address of the watch point is given by the :F.watch_addr:eF. field. The
:F.size:eF. field gives the size of the watch point.
:P.
Return message:
:XMP.
    NONE
:eXMP.
.section REQ_SET_BREAK (24)
.np
Request to set a breakpoint at the address given.
:P.
Request message:
:XMP.
    access_req      req
    addr48_ptr      break_addr
:eXMP.
:PC.
The address of the break point is given by the :F.break_addr:eF. field.
:P.
Return message:
:XMP.
    unsigned_32     old
:eXMP.
:PC.
The :F.old:eF. field returns the original byte(s) at the address
:F.break_addr:eF..
.section REQ_CLEAR_BREAK (25)
.np
Request to clear a breakpoint at the address given. The trap file may
assume all breakpoints are cleared at once.
:P.
Request message:
:XMP.
    access_req      req
    addr48_ptr      break_addr
    unsigned_32     old
:eXMP.
:PC.
The address of the break point is given by the :F.break_addr:eF. field. The
:F.old:eF. field holds the old instruction returned from the REQ_SET_BREAK
request.
:P.
Return message:
:XMP.
    NONE
:eXMP.
.section REQ_GET_NEXT_ALIAS (26)
.np
Request to get alias information for a segment. In some protect mode
environments (typically 32-bit flat) two different selectors may refer
to the same physical memory. Which selectors do this is important to
the debugger in certain cases (so that symbolic information is properly
displayed).
:P.
Request message:
:XMP.
    access_req      req
    unsigned_16     seg
:eXMP.
:PC.
The :F.seg:eF. field contains the segment. To get the first alias, put zero
in this field.
:P.
Return message:
:XMP.
    unsigned_16     seg
    unsigned_16     alias
:eXMP.
:PC.
The :F.seg:eF. field contains the next segment where an alias appears. If this field
returns zero, it implies no more aliases can be found. The :F.alias:eF. field
returns the alias of the input segment. Zero indicates a previously set alias
should be deleted.
.section REQ_SET_USER_SCREEN (27)
.np
Request to make the debuggee's screen visible.
:P.
Request message:
:XMP.
    access_req      req
:eXMP.
:P.
Return message:
:XMP.
    NONE
:eXMP.
.section REQ_SET_DEBUG_SCREEN (28)
.np
Request to make the debugger's screen visible.
:P.
Request message:
:XMP.
    access_req      req
:eXMP.
:P.
Return message:
:XMP.
    NONE
:eXMP.
.section REQ_READ_USER_KEYBOARD (29)
.np
Request to read the remote keyboard input.
:P.
Request message:
:XMP.
    access_req      req
    unsigned_16     wait
:eXMP.
:PC.
The request will be time out if it waits longer than the period specifies in the
:F.wait:eF. field. The waiting period is measured in seconds. A value of zero
means to wait forever.
:P.
Return message:
:XMP.
    unsigned_8      key
:eXMP.
:PC.
The :F.key:eF. field returns the input character from remote machine.
.section REQ_GET_LIB_NAME (30)
.np
Request to get the name of a newly loaded library (DLL).
:P.
Request message:
:XMP.
    access_req      req
    trap_mhandle    handle
:eXMP.
:PC.
The :F.handle:eF. field contains the library handle. It should be zero
to get the name of the first DLL or the value from the :F.handle:eF.
of a previous request.
:P.
Return message:
:XMP.
    trap_mhandle    handle
    ---------------------------
    string          name
:eXMP.
:PC.
The :F.handle:eF. field contains the library handle. It contains zero if
there are no more DLL names to be returned. The name of the library will be returned in
:F.name:eF. field. If the :F.name:eF. field is an empty string (consists
just of the '\0' character), then this is a indication that the DLL indicated
by the given handle has been unloaded, and the debugger should remove
any symbolic information for the image. It is an error to attempt to remove
a handle that has not been loaded in a previous REQ_GET_LIB_NAME request.
.section REQ_GET_ERR_TEXT (31)
.np
Request to get the error message text for an error code.
:P.
Request message:
:XMP.
    access_req      req
    trap_error      err
:eXMP.
:PC.
The :F.err:eF. field contains the error code number of the error text
requested.
:P.
Return message:
:XMP.
    string          error_msg
:eXMP.
:PC.
The error message text will be returned in :F.error_msg:eF. field.
.section REQ_GET_MESSAGE_TEXT (32)
.np
Request to retrieve generic message text. After a REQ_PROG_LOAD, REQ_PROG_GO
or REQ_PROG_STEP has returned with COND_MESSAGE or COND_EXCEPTION,
the debugger will make this request to obtain the message text. In the
case of a COND_EXCEPTION return text describing the machine exception
that caused the return to the debugger. Otherwise return whatever generic
message text that the trap file wants to display to the user.
:P.
Request message:
:XMP.
    access_req      req
:eXMP.
:P.
Return message:
:XMP.
    unsigned_8      flags
    ---------------------
    string          msg
:eXMP.
:PC.
The message text will be returned in the :F.msg:eF. field.
The :F.flags:eF. contains a number of bits which control the next action
of the debugger. They are:
:XMP.
    Bit 0       : MSG_NEWLINE
    Bit 1       : MSG_MORE
    Bit 2       : MSG_WARNING
    Bit 3       : MSG_ERROR
    Bit 4 - 7   : not used
:eXMP.
:PC.
The MSG_NEWLINE bit indicates that the debugger should scroll its display
to a new line after displaying the message.
The MSG_MORE bit indicates that there is another line of output to come
and the debugger should make another REQ_GET_MESSAGE_TEXT.
MSG_WARNING indicates that the message is a warning level message while
MSG_ERROR is an error level message. If neither of these bits are on, the
message is merely informational.
.section REQ_REDIRECT_STDIN (33)/REQ_REDIRECT_STDOUT (34)
.np
Request to redirect the standard input (REQ_REDIRECT_STDIN) or
standard output (REQ_REDIRECT_STDOUT) of the debuggee.
:P.
Request message:
:XMP.
    access_req      req
    ------------------------
    string          name
:eXMP.
:PC.
The file name to be redirected
to/from is given by the :F.name:eF. field.
:P.
Return message:
:XMP.
    trap_error      err
:eXMP.
:PC.
When an error has occurred, the :F.err:eF. field contains an error code
indicating the type of error that has been detected.
.section REQ_SPLIT_CMD (35)
.np
Request to split the command line into the command name and
parameters.
:P.
Request message:
:XMP.
    access_req      req
    ------------------------
    string          cmd
:eXMP.
:PC.
The :F.cmd:eF.
field contains the command. Command can be a single command line or an array
of command strings.
:P.
Return message:
:XMP.
    unsigned_16     cmd_end
    unsigned_16     parm_start
:eXMP.
:PC.
The :F.cmd_end:eF. field tells the position in command line where the command
name ends. The :F.parm_start:eF. field stores the position where the
program arguments begin.
.section REQ_READ_REGS (36)
.np
Request to read CPU register contents. The data returned depends on
the target architecture and is defined by the MAD file.
:P.
Request message:
:XMP.
    access_req      req
:eXMP.
:PC.
:P.
Return message:
:XMP.
    unspecified
:eXMP.
:PC.
The return message content is specific to the MAD in use and will contain
a :F.mad_registers:eF. union (defined in :F.madtypes.h:eF.).
.section REQ_WRITE_REGS (37)
.np
Request to write CPU register contents. The data is target architecture
specific.
:P.
Request message:
:XMP.
    access_req      req
    -------------------
    unspecified
:eXMP.
:PC.
The message content is specific to the MAD in use and will contain a
:F.mad_registers:eF. union.
:P.
Return message:
:XMP.
    NONE
:eXMP.
:PC.
.section REQ_MACHINE_DATA (38)
.np
Request to retrieve machine specific data.
:P.
Request message:
:XMP.
    access_req      req;
    unsigned_8      info_type;
    addr48_ptr      addr;
    -----------------------------
    unspecified
:eXMP.
:PC.
The :F.info_type:eF. field specifies what kind of information should be
returned and :F.addr:eF. determines the address for which the information
is requested. The remainder of the message is MAD specific.
:P.
Return message:
:XMP.
    addr48_off      cache_start;
    addr48_off      cache_end;
    -----------------------------
    unspecified
:eXMP.
:PC.
The return message content is specific to the MAD in use.
.endlevel
.*
.*
.*
.section File I/O requests
.np
This section describes requests that deal with file input/output on the
target (debuggee) machine.
These requests are actually performed by the
core request REQ_PERFORM_SUPPLEMENTARY_SERVICE and appropriate service ID.
The following descriptions do not show that "prefix" to the
request messages.
:P.
The service name to be used in the REQ_GET_SUPPLEMENTARY_SERVICE is
"Files".
:P.
The file requests use a new basic type in addition to the ones already
described:
:DL.
:DTHD.Type
:DDHD.Definition
:DT.trap_fhandle
:DD.This is an :F.unsigned_32:eF. which holds a debuggee file handle.
:eDL.
.beglevel
.section REQ_FILE_GET_CONFIG (0)
.np
Request to retreive characteristics of the remote file system.
:P.
Request message:
:XMP.
    access_req      req
:eXMP.
:P.
Return message:
:XMP.
    char            ext_separator;
    char            path_separator[3];
    char            newline[2];
:eXMP.
:PC.
The
:F.ext_separator:eF. contains the separator for file name extensions. The possible
path separators can be found in array :F.path_separator:eF.. The first one is the
"preferred" path separator for that operating system.
This is the path separator that the debugger will use if it needs to construct
a file name for the remote system.
The new line control
characters are stored in array :F.newline:eF.. If the operating system uses only
a single character for newline, put a zero in the second element.
.section REQ_FILE_OPEN (1)
.np
Request to create/open a file.
:P.
Request message:
:XMP.
    access_req          req
    unsigned_8          mode
    ------------------------
    string              name
:eXMP.
:PC.
The name of the file to be opened is given by :F.name:eF..
The :F.mode:eF. field stores the access mode of the
file. The following bits are defined:
:XMP.
    Bit 0      :  TF_READ
    Bit 1      :  TF_WRITE
    Bit 2      :  TF_CREATE
    Bit 3      :  TF_EXEC
    Bit 4 - 7  :  not used
:eXMP.
:PC.
For read/write mode, turn both :F.TF_READ:eF. and :F.TF_WRITE:eF. bits on.
The :F.TF_EXEC:eF. bit should only be used together with :F.TF_CREATE:eF. and
indicates that the created file needs executable permission (if relevant on
the target platform).
:P.
Return message:
:XMP.
    trap_error      err
    trap_fhandle    handle
:eXMP.
:PC.
If successful, the :F.handle:eF. returns a handle for the file. When an error
has occurred, the :F.err:eF. field contains a value indicating the type
of error that has been detected.
.section REQ_FILE_SEEK (2)
.np
Request to seek to a particular file position.
:P.
Request message:
:XMP.
    access_req      req
    trap_fhandle    handle
    unsigned_8      mode
    unsigned_32     pos
:eXMP.
:PC.
The handle of the file is given by the :F.handle:eF. field. The :F.mode:eF.
field stores the seek mode. There are three seek modes:
:XMP.
    TF_SEEK_ORG = 0  - Relative to the start of file
    TF_SEEK_CUR = 1  - Relative to the current file position
    TF_SEEK_END = 2  - Rrelative to the end of file
:eXMP.
:PC.
The position to seek to is in the :F.pos:eF. field.
:P.
Return message:
:XMP.
    trap_error      err
    unsigned_32     pos
:eXMP.
:PC.
If an error has occurred, the :F.err:eF. field contains a value indicating
the type of error that has been detected. The :F.pos:eF. field returns the current
position of the file.
.section REQ_FILE_READ (3)
.np
Request to read a block of data from a file.
:P.
Request message:
:XMP.
    access_req          req
    trap_fhandle        handle
    unsigned_16         len
:eXMP.
:PC.
The handle of the file is given by the :F.handle:eF. field. The :F.len:eF.
field stores the number of bytes to be transmitted.
:P.
Return message:
:XMP.
    trap_error          err
    --------------------------
    bytes               data
:eXMP.
:PC.
If successful, the :F.data:eF. returns the block of data.
The length of returned data is given by the return value of TrapRequest
minus 4 (to account for the size of :F.err:eF.).
The length will normally be equal to the :F.len:eF.
field. If the end of file is encountered before the read completes, the return
value will be less than the number of bytes requested. When an error has
occurred, the :F.err:eF. field contains a value indicating the type of
error that has been detected.
.section REQ_FILE_WRITE (4)
.np
Request to write a block of data to a file.
:P.
Request message:
:XMP.
    access_req      req
    trap_fhandle    handle
    ------------------------
    bytes           data
:eXMP.
:PC.
The handle of the file is given by the :F.handle:eF. field. The data is given in
:F.data:eF. field.
:P.
Return message:
:XMP.
    trap_error      err
    unsigned_16     len
:eXMP.
:PC.
If there is no error, :F.len:eF. will equal to that in the :F.data_len:eF.
field. When an error has occurred, the :F.err:eF. field contains a value
indicating the type of error that has been detected.
.section REQ_FILE_WRITE_CONSOLE (5)
.np
Request to write a block of data to the debuggee's screen.
:P.
Request message:
:XMP.
    access_req      req
    ------------------------
    bytes           data
:eXMP.
:PC.
The data is given in :F.data:eF. field.
:P.
Return message:
:XMP.
    trap_error      err
    unsigned_16     len
:eXMP.
:PC.
If there is no error, :F.len:eF. will equal to the :F.data_len:eF.
field. When an error has occurred, the :F.err:eF. field contains a value
indicating the type of error that has been detected.
.section REQ_FILE_CLOSE (6)
.np
Request to close a file.
:P.
Request message:
:XMP.
    access_req      req
    trap_fhandle    handle
:eXMP.
:PC.
The handle of the file is given by the :F.handle:eF. field.
:P.
Return message:
:XMP.
    trap_error      err
:eXMP.
:PC.
When an error has occurred, the :F.err:eF. field contains a value
indicating the type of error that has been detected.
.section REQ_FILE_ERASE (7)
.np
Request to erase a file.
:P.
Request message:
:XMP.
    access_req      req
    -------------------------
    string          file_name
:eXMP.
:PC.
The :F.file_name:eF
field contains the file name to be deleted.
:P.
Return message:
:XMP.
    trap_error  err
:eXMP.
:PC.
If error has occurred when erasing the file, the :F.err:eF. field will
return the error code number.
.section REQ_FILE_STRING_TO_FULLPATH (8)
.np
Request to convert a file name to its full path name.
:P.
Request message:
:XMP.
    access_req      req
    unsigned_8      file_type
    -------------------------
    string          file_name
:eXMP.
:PC.
The :F.file_type:eF. field indicates the type of the input file.
File types can be:
:XMP.
    TF_FILE_EXE  =  0
    TF_FILE_DBG  =  1
    TF_FILE_PRS  =  2
    TF_FILE_HLP  =  3
:eXMP.
:PC.
This is
so the trap file can search different paths for the different types
of files. For example, under QNX, the PATH environment variable is searched
for the FILE_EXE type, and the WD_PATH environment variable is searched
for the others.
The :F.file_name:eF
field contains the file name to be converted.
:P.
Return message:
:XMP.
    trap_error      err
    --------------------------
    string          path_name
:eXMP.
:PC.
If no error occurs the :F.err:eF. field returns a zero and the full path name
will be stored in the :F.path_name:eF. field. When an error has occurred, the
:F.err:eF. field contains an error code indicating the type of error
that has been detected.
.section REQ_FILE_RUN_CMD (9)
.np
Request to run a command on the target (debuggee's) system.
:P.
Request message:
:XMP.
    access_req      req
    unsigned_16     chk_size
    ------------------------
    string          cmd
:eXMP.
:PC.
The :F.chk_size:eF. field gives the check size in kilobytes. This field
is only useful in the DOS implementation. It contains the value of
the /CHECKSIZE debugger command line option and represents the amount of
memory the user wishes to have free for the spawned sub-shell. The :F.cmd:eF.
field stores the command to be executed.
:P.
Return message:
:XMP.
    trap_error      err
:eXMP.
:PC.
If error has occurred when executing the command, the :F.err:eF. field
will return the error code number.
.endlevel
.*
.*
.*
.section Overlay requests
.np
This section describes requests that deal with overlays (supported
only under 16-bit DOS).
These requests are actually performed by the
core request REQ_PERFORM_SUPPLEMENTARY_SERVICE and appropriate service ID.
The following descriptions do not show that "prefix" to the
request messages.
:P.
The service name to be used in the REQ_GET_SUPPLEMENTARY_SERVICE is
"Overlays".
:P.
The overlay requests use a new basic type in addition to the ones already
described:
:DL.
:DTHD.Type
:DDHD.Definition
:DT.addr32_ptr
:DD.This type encapsulates the concept of a 16:16 pointer into the
debuggee's address space. Since overlays are
only useful for 16-bit environments, using the addr48_ptr type would be
inefficient.
The structure is defined as follows:
:XMP.
    typedef struct {
        unsigned_16    offset;
        unsigned_16    segment;
    } addr32_ptr;
:eXMP.
The :F.segment:eF. field contains the segment of the address and the :F.offset:eF.
field stores the offset of the address.
:DT.ovl_address
:DD.This type contains the overlay address and
the number of entries down in the overlay stack.
The structure is defined as follows:
:XMP.
    typedef struct {
        addr32_ptr  mach;
        unsigned_16 sect_id;
    } ovl_address;
:eXMP.
The :F.mach:eF. field is the machine address. The :F.sect_id:eF.
field stores the address section number.
:eDL.
.beglevel
.section REQ_OVL_STATE_SIZE (0)
.np
Request to return the size of the overlay state information in bytes of the
task program.
This request maps onto the
overlay manager's GET_STATE_SIZE request. See the Overlay Manager
Interface document for more information on the contents of the return
message.
:P.
Request message:
:XMP.
    access_req      req
:eXMP.
:PC.
The :F.req:eF. field contains the request.
:P.
Return message:
:XMP.
    unsigned_16     size
:eXMP.
:PC.
The :F.size:eF. field returns the size in bytes. A value of zero indicates
no overlays are present in the debuggee and none of the other requests
dealing with overlays will ever be called.
.section REQ_OVL_GET_DATA (1)
:P.
Request to get the address and size of an overlay section.
This request maps onto the
overlay manager's GET_SECTION_DATA request. See the Overlay Manager
Interface document for more information on the contents of the return
message.
:P.
Request message:
:XMP.
    access_req      req
    unsigned_16     sect_id
:eXMP.
The :F.sect_id:eF. field indicates the overlay section the information is
being requested of.
:P.
Return message:
:XMP.
    unsigned_16     segment
    unsigned_32     size
:eXMP.
:PC.
The :F.segment:eF. field contains the segment value where the overlay
section is loaded (or would be loaded if it was brought into memory).
The :F.size:eF. field gives the size, in bytes, of the overlay section.
If there is no section for the given id, the :F.segment:eF. field will be
zero.
.section REQ_OVL_READ_STATE (2)
:P.
Request to read the overlay table state.
This request maps onto the
overlay manager's GET_OVERLAY_STATE request. See the Overlay Manager
Interface document for more information on the contents of the return
message.
The size of the returned data is provided by the REQ_OVL_STATE_SIZE
trap file request.
:P.
Request message:
:XMP.
    access_req      req
:eXMP.
:P.
Return message:
:XMP.
    bytes           data
:eXMP.
:PC.
The :F.data:eF. field contains the overlay state information requested.
.section REQ_OVL_WRITE_STATE (3)
:P.
Request to write the overlay table state.
This request maps onto the
overlay manager's SET_OVERLAY_STATE request. See the Overlay Manager
Interface document for more information on the contents of the return
message.
:P.
Request message:
:XMP.
    access_req      req
    --------------------
    bytes           data
:eXMP.
:PC.
The :F.data:eF. field contains the overlay state information to be restored.
:P.
Return message:
:XMP.
    NONE
:eXMP.
.section REQ_OVL_TRANS_VECT_ADDR (4)
.np
Request to check if the input overlay address is actually an overlay
vector.
This request maps onto the
overlay manager's TRANSLATE_VECTOR_ADDR request. See the Overlay Manager
Interface document for more information on the contents of the
messages.
:P.
Request message:
:XMP.
    access_req      req
    ovl_address     ovl_addr
:eXMP.
:PC.
The :F.mach:eF. field is the machine address. The :F.sect_id:eF.
field stores the number of entries down in the overlay stack.
:P.
Return message:
:XMP.
    ovl_address     ovl_addr
:eXMP.
:PC.
The translated address will be returned in the :F.ovl_addr:eF. field.
If the address is not an overlay vector, then the input address will be
returned and the :F.sect_id:eF. field will be zero.
.section REQ_OVL_TRANS_RET_ADDR (5)
.np
Request to check if the address is the overlay manager parallel return
code.
This request maps onto the
overlay manager's TRANSLATE_RETURN_ADDR request. See the Overlay Manager
Interface document for more information on the contents of the
messages.
:P.
Request message:
:XMP.
    access_req      req
    ovl_address     ovl_addr
:eXMP.
:P.
Return message:
:XMP.
    ovl_address     ovl_addr
:eXMP.
:PC.
The translated address will be returned in the :F.ovl_addr:eF. field.
If the address is not an parallel return code, then the input address will be
returned and the :F.sect_id:eF. field in the structure :F.ovl_addr:eF. will
be zero.
.section REQ_OVL_GET_REMAP_ENTRY (6)
.np
Request to check if the overlay address needs to be remapped.
This request maps onto the
overlay manager's GET_MOVED_SECTION request. See the Overlay Manager
Interface document for more information on the contents of the
messages.
:P.
Request message:
:XMP.
    access_req      req
    ovl_address     ovl_addr
:eXMP.
:PC.
The :F.ovl_addr:eF. field contains the overlay address.
:P.
Return message:
:XMP.
    unsigned_8      remapped
    ovl_address     ovl_addr
:eXMP.
:PC.
If the address gets remapped the :F.remapped:eF. field will return one.
The remapped address will be returned in the :F.ovl_addr:eF. field.
The input address will be unchanged if the address has not been remapped.
.endlevel
.*
.*
.*
.section Thread requests
.np
This section descibes requests that deal with threads.
These requests are actually performed by the
core request REQ_PERFORM_SUPPLEMENTARY_SERVICE and appropriate service ID.
The following descriptions do not show that "prefix" to the request messages.
:P.
The service name to be used in the REQ_GET_SUPPLEMENTARY_SERVICE is
"Threads".
:P.
The thread requests use a new basic type in addition to the ones already
described:
:DL.
:DTHD.Type
:DDHD.Definition
:DT.trap_thandle
:DD.This is an :F.unsigned_32:eF. which holds a thread handle.
:eDL.
.beglevel
.section REQ_THREAD_GET_NEXT (0)
.np
Request to get next thread.
:P.
Request message:
:XMP.
    access_req      req
    trap_thandle    thread
:eXMP.
:PC.
The :F.thread:eF. contains the either a zero to get information on the
first thread, or the value of the :F.thread:eF. field in the
return message of a previous request.
:P.
Return message:
:XMP.
    trap_thandle    thread
    unsigned_8      state
:eXMP.
:PC.
The :F.thread:eF. field returns the thread ID. There are no more threads in
the list, it will contain zero.
The :F.state:eF. field can have two values:
:XMP.
    THREAD_THAWED = 0
    THREAD_FROZEN = 1
:eXMP.
.section REQ_THREAD_SET (1)
.np
Request to set a given thread ID to be the current thread.
:P.
Request message:
:XMP.
    access_req      req
    trap_thandle    thread
:eXMP.
:PC.
The :F.thread:eF. contains the thread number to set. If it's zero, do not
attempt to set the thread, just return the current thread ID.
:P.
Return message:
:XMP.
    trap_error      error
    trap_thandle    old_thread
:eXMP.
:PC.
The :F.old_thread:eF. field returns the previous thread ID. If the set fails, the
:F.err:eF. field will be non-zero.
.section REQ_THREAD_FREEZE (2)
.np
Request to freeze a thread so that it will not be run next time when executing
the task program.
:P.
Request message:
:XMP.
    access_req      req
    trap_thandle    thread
:eXMP.
:PC.
The :F.thread:eF. contains the thread number to freeze.
:P.
Return message:
:XMP.
    trap_error      err
:eXMP.
If the thread cannot be frozen, the :F.err:eF. field returns non-zero value.
.section REQ_THREAD_THAW (3)
.np
Request to allow a thread to run next time when executing the program.
:P.
Request message:
:XMP.
    access_req      req
    trap_thandle    thread
:eXMP.
:PC.
The :F.thread:eF. contains the thread number to thaw.
:P.
Return message:
:XMP.
    trap_error      err
:eXMP.
If the thread cannot be thawed, the :F.err:eF. field returns non zero value.
.section REQ_THREAD_GET_EXTRA (4)
.np
Request to get extra information about a thread. This is arbitrary textual
data which the debugger merely displays in its thread window. The trap
file can place any information in the return message which it feels would
be useful for the user to know.
:P.
Request message:
:XMP.
    access_req      req
    unsigned_32     thread
:eXMP.
:PC.
The :F.thread:eF. field contains the thread ID. A zero value means to get the
title string for the thread extra information. This is displayed at the
top of the thread window.
:P.
Return message:
:XMP.
    string          extra
:eXMP.
:PC.
The extra information of the thread will be returned in :F.extra:eF. field.
.endlevel
.section RFX requests
.np
This section deals with requests that are only used by the RFX (Remote
File Xfer) program. These requests are actually performed by the
core request REQ_PERFORM_SUPPLEMENTARY_SERVICE and appropriate service ID.
The following descriptions do not show that "prefix" to the request messages.
:P.
The service name to be used in the REQ_GET_SUPPLEMENTARY_SERVICE is
"RFX".
.beglevel
.section REQ_RFX_RENAME (0)
.np
Request to rename a file on the debuggee's system.
:P.
Request message:
:XMP.
    access_req      req
    -------------------------
    string          from_name
    -------------------------
    string          to_name
:eXMP.
:PC.
The file whose name is indicated by the field :F.from_name:eF. will be renamed
to the name given by the field :F.to_name:eF.
:P.
Return message:
:XMP.
    trap_error      err
:eXMP.
:PC.
If error has occurred, the :F.err:eF. field will return the error code
number.
.section REQ_RFX_MKDIR (1)
.np
Request to create a directory on the target (debuggee) system.
:P.
Request message:
:XMP.
    access_req      req
    ------------------------
    string          dir_name
:eXMP.
:PC.
The :F.dir_name:eF field contains the name of the directory to be created.
:P.
Return message:
:XMP.
    trap_error      err
:eXMP.
:PC.
If error has occurred when creating the directory, the :F.err:eF. field
will return the error code number.
.section REQ_RFX_RMDIR (2)
.np
Request to remove a directory on the target system.
:P.
Request message:
:XMP.
    access_req      req
    ------------------------
    string          dir_name
:eXMP.
:PC.
The :F.dir_name:eF field contains the name of the directory to be removed.
:P.
Return message:
:XMP.
    trap_error      err
:eXMP.
:PC.
If error has occurred, the :F.err:eF. field will return the error code
number.
.section REQ_RFX_SETDRIVE (3)
.np
Request to set the current drive on the target system.
:P.
Request message:
:XMP.
    access_req      req
    unsigned_8      drive
:eXMP.
:PC.
The :F.drive:eF field contains the drive number to be set on the target system.
:P.
Return message:
:XMP.
    trap_error      err
:eXMP.
:PC.
If error has occurred, the :F.err:eF. field will return the error code
number.
.section REQ_RFX_GETDRIVE (4)
.np
Request to get the current drive on the target system.
:P.
Request message:
:XMP.
    access_req      req
:eXMP.
:PC.
The :F.req:eF. field contains the request.
:P.
Return message:
:XMP.
    unsigned_8      drive
:eXMP.
:PC.
The :F.drive:eF field returns the current drive number on the target system.
.section REQ_RFX_SETCWD (5)
.np
Request to set a directory on the target system.
:P.
Request message:
:XMP.
    access_req      req
    ------------------------
    string          dir_name
:eXMP.
:PC.
The :F.dir_name:eF field contains the name of the directory to be set.
:P.
Return message:
:XMP.
    trap_error      err
:eXMP.
:PC.
If error has occurred, the :F.err:eF. field will return the error code
number.
.section REQ_RFX_GETCWD (6)
.np
Request to get the current directory name on the target system.
:P.
Request message:
:XMP.
    access_req      req
    unsigned_8      drive
:eXMP.
:PC.
The :F.drive:eF field contains the target drive number.
:P.
Return message:
:XMP.
    trap_error      err
    --------------------------
    string          dir_name
:eXMP.
:PC.
The :F.dir_name:eF field contains the name of the directory to be set. If error
has occurred, the :F.err:eF. field will return the error code number.
.section REQ_RFX_SETDATETIME (7)
.np
Request to set a file's date and time information on the target system.
:P.
Request message:
:XMP.
    access_req      req
    trap_fhandle    handle
    time_t          time
:eXMP.
:PC.
The :F.handle:eF. contains the file handle. The :F.time:eF. field follows
the UNIX time format. The :F.time:eF. represents the time since January 1, 1970
(UTC).
:P.
Return message:
:XMP.
    NONE
:eXMP.
.section REQ_RFX_GETDATETIME (8)
.np
Request to get the date and time information for a file on the target system.
:P.
Request message:
:XMP.
    access_req      req
    trap_fhandle    handle
:eXMP.
:PC.
The :F.handle:eF. contains the file handle.
:P.
Return message:
:XMP.
    time_t          time
:eXMP.
:PC.
The :F.time:eF. field follows the UNIX time format. The :F.time:eF.
represents the time since January 1, 1970 (UTC).
.section REQ_RFX_GETFREESPACE (9)
.np
Request to get the amount of free space left on the drive.
:P.
Request message:
:XMP.
    access_req      req
    unsigned_8      drive
:eXMP.
:PC.
The :F.drive:eF field contains the target drive number.
:P.
Return message:
:XMP.
    unsigned_32     size
:eXMP.
:PC.
The :F.size:eF. field returns the number of bytes left on the drive.
.section REQ_RFX_SETFILEATTR (10)
.np
Request to set the file attribute of a file.
:P.
Request message:
:XMP.
    access_req      req
    unsigned_32     attribute
    -------------------------
    string          name
:eXMP.
:PC.
The :F.name:eF field contains the name whose attributes are to be set.
The :F.attribute:eF. field contains the
new attributes of the file.
:P.
Return message:
:XMP.
    trap_error      err
:eXMP.
:PC.
If error has occurred, the :F.err:eF. field will return the error code
number.
.section REQ_RFX_GETFILEATTR (11)
.np
Request to get the file attribute of a file.
:P.
Request message:
:XMP.
    access_req      req
    --------------------
    string          name
:eXMP.
:PC.
The :F.name:eF field contains the name to be checked.
:P.
Return message:
:XMP.
    unsigned_32     attribute
:eXMP.
:PC.
The :F.attribute:eF. field returns the attribute of the file.
.section REQ_RFX_NAMETOCANNONICAL (12)
.np
Request to convert a file name to its canonical form.
:P.
Request message:
:XMP.
    access_req      req
    -------------------------
    string          file_name
:eXMP.
:PC.
The :F.file_name:eF
field contains the file name to be converted.
:P.
Return message:
:XMP.
    trap_error      err
    --------------------------
    string          path_name
:eXMP.
:PC.
If there is no error, the :F.err:eF. field returns a zero and the full path name
will be stored in the :F.path_name:eF. field. When an error has occurred, the
:F.err:eF. field contains an error code indicating the type of error
that has been detected.
.section REQ_RFX_FINDFIRST (13)
.np
Request to find the first file in a directory.
:P.
Request message:
:XMP.
    access_req      req
    unsigned_8      attrib
    ----------------------
    string          name
:eXMP.
:PC.
The :F.name:eF. field contains the name of the directory and the :F.attrib:eF. field
contains the attribute of the files to list in the directory.
:P.
Return message:
:XMP.
    trap_error      err
    -----------------------
    dta             info
:eXMP.
:PC.
If found, the :F.err:eF. field will be zero. The location and
information of about the first file will be in the structure :F.info.:eF. Definition
of the structure :F.dta:eF. is as follows:
:XMP.
    typedef struct dta {
        struct {
            unsigned_8          spare1[13];
            unsigned_16         dir_entry_num;
            unsigned_16         cluster;
            unsigned_8          spare2[4];
        } dos;
        unsigned_8          attr;
        unsigned_16         time;
        unsigned_16         date;
        unsigned_32         size;
        unsigned_8          name[14];
    } dta;
:eXMP.
.section REQ_RFX_FINDNEXT (14)
.np
Request to find the next file in the directory. This request should be used only after
REQ_RFX_FINDFIRST.
:P.
Request message:
:XMP.
    access_req      req
    --------------------
    dta             info
:eXMP.
:PC.
The :F.req:eF. field contains the request. The :F.info:eF. field contains
the dta returned from the previous REQ_FIND_NEXT or REQ_FIND_FIRST.
:P.
Return message:
:XMP.
    trap_error      err
    -----------------------
    dta             info
:eXMP.
:PC.
The :F.info:eF. field is the same as in REQ_FIND_FIRST.
.section REQ_RFX_FINDCLOSE (15)
.np
Request to end the directory search operation.
:P.
Request message:
:XMP.
    access_req      req
:eXMP.
:PC.
The :F.req:eF. field contains the request.
:P.
Return message:
:XMP.
    trap_error      err
:eXMP.
:PC.
If successful, the :F.err:eF. field will be zero, otherwise the
system error code will be returned.
.endlevel
.*
.*
.*
.chap System Dependent Aspects
Every environment has a different method of loading the code for the trap
file and locating the TrapInit, TrapRequest, and TrapFini routines. This
section descibes how the &company debugger performs these operations for
the various systems.
.section Trap Files Under DOS
.np
A trap file is an "EXE" format file with the extension ".TRP". The
debugger searches the directories specified by the PATH environment variable.
Once found, it is loaded into memory and has the normal EXE style relocations
applied to the image. Then the lowest address in the load image (NOTE: not
the starting address from EXE header information) is examined for the
following structure:
:XMP.
typedef struct {
    unsigned_16     signature; /* == 0xDEAF */
    unsigned_16     init_off;
    unsigned_16     acc_off;
    unsigned_16     fini_off;
} trap_header;
:eXMP.
:PC.
If the first 2 bytes contain the value 0xDEAF, the file is considered to
be a valid trap file and the :F.init_off:eF., :F.acc_off:eF., and
:F.fini_off:eF. fields are used to obtain the offsets of the TrapInit,
TrapRequest, and TrapFini routines repectively.
:P.
The starting address field of the EXE header should be set to point at
some code which prints out a message about not being able to be run from
the command line and then terminates.
.section Trap Files Under OS/2
.np
A trap file is a normal OS/2 DLL. The system automatically searches the
directories specified by the LIBPATH command in the CONFIG.SYS file. Once
loaded, the &company debugger uses export ordinal 1 from the DLL for TrapInit, export
ordinal 2 for TrapFini and export ordinal 3 for TrapRequest.
Some example code follows:
:XMP.
    rc = DosLoadModule( NULL, 0, trap_file_name, &dll_module );
    if( rc != 0 ) {
    return( "unable to load trap file" );
    }
    if( DosGetProcAddr( dll_module, "#1", &TrapInit ) != 0
     || DosGetProcAddr( dll_module, "#2", &TrapFini ) != 0
     || DosGetProcAddr( dll_module, "#3", &TrapRequest ) != 0 ) {
    return( "incorrect version of trap file" );
    }
:eXMP.
.section Trap Files Under Windows.
.np
A trap file is a normal Windows DLL. The system automatically searches the
directories specified by the PATH environment variable.
Once
loaded, the &company debugger uses export ordinal 2 from the DLL for TrapInit, export
ordinal 3 for TrapFini and export ordinal 4 for TrapRequest.
Some example code follows:
:XMP.
    dll = LoadLibrary( trap_file_name );
    if( dll < 32 ) {
        return( "unable to load trap file" );
    }
    TrapInit   = (LPVOID) GetProcAddress( dll, (LPSTR)2 );
    TrapFini   = (LPVOID) GetProcAddress( dll, (LPSTR)3 );
    TrapRequest = (LPVOID) GetProcAddress( dll, (LPSTR)4 );
    if( TrapInit == NULL || TrapFini == NULL || TrapRequest == NULL ) {
        return( "incorrect version of trap file" );
    }
:eXMP.
.section Trap Files Under Windows NT.
.np
A trap file is a normal Windows NT DLL. The system automatically searches the
directories specified by the PATH environment variable.
Once
loaded, the &company debugger uses export ordinal 1 from the DLL for TrapInit, export
ordinal 2 for TrapFini and export ordinal 3 for TrapRequest.
Some example code follows:
:XMP.
    dll = LoadLibrary( trap_file_name );
    if( dll < 32 ) {
        return( "unable to load trap file" );
    }
    TrapInit   = (LPVOID) GetProcAddress( dll, (LPSTR)1 );
    TrapFini   = (LPVOID) GetProcAddress( dll, (LPSTR)2 );
    TrapRequest = (LPVOID) GetProcAddress( dll, (LPSTR)3 );
    if( TrapInit == NULL || TrapFini == NULL || TrapRequest == NULL ) {
        return( "incorrect version of trap file" );
    }
:eXMP.
.section Trap Files Under QNX
.np
A trap file is a QNX load module format file with the extension ".trp"
and whose file permissions are not marked as executable. The
debugger searches the directories specified by the WD_PATH environment variable
and then the "/usr/watcom/wd" directory.
Once found, it is loaded into memory and has the normal loader relocations
applied to the image. Then the lowest address in the load image (NOTE: not
the starting address from load module header information) is examined for the
following structure:
:XMP.
typedef struct {
    unsigned_16     signature; /* == 0xDEAF */
    unsigned_16     init_off;
    unsigned_16     acc_off;
    unsigned_16     fini_off;
} trap_header;
:eXMP.
:PC.
If the first 2 bytes contain the value 0xDEAF, the file is considered to
be a valid trap file and the :F.init_off:eF., :F.acc_off:eF., and
:F.fini_off:eF. fields are used to obtain the offsets of the TrapInit,
TrapRequest, and TrapFini routines repectively.
:P.
The starting address field of the load image header should be set to point at
some code which prints out a message about not being able to be run from
the command line and then terminates.
.section Trap Files Under Netware 386 or PenPoint
.np
The trap file routines are linked directly into the remote server code and
TrapInit, TrapRequest, TrapFini are directly called.
