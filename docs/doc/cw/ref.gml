.*
.* Macros for API heading boxes, api:eapi for API name, dsc:edsc for 
.* description.
.*
.dm @api begin
:HP2.&*.
.dm @api end
.gt api add @api cont
.*
.dm @eapi begin
:eHP2.&*
.dm @eapi end
.gt eapi add @eapi cont
.*
.*
.dm @dsc begin
:HP0.&*.
.dm @dsc end
.gt dsc add @dsc cont
.*
.dm @edsc begin
:eHP0.&*
.dm @edsc end
.gt edsc add @edsc cont
.*
.*
.*
.* Error message macros.
.*
.dm errsect begin
.if &e'&dohelp eq 0 .do begin
.   .begnote
.do end
.dm errsect end
.*
.dm eerrsect begin
.if &e'&dohelp eq 0 .do begin
.   .endnote
.do end
.dm eerrsect end
.*
.dm errnote begin
.se *ttl$=&'substr(&*,&'pos(' ',&*)+1)
.se *pos=0
.if &e'&dohelp. eq 0 .do begin
.   .note Error &*1
.   .bd &*ttl$
.   .ix 'Error' '&*1'
.do end
.el .do begin
:ZH2.&*
.   .cntents &*
.   .pu 1 .ixsect &*
:ZI1.&*ttl$
.   .pu 1 .ixline `&*ttl$`
.   .se *ctxn=&'substr(&*,1,&'pos(' ',&*)-1)
.   .se *pos=&'pos(',',&*1)
.   .if &*pos. ne 0 .do begin
.   .   .se *ctxn=&'substr(&*,1,&*pos.)
.   .   .se *ttl$=&'substr(&*,&*pos.+1)
:ZI1.&*ttl$
.   .   .pu 1 .ixline `&*ttl$`
.   .do end
.   .if '&dotarget' eq 'os2' .do begin
.   .   .if &'length(&*ctxn.) eq 3 .do begin
.   .   .   .se *ctxn='20&*ctxn.'
.   .   .do end
.   .   .el .do begin
.   .   .   .se *ctxn='2&*ctxn.'
.   .   .do end
.   .do end
.   .ctxstr &*
.   .pu 2 #define HLP_&'upper(&ctx_str.) &*ctxn
.*  .np
.*  .us &*
.do end
.dm errnote end
.*
.*
.*
.chap CauseWay Services
.*
.np
The following information describes the services available through
CauseWay for low-level protected mode compatible functions and interrupt
servicing.
.np
The CauseWay services support both 16- and 32-bit selectors. Use of
32-bit selectors allows developers to directly access many megabytes of
memory in a CauseWay program using only one selector value.  In addition
to the normal segment registers used in real mode programs, the
additional segment registers FS and GS are always available for use by
developers to access memory.  Refer to a 386, 486, Pentium or compatible
CPU reference book or manual for more information on 386+ level registers and
instructions.
.*
.section Internal Operation
.*
.np
A valid protected mode selector:offset is placed in the PSP at offset
34h for the file handle list pointer. Note that the default value in the
program's PSP will point to the real mode PSP, not the protected mode
PSP, even if the handle count is less than or equal to twenty. Code that
makes use of the handle list should use the address at PSP+34h rather
than assuming the list's position within the PSP.  Also, when an
application is operating in non-DPMI conditions the handle table will
have an entry for CauseWay's VMM swap file.
.np
The GetMem and GetMem32 calls with CX:DX and ECX set to -1 will report
the largest free memory block, rather than total free memory.  This
value may be substantially lower than total free memory due to
fragmentation of the linear memory blocks.  Set CX to -1 (0FFFFH) and DX
to -2 (0FFFEH) or ECX to -2 (0FFFFFFFEH) for GetMem and GetMem32,
respectively, for the total free memory value.
.*
.section Functions
.*
.np
.ix 'DPMI'
The CauseWay functions are based on the DPMI specification and offer
additional enhancements. This means that several of the DPMI 1.0
functions are available to the CauseWay programmer in all systems
providing access to lower level functions should you need them. All DPMI
0.9 functions are always available.
.*
.section CauseWay API
.*
.np
CauseWay provides an API for C and assembly language programmers as an
extension of the DPMI API via INT 31h.  Including the file CW.INC allows
easy access to this API through appropriately named functions.  You may
also call the CauseWay API directly with appropriate register setup.
.np
.ix 'DPMI host'
CauseWay also provides all of the DPMI 0.9 API services on systems
without a true DPMI server (thus CauseWay itself is the DPMI host in
such situations).
.np
.ix 'register list'
Some of the API services require pointers to a real mode register list.
The format of this list follows:
.millust begin
dword     EDI
dword     ESI
dword     EBP
dword     Reserved
dword     EBX
dword     EDX
dword     ECX
dword     EAX
word      Flags
word      ES
word      DS
word      FS
word      GS
word      IP
word      CS
word      SP
word      SS
.millust end
The values are passed to the target routine without any interpretation
of their contents.  There is no need to set the high words of the
extended register entries unless the target routine requires them.
.*
.section Stack Frames
.*
.np
Stack frames for 16-bit interrupts are the same as for real mode.
.np
The stack frame for 16-bit exceptions follows:
.millust begin
word SS
word SP       - Original stack address
word Flags
word CS
word IP       - Original Flags:CS:IP values
word Err Code - Processor supplied exception error code
word CS
word IP       - Return address, returns to
                interrupt/exception dispatch code
.millust end
.*
.section Default API
.*
.np
.ix 'CauseWay API'
The default CauseWay API follows.  Functions that include the text
.mono near
are intended only for backwards compatibility with CauseWay's near
memory model.  This model is now obsolete.  The assembly language
include file *CW.INC* also contains this list.
.*
.beglevel
.*
.section API functions (numerical index)
.*
.np
.ix 'CauseWay API numerical index'
.begnote $setptnt 15
.note FF00 Info
Get system selectors/flags
.note FF01 IntXX
Simulate real mode interrupt
.note FF02 FarCallReal
Simulate real mode far call
.note FF03 GetSel
Allocate a new selector
.note FF04 RelSel
Release a selector
.note FF05 CodeSel
Make a selector execute/read type
.note FF06 AliasSel
Create a read/write data selector from source selector
.note FF07 GetSelDet
Get selector linear base and limit
.note FF08 GetSelDet32
Get selector linear base and limit
.note FF09 SetSelDet
Set selector linear base and limit
.note FF0A SetSelDet32
Set selector linear base and limit
.note FF0B GetMem
Allocate a block of memory
.note FF0C GetMem32
Allocate a block of memory
.note FF0D ResMem
Resize a previously allocated block of memory
.note FF0E ResMem32
Resize a previously allocated block of memory
.note FF0F RelMem
Release memory allocated by either GetMem or GetMem32
.note FF10 GetMemLinear
Allocate a block of memory without a selector
.note FF11 GetMemLinear32
Allocate a block of memory without a selector
.note FF12 ResMemLinear
Resize a previously allocated block of memory without a selector
.note FF13 ResMemLinear32
Resize a previously allocated block of memory without a selector
.note FF14 RelMemLinear
Release previously allocated block of memory (linear address)
.note FF15 RelMemLinear32
Release previously allocated block of memory (linear address)
.note FF16 GetMemNear
Deprecated - Allocate an application relative block of memory
.note FF17 ResMemNear
Deprecated - Resize a previously allocated application relative block of memory
.note FF18 RelMemNear
Deprecated - Release previously allocated application relative block of memory
.note FF19 Linear2Near
Deprecated - Convert linear address to application relative address
.note FF1A Near2Linear
Deprecated - Convert application relative address to linear address
.note FF1B LockMem
Lock a region of memory
.note FF1C LockMem32
Lock a region of memory
.note FF1D UnLockMem
Unlock a region of memory
.note FF1E UnLockMem32
Unlock a region of memory
.note FF1F LockMemNear
Deprecated - Lock a region of memory using application relative address
.note FF20 UnLockMemNear
Deprecated - Unlock a region of memory using application relative address
.note FF21 GetMemDOS
Allocate a region of DOS (conventional) memory
.note FF22 ResMemDOS
Resize a block of DOS (conventional) memory allocated with GetMemDOS
.note FF23 RelMemDOS
Release a block of DOS (conventional) memory allocated by GetMemDOS
.note FF24 Exec
Run another CauseWay program directly
.note FF25 GetDOSTrans
Get current address and size of the buffer used for DOS memory transfers
.note FF26 SetDOSTrans
Set new address and size of the buffer used for DOS memory transfers
.note FF27 GetMCBSize
Get current memory control block (MCB) memory allocation block size
.note FF28 SetMCBSize
Set new MCB memory allocation block size
.note FF29 GetSels
Allocate multiple selectors
.note FF2A cwLoad
Load another CauseWay program as an overlay
.note FF2B cwcInfo
Validate and get expanded length of a CWC'ed file
.note FF2C GetMemSO
Allocate a block of memory with selector:offset
.note FF2D ResMemSO
Resize a block of memory allocated via GetMemSO
.note FF2E RelMemSO
Release a block of memory allocated via GetMemSO
.note FF2F UserDump
Setup user-defined error buffer dump in CW.ERR
.note FF30 SetDump
Disable/enable error display and CW.ERR creation
.note FF31 UserErrTerm
Call user error termination routine
.note FF32 CWErrName
Change error file name, with optional drive/pathspec
.*
.note FFF9 ID
Get CauseWay identifier, PageDIRLinear and Page1stLinear info
.note FFFA GetPatch
Get patch table address
.note FFFB cwcLoad
Load/Expand a CWC'ed data file into memory
.note FFFC LinearCheck
Check linear address of memory
.note FFFD ExecDebug
Load CauseWay program for debug
.note FFFE CleanUp
Close all open file handles
.endnote
.*
.section API functions (alphabetical order)
.*
.np
.ix 'CauseWay API reference'
.mbox begin
:api.AliasSel:eapi. :dsc.Create a read/write data selector from source selector.:edsc.
.mbox end
.begnote
.note Inputs:
AX= 0ff06h
.br
BX= Source selector
.note Outputs:
Carry set on error, else
.br
AX= New data selector
.note Errors:
If an invalid selector is passed in BX, this function
returns with carry set.
.note Notes:
This function always creates a read/write data
selector regardless of the source selector type.  It can be used to
provide write access to variables in a code segment.
.endnote
.*
.mbox begin
:api.CleanUp:eapi. :dsc.Close all open file handles.:edsc.
.mbox end
.begnote
.note Inputs:
AX= 0fffeh
.note Outputs:
None.
.note Errors:
None.
.endnote
.*
.mbox begin
:api.CodeSel:eapi. :dsc.Make a selector execute/read type.:edsc.
.mbox end
.begnote
.note Inputs:
AX= 0ff05h
.br
BX= Selector
.br
CL= Default operation size. (0=16-bit,1=32-bit)
.note Outputs:
Carry set on error.
.note Errors:
If an invalid selector is passed in BX, this function
returns with carry set.
.note Notes:
This functions allows a selector to be converted to a
type suitable for execution.
.endnote
.*
.mbox begin
:api.cwcInfo:eapi. :dsc.Validate and get expanded length of a CWC'd file.:edsc.
.mbox end
.begnote
.note Inputs:
AX= 0ff2bh
.br
BX= File handle.
.note Outputs:
Carry set if not a CWC'd file, else
.br
ECX= Expanded data size.
.note Errors:
None.
.note Notes:
The file pointer is not altered by this function.
.endnote
.*
.mbox begin
:api.cwcLoad:eapi. :dsc.Load/Expand a CWC'ed data file into memory.:edsc.
.mbox end
.begnote
.note Inputs:
AX= 0fffbh
.br
BX= Source file handle.
ES:EDI= Destination memory.
.note Outputs:
Carry set on error and EAX is error code, else
.br
ECX= Expanded data length.
.note Errors:
1 - Error during file access.
.br
2 - Bad data.
.br
3 - Not a CWC'ed file.
.note Notes:
The source file's file pointer doesn't have to be at zero. A single file might
be several CWC'ed files lumped together, as long as the file pointer is moved
to the right place before calling this function.
.br
If error codes 1 or 2 are reported then the file pointer will be wherever it
was last moved to by this function. For error code 3 the file pointer will be
back at its original position on entry to this function. If no error occurs
then the file pointer will be moved to whatever comes after the compressed
data.
.endnote
.*
.mbox begin
:api.CWErrName:eapi. :dsc.Change error file name, with optional drive/pathspec.:edsc.
.mbox end
.begnote
.note Inputs:
AX = 0ff32h
.br
CX:[E]DX = selector:offset of ASCIIZ error file name
.note Outputs:
None
.note Errors:
None
.note Notes:
If the error file name is invalid when a fault occurs,
CauseWay defaults to using the standard CW.ERR file name in the current
directory.  The file name including any path and drive must not exceed
80 characters or it will be truncated.  CX:EDX are not checked for
validity and passing invalid values may cause a fault within the DOS
extender.  The ASCIIZ name pointed to by CX:EDX is copied to an internal
DOS extender location and may be safely modified after calling the
CWErrName function.
.endnote
.*
.mbox begin
:api.cwLoad:eapi. :dsc.Load another CauseWay program as an overlay.:edsc.
.mbox end
.begnote
.note Inputs:
AX= 0ff2ah
.br
DS:EDX= File name.
.note Outputs:
Carry set on error and AX = error code, else
.br
CX:EDX= Entry CS:EIP
.br
BX:EAX= Entry SS:ESP
.br
SI= PSP.
.note Errors:
1 - DOS file access error.
.br
2 - Not recognisable file format.
.br
3 - Not enough memory.
.note Notes:
Program is loaded into memory, but not executed.
.br
The PSP returned in SI can be passed to RelMem to release the loaded programs
memory and selectors. Only the memory and selectors allocated during loading
will be released, it is the programs responsability to release any additional
memory etc allocated while the program is running. Alternatively, if you pass
the PSP value to INT 21h, AH=50h before makeing additional memory requests
and then reset to the origional PSP the memory allocated will be released
when the PSP is released.
.endnote
.*
.mbox begin
:api.Exec:eapi. :dsc.Run another CauseWay program directly.:edsc.
.mbox end
.begnote
.note Inputs:
AX= 0ff24h
.br
DS:[E]DX= File name.
.br
ES:[E]SI= Command line. First byte is length, then real data.
.br
CX= Environment selector, 0 to use existing copy. 
.note Outputs:
Carry set on error and AX = error code, else
.br
AL=ErrorLevel.
.note Errors:
1 - DOS file access error.
.br
2 - Not recognisable file format.
.br
3 - Not enough memory.
.note Notes:
Only the first byte of the command line (length) has any significance to
CauseWay so you are not restricted to ASCII values. It is still stored in the
PSP at 80h though so the length is still limited to 127 bytes.
.endnote
.*
.mbox begin
:api.ExecDebug:eapi. :dsc.Load CauseWay program for debug.:edsc.
.mbox end
.begnote
.note Inputs:
AX= 0fffdh
.br
DS:EDX= File name.
.br
ES:ESI= Command line. First byte is length, then real data.
.br
CX= Environment selector, 0 to use existing copy. 
.note Outputs:
Carry set on error and AX = error code, else
.br
CX:EDX= Entry CS:EIP
.br
BX:EAX= Entry SS:ESP
.br
SI= PSP.
.br
DI= Auto DS.
.br
EBP= Segment definition memory.
.note Errors:
1 - DOS file access error.
.br
2 - Not recognisable file format.
.br
3 - Not enough memory.
.endnote
.*
.mbox begin
:api.FarCallReal:eapi. :dsc.Simulate real mode far call.:edsc.
.mbox end
.begnote
.note Inputs:
AX= 0ff02h
.br
ES:[E]DI= Real mode register list.
.note Outputs:
Register list updated.
.note Errors:
None.
.note Notes:
This function works much the same as IntXX but
provides a 16 bit FAR stack frame and the CS:IP values are used to pass
control to the real mode code.
.endnote
.*
.mbox begin
:api.GetCallBack:eapi. :dsc.Allocate real mode call back address.:edsc.
.mbox end
.begnote
.note Inputs:
AX= 0303h
.br
DS:[E]SI= Call address.
.br
ES:[E]DI= Real mode register structure.
.note Outputs:
Carry set on error, else
.br
CX:DX= Real mode address to trigger mode switch.
.note Errors:
Call-backs are a limited resource. Normally only 16
are available per virtual machine.  Use them carefully and release them
as soon as they are no longer required.
.note Call-Back:
Interrupts disabled.
.br
DS:[E]SI = Selector:Offset of real mode SS:SP.
.br
ES:[E]DI = Selector:Offset of real mode call structure.
.br
SS:[E]SP = Locked protected mode stack.
.br
All other registers undefined.
.br
To return from call-back procedure, execute an
IRET to return.
.br
ES:[E]DI =  Selector:Offset of real mode call
structure to restore.
.note Notes:
Real mode call-backs provide a means of switching from
real mode to protected mode. This function returns a unique real mode
address that when given control in real mode, switches to protected mode
and passes control to the protected mode routine supplied at entry to
this function.  On entry to the protected mode code the real mode
register structure contains all the real mode register values.
.endnote
.*
.mbox begin
:api.GetDOSTrans:eapi. :dsc.Get current address and size of the buffer used for DOS memory transfers.:edsc.
.mbox end
.begnote
.note Inputs:
AX = 0ff25h
.note Outputs:
BX = Real mode segment of buffer.
.br
DX = Protected mode selector for buffer.
.br
ECX = Buffer size
.note Errors:
None
.note Notes:
This buffer is used by the INT API translation
services, e.g., INT 21h, AH=40h (write to file). The default buffer is
8K and uses memory that would otherwise be wasted. This default is
sufficient for most file I/O but if you are writing a program that
reads/writes large amounts of data you should consider allocating your
own larger buffer and pass the buffer's address to CauseWay to speed
file I/O.
.endnote
.*
.mbox begin
:api.GetEVect:eapi. :dsc.Get Protected mode exception handler address.:edsc.
.mbox end
.begnote
.note Inputs:
AX= 0202h
.br
BL= Exception vector number.
.note Outputs:
Carry set on error, else
.br
CX:[E]DX= selector:offset of handler.
.note Errors:
The number in BL must be in the range 0-1Fh.  Anything
outside this range returns carry set.
.endnote
.*
.mbox begin
:api.GetMCBSize:eapi. :dsc.Get current memory control block (MCB) memory allocation block size.:edsc.
.mbox end
.begnote
.note Inputs:
AX = 0ff27h
.note Outputs:
ECX = Current threshold
.note Errors:
None
.note Notes:
See SetMCBMax
.endnote
.*
.mbox begin
:api.GetMem:eapi. :dsc.Allocate a block of memory.:edsc.
.mbox end
.begnote
.note Inputs:
AX= 0ff0bh
.br
CX:DX= Size of block required in bytes. (use
-1:-1 to get maximum memory size)
.note Outputs:
Carry set on error, else
.br
BX= Selector to access the block with or if CX:DX
was -1,
.br CX:DX= size of largest block available.
.note Errors:
The amount of memory available is limited by physical
memory present and free disk space of the drive being used by the VMM.
If CauseWay is unable to find a large enough block this function returns
carry set.
.note Notes:
This function allocates a block of extended
(application) memory and allocates a selector with a suitable base and
limit.
.endnote
.*
.mbox begin
:api.GetMem32:eapi. :dsc.Allocate a block of memory.:edsc.
.mbox end
.begnote
.note Inputs:
AX= 0ff0ch
.br
ECX= Size of block required in bytes. (-1 to get
maximum memory size)
.note Outputs:
Carry set on error, else
.br
BX= Selector to access the block with or if ECX was -1,
.br
ECX= size of largest block available.
.note Errors:
See GetMem
.note Notes:
This function allocates a block of extended
(application) memory and allocates a selector with a suitable base and
limit.
.endnote
.*
.mbox begin
:api.GetMemDOS:eapi. :dsc.Allocate a region of DOS (conventional) memory.:edsc.
.mbox end
.begnote
.note Inputs:
AX= 0ff21h
.br
BX= Number of paragraphs (16 byte blocks) required.
.note Outputs:
Carry set on error and BX= largest block size,
.br
AX=DOS error, else
.br
AX= Initial real mode segment of allocated block
.br
DX= Initial selector for allocated block
.note Errors:
If there are not enough selectors or memory available
then this function returns carry set.
.note Notes:
If the size of the block requested is greater than 64KB
bytes (BX >1000h) then contiguous descriptors are allocated.  If more
than one descriptor is allowed under 32-bit applications, the limit of
the first descriptor is set to the size of the entire block.  All
subsequent descriptors have a limit of 64KB except for the final
descriptor which has a limit of block size modulo 64KB. For 16-bit
applications, always set the limit of the first descriptor to 64KB.
.endnote
.*
.mbox begin
:api.GetMemLinear:eapi. :dsc.Allocate a block of memory without a selector.:edsc.
.mbox end
.begnote
.note Inputs:
AX= 0ff10h
.br
CX:DX= Size of block required in bytes.
.note Outputs:
Carry set on error, else
.br
SI:DI= Linear address of block allocated.
.note Errors:
If not enough memory is available to satisfy the
request then this function returns carry set.
.note Notes:
Addresses returned by this function may be above 16MB.
.endnote
.*
.mbox begin
:api.GetMemLinear32:eapi. :dsc.Allocate a block of memory without a selector.:edsc.
.mbox end
.begnote
.note Inputs:
AX= 0ff11h
.br
ECX= Size of block required in bytes.
.note Outputs:
Carry set on error, else
.br
ESI= Linear address of block allocated.
.note Errors:
See GetMemLinear
.note Notes:
Addresses returned by this function may be above 16MB.
.endnote
.*
.mbox begin
:api.GetMemSO:eapi. :dsc.Allocate a block of memory with selector:offset.:edsc.
.mbox end
.begnote
.note Inputs:
AX = 0ff2ch
.br
CX:DX = Size of block required in bytes
.note Outputs:
Carry set on error, else
.br
SI:DI = selector:offset of allocated memory
.note Errors:
See GetMem
.note Notes:
This function allocates a block of memory with an
associated selector:offset.  The allocation does not consume a selector
on each call as does GetMem because a non-zero offset from an existing
selector for this allocation type is returned.  GetMemSO is useful for
applications which make a large number of allocations where running out
of selectors with GetMem could be a problem.  A potential drawback is
that memory accesses beyond the allocation boundary may go undetected
since the selector is shared among several allocations.  Also, resizing
the block can change the selector:offset of the block.
.endnote
.*
.mbox begin
:api.GetPatch:eapi. :dsc.Get patch table address.:edsc.
.mbox end
.begnote
.note Inputs:
AX= 0fffah
.note Outputs:
EDX= Linear address of patch table.
.note Errors:
None.
.endnote
.*
.mbox begin
:api.GetRVect:eapi. :dsc.Get real mode interrupt handler address.:edsc.
.mbox end
.begnote
.note Inputs:
AX= 0200h
.br
BL= Interrupt vector number.
.note Outputs:
CX:DX= selector:offset of handler.
.note Errors:
None.
.endnote
.*
.mbox begin
:api.GetSel:eapi. :dsc.Allocate a new selector.:edsc.
.mbox end
.begnote
.note Inputs:
AX= 0ff03h
.note Outputs:
Carry set on error, else
.br
BX= Selector.
.note Errors:
Approximately 8192 selectors are available initially.
While this is a relatively large quantity, it is obviously possible
to run out if the system is heavily loaded or selectors are being wasted.
.note Notes:
A selector is allocated and initialized with a base of
0, a limit of 0 and as read/write expand up data.  Use SetSelDet to make
the selector useful, setting an appropriate base and limit.
.endnote
.*
.mbox begin
:api.GetSelDet:eapi. :dsc.Get selector linear base and limit.:edsc.
.mbox end
.begnote
.note Inputs:
AX= 0ff07h
.br
BX= Selector
.note Outputs:
Carry set on error, else
.br
CX:DX= Linear base.
.br
SI:DI= Byte granular limit.
.note Errors:
If an invalid selector is passed in BX, this function
returns with carry set.
.endnote
.*
.mbox begin
:api.GetSelDet32:eapi. :dsc.Get selector linear base and limit.:edsc.
.mbox end
.begnote
.note Inputs:
AX= 0ff08h
.br
BX= Selector
.note Outputs:
Carry set on error, else
.br
EDX= Linear base.
.br
ECX= Byte granular limit.
.note Errors:
If an invalid selector is passed in BX, this function
returns with carry set.
.endnote
.*
.mbox begin
:api.GetSels:eapi. :dsc.Allocate multiple selectors.:edsc.
.mbox end
.begnote
.note Inputs:
AX= 0ff29h
.br
CX= Number of selectors. 
.note Outputs:
BX= Base selector.
.note Errors:
None.
.note Notes:
The selectors are allocated and initialised with a base of 0, a limit of 0 and
as read/write expand up data. Use SetSelDet to make the selectors useful.
.endnote
.*
.mbox begin
:api.GetVect:eapi. :dsc.Get Protected mode interrupt handler address.:edsc.
.mbox end
.begnote
.note Inputs:
AX= 0204h
.br
BL= Interrupt vector number.
.note Outputs:
CX:[E]DX= selector:offset of handler.
.note Errors:
None.
.endnote
.*
.mbox begin
:api.ID:eapi. :dsc.Get CauseWay identifier, PageDIRLinear and Page1stLinear info.:edsc.
.mbox end
.begnote
.note Inputs:
AX= 0fff9h
.note Outputs:
ECX:EDX= CauseWay identifies.
.br
ESI= Linear address (PageDIRLinear)
.br
EDI= Linear address (Page1stLinear)
.note Errors:
None.
.endnote
.*
.mbox begin
:api.Info:eapi. :dsc.Get system selectors/flags.:edsc.
.mbox end
.begnote
.note Inputs:
AX= 0ff00h
.note Outputs:
AX= Selector for real mode segment address of 00000h, 4GB limit.
.br
BX= Selector for current PSP segment. 100h limit.
.br
[E]CX= DOS transfer buffer size. Always <64KB.
.br
DX= DOS transfer buffer real mode segment address.
.br
ES:[E]SI= DOS transfer buffer address.
.br
ESI+ECX always <64KB
.br
EDI= System flags. Bits significant if set.
.br
0 - 32 bit code default.
.br
1 - Virtual memory manager functional.
.br
2-3 - Mode, 0 - raw, 1 - VCPI, 2 - DPMI.
.br
4 - DPMI available.
.br
5 - VCPI available.
.br
6 - No memory managers.
.br
7 - Descriptor table type. 0 - GDT, 1 - LDT.
.note Errors:
None
.note Notes:
Bits 1-2 of DI indicate the interface type being used
by CauseWay.
.np
Bits 4-5 indicate the interface types that are
available.  Bit 7 indicates the descriptor table being used to allocate
selectors to  the application when on a raw/VCPI system. The DOS
transfer buffer is the area CauseWay uses to transfer data between
conventional and extended memory during DOS interrupts.  This memory can
be used as temporary work space to access real mode code as long as you
remember it may be overwritten the next time you issue an INT in
protected mode that requires segment pointers.
.endnote
.*
.mbox begin
:api.IntXX:eapi. :dsc.Simulate real mode interrupt.:edsc.
.mbox end
.begnote
.note Inputs:
AX= 0ff01h
.br
BL= Interrupt number.
.br
ES:[E]DI= Real mode register list.
.note Outputs:
Register list updated.
.note Errors:
None.
.note Notes:
The real mode register list referenced by ES:[E]DI
should contain the register values you want passed to the real mode
interrupt handler. CauseWay fills in the SS:SP and Flags values to
ensure that legal values are used and the CS:IP entries are ignored.
This function bypasses protected mode interrupt handlers and provides
access to INT APIs that require segment pointers.
.endnote
.*
.mbox begin
:api.LinearCheck:eapi. :dsc.Check linear address of memory.:edsc.
.mbox end
.begnote
.note Inputs:
AX= 0fffch
.br
ESI= Linear address of memory.
.note Outputs:
Carry set on invalid memory address.
.note Errors:
None.
.endnote
.*
.mbox begin
:api.LockMem:eapi. :dsc.Lock a region of memory.:edsc.
.mbox end
.begnote
.note Inputs:
AX= 0ff1bh
.br
BX:CX= Starting linear address of memory to lock.
.br
SI:DI= Size of region to lock in bytes.
.note Outputs:
Carry set on error.
.note Errors:
If any of the region specified is invalid or if not
enough physical memory is available to fill the region specified, then
none of the memory is locked and this function returns carry set.
.note Notes:
Memory that is locked cannot be swapped to disk by the
VMM. Locking applies to memory on page (4KB) boundaries. Therefore, areas of
memory below and above the memory being locked are locked if the
specified region is not aligned to a page boundary.
.endnote
.*
.mbox begin
:api.LockMem32:eapi. :dsc.Lock a region of memory.:edsc.
.mbox end
.begnote
.note Inputs:
AX= 0ff1ch
.br
ESI= Starting linear address of memory to lock.
.br
ECX= Size of region to lock in bytes.
.note Outputs:
Carry set on error.
.note Errors:
See LockMem.
.note Notes:
Memory that is locked cannot be swapped to disk by the
VMM. Locking applies to memory on page (4KB) boundaries. Therefore, areas of
memory below and above the memory being locked are locked if the
specified region is not aligned to a page boundary.
.endnote
.*
.mbox begin
:api.RelCallBack:eapi. :dsc.Release a real mode call back entry.:edsc.
.mbox end
.begnote
.note Inputs:
AX= 0304h
.br
CX:DX= Real mode address returned by GetCallBack
.note Outputs:
None.
.note Errors:
None.
.note Notes:
Uspe this function to release call-back addresses once
they are no longer needed.
.endnote
.*
.mbox begin
:api.RelMem:eapi. :dsc.Release memory allocated by either GetMem or GetMem32.:edsc.
.mbox end
.begnote
.note Inputs:
AX= 0ff0fh
.br
BX= Selector for block to release.
.note Outputs:
Carry set on error.
.note Errors:
If an invalid selector is passed in BX or the memory
was not allocated via GetMem or GetMem32, this function returns carry set.
.endnote
.*
.mbox begin
:api.RelMemDOS:eapi. :dsc.Release a block of DOS (conventional) memory allocated by GetMemDOS.:edsc.
.mbox end
.begnote
.note Inputs:
AX= 0ff23h
.br
DX= Selector of block to free.
.note Outputs:
Carry set on error and AX= DOS error code.
.note Errors:
If an invalid block is passed, this function returns
carry set.
.note Notes:
All descriptors allocated for the memory block are
automatically freed and therefore should not be accessed once the block
is freed by this function.
.endnote
.*
.mbox begin
:api.RelMemLinear:eapi. :dsc.Release previously allocated block of memory (linear address).:edsc.
.mbox end
.begnote
.note Inputs:
AX= 0ff14h
.br
SI:DI= Linear address of block to release.
.note Outputs:
Carry set on error.
.note Errors:
If the address passed in SI:DI is not a valid memory
block, this function returns carry set.
.endnote
.*
.mbox begin
:api.RelMemLinear32:eapi. :dsc.Release previously allocated block of memory (linear address).:edsc.
.mbox end
.begnote
.note Inputs:
AX= 0ff15h
.br
ESI= Linear address of block to release.
.note Outputs:
Carry set on error.
.note Errors:
See RelMemLinear
.endnote
.*
.mbox begin
:api.RelMemSO:eapi. :dsc.Release a block of memory allocated via GetMemSO.:edsc.
.mbox end
.begnote
.note Inputs:
AX = 0ff2eh
.br
SI:DI = Selector:offset of block to release
.note Outputs:
Carry set on error.
.note Errors:
If an invalid selector:offset is passed in SI:DI or
the memory was not allocated via GetMemSO, then this function returns
carry set.
.endnote
.*
.mbox begin
:api.RelSel:eapi. :dsc.Release a selector.:edsc.
.mbox end
.begnote
.note Inputs:
AX= 0ff04h
.br
BX= Selector.
.note Outputs:
Carry set on error.
.note Errors:
If an invalid selector is passed in BX, this function
returns with carry set.
.note Notes:
Use this function to release selectors allocated by
GetSel or AliasSel.
.endnote
.*
.mbox begin
:api.ResMem:eapi. :dsc.Resize a previously allocated block of memory.:edsc.
.mbox end
.begnote
.note Inputs:
AX= 0ff0dh
.br
BX= Selector for block.
.br
CX:DX= New size of block required in bytes.
.note Outputs:
Carry set on error.
.note Errors:
If an invalid selector is passed in BX or not enough
memory is available when increasing the block size, then this function
returns carry set.
.note Notes:
If the memory block cannot be resized in its current
location, but a free block of memory of the new size exists, the memory
is copied to a new block and the old one is released.  The application
is not affected as long as only the selector originally allocated with
GetMem accesses the memory.
.endnote
.*
.mbox begin
:api.ResMemSO:eapi. :dsc.Resize a block of memory allocated via GetMemSO.:edsc.
.mbox end
.begnote
.note Inputs:
AX = 0ff2dh
.br
SI:DI = Selector:offset of block to resize
.br
CX:DX =  New size of block required in bytes
.note Outputs:
Carry set on error, else
.br
SI:DI = selector:offset of new memory block address.
.note Errors:
If an invalid selector:offset is passed in SI:DI or
not enough memory is available when increasing the block size, then this
function returns carry set.
.note Notes:
If the memory block cannot be resized in its current
location, but a free block of memory of the new size exists, the memory
is copied to a new block and the old one is released.  The
selector:offset will change if this occurs, so the SI:DI return value
should be used to update all references and pointers to the memory block
when this function is called.
.endnote
.*
.mbox begin
:api.ResMem32:eapi. :dsc.Resize a previously allocated block of memory.:edsc.
.mbox end
.begnote
.note Inputs:
AX= 0ff0eh
.br
BX= Selector for block.
.br
ECX= New size of block required in bytes.
.note Outputs:
Carry set on error.
.note Errors:
See ResMem
.note Notes:
If the memory block cannot be resized in its current
location, but a free block of memory of the new size exists, the memory
is copied to a new block and the old one released. This is transparent
to the application as long as only the selector originally allocated
with GetMem is used to access the memory.
.endnote
.*
.mbox begin
:api.ResMemDOS:eapi. :dsc.Resize a block of DOS (conventional) memory allocated with GetMemDOS.:edsc.
.mbox end
.begnote
.note Inputs:
AX= 0ff22h
.br
BX= New block size in paragraphs
.br
DX= Selector of block to modify
.note Outputs:
Carry set on error, AX= DOS error code, BX= Maximum
block size in paragraphs.
.note Errors:
If an invalid block is passed or if not enough
selectors or memory are available when expanding the block this function
returns carry set.
.note Notes:
Growing a memory block is often likely to fail since
other DOS block allocations prevent increasing the size of the block.
Also, if the size of a block grows past a 64KB boundary then the
allocation fails if the next descriptor in the LDT is not free.
.endnote
.*
.mbox begin
:api.ResMemLinear:eapi. :dsc.Resize a previously allocated block of memory without a selector.:edsc.
.mbox end
.begnote
.note Inputs:
AX= 0ff12h
.br
SI:DI= Linear address of block to resize.
.br
CX:DX= Size of block required in bytes.
.note Outputs:
Carry set on error, else
.br
SI:DI= New linear address of block.
.note Errors:
If not enough memory is available when extending the
block size this function returns carry set.
.note Notes:
If the memory block cannot be expanded to the desired
size, and a free block of sufficient size exists, the existing memory is
copied to the free block and released.  The new block is allocated in
place of the old.
.endnote
.*
.mbox begin
:api.ResMemLinear32:eapi. :dsc.Resize a previously allocated block of memory without a selector.:edsc.
.mbox end
.begnote
.note Inputs:
AX= 0ff13h
.br
ESI= Linear address of block to resize.
.br
ECX= Size of block required in bytes.
.note Outputs:
Carry set on error, else
.br
ESI= New linear address of block.
.note Errors:
See ResMemLinear
.note Notes:
If the memory block cannot be expanded to the desired
size, and a free block of sufficient size exists, the existing memory is
copied to the free block and released.  The new block is allocated in
place of the old.
.endnote
.*
.mbox begin
:api.SetDOSTrans:eapi. :dsc.Set new address and size of the buffer used for DOS memory transfers.:edsc.
.mbox end
.begnote
.note Inputs:
AX = 0ff26h
.br
BX = Real mode segment of buffer.
.br
DX = Protected mode selector for buffer.
.br
ECX = Buffer size (should be <=64KB)
.note Outputs:
None
.note Errors:
None
.note Notes:
The buffer must be in conventional memory and only the
first 64KB will be used even if a bigger buffer is specified. CauseWay
will automatically restore the previous buffer setting when the
application terminates but GetDOSTrans can be used to get the current
buffer's settings if you only want the change to be temporary.
.np
You can still use the default buffer for your own
purposes even after setting a new address.
.endnote
.*
.mbox begin
:api.SetDump:eapi. :dsc.Disable/enable error display and CW.ERR creation.:edsc.
.mbox end
.begnote
.note Inputs:
AX = 0ff30h
.br
CL = 0 if disable error display and CW.ERR file
.br
CL = nonzero if enable error display and CW.ERR file
.note Outputs:
None
.note Errors:
None
.note Notes:
By default, register dump display to screen and CW.ERR
file creation are enabled on CPU faults.  This option may be used to
turn on and off CauseWay error processing output any number of times
within an application.
.endnote
.*
.mbox begin
:api.SetEVect:eapi. :dsc.Set Protected mode exception handler address.:edsc.
.mbox end
.begnote
.note Inputs:
AX= 0203h
.br
BL= Exception vector number.
.br
CX:[E]DX= selector:offset of new handler.
.note Outputs:
None
.note Errors:
The number in BL must be in the range 0-1Fh. Anything
outside this range returns carry set.
.endnote
.*
.mbox begin
:api.SetMCBMax:eapi. :dsc.Set new memory control block (MCB) memory allocation block size.:edsc.
.mbox end
.begnote
.note Inputs:
AX = 0ff28h
.br
ECX = New value to set (<=64KB)
.note Outputs:
None
.note Errors:
Carry set on error
.note Notes:
The maximum block size that will be allocated from MCB
memory is 16 bytes less than the value set by this function. The default
value is 16384. The maximum value is 65536.
.np
The CauseWay API memory allocation functions
allocate memory from two sources.  Allocation requests below the value
returned by this function are allocated from a memory pool controlled
via conventional style MCB's. Requests above this value are allocated
via the normal DPMI functions. Because DPMI memory is always allocated
in multiples of pages (4KB) it can become very inefficient for any program 
that needs to allocate small blocks of memory. The value set by this function
controls the size of memory chunks that will be allocated to and managed
by the MCB system.
.np
A value of zero can be passed to this function to
disable the MCB allocation system.
.np
The value passed will be rounded up to the
nearest page (4KB) boundary.
.endnote
.*
.mbox begin
:api.SetRVect:eapi. :dsc.Set real mode interrupt handler address.:edsc.
.mbox end
.begnote
.note Inputs:
AX= 0201h
.br
BL= Interrupt vector number.
.br
CX:DX= selector:offset of new handler.
.note Outputs:
None.
.note Errors:
None.
.endnote
.*
.mbox begin
:api.SetSelDet:eapi. :dsc.Set selector linear base and limit.:edsc.
.mbox end
.begnote
.note Inputs:
AX= 0ff09h
.br
BX= Selector.
.br
CX:DX= Linear base.
.br
SI:DI= Byte granular limit.
.note Outputs:
Carry set on error.
.note Errors:
If an invalid selector is passed in BX, this function
returns with carry set.
.endnote
.*
.mbox begin
:api.SetSelDet32:eapi. :dsc.Set selector linear base and limit.:edsc.
.mbox end
.begnote
.note Inputs:
AX= 0ff0ah
.br
BX= Selector.
.br
EDX= Linear base.
.br
ECX= Byte granular limit.
.note Outputs:
Carry set on error.
.note Errors:
If an invalid selector is passed in BX, this function
returns with carry set.
.endnote
.*
.mbox begin
:api.SetVect:eapi. :dsc.Set Protected mode interrupt handler address.:edsc.
.mbox end
.begnote
.note Inputs:
AX= 0205h
.br
BL= Interrupt vector number.
.br
CX:[E]DX= selector:offset of new handler.
.note Outputs:
None.
.note Errors:
None.
.endnote
.*
.mbox begin
:api.UnLockMem:eapi. :dsc.Unlock a region of memory.:edsc.
.mbox end
.begnote
.note Inputs:
AX= 0ff1dh
.br
BX:CX= Starting linear address of memory to unlock
.br
SI:DI= Size of region to unlock in bytes
.note Outputs:
Carry set on error.
.note Errors:
If any of the memory region specified is invalid this
function returns carry set.
.note Notes:
This function allows the unlocked memory to be swapped
to disk by the VMM if necessary.  Areas below and above the specified
memory to the nearest page (4KB) boundary are unlocked if the specified region
is not aligned to a page boundary.
.endnote
.*
.mbox begin
:api.UnLockMem32:eapi. :dsc.Unlock a region of memory.:edsc.
.mbox end
.begnote
.note Inputs:
AX= 0ff1eh
.br
ESI= Starting linear address of memory to unlock
.br
ECX= Size of region to unlock in bytes
.note Outputs:
Carry set on error.
.note Errors:
See UnLockMem
.note Notes:
This function allows the memory to be swapped to disk
by the VMM if necessary.  Areas below and above the specified memory to
the nearest page (4KB) boundary are unlocked if the specified region is not
aligned to a page boundary.
.endnote
.*
.mbox begin
:api.UserDump:eapi. :dsc.Setup user-defined error buffer dump in CW.ERR.:edsc.
.mbox end
.begnote
.note Inputs:
AX = 0ff2fh
.br
ES:[E]DI - user buffer to display in CW.ERR
.br
CX = count of bytes to display from buffer in CW.ERR
.br
BL = 'A' if ASCII dump (non-binary display of bytes,
control characters display as periods)
.br
BH = nonzero if preset ASCII buffer to word value,
ignored for non-ASCII
.br
DX = word value to fill ASCII dump buffer if BH is
nonzero, ignored for non-ASCII
.note Outputs:
Carry set on ASCII dump invalid buffer address.
.note Errors:
The user buffer must be a valid readable selector and
offset value when this function is called or else the request is ignored
and a carry flag condition is returned.  If BH is set to nonzero to flag
presetting the buffer bytes, the selector must be writable.  Specifying
a larger CX count than available buffer size will also return an error.
.note Notes:
If the fill ASCII buffer condition is specified, any
values previously in the buffer will be overwritten when this call is made.
.endnote
.*
.mbox begin
:api.UserErrTerm:eapi. :dsc.Call user error termination routine.:edsc.
.mbox end
.begnote
.note Inputs:
AX = 0ff31h
.br
CL = 0 if 16-bit termination routine
.br
CL = nonzero if 32-bit termination routine
.br
DS:[E]SI = user termination routine address.
If DS is zero, the user termination routine
call is removed.
.br
ES:[E]DI = Information dump buffer address, 104
bytes.        If ES is zero, no information dump is performed.
.note Outputs:
None
.note Errors:
None
.note Notes:
The user termination routine is responsible for
returning to the CauseWay termination routines to allow proper shutdown
of the application.  The instruction must be the proper 16- or 32-bit
return to match the CL register setting.  For ease of use with
high-level languages (specifically Watcom C and setting SS back to
DGROUP), [E]SI equals the internal DOS extender stack ESP immediately
prior to the 32- or 16-bit call to the termination routine.  If an
information dump buffer address is provided, register and other
termination values are placed into it using the following format:
.millust begin
dword            EBP;
dword            EDI;
dword            ESI;
dword            EDX;
dword            ECX;
dword            EBX;
dword            EAX;
word             GS;
word             FS;
word             ES;
word             DS;
dword            EIP;
word             CS;
word             reserved1;
dword            EFLAGS;
dword            ESP;
word             SS;
word             reserved2;
word             TR;
dword            CR0;
dword            CR1;
dword            CR2;
dword            CR3;
dword            csAddress;
dword            dsAddress;
dword            esAddress;
dword            fsAddress;
dword            gsAddress;
dword            ssAddress;
word             ExceptionNumber;
dword            ErrorCode;
.millust end
.endnote
.*
.endlevel
.*
.section API Notes
.*
.np
A fixed segment selector at 40h is always available to the application.
This selector maps the real mode memory at 400h where most of the BIOS
variables can be found.  CauseWay also provides selectors at standard
video addresses 0B000h, 0B800h and 0A000h in non-DPMI environments to
ease conversion of real mode code.
.np
The environment variable block address in the Program Segment Prefix
(PSP) is a valid protected mode selector.  A valid protected mode
selector:offset is also placed in the PSP at offset 34h for the file
handle list pointer. Note that the default value in the program's PSP
will point to the real mode PSP, not the protected mode PSP, even if the
handle count is less than or equal to twenty. Code that makes use of the
handle list should use the address at PSP+34h rather than assuming the
list's position within the PSP.  Also, when an application is operating
non-DPMI conditions the handle table will have an entry for CauseWay's
VMM swap file.
.np
DOS functions which use the obsolete file control blocks (FCBs) are not
supported by CauseWay, although such support may be added by the developer.
.np
CauseWay applications should terminate using INT 21H function 4Ch. As
with real mode operation, the error level passed to this function in the
AL register is returned to the parent program or DOS.
.np
If a CauseWay application needs to terminate and stay resident (TSR),
then INT 21H function 31h may be used.  Unlike real mode operation, no
memory value is required for this function.  All memory owned by the
application when the TSR function is issued remains the program's
property.   There is currently no way of removing the CauseWay
application from memory once it becomes a TSR without rebooting the
machine or using a third party TSR manager.  However, a TSR manager will
not automatically release extended memory allocated for the CauseWay TSR.
.np
Unhandled exceptions terminate the program with a register display dump
to screen and a text file called CW.ERR.  CW.ERR contains other
potentially useful information about the state of the application when
it terminated.  Refer to the appendices for more information on the
CW.ERR file information format.
.np
CauseWay runs applications at privilege level 3.  Privilege level 0
reserved instructions will cause a general protection fault (GPF).
CauseWay emulates the four instructions MOV EAX,CR0; MOV CR0,EAX; MOV
EAX,CR3; and MOV CR3,EAX in the GPF handler so that they may be used by
an application.
.np
CauseWay will match a DPMI 0A00h function call with target string
RATIONAL/4G.  This in conjunction with CR0 emulation allows support of
floating point emulation by an exception handler.  Watcom uses this
approach under DOS/4GW operation if the floating point emulation library
is not linked in.  Note that this routine does not work for either
CauseWay or DOS/4GW under a DPMI host such as Windows or OS/2.
.*
.chap Interrupt Services
.*
.section Extended or Altered Interrupt Services
.*
.np
The size of registers used by CauseWay's extended or modified interrupt
services depends on the limit of the selector. Extended 32-bit registers
are used for 32-bit sized selectors and 16-bit registers are used for
selectors within 16-bits.  To reflect this, the convention of an [E] in
brackets is used before a listed register when it must be a 32-bit value
with a 32-bit selector and a 16-bit value with a 16-bit selector.
.np
Required registers that are not specified in this list should be set up
in the same way as required for normal DOS real mode operation. For INT
APIs that are not listed and require segemnt pointers, either handle
them using the CauseWay IntXX function or create your own interrupt
translation code.
.np
INT 10h
.np
10h sub function 02h, [E]DX instead of DX.
.br
10h sub function 09h, [E]DX instead of DX.
.br
10h sub function 12h, [E]DX instead of DX.
.br
10h sub function 17h, [E]DX instead of DX.
.br
13h [E]BP instead of BP.
.br
1Ch sub function 01h, [E]BX instead of BX.
.br
1Ch sub function 02h, [E]BX instead of BX.
.np
INT 21h
.np
09h [E]DX instead of DX
.br
0Ah [E]DX instead of DX
.br
0FH - 17H not supported; use corresponding file handle function.
.br
1Ah [E]DX instead of DX
.br
21h - 24h not supported; use corresponding file handle function.
.br
25h [E]DX instead of DX. Protected mode vector will be set.
.br
26h - 29h not supported; use corresponding file handle function.
.br
2Fh [E]BX instead of BX
.br
31h No value is required in DX
.br
35h [E]DX instead of BX. Protected mode vector will be returned.
.br
39h [E]DX instead of DX
.br
3Ah [E]DX instead of DX
.br
3Bh [E]DX instead of DX
.br
3Ch [E]DX instead of DX
.br
3Dh [E]DX instead of DX
.br
3Fh [E]DX instead of DX
.br
40h [E]DX instead of DX
.br
41h [E]DX instead of DX
.br
43h [E]DX instead of DX
.br
44h subfunction 02h, use [E]DX instead
.br
44h subfunction 03h, use [E]DX instead
.br
44h subfunction 04h, use [E]DX instead
.br
44h subfunction 05h, use [E]DX instead
.br
47h [E]SI instead of SI
.br
48h Protected mode memory will be allocated
.br
49h Protected mode memory will be released
.br
4Ah Protected mode memory will be resized
.br
4Bh [E]DX & [E]BX instead of DX & BX
.br
    Parameter block offset entries are [d]word
.br
4Eh [E]DX instead of DX
.br
56h [E]DX & [E]DI instead of DX & DI
.br
5Ah [E]DX instead of DX
.br
5Bh [E]DX instead of DX
.br
62h Protected mode selector will be returned
.br
6Ch [E]SI instead of SI
.np
INT 23h   Control-C Handler Address
.np
This interrupt is always reflected back to the protected mode handler to
ensure the CauseWay application can handle it correctly. The default
handler aborts the application in the same manner as DOS. If you need to
terminate your application in your own handler, perform an INT 21h
AH=4ch as normal.
.np
INT 24h   Critical Error Handler Address
.np
This interrupt is always reflected back to the protected mode handler to
ensure the CauseWay application can handle it correctly. The default
handler behaves in the same way as the DOS handler and it aborts your
application, if appropriate. If you install your own handler, all memory
accessed by this interrupt as code or data must be locker.
.np
The register values normally placed on the stack by DOS before entry to
the interrupt handler are not present in protected mode. Only the
register values are valid. You may terminate your application from
within this interrupt with INT 21h, AH=4ch as normal.
.np
INT 33h
.np
09h [E]DX instead of DX
.br
0Ch [E]DX instead of DX
.br
16h [E]DX instead of DX
.br
17h [E]DX instead of DX
.np
Notes:
.np
With the exception of software interrupts that require segment pointers
as parameters, all interrupts can be issued as normal. The most common
interrupt APIs that require segment pointers are intercepted by CauseWay
to provide normal access to these services. Any other real mode
interrupt services that require segment pointers can be accessed using
CauseWay's simulated real mode interrupt/far call services.
.np
Hardware interrupts are always reflected to protected mode handlers even
when signaled during real mode operations. This ensures that protected
mode applications always retain control without requiring you to patch
real mode interrupt vectors. The remaining interrupts are serviced via
the vector table appropriate to the mode. Use the real to protected mode
call-back services to provide real mode code with access to protected
mode code, and allow any interrupt to be re-signaled in protected mode.
.np
If you add your own hardware interrupt handlers, suchas the timer tick
at vector 08h, any memory that the handler reads or writes, including
its code, must reside in locked memory. (CauseWay provides a locked
stack.) This limitation is required because DOS is not re-entrant and
hardware interrupts can occur at any time. Interrupts occurring during
DOS activity prevent CauseWay's virtual memory manager from accessing
its swap file. Lock memory will not move to the swap file.
.*
.chap Troubleshooting
.*
.section First Steps
.*
.np
If you have problems using CauseWay, first try linking and running a
one-line program that simply prints "Hello" on your computer screen.
This will help establish if the problem is a basic incompatibility with
CauseWay and your system setup, or if the error may lie elsewhere (e.g.
a third party library).
.np
The remainder of this chapter provides a description of error and
warning messages that you may encounter when using CauseWay.  Suggested
solutions to correct the errors are included where possible.
.*
.section DOS Extender Error Messages and Return Values
.*
.np
DOS extender error messages are displayed by the CauseWay DOS extender
when a CauseWay application is running and encounters a serious problem
that it cannot recover from.  The DOS extender then terminates the
application with the appropriate return code, displaying a dump of
register values, and writing system information to the file CW.ERR.
.np
.errsect
.errnote 01 Unable to resize program memory block.
.np
Generated if DOS reports an error when CauseWay tries to resize
its real mode memory block.  As the block is always shrunk, the only
possible cause of this is corrupted memory control blocks (MCBs).
Reboot the system to correct this error.
.np
.errnote 02 386 or better required.
.np
Generated if CauseWay is run on any machine with a processor
below a 386SX.  To correct this error, run the application on another
machine or upgrade the machine's processor.
.np
.errnote 03 Non-standard protected mode program already active.
.np
Generated if the system is already operating under the control
of another protected mode program which doesn't conform to either VCPI
or DPMI standards.  Identify and remove the other application before
running the CauseWay application.
.np
.errnote 04 DOS 3.1 or better required.
.np
Generated if DOS version is less than 3.1.  You need to upgrade
the machine's DOS version or use another machine to operate the CauseWay
application.
.np
.errnote 05 Not enough memory for CauseWay.
.np
Generated if the system doesn't have enough free physical
memory to initialize the CauseWay kernel code and data. Free additional
memory before running the CauseWay application.  The memory can be any
of the extended or conventional types supported by CauseWay.
.np
.errnote 06 VCPI failed to switch into protected mode.
.np
Generated if a VCPI server is detected and the server fails to
switch into protected mode when requested.  The only likely cause of
this error is a corrupted system.  Reboot the system and try again.
.np
.errnote 07 Unable to control A20.
.np
Generated if CauseWay detects an A20 line that doesn't respond
to the normal control methods.  This may indicate either a hardware
fault or a nonstandard system.  There is no software solution for these
hardware problems.  Installing an XMS driver such as HIMEM.SYS should
address nonstandard systems.
.np
.errnote 08 Selector allocation error.
.np
Generated if DPMI refuses to allocate enough selectors for
CauseWay to function.  Remove one or more programs that are also using DPMI.
.np
.errnote 09 Unrecoverable exception. Program terminated.
.np
This is the standard General Protection Fault, or GPF,
message.  It is generated if a nonrecoverable exception occurs which
suggests a bug in the application.  Use the register dump displayed with
this message along with the information in CW.ERR and the program's .MAP
file to help track down the location and cause of the problem.
.np
.errnote 10 Unable to find application to load.
.np
Generated if CauseWay is unable to find the application within
the executable .EXE file.  This situation indicates a corrupted file.
Rebuild or obtain another copy of the application.
.np
.errnote 11 DOS reported error while accessing application.
.np
Generated if any kind of error is detected while accessing the
CauseWay application executable file.  This situation indicates a
corrupted or missing file.  Rebuild or obtain another copy of the
application.
.np
.errnote 12 Not enough memory to load application.
.np
Generated if CauseWay is unable to provide enough memory to
load the application.  Free additional memory and/or disk space before
running the application.  Check for CAUSEWAY=SWAP, TEMP and TMP
environment variables that point to a disk with little free space.  If
running under an operating system that provides DPMI per application,
increase the application's DPMI allocation.
.np
.errnote 13 DPMI failed to switch to protected mode.
.np
Generated if the machine is using a DPMI server and it fails to
switch to protected mode.  If the DPMI server only supports multiple
clients of the same type (either 16- or 32-bit) then the problem is
probably that different types of applications are already being run.
Remove the other type of DPMI application(s) before running the CauseWay
application.
.np
.errnote 14 Memory structures destroyed. Program terminated.
.np
Generated if internal memory management structures become
corrupted.  This is caused by the CauseWay application writing to memory
regions that have not been allocated to it and is a bug in the
application.  Obtain a corrected version of the application to fix this
error.
.np
.errnote 15 DOS reported an error while accessing swap file. Program terminated.
.np
Generated if any level of error is detected while accessing the
swap file.  The swap file has probably been deleted inadvertently by the
application or perhaps marked as read-only.
.np
.errnote 16 Unsupported DOS function call. Program terminated.
.np
The CauseWay application attempted to use an obsolete DOS
function which used file control blocks (FCBs).  Use the file handle DOS
functions in the application instead.
.eerrsect
