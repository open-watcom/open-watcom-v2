.* (c) Portions Copyright 1990, 1995 by Tenberry Software, Inc.
.*
.chap &dos4gprd Commonly Asked Questions
.*
.np
.ix '&dos4gprd' 'common questions'
.ix 'common questions' '&dos4gprd'
The following information has been provided by Tenberry Software, Inc.
for their &dos4gprd and &dos4gprd Professional product.
The content of this chapter has been edited by &company..
In most cases, the information is applicable to both products.
.np
This chapter covers the following topics:
.begbull
.bull
Access to technical support
.bull
Differences within the DOS/4G product line
.bull
Addressing
.bull
Interrupt and exception handling
.bull
Memory management
.bull
DOS, BIOS, and mouse services
.bull
Virtual memory
.bull
Debugging
.bull
Compatibility
.endbull
.*
.section Access to Technical Support
.*
.begnote $setptnt 2
.*
.note 1a. How to reach technical support.
.*
.np
.ix 'technical support' 'Tenberry Software'
Here are the various ways you may contact Tenberry Software for
technical support.
.millust begin
WWW:    http://www.tenberry.com/dos4g/
Email:  4gwhelp@tenberry.com
Phone:  1.480.767.8868
Fax:    1.480.767.8709
Mail:   Tenberry Software, Inc.
        PO Box 20050
        Fountain Hills, Arizona
        U.S.A  85269-0050
.millust end
.bi PLEASE GIVE YOUR SERIAL NUMBER WHEN YOU CONTACT TENBERRY.
.np
.*
.keep
.note 1b. When to contact &company, when to contact Tenberry.
.*
.np
.ix '&dos4gprd' 'contacting Tenberry'
Since &dos4gprd Professional is intended to be completely compatible
with &dos4gprd, you may wish to ascertain whether your program works
properly under &dos4gprd before contacting Tenberry Software for
technical support. (This is likely to be the second question we ask
you, after your serial number.)
.np
If your program fails under both &dos4gprd and &dos4gprd Professional,
and you suspect your own code or a problem compiling or linking, you
may wish to contact &company first. Tenberry Software support
personnel are not able to help you with most programming questions, or
questions about using the &company tools.
.np
If your program only fails with &dos4gprd Professional, you have
probably found a bug in &dos4gprd Professional, so please contact us
right away.
.*
.keep
.note 1c. Telephone support.
.*
.np
.ix '&dos4gprd' 'telephone support'
Tenberry Software's hours for telephone support are 9am-6pm EST.
Please note that telephone support is free for the first 30 days only.
A one-year contract for continuing telephone support on &dos4gprd
Professional is US$500 per developer, including an update subscription
for one year, to customers in the United States and Canada; for
overseas customers, the price is $600. Site licenses may be
negotiated.
.np
There is no time limit on free support by fax, mail, or electronic
means.
.*
.keep
.note 1d. References.
.*
.np
.ix '&dos4gprd' 'documentation'
The &dos4gprd documentation from &company is the primary reference for
&dos4gprd Professional as well.
.ix 'DPMI specification'
Another useful reference is the DPMI specification.
In the past, the DPMI specification could be obtained free of charge
by contacting Intel Literature.
We have been advised that the DPMI specification is no longer
available in printed form.
.np
However, the DPMI 1.0 specification can be obtained at:
.code begin
http://www.delorie.com/djgpp/doc/dpmi/
.code end
.pc
Online HTML as well as a downloadable archive are provided.
.endnote
.*
.section Differences Within the DOS/4G Product Line
.*
.begnote $setptnt 2
.*
.note 2a. &dos4gprd Professional versus &dos4gprd.
.*
.ix '&dos4gprd' 'differences with Professional version'
.np
&dos4gprd Professional was designed to be a higher-performance version
of &dos4gprd suitable for commercial applications. Here is a summary
of the advantages of &dos4gprd Professional with respect to &dos4gprd:
.begbull
.bull
Extender binds to the application program file
.bull
Extender startup time has been reduced
.bull
Support for &company floating-point emulator has been optimized
.bull
Virtual memory manager performance has been greatly improved
.bull
Under VMM, programs are demand loaded
.bull
Virtual address space is 4 GB instead of 32 MB
.bull
Extender memory requirements have been reduced by more than 50K
.bull
Extender disk space requirements have been reduced by 40K
.bull
Can omit virtual memory manager to save 50K more disk space
.bull
Support for INT 31h functions 301h-304h and 702h-703h
.endbull
.np
&dos4gprd Professional is intended to be fully compatible with
programs written for &dos4gprd 1.9 and up. The only functional
difference is that the extender is bound to your program instead of
residing in a separate file. Not only does this help reduce startup
time, but it eliminates version-control problems when someone has both
&dos4gprd and &dos4gprd Professional applications present on one
machine.
.*
.keep
.note 2b. &dos4gprd Professional versus DOS/4G.
.*
.ix '&dos4gprd' 'differences with DOS/4G'
.np
&dos4gprd Professional is not intended to provide any other new DOS
extender functionality. Tenberry Software's top-of-the-line 32-bit
extender, DOS/4G, is not sold on a retail basis but is of special
interest to developers who require more flexibility (such as OEMs).
DOS/4G offers these additional features beyond &dos4gprd and &dos4gprd
Professional:
.begbull
.bull
Complete documentation
.bull
DLL support
.bull
TSR support
.bull
Support for INT 31h functions 301h-306h, 504h-50Ah, 702h-703h
.bull
A C language API that offers more control over interrupt handling and
program loading, as well as making it easier to use the extender
.bull
An optional (more protected) nonzero-based flat memory model
.bull
Remappable error messages
.bull
More configuration options
.bull
The D32 debugger, GLU linker, and other tools
.bull
Support for other compilers besides &company
.bull
A higher level of technical support
.bull
Custom work is available (e.g., support for additional executable
formats, operating system API emulations, mixed 16-bit and 32-bit
code)
.endbull
.np
Please contact Tenberry Software if you have questions about other
products (present or future) in the DOS/4G line.
.*
.keep
.note 2c. DPMI functions supported by &dos4gprd..
.*
.ix '&dos4gprd' 'DPMI support'
.np
Note that when a &dos4gprd application runs under a DPMI host, such as
Windows 3.1 in enhanced mode, an OS/2 virtual DOS machine, 386Max
(with DEBUG=DPMIXCOPY), or QDPMI (with EXTCHKOFF), the DPMI host
provides the DPMI services, not &dos4gprd.. The DPMI host also
provides virtual memory, if any. Performance (speed and memory use)
under different DPMI hosts varies greatly due to the quality of the
DPMI implementation.
.np
These are the services provided by &dos4gprd and &dos4gprd
Professional in the absence of a DPMI host.
.begnote $compact
.note 0000
Allocate LDT Descriptors
.note 0001
Free LDT Descriptor
.note 0002
Map Real-Mode Segment to Descriptor
.note 0003
Get Selector Increment Value
.note 0006
Get Segment Base Address
.note 0007
Set Segment Base Address
.note 0008
Set Segment Limit
.note 0009
Set Descriptor Access Rights
.note 000A
Create Alias Descriptor
.note 000B
Get Descriptor
.note 000C
Set Descriptor
.note 000D
Allocate Specific LDT Descriptor
.np
.note 0100
Allocate DOS Memory Block
.note 0101
Free DOS Memory Block
.note 0102
Resize DOS Memory Block
.np
.note 0200
Get Real-Mode Interrupt Vector
.note 0201
Set Real-Mode Interrupt Vector
.note 0202
Get Processor Exception Handler
.note 0203
Set Processor Exception Handler
.note 0204
Get Protected-Mode Interrupt Vector
.note 0205
Set Protected-Mode Interrupt Vector
.np
.note 0300
Simulate Real-Mode Interrupt
.note 0301
Call Real-Mode Procedure with Far Return Frame
(&dos4gprd Professional only)
.note 0302
Call Real-Mode Procedure with IRET Frame
(&dos4gprd Professional only)
.note 0303
Allocate Real-Mode Callback Address
(&dos4gprd Professional only)
.note 0304
Free Real-Mode Callback Address
(&dos4gprd Professional only)
.np
.note 0400
Get DPMI Version
.np
.note 0500
Get Free Memory Information
.note 0501
Allocate Memory Block
.note 0502
Free Memory Block
.note 0503
Resize Memory Block
.np
.note 0600
Lock Linear Region
.note 0601
Unlock Linear Region
.note 0604
Get Page Size (VM only)
.np
.note 0702
Mark Page as Demand Paging Candidate
(&dos4gprd Professional only)
.note 0703
Discard Page Contents
(&dos4gprd Professional only)
.np
.note 0800
Physical Address Mapping
.note 0801
Free Physical Address Mapping
.np
.note 0900
Get and Disable Virtual Interrupt State
.note 0901
Get and Enable Virtual Interrupt State
.note 0902
Get Virtual Interrupt State
.np
.note 0A00
Get Tenberry Software API Entry Point
.np
.note 0E00
Get Coprocessor Status
.note 0E01
Set Coprocessor Emulation
.endnote
.*
.endnote
.*
.section Addressing
.*
.begnote $setptnt 2
.*
.note 3a. Converting between pointers and linear addresses.
.*
.ix '&dos4gprd' 'pointers vs linear addresses'
.np
Because &dos4gprd uses a zero-based flat memory model, converting
between pointers and linear addresses is trivial. A pointer value is
always relative to the current segment (the value in CS for a code
pointer, or in DS or SS for a data pointer). The segment bases for the
default DS, SS, and CS are all zero. Hence a near pointer is exactly
the same thing as a linear address: a null pointer points to linear
address 0, and a pointer with value 0x10000 points to linear address
0x10000.
.*
.keep
.note 3b. Converting between code and data pointers.
.*
.ix '&dos4gprd' 'code and data addresses'
.np
Because DS and CS have the same base address, they are natural aliases
for each other. To create a data alias for a code pointer, merely
create a data pointer and set it equal to the code pointer. It's not
necessary for you to create your own alias descriptor. Similarly, to
create a code alias for a data pointer, merely create a code pointer
and set it equal to the data pointer.
.*
.keep
.note 3c. Converting between pointers and low memory addresses.
.*
.ix '&dos4gprd' 'low memory access'
.np
Linear addresses under 1 MB map directly to physical memory. Hence the
real-mode interrupt vector table is at address 0, the BIOS data
segment is at address 0x400, the monochrome video memory is at address
0xB0000, and the color video memory is at address 0xB8000. To read and
write any of these, you can just use a pointer set to the proper
address. You don't need to create a far pointer, using some magic
segment value.
.*
.keep
.note 3d. Converting between linear and physical addresses.
.*
.ix '&dos4gprd' 'linear vs physical addresses'
.np
Linear addresses at or above 1 MB do not map directly to physical
memory, so you can not in general read or write extended memory
directly, nor can you tell how a particular block of extended memory
has been used.
.np
&dos4gprd supports the DPMI call INT 31h/800h, which maps physical
addresses to linear addresses. In other words, if you have a
peripheral device in your machine that has memory at a physical
address of 256 MB, you can issue this call to create a linear address
that points to that physical memory. The linear address is the same
thing as a near pointer to the memory and can be manipulated as such.
.np
There is no way in a DPMI environment to determine the physical
address corresponding to a given linear address. This is part of the
design of DPMI. You must design your application accordingly.
.*
.keep
.note 3e. Null pointer checking.
.*
.ix '&dos4gprd' 'NULL pointer references'
.ix 'DOS4G' 'NULLP option'
.np
&dos4gprd will trap references to the first sixteen bytes of physical
memory if you set the environment variable DOS4G=NULLP. This is
currently the only null-pointer check facility provided by &dos4gprd..
.np
As of release 1.95, &dos4gprd traps both reads and writes. Prior to
this, it only trapped writes.
.np
You may experience problems if you set DOS4G=NULLP and use some
versions of the &dbgname with a 1.95 or later extender. These problems
have been corrected in later versions of the &dbgname..
.endnote
.*
.section Interrupt and Exception Handling
.*
.begnote $setptnt 2
.*
.note 4a. Handling asynchronous interrupts.
.*
.ix '&dos4gprd' 'asynchronous interrupts'
.np
Under &dos4gprd, there is a convenient way to handle asynchronous
interrupts and an efficient way to handle them.
.np
Because your CPU may be in either protected mode (when 32-bit code is
executing) or real mode (a DOS or BIOS call) when a hardware interrupt
comes in, you have to be prepared to handle interrupts in either mode.
Otherwise, you may miss interrupts.
.np
You can handle both real-mode and protected-mode interrupts with a
single handler, if 1) the interrupt is in the auto-passup range, 8 to
2Eh; and 2) you install a handler with INT 21h/25h or _dos_setvect();
3) you do not install a handler for the same interrupt using any other
mechanism. &dos4gprd will route both protected-mode interrupts and
real-mode interrupts to your protected-mode handler. This is the
convenient way.
.np
The efficient way is to install separate real-mode and protected-mode
handlers for your interrupt, so your CPU won't need to do unnecessary
mode switches. Writing a real-mode handler is tricky; all you can
reasonably expect to do is save data in a buffer and IRET. Your
protected-mode code can periodically check the buffer and process any
queued data. (Remember, protected-mode code can access data and
execute code in low memory, but real-mode code can't access data or
execute code in extended memory.)
.np
For performance, it doesn't matter how you install the real-mode
handler, but we recommend the DPMI function INT 31h/201h for
portability.
.np
It does matter how you install the protected-mode handler. You can't
install it directly into the IDT, because a DPMI provider must
distinguish between interrupts and exceptions and maintain separate
handler chains. Installing with INT 31h/205h is the recommended way to
install your protected-mode handler for both performance and
portability.
.np
If you install a protected-mode handler with INT 21h/25h, both
interrupts and exceptions will be funneled to your handler, to mimic
DOS. Since DPMI exception handlers and interrupt handlers are called
with different stack frames, &dos4gprd executes a layer of code to
cover these differences up; the same layer is used to support the
DOS/4G API (not part of &dos4gprd). This layer is the reason that
hooking with INT 21h/25h is less efficient than hooking with INT
31h/205h.
.*
.keep
.note 4b. Handling asynchronous interrupts in the second IRQ range.
.*
.ix '&dos4gprd' 'int 70h-77h'
.np
Because the second IRQ range (normally INTs 70h-77h) is outside the
&dos4gprd auto-passup range (8-2Eh, excluding 21h) you may not handle
these interrupts with a single handler, as described above (the
"convenient" method). You must install separate real-mode and
protected-mode handlers (the "efficient" method).
.np
DOS/4G does allow you to specify additional passup interrupts,
however.
.*
.keep
.note 4c. Asynchronous interrupt handlers and DPMI.
.*
.np
.ix '&dos4gprd' 'locking memory'
The DPMI specification requires that all code and data referenced by a
hardware interrupt handler MUST be locked at interrupt time. A DPMI
virtual memory manager can use the DOS file system to swap pages of
memory to and from the disk; because DOS is not reentrant, a DPMI host
is not required to be able to handle page faults during asynchronous
interrupts. Use INT 31h/600h (Lock Linear Region) to lock an address
range in memory.
.np
If you fail to lock all of your code and data, your program may run
under &dos4gprd, but fail under the &dos4gprd Virtual Memory Manager
or under another DPMI host such as Windows or OS/2.
.np
You should also lock the code and data of a mouse callback function.
.*
.keep
.note 4d. &company signal() function and Ctrl-Break.
.*
.ix '&dos4gprd' 'Ctrl-Break handling'
.np
In earlier versions of the &company C/C++ library, there was a bug
that caused signal(SIGBREAK) not to work.
Calling signal(SIGBREAK) did not actually install an interrupt handler
for Ctrl-Break (INT 1Bh), so Ctrl-Break would terminate the
application rather than invoking the signal handler.
.np
With these earlier versions of the library, you could work around this
problem by hooking INT 1Bh directly.
With release 10.0, this problem has been fixed.
.*
.keep
.note 4e. More tips on writing hardware interrupt handlers.
.*
.ix '&dos4gprd' 'interrupt handlers'
.in -0.3i
.begbull
.bull
It's more like handling interrupts in real mode than not.
.np
The same problems arise when writing hardware interrupt handlers for
protected mode as arise for real mode. We assume you know how to write
real-mode handlers; if our suggestions don't seem clear, you might
want to brush up on real-mode interrupt programming.
.bull
Minimize the amount of time spent in your interrupt handlers.
.np
When your interrupt handlers are called, interrupts are disabled. This
means that no other system tasks can be performed until you enable
interrupts (an STI instruction) or until your handler returns. In
general, it's a good idea to handle interrupts as quickly as possible.
.bull
Minimize the amount of time spent in the DOS extender by installing
separate real-mode and protected-mode handlers.
.np
If you use a passup interrupt handler, so that interrupts received in
real mode are resignalled in protected mode by the extender, your
application has to switch from real mode to protected mode to real
mode once per interrupt. Mode switching is a time-consuming process,
and interrupts are disabled during a mode switch. Therefore, if you're
concerned about performance, you should install separate handlers for
real-mode and protected-mode interrupts, eliminating the mode switch.
.bull
If you can't just set a flag and return, enable interrupts (STI).
.np
Handlers that do more than just set a flag or store data in a buffer
should re-enable interrupts as soon as it's safe to do so. In other
words, save your registers on the stack, establish your addressing
conventions, switch stacks if you're going to &mdash. and then enable
interrupts (STI), to give priority to other hardware interrupts.
.bull
If you enable interrupts (STI), you should disable interrupts (CLI).
.np
Because some DPMI hosts virtualize the interrupt flag, if you do an
STI in your handler, you should be sure to do a CLI before you return.
(CLI, then switch back to the original stack if you switched away,
then restore registers, then IRET.) If you don't do this, the IRET
will not necessarily restore the previous interrupt flag state, and
your program may crash. This is a difference from real-mode
programming, and it tends to show up as a problem when you try running
your program in a Windows or OS/2 DOS box for the first time (but not
before).
.bull
Add a reentrancy check.
.np
If your handler doesn't complete its work by the time the next
interrupt is signalled, then interrupts can quickly nest to the point
of overflowing the transfer stack. This is a design flaw in your
program, not in the DOS extender; a real-mode DOS program can have
exactly the same behavior. If you can conceive of a situation where
your interrupt handler can be called again before the first instance
returns, you need to code in a reentrancy check of some sort (before
you switch stacks and enable interrupts (STI), obviously).
.np
Remember that interrupts can take different amounts of time to execute
on different machines; the CPU manufacturer, CPU speed, speed of
memory accesses, and CMOS settings (e.g. "system BIOS shadowing") can
all affect performance in subtle ways. We recommend you program
defensively and always check for unexpected reentry, to avoid transfer
stack overflows.
.bull
Switch to your own stack.
.np
Interrupt handlers are called on a stack that typically has only a
small amount of stack available (512 bytes or less). If you need to
use more stack than this, you have to switch to your own stack on
entry into the handler, and switch back before returning.
.np
If you want to use C run-time library functions, which are compiled
for flat memory model (SS == DS, and the base of CS == the base of
DS), you need to switch back to a stack in the flat data segment
first.
.np
Note that switching stacks by itself won't prevent transfer stack
overflows of the kind described above.
.endbull
.in +0.3i
.*
.endnote
.*
.section Memory Management
.*
.begnote $setptnt 2
.*
.note 5a. Using the realloc() function.
.*
.ix '&dos4gprd' 'realloc'
.np
In versions of &company C/C++ prior to 9.5b, there was a bug in the
library implementation of realloc() under &dos4gprd and &dos4gprd
Professional. This bug was corrected by &company in the 9.5b release.
.*
.keep
.note 5b. Using all of physical memory.
.*
.ix '&dos4gprd' 'memory addressability'
.np
&dos4gprd Professional is currently limited to 64 MB of physical
memory. We do not expect to be able to fix this problem for at least
six months. If you need more than 64 MB of memory, you must use
virtual memory.
.*
.endnote
.*
.section DOS, BIOS, and Mouse Services
.*
.begnote $setptnt 2
.*
.note 6a. Speeding up file I/O.
.*
.ix '&dos4gprd' 'DOS file I/O'
.ix 'DOS file I/O'
.np
The best way to speed up DOS file I/O in &dos4gprd is to write large
blocks (up to 65535 bytes, or the largest number that will fit in a
16-bit int) at a time from a buffer in low memory. In this case,
&dos4gprd has to copy the least amount of data and make the fewest
number of DOS calls in order to process the I/O request.
.np
.ix 'setvbuf'
Low memory is allocated through INT 31h/0100h, Allocate DOS Memory
Block. You can convert the real-mode segment address returned by INT
31h/0100h to a pointer (suitable for passing to setvbuf()) by shifting
it left four bits.
.*
.keep
.note 6b. Spawning.
.*
.ix '&dos4gprd' 'spawning'
.np
It is possible to spawn one &dos4gprd application from another.
However, two copies of the DOS extender will be loaded into memory.
DOS/4G supports loading of multiple programs atop a single extender,
as well as DLLs.
.*
.keep
.note 6c. Mouse callbacks.
.*
.ix '&dos4gprd' 'mouse support'
.np
&dos4gprd Professional now supports the INT 31h interface for managing
real-mode callbacks. However, you don't need to bother with them for
their single most important application: mouse callback functions.
Just register your protected-mode mouse callback function as you would
in real mode, by issuing INT 33h/0Ch with the event mask in CX and the
function address in ES:EDX, and your function will work as expected.
.np
Because a mouse callback function is called asynchronously, the same
locking requirement exists for a mouse callback function as for a
hardware interrupt handler. See (4c) above.
.*
.keep
.note 6d. VESA support.
.*
.ix '&dos4gprd' 'VESA support'
.np
While &dos4gprd automatically handles most INT 10h functions so that
you can you can issue them from protected mode, it does not translate
the INT 10h VESA extensions. The workaround is to use INT 31h/300h
(Simulate Real-Mode Interrupt).
.*
.endnote
.*
.section Virtual Memory
.*
.begnote $setptnt 2
.*
.note 7a. Testing for the presence of VMM.
.*
.ix '&dos4gprd' 'VMM'
.ix 'VMM'
.ix 'virtual memory manager'
.np
INT 31h/400h returns a value (BX, bit 2) that tells if virtual memory
is available. Under a DPMI host such as Windows 3.1, this will be the
host's virtual memory manager, not &dos4gprd's.
.np
You can test for the presence of a DOS/4G-family DOS extender with INT
31h/0A00h, with a pointer to the null-terminated string "RATIONAL
DOS/4G" in DS:ESI. If the function returns with carry clear, a
DOS/4G-family extender is running.
.*
.keep
.note 7b. Reserving memory for a spawned application.
.*
.ix 'DOS4GVM' 'DELETESWAP'
.ix 'DELETESWAP virtual memory option'
.np
If you spawn one &dos4gprd application from another, you should set
the DELETESWAP configuration option (i.e., SET DOS4GVM=deleteswap) so
that the two applications don't try to use the same swap file. You
should also set the MAXMEM option low enough so that the parent
application doesn't take all available physical memory; memory that's
been reserved by the parent application is not available to the child
application.
.*
.keep
.note 7c. Instability under VMM.
.*
.ix '&dos4gprd' 'VMM instability'
.np
A program that hooks hardware interrupts, and works fine without VMM
but crashes sporadically with it, probably needs to lock the code and
data for its hardware interrupt handlers down in memory. &dos4gprd
does not support page faults during hardware interrupts, because DOS
services may not be available at that time. See (4c) and (6c) above.
.np
Memory can be locked down with INT 31h/600h (Lock Linear Region).
.*
.keep
.note 7d. Running out of memory with a huge virtual address space.
.*
.ix '&dos4gprd' 'out of memory'
.ix 'DOS4GVM' 'VIRTUALSIZE'
.ix 'VIRTUALSIZE virtual memory option'
.np
Because &dos4gprd has to create page tables to describe your virtual
address space, we recommend that you set your VIRTUALSIZE parameter
just large enough to accommodate your program. If you set your
VIRTUALSIZE to 4 GB, the physical memory occupied by the page tables
will be 4 MB, and that memory will not be available to &dos4gprd..
.*
.keep
.note 7e. Reducing the size of the swap file.
.*
.ix 'DOS4GVM' 'VIRTUALSIZE'
.ix 'VIRTUALSIZE virtual memory option'
.ix 'DOS4GVM' 'SWAPMIN'
.ix 'SWAPMIN virtual memory option'
.ix 'DOS4GVM' 'SWAPINC'
.ix 'SWAPINC virtual memory option'
.np
&dos4gprd will normally create a swap file equal to your VIRTUALSIZE
setting, for efficiency. However, if you set the SWAPMIN parameter to
a size (in KB), &dos4gprd will start with a swap file of that size,
and will grow the swap file when it has to. The SWAPINC value (default
64 KB) controls the incremental size by which the swap file will grow.
.*
.keep
.note 7f. Deleting the swap file.
.*
.ix 'DOS4GVM' 'DELETESWAP'
.ix 'DELETESWAP virtual memory option'
.ix 'DOS4GVM' 'SWAPNAME'
.ix 'SWAPNAME virtual memory option'
.np
The DELETESWAP option has two effects: telling &dos4gprd to delete the
swap file when it exits, and causing &dos4gprd to provide a unique
swap file name if an explicit SWAPNAME setting was not given.
.np
DELETESWAP is required if one &dos4gprd application is to spawn
another; see (7b) above.
.*
.keep
.note 7g. Improving demand-load performance of large static arrays.
.*
.ix '&dos4gprd' 'demand-loading'
.np
&dos4gprd demand-loading feature normally cuts the load time of a
large program drastically. However, if your program has large amounts
of global, zero-initialized data (storage class BSS), the &company
startup code will explicitly zero it (version 9.5a or earlier).
Because the zeroing operation touches every page of the data, the
benefits of demand-loading are lost.
.np
Demand loading can be made fast again by taking advantage of the fact
that &dos4gprd automatically zeroes pages of BSS data as they are
loaded. You can make this change yourself by inserting a few lines
into the startup routine, assembling it (MASM 6.0 will work), and
listing the modified object module first when you link your program.
.np
Here are the changes for
.fi \watcom\src\startup\386\cstart3r.asm
(startup module from the C/C++ 9.5 compiler, library using register
calling conventions). You can modify the workaround easily for other
&company compilers:
.code begin
        ...                     ; cstart3r.asm, circa line 332
                                ; end of _BSS segment (start of STACK)
        mov     ecx,offset DGROUP:_end
                                ; start of _BSS segment
        mov     edi,offset DGROUP:_edata
;-------------------------------; RSI OPTIMIZATION
        mov     eax, edi        ; minimize _BSS initialization loop
        or      eax, 0FFFh      ; compute address of first page after
        inc     eax             ;   start of _BSS
        cmp     eax, ecx        ; if _BSS extends onto that page,
        jae     allzero         ;   then we can rely on the loader
        mov     ecx, eax        ;   zeroing the remaining pages
allzero:                        ;
;-------------------------------; END RSI OPTIMIZATION
        sub     ecx,edi         ; calc # of bytes in _BSS segment
        mov     dl,cl           ; save bottom 2 bits of count in edx
        shr     ecx,2           ; calc # of dwords
        sub     eax,eax         ; zero the _BSS segment
        rep     stosd           ; ...
        mov     cl,dl           ; get bottom 2 bits of count
        and     cl,3            ; ...
        rep     stosb           ; ...
        ...
.code end
.np
Note that the 9.5b and later versions of the &company C library
already contain this enhancement.
.*
.keep
.note 7h. How should I configure VM for best performance?
.*
.ix '&dos4gprd' 'VM configuration'
.np
Here are some recommendations for setting up the &dos4gprd virtual
memory manager.
.begnote
.note VIRTUALSIZE
.ix 'DOS4GVM' 'VIRTUALSIZE'
.ix 'VIRTUALSIZE virtual memory option'
Set to no more than twice the total amount of memory (virtual and
otherwise) your program requires. If your program has 16 MB of code
and data, set to 32 MB. (There is only a small penalty for setting
this value larger than you will need, but your program won't run if
you set it too low.) See (7d) above.
.note MINMEM
.ix 'DOS4GVM' 'MINMEM'
.ix 'MINMEM virtual memory option'
Set to the minimum hardware requirement for running your application.
(If you require a 2 MB machine, set to 2048).
.note MAXMEM
.ix 'DOS4GVM' 'MAXMEM'
.ix 'MAXMEM virtual memory option'
Set to the maximum amount of memory you want your application to use.
If you don't spawn any other applications, set this large (e.g.,
32000) to make sure you can use all available physical memory. If you
do spawn, see (7b) above.
.note SWAPMIN
.ix 'DOS4GVM' 'SWAPMIN'
.ix 'SWAPMIN virtual memory option'
Don't use this if you want the best possible VM performance. The
trade-off is that &dos4gprd will create a swap file as big as your
VIRTUALSIZE.
.note SWAPINC
.ix 'DOS4GVM' 'SWAPINC'
.ix 'SWAPINC virtual memory option'
Don't use this if you want the best possible VM performance.
.note DELETESWAP
.ix 'DOS4GVM' 'DELETESWAP'
.ix 'DELETESWAP virtual memory option'
&dos4gprd's VM will start up slightly slower if it has to create the
swap file afresh each time. However, unless your swap file is very
large, DELETESWAP is a reasonable choice; it may be required if you
spawn another &dos4gprd program at the same time. See (7b) above.
.endnote
.*
.endnote
.*
.section Debugging
.*
.begnote $setptnt 2
.*
.note 8a. Attempting to debug a bound application.
.*
.ix '&dos4gprd' 'debugging bound applications'
.np
You can't debug a bound application. The 4GWBIND utility (included
with &dos4gprd Professional) will allow you to take apart a bound
application so that you can debug it:
.millust begin
4GWBIND -U <boundapp.exe> <yourapp.exe>
.millust end
.*
.keep
.note 8b. Debugging with an old version of the &company debugger.
.*
.ix '&dos4gprd' 'debugger version'
.np
&dos4gprd supports versions 8.5 and up of the &company C, C++ and
FORTRAN compilers. However, in order to debug your unbound application
with a &company debugger, you must have version 9.5a or later of the
debugger.
.np
.ix '&dos4gprd' '4GWPRO.EXE'
.ix '4GWPRO.EXE'
If you have an older version of the debugger, we strongly recommend
that you contact &company to upgrade your compiler and tools. The only
way to debug a &dos4gprd Professional application with an old version
of the debugger is to rename 4GWPRO.EXE to DOS4GW.EXE and make sure
that it's either in the current directory or the first DOS4GW.EXE on
the DOS PATH.
.np
Tenberry will not provide technical support for this configuration;
it's up to you to keep track of which DOS extender is which.
.*
.keep
.note 8c. Meaning of "unexpected interrupt" message/error 2001.
.*
.ix '&dos4gprd' 'unexpected interrupt'
.np
In version 1.95 of &dos4gprd, we revised the "unexpected interrupt"
message to make it easier to understand.
.np
For example, the message:
.code begin
Unexpected interrupt 0E (code 0) at 168:10421034
.code end
.pc
is now printed:
.code begin
error (2001): exception 0Eh (page fault) at 168:10421034
.code end
.pc
followed by a register dump, as before.
.np
This message indicates that the processor detected some form of
programming error and signaled an exception, which &dos4gprd trapped
and reported. Exceptions which can be trapped include:
.millust begin
00h     divide by zero
01h     debug exception OR null pointer used
03h     breakpoint
04h     overflow
05h     bounds
06h     invalid opcode
07h     device not available
08h     double fault
09h     overrun
0Ah     invalid TSS
0Bh     segment not present
0Ch     stack fault
0Dh     general protection fault
0Eh     page fault
.millust end
.np
When you receive this message, this is the recommended course of
action:
.autonote
.note
Record all of the information from the register dump.
.note
Determine the circumstances under which your program fails.
.note
Consult your debugger manual, or an Intel 386, 486 or Pentium
Programmer's Reference Manual, to determine the circumstances under
which the processor will generate the reported exception.
.note
Get the program to fail under your debugger, which should stop the
program as soon as the exception occurs.
.note
Determine from the exception context why the processor generated an
exception in this particular instance.
.endnote
.*
.keep
.note 8d. Meaning of "transfer stack overflow" message/error 2002.
.*
.ix '&dos4gprd' 'transfer stack overflow'
.np
In version 1.95 of &dos4gprd, we added more information to the
"transfer stack overflow" message. The message (which is now followed
by a register dump) is printed:
.code begin
error (2002): transfer stack overflow
on interrupt <number> at <address>
.code end
.np
This message means &dos4gprd detected an overflow on its interrupt
handling stack. It usually indicates either a recursive fault, or a
hardware interrupt handler that can't keep up with the rate at which
interrupts are occurring. The best way to understand the problem is to
use the VERBOSE option in &dos4gprd to dump the interrupt history on
the transfer stack; see (8e) below.
.*
.keep
.note 8e. Making the most of a &dos4gprd register dump.
.*
.ix '&dos4gprd' 'register dump'
.ix 'DOS4G' 'VERBOSE option'
.np
If you can't understand your problem by running it under a debugger,
the &dos4gprd register dump is your best debugging tool. To maximize
the information available for postmortem debugging, set the
environment variable DOS4G to VERBOSE, then reproduce the crash and
record the output.
.np
Here's a typical register dump with VERBOSE turned on, with
annotations.
.in -0.3i
.code begin
1 &dos4gprd error (2001): exception 0Eh (page fault)
                                              at 170:0042C1B2
2 TSF32: prev_tsf32 67D8
3 SS       178 DS       178 ES       178 FS         0 GS        20
  EAX 1F000000 EBX        0 ECX   43201C EDX        E
  ESI        E EDI        0 EBP   431410 ESP   4313FC
  CS:IP  170:0042C1B2 ID 0E COD        0 FLG    10246
4 CS=  170, USE32, page granular, limit FFFFFFFF, base        0, acc CF9B
  SS=  178, USE32, page granular, limit FFFFFFFF, base        0, acc CF93
  DS=  178, USE32, page granular, limit FFFFFFFF, base        0, acc CF93
  ES=  178, USE32, page granular, limit FFFFFFFF, base        0, acc CF93
  FS=    0, USE16, byte granular, limit        0, base       15, acc  0
  GS=   20, USE16, byte granular, limit     FFFF, base     6AA0, acc 93
5 CR0: PG:1 ET:1 TS:0 EM:0 MP:0 PE:1   CR2: 1F000000 CR3: 9067
6 Crash address (unrelocated) = 1:000001B2
7 Opcode stream: 8A 18 31 D2 88 DA EB 0E 50 68 39 00 43 00 E8 1D
  Stack:
8 0178:004313FC 000E 0000 0000 0000 C2D5 0042 C057 0042 0170 0000 0000 0000
  0178:00431414 0450 0043 0452 0043 0000 0000 1430 0043 CBEF 0042 011C 0000
  0178:0043142C C568 0042 0000 0000 0000 0000 0000 0000 F248 0042 F5F8 0042
  0178:00431444 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000
  0178:0043145C 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000
  0178:00431474 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000
9 Last 4 ints: 21 @ 170:42CF48/21 @ 170:42CF48/21 @ 170:42CF48/E @ 170:42C1B2/
.code end
.in +0.3i
.autonote
.note
The error message includes a synopsis of the problem. In this case,
the processor signaled a page fault exception while executing at
address 170:0042C1B2.
.note
The prev_tsf32 field is not usually of interest.
.note
These are the register values at the time of the exception. The
interrupt number and error code (pushed on the stack by the processor
for certain exceptions) are also printed.
.note
The descriptors referenced by each segment register are described for
your convenience. USE32 segments in general belong to your program;
USE16 segments generally belong to the DOS extender. Here, CS points
to your program's code segment, and SS, DS, and ES point to your data
segment. FS is NULL and GS points to a DOS extender segment.
.note
The control register information is not of any general interest,
except on a page fault, when CR2 contains the address value that
caused the fault. Since EAX in this case contains the same value, an
attempt to dereference EAX could have caused this particular fault.
.note
If the crash address (unrelocated) appears, it tells you where the
crash occurred relative to your program's link map. You can therefore
tell where a crash occurred even if you can't reproduce the crash in a
debugger.
.note
The opcode stream, if it appears, shows the next 16 bytes from the
code segment at the point of the exception. If you disassemble these
instructions, you can tell what instructions caused the crash, even
without using a debugger. In this case, 8A 18 is the instruction
.mono mov bl,[eax].
.note
72 words from the top of the stack, at the point of the exception, may
be listed next. You may be able to recognize function calls or data
from your program on the stack.
.note
The four interrupts least to most recently handled by &dos4gprd in
protected mode are listed next. In this example, the last interrupt
issued before the page fault occurred was an INT 21h (DOS call) at
address 170:42CF48. Sometimes, this information provides helpful
context.
.endnote
.np
Here's an abridged register dump from a stack overflow.
.in -0.3i
.code begin
  &dos4gprd error (2002): transfer stack overflow
                                  on interrupt 70h at 170:0042C002
  TSF32: prev_tsf32 48C8
  SS        C8 DS       170 ES        28 FS         0 GS        20
  EAX AAAAAAAA EBX BBBBBBBB ECX CCCCCCCC EDX DDDDDDDD
  ESI 51515151 EDI D1D1D1D1 EBP B1B1B1B1 ESP     4884
1 CS:IP  170:0042C002 ID 70 COD        0 FLG        2
  ...
2 Previous TSF:
  TSF32: prev_tsf32 498C
  SS        C8 DS       170 ES        28 FS         0 GS        20
  EAX AAAAAAAA EBX BBBBBBBB ECX CCCCCCCC EDX DDDDDDDD
  ESI 51515151 EDI D1D1D1D1 EBP B1B1B1B1 ESP     4960
3 CS:IP  170:0042C002 ID 70 COD        0 FLG        2
  ...
  Previous TSF:
  TSF32: prev_tsf32 67E4
  SS       178 DS       170 ES        28 FS         0 GS        20
  EAX AAAAAAAA EBX BBBBBBBB ECX CCCCCCCC EDX DDDDDDDD
  ESI 51515151 EDI D1D1D1D1 EBP B1B1B1B1 ESP   42FFE0
4 CS:IP  170:0042C039 ID 70 COD        0 FLG      202
5 Opcode stream: CF 66 B8 62 25 66 8C CB 66 8E DB BA 00 C0 42 00
  Last 4 ints: 70 @ 170:42C002/70 @ 170:42C002/70 @ 170:42C002/70 @ 170:42C002/
.code end
.in +0.3i
.autonote
.note
We overflowed the transfer stack while trying to process an interrupt
70h at 170:0042C002.
.note
The entire interrupt history from the transfer stack is printed next.
The prev_tsf32 numbers increase as we progress from most recent to
least recent interrupt. All of these interrupts are still pending,
which is why we ran out of stack space.
.note
Before we overflowed the stack, we got the same interrupt at the same
address. For a recursive interrupt situation, this is typical.
.note
The oldest frame on the transfer stack shows the recursion was touched
off at a slightly different address. Looking at this address may help
you understand the recursion.
.note
The opcode stream and last four interrupt information comes from the
newest transfer stack frame, not the oldest.
.endnote
.*
.endnote
.*
.section Compatibility
.*
.begnote $setptnt 2
.*
.note 9a. Running &dos4gprd applications from inside Lotus 1-2-3.
.*
.ix '&dos4gprd' 'Lotus 1-2-3'
.ix 'Lotus 1-2-3'
.ix 'PRIVATXM'
.np
In order to run &dos4gprd applications while "shelled out" from Lotus
1-2-3, you must use the PRIVATXM program included with your &company
compiler. Otherwise, 1-2-3 will take all of the memory on your machine
and prevent &dos4gprd from using it.
.np
Before starting 1-2-3, you must set the DOS16M environment variable to
limit Lotus' memory use (see your &company manual). After shelling
out, you must run PRIVATXM, then clear the DOS16M environment variable
before running your application.
.*
.keep
.note 9b. EMM386.EXE provided with DOS 6.0.
.*
.ix '&dos4gprd' 'EMM386.EXE'
.ix 'EMM386.EXE'
.np
We know of at least three serious bugs in the EMM386.EXE distributed
with MS-DOS 6.0, one involving mis-counting the amount of available
memory, one involving mapping too little of the High Memory Area (HMA)
into its page tables, and one involving allocation of EMS memory.
Version 1.95 of &dos4gprd contains workarounds for some of these
problems.
.np
If you are having problems with &dos4gprd and you are using an
EMM386.EXE dated 3-10-93 at 6:00:00, or later, you may wish to try the
following workarounds, in sequence, until the problem goes away.
.begbull
.bull
Configure EMM386 with both the NOEMS and NOVCPI options.
.bull
Convert the DEVICEHIGH statements in your CONFIG.SYS to DEVICE
statements, and remove the LH (Load High) commands from your
AUTOEXEC.BAT.
.bull
Run in a Windows DOS box.
.bull
Replace EMM386 with another memory manager, such as QEMM-386, 386Max,
or an older version of EMM386.
.bull
Run with HIMEM.SYS alone.
.endbull
.np
You may also wish to contact Microsoft Corporation to inquire about
the availability of a fix.
.*
.keep
.note 9c. Spawning under OS/2 2.1.
.*
.ix '&dos4gprd' 'OS/2 bug'
.np
We know of a bug in OS/2 2.1 that prevents one &dos4gprd application
from spawning another over and over again. The actual number of
repeated spawns that are possible under OS/2 varies from machine to
machine, but is generally about 30.
.np
This bug also affects programs running under other DOS extenders, and
we have not yet found a workaround, other than linking your two
programs together as a single program.
.*
.keep
.note 9d. "DPMI host error: cannot lock stack".
.*
.ix '&dos4gprd' 'cannot lock stack'
.ix 'DPMI_MEMORY_LIMIT' 'DOS setting'
.np
This error message almost always indicates insufficient memory, rather
than a real incompatibility. If you see it under an OS/2 DOS box, you
probably need to edit your DOS Session settings and make
DPMI_MEMORY_LIMIT larger.
.*
.keep
.note 9e. Bug in Novell TCPIP driver.
.*
.ix '&dos4gprd' 'TCPIP.EXE'
.ix 'TCPIP.EXE'
.ix 'Novell' 'TCPIP.EXE'
.np
Some versions of a program from Novell called TCPIP.EXE, a real-mode
program, will cause the high words of EAX and EDX to be altered during
a hardware interrupt. This bug breaks protected-mode software (and
other real-mode software that uses the 80386 registers). Novell has
released a newer version of TCPIP that fixes the problem; contact
Novell to obtain the fix.
.*
.keep
.note 9f. Bugs in Windows NT.
.*
.ix '&dos4gprd' 'Windows NT bug'
.ix '&dos4gprd' 'DOSX.EXE'
.ix 'DOSX.EXE'
.np
The initial release of Windows NT includes a DPMI host, DOSX.EXE, with
several serious bugs, some of which apparently cannot be worked
around. We cannot warranty operation of &dos4gprd under Windows NT at
this time, but we are continuing to exercise our best efforts to work
around these problems.
.np
You may wish to contact Microsoft Corporation to inquire about the
availability of a new version of DOSX.EXE.
.endnote
