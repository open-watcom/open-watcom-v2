.if &'compare(&book.,'wddoc') eq 0 .do begin
.chap *refid=Introd1 Trap File Interface
.do end
.el .do begin
.chap *refid=Introd1 Introduction
.do end
.*
The &company debugger consists of a number of separate pieces of code.
The main executable, WD.EXE (wd on UNIX systems), provides a debugging
`engine' and user interface.
When the engine wishes to perform an operation upon the program being
debugged such as reading memory or setting a breakpoint, it creates
a request structure and sends it to the `trap file' (so called because
under DOS, it contains the first level trap handlers).
The trap file examines the request structure, performs the indicated action
and returns a result structure to the debugger.
The debugger and trap files also use Machine Architecture Description (MAD)
files which abstract the CPU architecture.
This design has the following benefits:
.autonote
.note
OS debugging interfaces tend to be wildly varying in how they are accessed.
By moving all the OS specific interface code into the trap file and having
a defined interface to access it, porting the debugger becomes much easier.
.note
By abstracting the machine architecture specifics through MAD files, it
becomes possible to use one debugger for several target CPU architectures
(such as x86 and Alpha AXP).
Unlike most other debuggers, the &company debugger is not tied to a single
host/target combination and if appropriate trap and MAD files are available,
the debugger running on any host can remotely debug any target.
.note
The trap file does not have to actually perform the operation.
Instead it could send the request out to a remote server by a communication
link such as a serial line or LAN.
The remote server can retrieve the request, perform the operation on the
remote machine and send the results back via the link.
This enables the debugger to debug applications in cases where there are
memory constraints or other considerations which prevent the debugger proper
from running on the remote system (such as Novell Netware 386).
.endnote
.np
This document describes the interface initially used by version 4.0 of the
WATCOM debugger (shipped with the 10.0 C/C++ and FORTRAN releases).
It has been revised to describe changes incorporated in Watcom 11.0 release,
as well as subsequent Open Watcom releases.
It is expected to be modified in future releases.
Where possible, notification of expected changes are given in the document,
but all aspects are subject to revision.
.*
.section Some Definitions
.*
.np
Next follow some general trap definitions.
.*
.beglevel
.*
.section Byte Order
.*
.np
The trap file interface is defined to use little endian byte order.
That is, the least significant byte is stored at the lowest address.
Little endian byte order was chosen for compatibility with existing trap
files and tools.
Fixed byte order also eases network communication between debuggers and trap
files running on machines with different byte order.
.*
.section Pointer Sizes
.*
.np
In a 16-bit hosted environment such as DOS, all pointers used by the trap file
are "far" 16:16 pointers.
In a 32-bit environment such as Windows NT the pointers are "near" 0:32
pointers.
.*
.section Base Types
.*
.np
A number of basic types are used in the interface.
They are defined as follows:
.begnote
.notehd1 Type
.notehd2 Definition
.note unsigned_8
1 byte unsigned quantity
.note unsigned_16
2 byte unsigned quantity
.note unsigned_32
4 byte unsigned quantity
.note access_req
The first field of every request is of this type.
It is a 1 byte field which identifies the request to be performed.
.note addr48_ptr
This type encapsulates the concept of a 16:32 pointer.
All addresses in the debuggee memory are described with these.
The debugger always acts as if the debuggee were in a 32-bit large model
environment since the 32-bit flat model and all 16-bit memory models are
subsets.
The structure is defined as follows:
.millust begin
typedef struct {
    unsigned_32    offset;
    unsigned_16    segment;
} addr48_ptr;
.millust end
The
.id segment
field contains the segment of the address and the
.id offset
field stores the offset of the address.
.note bytes
The type bytes is an array of unsigned_8.
The length is provided by other means.
Typically a field of type bytes is the last one in a request and the length
is calculated from the total length of the request.
.note string
The type string is actually an array of characters.
The array is terminated by a null ('\0') character.
The length is provided by other means.
Typically a field of type string is the last one in a request and the length
is calculated from the total length of the request.
.note trap_error
Some trap file requests return debuggee operating system error codes, notably
the requests to perform file I/O on the remote system.
These error codes are returned as an unsigned_32.
The debugger considers the value zero to indicate no error.
.note trap_phandle
This is an unsigned_32 type which holds process (task) handle.
A task handle is used to uniquely identify a debuggee process.
.note trap_mhandle
This is an unsigned_32 type which holds a module handle.
Typically the main executable will be one module, and on systems which support
DLLs or shared libraries, each library will be identified by a unique module
handle.
.endnote
.endlevel
.*
.*
.chap The Request Interface
.*
Next follow detailed description of interface elements.
.*
.section Request Structure
.*
.np
Each request is a composed of two sequences of bytes provided by the
debugger called messages.
The first set contains the actual request code and whatever parameters that
are required by the request.
The second sequence is where the result of the operation is to be stored by
the trap file.
.np
The two sequences need not be contiguous.
The sequences are described to the trap file through two arrays of message
entry structures.
This allows the debugger to avoid unnecessary packing and unpacking of
messages, since
.id mx_entry
can be set to point directly at parameter/result buffers.
.np
Multiple requests are :HP2.not:eHP2. allowed in a single message.
The
.id mx_entry
is only used to provide scatter/gather capabilities for one request at a time.
.np
The message entry structure is as follows (defined in
.id trptypes.h
):
.millust begin
typedef struct {
    void            *ptr;
    unsigned        len;
} mx_entry;
.millust end
.pp
The
.id ptr
is pointing to a block of data for that message entry.
The
.id len
field gives the length of that block.
One array of
.id mx_entry
describes the request message.
The second array describes the return message.
.np
It is not legal to split a message into arbitrary pieces with mx_entries.
Each request documents where an
.id mx_entry
is allowed to start with a line of dashes.
.*
.section The Interface Routines
.*
.np
The trap file interface must provide three routines:
.id TrapInit
,
.id TrapRequest
, and
.id TrapFini
.period
How the debugger determines the address of these routines after loading a trap
file, as well as the calling convention used, is system dependent and
described later.
These functions are prototyped in
.id trpimp.h
.period
.*
.beglevel
.*
.section TrapInit
.*
.np
This function initializes the environment for proper operation of
.id TrapRequest
.period
.millust begin
trap_version TRAPENTRY TrapInit(
    char       *parm,
    char       *error,
    unsigned_8 remote
);
.millust end
.pp
The
.id parm
is a string that the user passes to the trap file.
Its interpretation is completely up to the trap file.
In the case of the &company debugger, all the characters following
the semicolon in the
.id /TRAP
option are passed as the
.id parm
.period
For example:
.millust begin
wd /trap=nov;testing program
.millust end
.np
The
.id parm
would be "testing".
Any error message will be returned in
.id error
.period
The
.id remote
field is a zero if the &company debugger is loading the trap file and a one
if a remote server is loading it.
This function returns a structure
.id trap_version
of the following form
(defined in
.id trptypes.h
):
.millust begin
typedef struct {
    unsigned_8  major;
    unsigned_8  minor;
    unsigned_8  remote;
} trap_version;
.millust end
.pp
The
.id major
field contains the major version number of the trap file while the
.id minor
field tells the minor version number of the trap file.
.id Major
is changed whenever there is a modification made to the trap file
that is not upwardly compatable with previous versions.
.id Minor
increments by one whenever a change is made to the trap file that is upwardly
compatible with previous versions.
The current major verion is &trp_majver., the current minor version is &trp_minver..
The
.id remote
field informs the
debugger whether the trap file communicates with a remote machine.
.np
.id TrapInit
must be called before using
.id TrapRequest
to send a request.
Failure to do so may result in unpredictable operation of
.id TrapRequest
.period
.*
.section TrapRequest
.*
.np
All requests between the server and the remote trap file are handled by
TrapRequest.
.millust begin
unsigned TRAPENTRY TrapRequest(
    unsigned num_in_mx,
    mx_entry *mx_in,
    unsigned num_out_mx,
    mx_entry *mx_out
);
.millust end
.pp
The
.id mx_in
points to an array of request mx_entry's.
The
.id num_in_mx
field contains the number of elements of the array.
Similarly, the
.id mx_out
will point to an array of return mx_entry's.
The number of elements will be given by the
.id num_out_mx
field.
The total number of bytes actually filled in to the return message by the trap
file is returned by the function (this may be less than the total number of
bytes described by the
.id mx_out
array).
.np
Since every request must start with an
.id access_req
field, the minimum size of a request message is one byte.
.np
Some requests do not require a return message.
In this case, the program invoking TrapRequest :HP2.must:eHP2. pass zero for
.id num_out_mx
and NULL for
.id mx_out
.period
.*
.beglevel
.*
.section Request Example
.*
.np
The request REQ_READ_MEM needs the memory address and length
of memory to read as input and will return the memory block in the
output message.
To read 30 bytes of memory from address 0x0010:0x8000 into a buffer, we can
write:
.millust begin
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
.millust end
.pp
The program will print "OK" if it has transferred 30 bytes of data from the
debuggee's address space to the
.id buffer
variable.
If less than 30 bytes is transfered, an error message is printed out.
.endlevel
.*
.*
.section TrapFini
.*
.np
The function terminates the link between the debugger and the trap file.
It should be called
after finishing all access requests.
.millust begin
void TRAPENTRY TrapFini( void );
.millust end
.pp
After calling
.id TrapFini
, it is illegal to call
.id TrapRequest
without calling
.id TrapInit
again.
.endlevel
.*
.*
.chap The Requests
.*
This section descibes the individual requests, their parameters, and
their return values.
A line of dashes indicates where an
.id mx_entry
is allowed (but not required) to start.
The debugger allows
(via REQ_GET_SUPPLEMENTARY_SERVICE/REQ_PERFORM_SUPPLEMENTARY_SERVICE) optional
components to be implemented only on specific systems.
.np
The numeric value of the request which is placed in the
.id req
field follows the symbolic name in parentheses.
.*
.section Core Requests
.*
.np
These requests need to be implemented in all versions of the trap file,
although some of them may only be stub implementations in some environments.
Note that structures suitable for individual requests are declared in
.id trpcore.h
.period
.*
.beglevel
.*
.section REQ_CONNECT
.*
.np
Request to connect to the remote machine.
This must be the first request made.
.np
Request message:
.millust begin
access_req      req
unsigned_8      major;   <-+- struct trap_version
unsigned_8      minor;     |
unsigned_8      remote;  <-+
.millust end
.pp
The
.id req
field contains the request.
The
.id trap_version
structure tells the version of the program making the request.
The
.id major
field contains the major version number of the trap file while the
.id minor
field tells the minor version number of the trap file.
The
.id major
is changed whenever there is a modification made to the trap file that is not
upwardly compatable with previous versions.
The
.id minor
increments by one whenever a change is made to the trap file that is upwardly
compatable with previous versions.
The current major version is &trp_majver., the current minor version is
&trp_minver..
The
.id remote
field informs the trap file whether a remote server is between the &company
debugger and the trap file.
.np
Return message:
.millust begin
unsigned_16 max_msg_size
----------------------------
string      err_msg
.millust end
.pp
If error has occurred, the
.id err_msg
field will returns the error message string.
If there is no error,
.id error_msg
returns a null character and the field
.id max_msg_size
will contain the allowed maximum size of a message in bytes.
Any message (typically reading/writing memory or files)
which would require more than the maximum number of bytes
to transmit or receive must be broken up into multiple requests.
The minimum acceptable value for this
field is 256.
.*
.section REQ_DISCONNECT
.*
.np
Request to terminate the link between the local and remote machine.
After this request, a REQ_CONNECT must be the next one made.
.np
Request message:
.millust begin
access_req      req
.millust end
.pp
The
.id req
field contains the request.
.np
Return message:
.millust begin
NONE
.millust end
.*
.section REQ_SUSPEND
.*
.np
Request to suspend the link between the server and the remote trap file.
The debugger issues this message just before it spawns a sub-shell (the
"system" command).
This allows a remote server to enter a state where it allows other trap files
to connect to it (normally, once a remote server has connected to a trap file,
the remote link will fail any other attempts to connect to it).
This allows the user for instance to start up an RFX process and transfer any
missing files to the remote machine before continuing the debugging process.
.np
Request message:
.millust begin
access_req      req
.millust end
.pp
The
.id req
field contains the request.
.np
Return message:
.millust begin
NONE
.millust end
.*
.section REQ_RESUME
.*
.np
Request to resume the link between the server and the remote trap file.
The debugger issues this request when the spawned sub-shell exits.
.np
Request message:
.millust begin
access_req      req
.millust end
.pp
The
.id req
field contains the request.
.np
Return message:
.millust begin
NONE
.millust end
.*
.section REQ_GET_SUPPLEMENTARY_SERVICE
.*
.np
Request to obtain a supplementary service id.
.np
Request message:
.millust begin
access_req  req
------------------------
string      service_name
.millust end
.pp
The
.id req
field contains the request.
The
.id service_name
field contains a string identifying the supplementary service.
This string is case insensitive.
.np
Return message:
.millust begin
trap_error      err;
trap_shandle    id;
.millust end
.pp
The
err
field is non-zero if something went wrong in obtaining
or initializing the service.
.id Id
is the identifier for a particular supplementary service.
It need not be the same from one invocation of the trap file to another.
If both it and the
.id err
field are zero, it means that the
service is not available from this trap file.
:NOTE. In the future, we might allow for user developed add-ons to be
integrated with the debugger.
There would be two components, one to be added to the debugger and one
to be added to the trap file.
The two pieces could communicate with each other via the supplementary
services mechanism.
.*
.section REQ_PERFORM_SUPPLEMENTARY_SERVICE
.*
.np
Request to perform a supplementary service.
.np
Request message:
.millust begin
access_req  req
unsigned_32 service_id
------------------------
unspecified
.millust end
.pp
The
.id req
field contains the request.
The
.id service_id
field indicates which service is being requested.
The remainder of the request is specified by the individual supplementary
service provider.
.np
Return message:
.millust begin
unspecified
.millust end
.pp
The return message is specified by the individual supplementary service
provider.
.*
.section REQ_GET_SYS_CONFIG
.*
.np
Request to get system information from the remote machine.
.np
Request message:
.millust begin
access_req      req
.millust end
.pp
The
.id req
field contains the request.
.np
Return message:
.millust begin
unsigned_8  cpu;
unsigned_8  fpu;
unsigned_8  osmajor;
unsigned_8  osminor;
unsigned_8  os;
unsigned_8  huge_shift;
mad_handle  mad;
.millust end
.pp
The
.id mad
field specifies the MAD (Machine Architecture Description) in use and
determines how the other fields will be interpreted.
Currently the following MADs are used:
.millust begin
MAD_X86  - Intel Architecture IA-32 compatible
MAD_X64  - Intel Architecture X64 compatible
MAD_AXP  - Alpha Architecture
MAD_PPC  - PowerPC Architecture
MAD_MIPS - MIPS Architecture
MAD_MSJ  - Java Virtual Machine (Microsoft)
MAD_JVM  - Java Virtual Machine (Sun)
.millust end
.pp
The
.id cpu
fields returns the type of the remote CPU.
The size of that field is unsigned_8.
Possible CPU types for MAD_X86 are:
.millust begin
bits 0-3
   X86_86  = 0   - 8086
   X86_186 = 1   - 80186
   X86_286 = 2   - 80286
   X86_386 = 3   - 80386
   X86_486 = 4   - 80486
   X86_586 = 5   - Pentium
   X86_686 = 6   - Pentium Pro/II/III
   X86_P4  = 15  - Pentium 4
bit  4           - MMX registers
bit  5           - XMM registers
bits 6           - unused
bits 7           - unused
.millust end
.pp
The
.id fpu
fields tells the type of FPU.
The size of the field is unsigned_8.
FPU types for MAD_X86 include:
.millust begin
X86_EMU = -1     - Software emulated FPU
X86_NO  =  0     - No FPU
X86_87  =  1     - 8087
X86_287 =  2     - 80287
X86_387 =  3     - 80387
X86_487 =  4     - 486 integrated FPU
X86_587 =  5     - Pentium integrated FPU
X86_587 =  6     - Pentium Pro/II/III integrated FPU
X86_P47 =  15    - Pentium 4 integrated FPU
.millust end
.pp
The
.id osmajor
and
.id osminor
contains the major and minor version number for the operating system of the
remote machine.
The type of operating system can be found in
.id os
field.
The size of this field is unsigned_8.
The OS can be:
.millust begin
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
OS_FREEBSD   = 14   - FreeBSD
OS_WIN64     = 15   - Windows 64-bit
.millust end
.pp
The
.id huge_shift
field is used to determine the shift needed for huge arithmetic in that system.
It stores the number of left shifts required in order to calculate the next
segment correctly.
It is 12 for real mode programs.
The value in a protect mode environment must be obtained from the OS of the
debuggee machine.
This field is only relevant for 16-bit segmented architectures.
.*
.section REQ_MAP_ADDR
.*
.np
Request to map the input address to the actual address of the remote machine.
The addresses in the symbolic information provided by the linker do not
reflect any relocation performed on the executable by the system loader.
This request obtains that relocation information so that the debugger can
update its addresses.
.np
Request message:
.millust begin
access_req      req;
addr48_ptr      in_addr;
trap_mhandle    mod_handle;
.millust end
.pp
The
.id req
field contains the request.
The
.id in_addr
tells the address to map.
The
.id mod_handle
field identifies the module which the address is from.
The value from this field is obtained by REQ_PROG_LOAD or REQ_GET_LIB_NAME.
There are two magical values for the
.id in_addr.segment
field.
.millust begin
MAP_FLAT_CODE_SELECTOR  = -1
MAP_FLAT_DATA_SELECTOR  = -2
.millust end
.pp
When the
.id in_addr.segment
equals one of these values, the debugger does not have a map segment value
and is requesting that the trap file performs the mapping as if the given
offset was in the flat address space.
.np
Return message:
.millust begin
addr48_ptr      out_addr
addr48_off      lo_bound;
addr48_off      hi_bound;
.millust end
.pp
The mapped address is returned in
.id out_addr
.period
Note that in addition to the segment portion being modified, the offset of
the portion of the address may be adjusted as well if the loader performs
offset relocations (like OS/2 2.x or Windows NT).
The
.id lo_bound
and
.id hi_bound
fields identify the lowest and highest input offsets for which this mapping
is valid.
If the debugger needs to map another address whose input segment value
is the same as a previous request, and the input offset falls within the
valid range identified by the return of that previous request, it can
perform the mapping itself and not bother sending the request to the trap
file.
.*
.section REQ_CHECKSUM_MEM
.*
.np
Request to calculate the checksum for a block of memory in the debuggee's
address space.
This is used by the debugger to determine if the contents of the memory block
have changed since the last time it was read.
Since only a four byte checksum has to be transmitted back, it is more
efficient than actually reading the memory again.
The debugger does not care how the checksum is calculated.
.np
Request message:
.millust begin
access_req      req;
addr48_ptr      in_addr;
unsigned_16     len;
.millust end
.pp
The
.id req
field stores the request.
The
.id in_addr
contains the starting address and the
.id len
field tells how large the block of memory is.
.np
Return message:
.millust begin
unsigned_32     result
.millust end
.pp
The checksum will be returned in
.id result
.period
.*
.section REQ_READ_MEM
.*
.np
Request to read a block of memory.
.np
Request message:
.millust begin
access_req      req;
addr48_ptr      mem_addr;
unsigned_16     len;
.millust end
.pp
The
.id mem_addr
contains the address of the memory block to read from the remote machine.
The length of the block is determined by
.id len
.period
The memory data will be copied to output message.
.np
Return message:
.millust begin
bytes       data
.millust end
.pp
The
.id data
field stores the memory block read in.
The length of this memory block is given by the return value from TrapRequest.
If error has occurred in reading memory, the length of the data returns will
not be equal to the number of bytes requested.
.*
.section REQ_WRITE_MEM
.*
.np
Request to write a block of memory.
.np
Request message:
.millust begin
access_req      req
addr48_ptr      mem_addr
------------------------
bytes           data
.millust end
.pp
The
.id data
field stores the memory data to be transferred.
The data will be stored in the debuggee's address space starting at
the address in the
.id mem_addr
field.
.np
Return message:
.millust begin
unsigned_16 len
.millust end
.pp
The
.id len
field tells the length of memory block actually written to the debuggee
machine.
If error has occurred in writing the memory, the length returned will not be
equal to the number of bytes requested.
.*
.section REQ_READ_IO
.*
.np
Request to read data from I/O address space of the debuggee.
.np
Request message:
.millust begin
access_req      req
unsigned_32     IO_offset
unsigned_8      len
.millust end
.pp
The
.id IO_offset
contains the I/O address of the debuggee machine.
The length of the block is determined by
.id len
.period
It must be 1, 2 or 4 bytes.
The data will be copied from
.id IO_offset
to the return message.
.np
Return message:
.millust begin
bytes       data
.millust end
.pp
The
.id data
field stores the memory block read in.
The length of this memory block is given by the return value from TrapRequest.
If an error has occurred in reading, the length returned will not be equal to
the number of bytes requested.
.*
.section REQ_WRITE_IO
.*
.np
Request to write data to the I/O address space of the debuggee.
.np
Request message:
.millust begin
access_req      req
unsigned_32     IO_offset
-------------------------
bytes           data
.millust end
.pp
The
.id IO_offset
contains the I/O address of the debuggee machine.
The data stored in
.id data
field will be copied to
.id IO_offset
on the debuggee machine.
.np
Return message:
.millust begin
unsigned_8  len
.millust end
.pp
The
.id len
field tells the number of bytes actually written out.
If an error has occurred in writing, the length returned will not be equal
to the number of bytes requested.
.*
.section REQ_PROG_GO/REQ_PROG_STEP
.*
.np
Requests to execute the debuggee.
REQ_PROG_GO causes the debuggee to resume execution, while REQ_PROG_STEP
requests only a single machine instruction to be executed before returning.
In either case, this request will return when a breakpoint, watchpoint,
machine exception or other significant event has been encountered.
While executing, a trap file is allowed to return spurious COND_WATCH
indications.
The debugger always checks its own watchpoint table for changes before
reporting to the user.
This means that a legal implementation of a trap file (but :HP2.very:eHP2.
inefficient) can just single step the program and return COND_WATCH for every
instruction when there are active watchpoints present.
.np
Request message:
.millust begin
access_req      req
.millust end
.pp
The request is in
.id req
field.
.np
Return message:
.millust begin
addr48_ptr      stack_pointer
addr48_ptr      program_counter
unsigned_16     conditions
.millust end
.pp
The
.id stack_pointer
and
.id program_counter
fields store the latest values of SS:ESP and CS:EIP (or their non-x86
equivalents) respectively.
The
.id conditions
informs the debugger what conditions have changed since execution began.
It contains the following flags:
.millust begin
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
Bit 14  : COND_RUNNING          - Debuggee is running
Bit 15  : not used
.millust end
When a bit is off, the debugger avoids having to make additional requests
to determine the new state of the debuggee.
If the trap file is not sure that a particular item has changed, or if it is
expensive to find out, it should just turn the bit on.
.*
.section REQ_PROG_LOAD
.*
.np
Request to load a program.
.np
Request message:
.millust begin
access_req      req
unsigned_8      true_argv
-------------------------
bytes           argv
.millust end
.pp
The
.id true_argv
field indicates whether the argument consists of a single string, or a true
C-style argument vector.
This field is set to be one for a true argument vector and zero otherwise.
The
.id argv
is a set of zero-terminated strings, one following each other.
The first string gives the name of the program to be loaded.
The remainder of the
.id argv
field contains the program's arguments.
The arguments can be a single string or an array of strings.
.np
Return message:
.millust begin
trap_error      err
trap_phandle    task_id
trap_mhandle    mod_handle
unsigned_8      flags
.millust end
.pp
The
.id err
field returns the error code while loading the program.
The
.id task_id
shows the task (process) ID for the program loaded.
The
.id mod_handle
is the system module identification for the executable image.
It is used as input to the REQ_MAP_ADDR request.
The
.id flags
field contains the following information:
.millust begin
Bit 0   : LD_FLAG_IS_BIG            - 32-bit program (obsolete)
Bit 1   : LD_FLAG_IS_PROT           - Protected mode (obsolete)
Bit 2   : LD_FLAG_IS_STARTED        - Program already started
Bit 3   : LD_FLAG_IGNORE_SEGMENTS   - Ignore segments (flat)
Bit 4   : LD_FLAG_HAVE_RUNTIME_DLLS - DLL load breaks supported
Bit 5   : LD_FLAG_DISPLAY_DAMAGED   - Debugger must repaint screen
Bit 6   : not used
Bit 7   : not used
.millust end
.*
.section REQ_PROG_KILL
.*
.np
Request to kill the program.
.np
Request message:
.millust begin
access_req      req
trap_phandle    task_id
.millust end
.pp
The
.id req
field contains the request.
The
.id task_id
field (obtained from REQ_PROG_LOAD) identifies the program to be killed.
.np
Return message:
.millust begin
trap_error      err
.millust end
.pp
The
.id err
field returns the error code of the OS kill program operation.
.*
.section REQ_SET_WATCH
.*
.np
Request to set a watchpoint at the address given.
.np
Request message:
.millust begin
access_req      req
addr48_ptr      watch_addr
unsigned_8      size
.millust end
.pp
The address of the watchpoint is given by the
.id watch_addr
field.
The
.id size
field gives the number of bytes to be watched.
.np
Return message:
.millust begin
trap_error      err
unsigned_32     multiplier
.millust end
The
.id err
field returns the error code if the setting failed.
If the setting of the watchpoint worked, the 31 low order bits of
.id multiplier
indicate the expected slow down of the program when it's placed into execution.
The top bit of the field is set to one if a debug register is being used for
the watchpoint, and zero if the watchpoint is being done by software.
.*
.section REQ_CLEAR_WATCH
.*
.np
Request to clear a watchpoint at the address given.
The trap file may assume all watch points are cleared at once.
.np
Request message:
.millust begin
access_req      req
addr48_ptr      watch_addr
unsigned_8      size
.millust end
.pp
The address of the watch point is given by the
.id watch_addr
field.
The
.id size
field gives the size of the watch point.
.np
Return message:
.millust begin
NONE
.millust end
.*
.section REQ_SET_BREAK
.*
.np
Request to set a breakpoint at the address given.
.np
Request message:
.millust begin
access_req      req
addr48_ptr      break_addr
.millust end
.pp
The address of the break point is given by the
.id break_addr
field.
.np
Return message:
.millust begin
unsigned_32     old
.millust end
.pp
The
.id old
field returns the original byte(s) at the address
.id break_addr
.period
.*
.section REQ_CLEAR_BREAK
.*
.np
Request to clear a breakpoint at the address given.
The trap file may assume all breakpoints are cleared at once.
.np
Request message:
.millust begin
access_req      req
addr48_ptr      break_addr
unsigned_32     old
.millust end
.pp
The address of the break point is given by the
.id break_addr
field.
The
.id old
field holds the old instruction returned from the REQ_SET_BREAK request.
.np
Return message:
.millust begin
NONE
.millust end
.*
.section REQ_GET_NEXT_ALIAS
.*
.np
Request to get alias information for a segment.
In some protect mode environments (typically 32-bit flat) two different
selectors may refer to the same physical memory.
Which selectors do this is important to the debugger in certain cases
(so that symbolic information is properly displayed).
.np
Request message:
.millust begin
access_req      req
unsigned_16     seg
.millust end
.pp
The
.id seg
field contains the segment.
To get the first alias, put zero in this field.
.np
Return message:
.millust begin
unsigned_16     seg
unsigned_16     alias
.millust end
.pp
The
.id seg
field contains the next segment where an alias appears.
If this field returns zero, it implies no more aliases can be found.
The
.id alias
field returns the alias of the input segment.
Zero indicates a previously set alias should be deleted.
.*
.section REQ_SET_USER_SCREEN
.*
.np
Request to make the debuggee's screen visible.
.np
Request message:
.millust begin
access_req      req
.millust end
.np
Return message:
.millust begin
NONE
.millust end
.*
.section REQ_SET_DEBUG_SCREEN
.*
.np
Request to make the debugger's screen visible.
.np
Request message:
.millust begin
access_req      req
.millust end
.np
Return message:
.millust begin
NONE
.millust end
.*
.section REQ_READ_USER_KEYBOARD
.*
.np
Request to read the remote keyboard input.
.np
Request message:
.millust begin
access_req      req
unsigned_16     wait
.millust end
.pp
The request will be time out if it waits longer than the period specifies in the
.id wait
field.
The waiting period is measured in seconds.
A value of zero means to wait forever.
.np
Return message:
.millust begin
unsigned_8      key
.millust end
.pp
The
.id key
field returns the input character from remote machine.
.*
.section REQ_GET_LIB_NAME
.*
.np
Request to get the name of a newly loaded library (DLL).
.np
Request message:
.millust begin
access_req      req
trap_mhandle    mod_handle
.millust end
.pp
The
.id mod_handle
field contains the library handle.
It should be zero to get the name of the first DLL or the value from the
.id mod_handle
of a previous request.
.np
Return message:
.millust begin
trap_mhandle    mod_handle
---------------------------
string          name
.millust end
.pp
The
.id mod_handle
field contains the library handle.
It contains zero if there are no more DLL names to be returned.
The name of the library will be returned in
.id name
field.
If the
.id name
field is an empty string (consists just of the '\0' character), then this
is a indication that the DLL indicated by the given handle has been unloaded,
and the debugger should remove any symbolic information for the image.
It is an error to attempt to remove a handle that has not been loaded in
a previous REQ_GET_LIB_NAME request.
.*
.section REQ_GET_ERR_TEXT
.*
.np
Request to get the error message text for an error code.
.np
Request message:
.millust begin
access_req      req
trap_error      err
.millust end
.pp
The
.id err
field contains the error code number of the error text requested.
.np
Return message:
.millust begin
string          error_msg
.millust end
.pp
The error message text will be returned in
.id error_msg
field.
.*
.section REQ_GET_MESSAGE_TEXT
.*
.np
Request to retrieve generic message text.
After a REQ_PROG_LOAD, REQ_PROG_GO or REQ_PROG_STEP has returned with
COND_MESSAGE or COND_EXCEPTION, the debugger will make this request
to obtain the message text.
In the case of a COND_EXCEPTION return text describing the machine exception
that caused the return to the debugger.
Otherwise return whatever generic message text that the trap file wants
to display to the user.
.np
Request message:
.millust begin
access_req      req
.millust end
.np
Return message:
.millust begin
unsigned_8      flags
---------------------
string          msg
.millust end
.pp
The message text will be returned in the
.id msg
field.
The
.id flags
contains a number of bits which control the next action of the debugger.
They are:
.millust begin
Bit 0       : MSG_NEWLINE
Bit 1       : MSG_MORE
Bit 2       : MSG_WARNING
Bit 3       : MSG_ERROR
Bit 4       : not used
Bit 5       : not used
Bit 6       : not used
Bit 7       : not used
.millust end
.pp
The MSG_NEWLINE bit indicates that the debugger should scroll its display
to a new line after displaying the message.
The MSG_MORE bit indicates that there is another line of output to come
and the debugger should make another REQ_GET_MESSAGE_TEXT.
MSG_WARNING indicates that the message is a warning level message while
MSG_ERROR is an error level message.
If neither of these bits are on, the message is merely informational.
.*
.section REQ_REDIRECT_STDIN/REQ_REDIRECT_STDOUT
.*
.np
Request to redirect the standard input (REQ_REDIRECT_STDIN) or
standard output (REQ_REDIRECT_STDOUT) of the debuggee.
.np
Request message:
.millust begin
access_req      req
------------------------
string          name
.millust end
.pp
The file name to be redirected to/from is given by the
.id name
field.
.np
Return message:
.millust begin
trap_error      err
.millust end
.pp
When an error has occurred, the
.id err
field contains an error code indicating the type of error that has been
detected.
.*
.section REQ_SPLIT_CMD
.*
.np
Request to split the command line into the command name and
parameters.
.np
Request message:
.millust begin
access_req      req
------------------------
string          cmd
.millust end
.pp
The
.id cmd
field contains the command.
Command can be a single command line or an array of command strings.
.np
Return message:
.millust begin
unsigned_16     cmd_end
unsigned_16     parm_start
.millust end
.pp
The
.id cmd_end
field tells the position in command line where the command name ends.
The
.id parm_start
field stores the position where the program arguments begin.
.*
.section REQ_READ_REGS
.*
.np
Request to read CPU register contents.
The data returned depends on the target architecture and is defined by
the MAD file.
.np
Request message:
.millust begin
access_req      req
.millust end
.pp
.np
Return message:
.millust begin
unspecified
.millust end
.pp
The return message content is specific to the MAD in use and will contain
a
.id mad_registers
union (defined in
.id madtypes.h
).
.*
.section REQ_WRITE_REGS
.*
.np
Request to write CPU register contents.
The data is target architecture specific.
.np
Request message:
.millust begin
access_req      req
-------------------
unspecified
.millust end
.pp
The message content is specific to the MAD in use and will contain a
.id mad_registers
union.
.np
Return message:
.millust begin
NONE
.millust end
.pp
.*
.section REQ_MACHINE_DATA
.*
.np
Request to retrieve machine specific data.
.np
Request message:
.millust begin
access_req      req;
unsigned_8      info_type;
addr48_ptr      addr;
-----------------------------
unspecified
.millust end
.pp
The
.id info_type
field specifies what kind of information should be returned and
.id addr
determines the address for which the information is requested.
The remainder of the message is MAD specific.
.np
Return message:
.millust begin
addr48_off      cache_start;
addr48_off      cache_end;
-----------------------------
unspecified
.millust end
.pp
The return message content is specific to the MAD in use.
.endlevel
.*
.*
.section File I/O requests
.*
.np
This section describes requests that deal with file input/output on the
target (debuggee) machine.
These requests are actually performed by the
core request REQ_PERFORM_SUPPLEMENTARY_SERVICE and appropriate service ID.
The following descriptions do not show that "prefix" to the
request messages.
.np
The service name to be used in the REQ_GET_SUPPLEMENTARY_SERVICE is
"Files".
.np
The file requests use a new basic type in addition to the ones already
described:
.begnote
.notehd1 Type
.notehd2 Definition
.note trap_fhandle
This is an
.id unsigned_64
which holds a debuggee file handle.
.endnote
.*
.beglevel
.*
.section REQ_FILE_GET_CONFIG
.*
.np
Request to retreive characteristics of the remote file system.
.np
Request message:
.millust begin
access_req      req
.millust end
.np
Return message:
.millust begin
char            ext_separator;
char            path_separator[3];
char            newline[2];
.millust end
.pp
The
.id ext_separator
contains the separator for file name extensions.
The possible path separators can be found in array
.id path_separator
.period
The first one is the "preferred" path separator for that operating system.
This is the path separator that the debugger will use if it needs to construct
a file name for the remote system.
The new line control characters are stored in array
.id newline
.period
If the operating system uses only a single character for newline, put a zero
in the second element.
.*
.section REQ_FILE_OPEN
.*
.np
Request to create/open a file.
.np
Request message:
.millust begin
access_req          req
unsigned_8          mode
------------------------
string              name
.millust end
.pp
The name of the file to be opened is given by
.id name
.period
The
.id mode
field stores the access mode of the file.
The following bits are defined:
.millust begin
Bit 0      :  TF_READ
Bit 1      :  TF_WRITE
Bit 2      :  TF_CREATE
Bit 3      :  TF_EXEC
Bit 4      :  not used
Bit 5      :  not used
Bit 6      :  not used
Bit 7      :  not used
.millust end
.pp
For read/write mode, turn both
.id TF_READ
and
.id TF_WRITE
bits on.
The
.id TF_EXEC
bit should only be used together with
.id TF_CREATE
and indicates that the created file needs executable permission (if relevant
on the target platform).
.np
Return message:
.millust begin
trap_error      err
trap_fhandle    handle
.millust end
.pp
If successful, the
.id handle
returns a handle for the file.
When an error has occurred, the
.id err
field contains a value indicating the type of error that has been detected.
.*
.section REQ_FILE_SEEK
.*
.np
Request to seek to a particular file position.
.np
Request message:
.millust begin
access_req      req
trap_fhandle    handle
unsigned_8      mode
unsigned_32     pos
.millust end
.pp
The handle of the file is given by the
.id handle
field.
The
.id mode
field stores the seek mode.
There are three seek modes:
.millust begin
TF_SEEK_ORG = 0  - Relative to the start of file
TF_SEEK_CUR = 1  - Relative to the current file position
TF_SEEK_END = 2  - Rrelative to the end of file
.millust end
.pp
The position to seek to is in the
.id pos
field.
.np
Return message:
.millust begin
trap_error      err
unsigned_32     pos
.millust end
.pp
If an error has occurred, the
.id err
field contains a value indicating the type of error that has been detected.
The
.id pos
field returns the current position of the file.
.*
.section REQ_FILE_READ
.*
.np
Request to read a block of data from a file.
.np
Request message:
.millust begin
access_req          req
trap_fhandle        handle
unsigned_16         len
.millust end
.pp
The handle of the file is given by the
.id handle
field.
The
.id len
field stores the number of bytes to be transmitted.
.np
Return message:
.millust begin
trap_error          err
--------------------------
bytes               data
.millust end
.pp
If successful, the
.id data
returns the block of data.
The length of returned data is given by the return value of TrapRequest
minus 4 (to account for the size of
.id err
).
The length will normally be equal to the
.id len
field.
If the end of file is encountered before the read completes, the return
value will be less than the number of bytes requested.
When an error has occurred, the
.id err
field contains a value indicating the type of error that has been detected.
.*
.section REQ_FILE_WRITE
.*
.np
Request to write a block of data to a file.
.np
Request message:
.millust begin
access_req      req
trap_fhandle    handle
------------------------
bytes           data
.millust end
.pp
The handle of the file is given by the
.id handle
field.
The data is given in
.id data
field.
.np
Return message:
.millust begin
trap_error      err
unsigned_16     len
.millust end
.pp
If there is no error,
.id len
will equal to that in the
.id data_len
field.
When an error has occurred, the
.id err
field contains a value indicating the type of error that has been detected.
.*
.section REQ_FILE_WRITE_CONSOLE
.*
.np
Request to write a block of data to the debuggee's screen.
.np
Request message:
.millust begin
access_req      req
------------------------
bytes           data
.millust end
.pp
The data is given in
.id data
field.
.np
Return message:
.millust begin
trap_error      err
unsigned_16     len
.millust end
.pp
If there is no error,
.id len
will equal to the
.id data_len
field.
When an error has occurred, the
.id err
field contains a value indicating the type of error that has been detected.
.*
.section REQ_FILE_CLOSE
.*
.np
Request to close a file.
.np
Request message:
.millust begin
access_req      req
trap_fhandle    handle
.millust end
.pp
The handle of the file is given by the
.id handle
field.
.np
Return message:
.millust begin
trap_error      err
.millust end
.pp
When an error has occurred, the
.id err
field contains a value indicating the type of error that has been detected.
.*
.section REQ_FILE_ERASE
.*
.np
Request to erase a file.
.np
Request message:
.millust begin
access_req      req
-------------------------
string          file_name
.millust end
.pp
The
.id file_name
field contains the file name to be deleted.
.np
Return message:
.millust begin
trap_error  err
.millust end
.pp
If error has occurred when erasing the file, the
.id err
field will return the error code number.
.*
.section REQ_FILE_STRING_TO_FULLPATH
.*
.np
Request to convert a file name to its full path name.
.np
Request message:
.millust begin
access_req      req
unsigned_8      file_type
-------------------------
string          file_name
.millust end
.pp
The
.id file_type
field indicates the type of the input file.
File types can be:
.millust begin
TF_FILE_EXE  =  0
TF_FILE_DBG  =  1
TF_FILE_PRS  =  2
TF_FILE_HLP  =  3
.millust end
.pp
This is
so the trap file can search different paths for the different types of files.
For example, under QNX, the PATH environment variable is searched for the
FILE_EXE type, and the WD_PATH environment variable is searched for the others.
The
.id file_name
field contains the file name to be converted.
.np
Return message:
.millust begin
trap_error      err
--------------------------
string          path_name
.millust end
.pp
If no error occurs the
.id err
field returns a zero and the full path name will be stored in the
.id path_name
field.
When an error has occurred, the
.id err
field contains an error code indicating the type of error that has been
detected.
.*
.section REQ_FILE_RUN_CMD
.*
.np
Request to run a command on the target (debuggee's) system.
.np
Request message:
.millust begin
access_req      req
unsigned_16     chk_size
------------------------
string          cmd
.millust end
.pp
The
.id chk_size
field gives the check size in kilobytes.
This field is only useful in the DOS implementation.
It contains the value of the /CHECKSIZE debugger command line option and
represents the amount of memory the user wishes to have free for the spawned
sub-shell.
The
.id cmd
field stores the command to be executed.
.np
Return message:
.millust begin
trap_error      err
.millust end
.pp
If error has occurred when executing the command, the
.id err
field will return the error code number.
.endlevel
.*
.*
.section Overlay requests
.*
.np
This section describes requests that deal with overlays (supported
only under 16-bit DOS).
These requests are actually performed by the
core request REQ_PERFORM_SUPPLEMENTARY_SERVICE and appropriate service ID.
The following descriptions do not show that "prefix" to the
request messages.
.np
The service name to be used in the REQ_GET_SUPPLEMENTARY_SERVICE is
"Overlays".
.np
The overlay requests use a new basic type in addition to the ones already
described:
.begnote
.notehd1 Type
.notehd2 Definition
.note addr32_ptr
This type encapsulates the concept of a 16:16 pointer into the debuggee's
address space.
Since overlays are only useful for 16-bit environments, using the addr48_ptr
type would be inefficient.
The structure is defined as follows:
.millust begin
typedef struct {
    unsigned_16    offset;
    unsigned_16    segment;
} addr32_ptr;
.millust end
The
.id segment
field contains the segment of the address and the
.id offset
field stores the offset of the address.
.note ovl_address
This type contains the overlay address and
the number of entries down in the overlay stack.
The structure is defined as follows:
.millust begin
typedef struct {
    addr32_ptr  mach;
    unsigned_16 sect_id;
} ovl_address;
.millust end
The
.id mach
field is the machine address.
The
.id sect_id
field stores the address section number.
.endnote
.*
.beglevel
.*
.section REQ_OVL_STATE_SIZE
.*
.np
Request to return the size of the overlay state information in bytes of the
task program.
This request maps onto the overlay manager's GET_STATE_SIZE request.
See the Overlay Manager Interface document for more information on the
contents of the return message.
.np
Request message:
.millust begin
access_req      req
.millust end
.pp
The
.id req
field contains the request.
.np
Return message:
.millust begin
unsigned_16     size
.millust end
.pp
The
.id size
field returns the size in bytes.
A value of zero indicates no overlays are present in the debuggee and none
of the other requests dealing with overlays will ever be called.
.*
.section REQ_OVL_GET_DATA
.*
.np
Request to get the address and size of an overlay section.
This request maps onto the overlay manager's GET_SECTION_DATA request.
See the Overlay Manager Interface document for more information on
the contents of the return message.
.np
Request message:
.millust begin
access_req      req
unsigned_16     sect_id
.millust end
The
.id sect_id
field indicates the overlay section the information is being requested of.
.np
Return message:
.millust begin
unsigned_16     segment
unsigned_32     size
.millust end
.pp
The
.id segment
field contains the segment value where the overlay section is loaded (or
would be loaded if it was brought into memory).
The
.id size
field gives the size, in bytes, of the overlay section.
If there is no section for the given id, the
.id segment
field will be zero.
.*
.section REQ_OVL_READ_STATE
.*
.np
Request to read the overlay table state.
This request maps onto the overlay manager's GET_OVERLAY_STATE request.
See the Overlay Manager Interface document for more information on
the contents of the return message.
The size of the returned data is provided by the REQ_OVL_STATE_SIZE
trap file request.
.np
Request message:
.millust begin
access_req      req
.millust end
.np
Return message:
.millust begin
bytes           data
.millust end
.pp
The
.id data
field contains the overlay state information requested.
.*
.section REQ_OVL_WRITE_STATE
.*
.np
Request to write the overlay table state.
This request maps onto the overlay manager's SET_OVERLAY_STATE request.
See the Overlay Manager Interface document for more information on
the contents of the return message.
.np
Request message:
.millust begin
access_req      req
--------------------
bytes           data
.millust end
.pp
The
.id data
field contains the overlay state information to be restored.
.np
Return message:
.millust begin
NONE
.millust end
.*
.section REQ_OVL_TRANS_VECT_ADDR
.*
.np
Request to check if the input overlay address is actually an overlay vector.
This request maps onto the overlay manager's TRANSLATE_VECTOR_ADDR request.
See the Overlay Manager Interface document for more information on
the contents of the messages.
.np
Request message:
.millust begin
access_req      req
ovl_address     ovl_addr
.millust end
.pp
The
.id mach
field is the machine address.
The
.id sect_id
field stores the number of entries down in the overlay stack.
.np
Return message:
.millust begin
ovl_address     ovl_addr
.millust end
.pp
The translated address will be returned in the
.id ovl_addr
field.
If the address is not an overlay vector, then the input address will be
returned and the
.id sect_id
field will be zero.
.*
.section REQ_OVL_TRANS_RET_ADDR
.*
.np
Request to check if the address is the overlay manager parallel return
code.
This request maps onto the overlay manager's TRANSLATE_RETURN_ADDR request.
See the Overlay Manager Interface document for more information on
the contents of the messages.
.np
Request message:
.millust begin
access_req      req
ovl_address     ovl_addr
.millust end
.np
Return message:
.millust begin
ovl_address     ovl_addr
.millust end
.pp
The translated address will be returned in the
.id ovl_addr
field.
If the address is not an parallel return code, then the input address will be
returned and the
.id sect_id
field in the structure
.id ovl_addr
will be zero.
.*
.section REQ_OVL_GET_REMAP_ENTRY
.*
.np
Request to check if the overlay address needs to be remapped.
This request maps onto the overlay manager's GET_MOVED_SECTION request.
See the Overlay Manager Interface document for more information on
the contents of the messages.
.np
Request message:
.millust begin
access_req      req
ovl_address     ovl_addr
.millust end
.pp
The
.id ovl_addr
field contains the overlay address.
.np
Return message:
.millust begin
unsigned_8      remapped
ovl_address     ovl_addr
.millust end
.pp
If the address gets remapped the
.id remapped
field will return one.
The remapped address will be returned in the
.id ovl_addr
field.
The input address will be unchanged if the address has not been remapped.
.endlevel
.*
.*
.section Thread requests
.*
.np
This section descibes requests that deal with threads.
These requests are actually performed by the
core request REQ_PERFORM_SUPPLEMENTARY_SERVICE and appropriate service ID.
The following descriptions do not show that "prefix" to the request messages.
.np
The service name to be used in the REQ_GET_SUPPLEMENTARY_SERVICE is
"Threads".
.np
The thread requests use a new basic type in addition to the ones already
described:
.begnote
.notehd1 Type
.notehd2 Definition
.note trap_thandle
This is an unsigned_32 type which holds a thread handle.
.endnote
.*
.beglevel
.*
.section REQ_THREAD_GET_NEXT
.*
.np
Request to get next thread.
.np
Request message:
.millust begin
access_req      req
trap_thandle    thread
.millust end
.pp
The
.id thread
contains the either a zero to get information on the first thread, or
the value of the
.id thread
field in the return message of a previous request.
.np
Return message:
.millust begin
trap_thandle    thread
unsigned_8      state
.millust end
.pp
The
.id thread
field returns the thread ID.
There are no more threads in the list, it will contain zero.
The
.id state
field can have two values:
.millust begin
THREAD_THAWED = 0
THREAD_FROZEN = 1
.millust end
.*
.section REQ_THREAD_SET
.*
.np
Request to set a given thread ID to be the current thread.
.np
Request message:
.millust begin
access_req      req
trap_thandle    thread
.millust end
.pp
The
.id thread
contains the thread number to set.
If it's zero, do not attempt to set the thread, just return the current thread ID.
.np
Return message:
.millust begin
trap_error      error
trap_thandle    old_thread
.millust end
.pp
The
.id old_thread
field returns the previous thread ID.
If the set fails, the
.id err
field will be non-zero.
.*
.section REQ_THREAD_FREEZE
.*
.np
Request to freeze a thread so that it will not be run next time when executing
the task program.
.np
Request message:
.millust begin
access_req      req
trap_thandle    thread
.millust end
.pp
The
.id thread
contains the thread number to freeze.
.np
Return message:
.millust begin
trap_error      err
.millust end
If the thread cannot be frozen, the
.id err
field returns non-zero value.
.*
.section REQ_THREAD_THAW
.*
.np
Request to allow a thread to run next time when executing the program.
.np
Request message:
.millust begin
access_req      req
trap_thandle    thread
.millust end
.pp
The
.id thread
contains the thread number to thaw.
.np
Return message:
.millust begin
trap_error      err
.millust end
If the thread cannot be thawed, the
.id err
field returns non zero value.
.*
.section REQ_THREAD_GET_EXTRA
.*
.np
Request to get extra information about a thread.
This is arbitrary textual data which the debugger merely displays in its
thread window.
The trap file can place any information in the return message which it feels
would be useful for the user to know.
.np
Request message:
.millust begin
access_req      req
unsigned_32     thread
.millust end
.pp
The
.id thread
field contains the thread ID.
A zero value means to get the title string for the thread extra information.
This is displayed at the top of the thread window.
.np
Return message:
.millust begin
string          extra
.millust end
.pp
The extra information of the thread will be returned in
.id extra
field.
.endlevel
.*
.*
.section RFX requests
.*
.np
This section deals with requests that are only used by the RFX (Remote File
Xfer) program.
These requests are actually performed by the core request
REQ_PERFORM_SUPPLEMENTARY_SERVICE and appropriate service ID.
The following descriptions do not show that "prefix" to the request messages.
.np
The service name to be used in the REQ_GET_SUPPLEMENTARY_SERVICE is "Rfx".
.beglevel
.*
.section REQ_RFX_RENAME
.*
.np
Request to rename a file on the debuggee's system.
.np
Request message:
.millust begin
access_req      req
-------------------------
string          from_name
-------------------------
string          to_name
.millust end
.pp
The file whose name is indicated by the field
.id from_name
will be renamed to the name given by the field
.id to_name
.period
.np
Return message:
.millust begin
trap_error      err
.millust end
.pp
If error has occurred, the
.id err
field will return the error code number.
.*
.section REQ_RFX_MKDIR
.*
.np
Request to create a directory on the target (debuggee) system.
.np
Request message:
.millust begin
access_req      req
------------------------
string          dir_name
.millust end
.pp
The
.id dir_name
field contains the name of the directory to be created.
.np
Return message:
.millust begin
trap_error      err
.millust end
.pp
If error has occurred when creating the directory, the
.id err
field will return the error code number.
.*
.section REQ_RFX_RMDIR
.*
.np
Request to remove a directory on the target system.
.np
Request message:
.millust begin
access_req      req
------------------------
string          dir_name
.millust end
.pp
The
.id dir_name
field contains the name of the directory to be removed.
.np
Return message:
.millust begin
trap_error      err
.millust end
.pp
If error has occurred, the
.id err
field will return the error code number.
.*
.section REQ_RFX_SETDRIVE
.*
.np
Request to set the current drive on the target system.
.np
Request message:
.millust begin
access_req      req
unsigned_8      drive
.millust end
.pp
The
.id drive
field contains the drive number to be set on the target system (0=A,1=B,...).
.np
Return message:
.millust begin
trap_error      err
.millust end
.pp
If error has occurred, the
.id err
field will return the error code number.
.*
.section REQ_RFX_GETDRIVE
.*
.np
Request to get the current drive on the target system.
.np
Request message:
.millust begin
access_req      req
.millust end
.pp
The
.id req
field contains the request.
.np
Return message:
.millust begin
unsigned_8      drive
.millust end
.pp
The
.id drive
field returns the current drive number on the target system (0=A,1=B,...).
.*
.section REQ_RFX_SETCWD
.*
.np
Request to set a directory on the target system.
.np
Request message:
.millust begin
access_req      req
------------------------
string          dir_name
.millust end
.pp
The
.id dir_name
field contains the name of the directory to be set.
.np
Return message:
.millust begin
trap_error      err
.millust end
.pp
If error has occurred, the
.id err
field will return the error code number.
.*
.section REQ_RFX_GETCWD
.*
.np
Request to get the current directory name on the target system.
.np
Request message:
.millust begin
access_req      req
unsigned_8      drive
.millust end
.pp
The
.id drive
field contains the target drive number (0=current drive,1=A,2=B,...).
.np
Return message:
.millust begin
trap_error      err
--------------------------
string          dir_name
.millust end
.pp
The
.id dir_name
field contains the name of the directory to be set.
If error has occurred, the
.id err
field will return the error code number.
.*
.section REQ_RFX_SETDATETIME
.*
.np
Request to set a file's date and time information on the target system.
.np
Request message:
.millust begin
access_req      req
trap_fhandle    handle
time_t          time
.millust end
.pp
The
.id handle
contains the file handle.
The
.id time
field follows the UNIX time format.
The
.id time
represents the time since January 1, 1970 (UTC).
.np
Return message:
.millust begin
NONE
.millust end
.*
.section REQ_RFX_GETDATETIME
.*
.np
Request to get the date and time information for a file on the target system.
.np
Request message:
.millust begin
access_req      req
trap_fhandle    handle
.millust end
.pp
The
.id handle
contains the file handle.
.np
Return message:
.millust begin
time_t          time
.millust end
.pp
The
.id time
field follows the UNIX time format.
The
.id time
represents the time since January 1, 1970 (UTC).
.*
.section REQ_RFX_GETFREESPACE
.*
.np
Request to get the amount of free space left on the drive.
.np
Request message:
.millust begin
access_req      req
unsigned_8      drive
.millust end
.pp
The
.id drive
field contains the target drive number (0=current drive,1=A,2=B,...).
.np
Return message:
.millust begin
unsigned_32     size
.millust end
.pp
The
.id size
field returns the number of bytes left on the drive.
.*
.section REQ_RFX_SETFILEATTR
.*
.np
Request to set the file attribute of a file.
.np
Request message:
.millust begin
access_req      req
unsigned_32     attribute
-------------------------
string          name
.millust end
.pp
The
.id name
field contains the name whose attributes are to be set.
The
.id attribute
field contains the new attributes of the file.
.np
Return message:
.millust begin
trap_error      err
.millust end
.pp
If error has occurred, the
.id err
field will return the error code number.
.*
.section REQ_RFX_GETFILEATTR
.*
.np
Request to get the file attribute of a file.
.np
Request message:
.millust begin
access_req      req
--------------------
string          name
.millust end
.pp
The
.id name
field contains the name to be checked.
.np
Return message:
.millust begin
unsigned_32     attribute
.millust end
.pp
The
.id attribute
field returns the attribute of the file.
.*
.section REQ_RFX_NAMETOCANONICAL
.*
.np
Request to convert a file name to its canonical form.
.np
Request message:
.millust begin
access_req      req
-------------------------
string          file_name
.millust end
.pp
The
.id file_name
field contains the file name to be converted.
.np
Return message:
.millust begin
trap_error      err
--------------------------
string          path_name
.millust end
.pp
If there is no error, the
.id err
field returns a zero and the full path name will be stored in the
.id path_name
field.
When an error has occurred, the
.id err
field contains an error code indicating the type of error that has been
detected.
.*
.section REQ_RFX_FINDFIRST
.*
.np
Request to find the first file in a directory.
.np
Request message:
.millust begin
access_req      req
unsigned_8      attrib
----------------------
string          name
.millust end
.pp
The
.id name
field contains the name of the directory and the
.id attrib
field contains the attribute of the files to list in the directory.
.np
Return message:
.millust begin
trap_error      err
-----------------------
rfx_find        info
.millust end
.pp
If found, the
.id err
field will be zero.
The location and information of about the first file will be in the structure
.id info
.period
Definition of the structure
.id rfx_find
is as follows:
.millust begin
typedef struct rfx_find {
    unsigned_8          reserved[21];
    unsigned_8          attr;
    unsigned_16         time;
    unsigned_16         date;
    unsigned_32         size;
    unsigned_8          name[260];
} rfx_find;
.millust end
.*
.section REQ_RFX_FINDNEXT
.*
.np
Request to find the next file in the directory.
This request should be used only after REQ_RFX_FINDFIRST.
.np
Request message:
.millust begin
access_req      req
--------------------
rfx_find        info
.millust end
.pp
The
.id req
field contains the request.
The
.id info
field contains the rfx_find structure returned from the previous REQ_FIND_NEXT
or REQ_FIND_FIRST.
.np
Return message:
.millust begin
trap_error      err
-----------------------
rfx_find        info
.millust end
.pp
The
.id info
field is the same as in REQ_FIND_FIRST.
.*
.section REQ_RFX_FINDCLOSE
.*
.np
Request to end the directory search operation.
.np
Request message:
.millust begin
access_req      req
.millust end
.pp
The
.id req
field contains the request.
.np
Return message:
.millust begin
trap_error      err
.millust end
.pp
If successful, the
.id err
field will be zero, otherwise the system error code will be returned.
.endlevel
.*
.*
.section Environment requests
.*
.np
This section describes requests that deal with Environment on the
target (debuggee) machine.
These requests are actually performed by the
core request REQ_PERFORM_SUPPLEMENTARY_SERVICE and appropriate service ID.
The following descriptions do not show that "prefix" to the
request messages.
.np
The service name to be used in the REQ_GET_SUPPLEMENTARY_SERVICE is
"Environment".
.*
.beglevel
.*
.section REQ_ENV_GET_VAR
.*
.np
Request to retreive Environment variable from the remote system.
.np
Request message:
.millust begin
access_req      req
------------------------
string          name
.millust end
.np
Return message:
.millust begin
trap_error      err
string          value;
.millust end
.pp
The
.id value
contains the value for
.id name
Environment variable from the remote system.
.*
.section REQ_ENV_SET_VAR
.*
.np
Request to set Environment variable on the remote system.
.np
Request message:
.millust begin
access_req      req
------------------------
string          name
string          value;
.millust end
.np
Return message:
.millust begin
trap_error      err
.millust end
.pp
The
.id value
contains the new value for Environment variable
.id name
on the remote system.
.endlevel
.*
.*
.section File Info requests
.*
.np
.beglevel
.*
.section REQ_FILE_INFO_GET_DATE
.*
.np
Request message:
.millust begin
access_req      req
.millust end
.np
Return message:
.millust begin
trap_error      err
.millust end
.pp
.*
.section REQ_FILE_INFO_SET_DATE
.*
.np
Request message:
.millust begin
access_req      req
.millust end
.np
Return message:
.millust begin
trap_error      err
.millust end
.pp
.endlevel
.*
.*
.section Asynchronous Debugging requests
.*
.np
.beglevel
.*
.section REQ_ASYNC_GO
.*
.np
Request message:
.millust begin
access_req      req
.millust end
.np
Return message:
.millust begin
trap_error      err
.millust end
.pp
.*
.section REQ_ASYNC_STEP
.*
.np
Request message:
.millust begin
access_req      req
.millust end
.np
Return message:
.millust begin
trap_error      err
.millust end
.pp
.*
.section REQ_ASYNC_POLL
.*
.np
Request message:
.millust begin
access_req      req
.millust end
.np
Return message:
.millust begin
trap_error      err
.millust end
.pp
.*
.section REQ_ASYNC_STOP
.*
.np
Request message:
.millust begin
access_req      req
.millust end
.np
Return message:
.millust begin
trap_error      err
.millust end
.pp
.*
.section REQ_ASYNC_ADD_BREAK
.*
.np
Request message:
.millust begin
access_req      req
.millust end
.np
Return message:
.millust begin
trap_error      err
.millust end
.pp
.*
.section REQ_ASYNC_REMOVE_BREAK
.*
.np
Request message:
.millust begin
access_req      req
.millust end
.np
Return message:
.millust begin
trap_error      err
.millust end
.pp
.endlevel
.*
.*
.section Non-blocking Thread requests
.*
.np
.beglevel
.*
.section REQ_RUN_THREAD_INFO
.*
.np
Request message:
.millust begin
access_req      req
.millust end
.np
Return message:
.millust begin
trap_error      err
.millust end
.pp
.*
.section REQ_RUN_THREAD_GET_NEXT
.*
.np
Request message:
.millust begin
access_req      req
.millust end
.np
Return message:
.millust begin
trap_error      err
.millust end
.pp
.*
.section REQ_RUN_THREAD_GET_RUNTIME
.*
.np
Request message:
.millust begin
access_req      req
.millust end
.np
Return message:
.millust begin
trap_error      err
.millust end
.pp
.*
.section REQ_RUN_THREAD_POLL
.*
.np
Request message:
.millust begin
access_req      req
.millust end
.np
Return message:
.millust begin
trap_error      err
.millust end
.pp
.*
.section REQ_RUN_THREAD_SET
.*
.np
Request message:
.millust begin
access_req      req
.millust end
.np
Return message:
.millust begin
trap_error      err
.millust end
.pp
.*
.section REQ_RUN_THREAD_GET_NAME
.*
.np
Request message:
.millust begin
access_req      req
.millust end
.np
Return message:
.millust begin
trap_error      err
.millust end
.pp
.*
.section REQ_RUN_THREAD_STOP
.*
.np
Request message:
.millust begin
access_req      req
.millust end
.np
Return message:
.millust begin
trap_error      err
.millust end
.pp
.*
.section REQ_RUN_THREAD_SIGNAL_STOP
.*
.np
Request message:
.millust begin
access_req      req
.millust end
.np
Return message:
.millust begin
trap_error      err
.millust end
.pp
.endlevel
.*
.*
.section Capabilities requests
.*
.np
.beglevel
.*
.section REQ_CAPABILITIES_GET_8B_BP
.*
.np
Request message:
.millust begin
access_req      req
.millust end
.np
Return message:
.millust begin
trap_error      err
.millust end
.pp
.*
.section REQ_CAPABILITIES_SET_8B_BP
.*
.np
Request message:
.millust begin
access_req      req
.millust end
.np
Return message:
.millust begin
trap_error      err
.millust end
.pp
.*
.section REQ_CAPABILITIES_GET_EXACT_BP
.*
.np
Request message:
.millust begin
access_req      req
.millust end
.np
Return message:
.millust begin
trap_error      err
.millust end
.pp
.*
.section REQ_CAPABILITIES_SET_EXACT_BP
.*
.np
Request message:
.millust begin
access_req      req
.millust end
.np
Return message:
.millust begin
trap_error      err
.millust end
.pp
.endlevel
.*
.*
.chap System Dependent Aspects
.*
Every environment has a different method of loading the code for the trap
file and locating the TrapInit, TrapRequest, and TrapFini routines.
This section descibes how the &company debugger performs these operations
for the various systems.
.*
.section Trap Files Under DOS
.*
.np
A trap file is an "EXE" format file with the extension ".TRP".
The debugger searches the directories specified by the PATH
environment variable.
Once found, it is loaded into memory and has the normal EXE style relocations
applied to the image.
Then the lowest address in the load image (NOTE: not the starting address
from EXE header information) is examined for the following structure:
.millust begin
typedef struct {
    unsigned_16     signature; /* == 0xDEAF */
    unsigned_16     init_off;
    unsigned_16     acc_off;
    unsigned_16     fini_off;
} trap_header;
.millust end
.pp
If the first 2 bytes contain the value 0xDEAF, the file is considered to
be a valid trap file and the
.id init_off
,
.id acc_off
, and
.id fini_off
fields are used to obtain the offsets of the TrapInit, TrapRequest, and
TrapFini routines repectively.
.np
The starting address field of the EXE header should be set to point at
some code which prints out a message about not being able to be run from
the command line and then terminates.
.*
.section Trap Files Under OS/2
.*
.np
A trap file is a normal OS/2 DLL.
The system automatically searches the directories specified by the LIBPATH
command in the CONFIG.SYS file.
Once loaded, the &company debugger uses export ordinal 1 from the DLL for
TrapInit, export ordinal 2 for TrapFini and export ordinal 3 for TrapRequest.
Some example code follows:
.millust begin
rc = DosLoadModule( NULL, 0, trap_file_name, &dll_module );
if( rc != 0 ) {
    return( "unable to load trap file" );
}
if( DosGetProcAddr( dll_module, "#1", &TrapInit ) != 0
 || DosGetProcAddr( dll_module, "#2", &TrapFini ) != 0
 || DosGetProcAddr( dll_module, "#3", &TrapRequest ) != 0 ) {
    return( "incorrect version of trap file" );
}
.millust end
.*
.section Trap Files Under Windows.
.*
.np
A trap file is a normal Windows DLL.
The system automatically searches the directories specified by the PATH
environment variable.
Once loaded, the &company debugger uses export ordinal 2 from the DLL for
TrapInit, export ordinal 3 for TrapFini and export ordinal 4 for TrapRequest.
Some example code follows:
.millust begin
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
.millust end
.*
.section Trap Files Under Windows NT.
.*
.np
A trap file is a normal Windows NT DLL.
The system automatically searches the directories specified by the PATH
environment variable.
Once loaded, the &company debugger uses export ordinal 1 from the DLL for
TrapInit, export ordinal 2 for TrapFini and export ordinal 3 for TrapRequest.
Some example code follows:
.millust begin
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
.millust end
.*
.section Trap Files Under QNX
.*
.np
A trap file is a QNX load module format file with the extension ".trp"
and whose file permissions are not marked as executable.
The debugger searches the directories specified by the WD_PATH environment
variable and then the "/usr/watcom/wd" directory.
Once found, it is loaded into memory and has the normal loader relocations
applied to the image.
Then the lowest address in the load image (NOTE: not the starting address
from load module header information) is examined for the following structure:
.millust begin
typedef struct {
    unsigned_16     signature; /* == 0xDEAF */
    unsigned_16     init_off;
    unsigned_16     acc_off;
    unsigned_16     fini_off;
} trap_header;
.millust end
.pp
If the first 2 bytes contain the value 0xDEAF, the file is considered to
be a valid trap file and the
.id init_off
,
.id acc_off
, and
.id fini_off
fields are used to obtain the offsets of the TrapInit, TrapRequest, and
TrapFini routines repectively.
.np
The starting address field of the load image header should be set to point at
some code which prints out a message about not being able to be run from
the command line and then terminates.
.*
.section Trap Files Under Netware 386 or PenPoint
.*
.np
The trap file routines are linked directly into the remote server code and
TrapInit, TrapRequest, TrapFini are directly called.
