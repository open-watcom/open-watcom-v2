.* (c) Portions Copyright 1990, 1995 by Tenberry Software, Inc.
.*
.chap *refid=int31 Interrupt 31H DPMI Functions
.*
.np
.ix 'DPMI host' 'Windows 3.1'
.ix 'DPMI host' 'OS/2 VDM'
.ix 'DPMI host' '386Max'
.ix 'DPMI host' 'QEMM QDPMI'
When a &dos4g application runs under a DPMI host, such as Windows 3.1
in enhanced mode, an OS/2 virtual DOS machine, 386Max (with
DEBUG=DPMIXCOPY), or QEMM/QDPMI (with EXTCHKOFF), the DPMI host
provides the DPMI services, not &dos4g..
The DPMI host also provides virtual memory, if any.
Performance (speed and memory use) under different DPMI hosts varies
greatly due to the quality of the DPMI implementation.
.np
.ix 'INT 31H'
DPMI services are accessed using Interrupt 31H.
.np
The following describes the services provided by &dos4g and
&dos4gprd Professional in the absence of a DPMI host.
&dos4g supports many of the common DPMI system services.
Not all of the services described below are supported by other DPMI
hosts.
.np
.ix 'DPMI'
.ix 'DOS Protected-Mode Interface'
Some of the information in this chapter was obtained from the the DOS
Protected-Mode Interface (DPMI) specification.
It is no longer in print; however the DPMI 1.0 specification can be
obtained from the Intel ftp site.
Here is the URL.
.code begin
ftp://ftp.intel.com/pub/IAL/software_specs/dpmiv1.zip
.code end
.np
This ZIP file contains a Postscript version of the DPMI 1.0
specification.
.*
.section Using Interrupt 31H Function Calls
.*
.np
.ix 'DPMI'
.ix 'DOS Protected-Mode Interface'
Interrupt 31H DPMI function calls can be used only by protected-mode
programs.
.* Real-mode programs must use the equivalent Interrupt 21 calls
.* described in the chapter, "Interrupt 21H Functions".
.np
The general ground rules for Interrupt 31H calls are as follows:
.begbull
.bull
All Interrupt 31H calls modify the AX register.
Unsupported or unsuccessful calls return an error code in AX.
Other registers are saved unless they contain specified return values.
.bull
All Interrupt 31H calls modify flags:
Unsupported or unsuccessful calls return with the carry flag set.
Successful calls clear the carry flag.
Only memory management and interrupt flag management calls modify the
interrupt flag.
.bull
Memory management calls can enable interrupts.
.bull
All calls are reentrant.
.endbull
.np
The flag and register information for each call is listed in the
following descriptions of supported Interrupt 31H function calls.
.*
.section Int31H Function Calls
.*
.np
.ix 'DPMI' 'function calls'
.ix 'int 31H function calls'
The Interrupt 31H subfunction calls supported by &dos4g are listed
below by category:
.begbull
.bull
Local Descriptor Table (LDT) management services
.bull
DOS memory management services
.bull
Interrupt services
.bull
Translation services
.bull
DPMI version
.bull
Memory management services
.bull
Page locking services
.bull
Demand paging performance tuning services
.bull
Physical address mapping
.bull
Virtual interrupt state functions
.bull
Vendor specific extensions
.bull
Coprocessor status
.endbull
.np
Only the most commonly used Interrupt 31H function calls are supported
in this version.
.*
.beglevel
.*
.section Local Descriptor Table (LDT) Management Services
.*
.begnote
.*
.note Function 0000H
.*
.ix 'local descriptor table'
.ix 'LDT'
This function allocates a specified number of descriptors from the LDT
and returns the base selector.
Pass the following information:
.begpoint $compact
.point AX = 0000H
.point CX = number of descriptors to be allocated
.endpoint
.np
If the call succeeds, the carry flag is clear and the base selector is
returned in AX.
If the call fails, the carry flag is set.
.np
An allocated descriptor is set to the present data type, with a base
and limit of zero.
The privilege level of an allocated descriptor is set to match the
code segment privilege level of the application.
To find out the privilege level of a descriptor, use the
.mono lar
instruction.
.np
Allocated descriptors must be filled in by the application.
If more than one descriptor is allocated, the returned selector is the
first of a contiguous array.
Use Function 0003H to get the increment for the next selector in the
array.
.*
.note Function 0001H
.*
This function frees the descriptor specified.
Pass the following information:
.begpoint $compact
.point AX = 0001H
.point BX = the selector to free
.endpoint
.np
Use the selector returned with function 0000h when the descriptor was
allocated.
To free an array of descriptors, call this function for each
descriptor.
Use Function 0003H to find out the increment for each descriptor in
the array.
.np
If the call succeeds, the carry flag is clear; if it fails, the carry
flag is set.
.np
You can use this function to free the descriptors allocated for the
program's initial CS, DS, and SS segments, but you should not free
other segments that were not allocated with Function 0000H or Function
000DH.
.*
.note Function 0002H
.*
This function converts a real-mode segment to a descriptor that a
protected-mode program can address.
Pass the following information:
.begpoint $compact
.point AX = 0002H
.point BX = real-mode segment address
.endpoint
.np
If the call succeeds, it clears the carry flag and returns the
selector mapped to the real-mode segment in AX.
If the call fails, the carry flag is set.
.np
If you call this function more than once with the same real-mode
segment address, you get the same selector value each time.
The descriptor limit is set to 64KB.
.np
The purpose of this function is to give protected-mode programs easy
access to commonly used real-mode segments.
However, because you cannot modify or free descriptors created by this
function, it should be used infrequently.
Do not use this function to get descriptors for private data areas.
.np
To examine real-mode addresses using the same selector, first allocate
a descriptor, and then use Function 0007H to change the linear base
address.
.*
.note Function 0003H
.*
This function returns the increment value for the next selector.
Use this function to get the value you add to the base address of an
allocated array of descriptors to get the next selector address.
Pass the following information:
.begpoint $compact
.point AX = 0003H
.endpoint
.np
This call always succeeds.
The increment value is returned in AX.
This value is always a power of two, but no other assumptions can be
made.
.*
.note Function 0006H
.*
This function gets the linear base address of a selector.
Pass the following information:
.begpoint $compact
.point AX = 0006H
.point BX = selector
.endpoint
.np
If the call succeeds, the carry flag is clear and CX:DX contains the
32-bit linear base address of the segment.
If the call fails, it sets the carry flag.
.np
If the selector you specify in BX is invalid, the call fails.
.*
.note Function 0007H
.*
This function changes the base address of a specified selector.
Only descriptors allocated through Function 0000H should be modified.
Pass the following information:
.begpoint $compact
.point AX = 0007H
.point BX = selector
.point CX:DX = the new 32-bit linear base address for the segment
.endpoint
.np
If the call succeeds, the carry flag is clear; if unsuccessful, the
carry flag is set.
.np
If the selector you specify in BX is invalid, the call fails.
.*
.note Function 0008H
.*
This function sets the upper limit of a specified segment.
Use this function to modify descriptors allocated with Function 0000H
only.
Pass the following information:
.begpoint $compact
.point AX = 0008H
.point BX = selector
.point CX:DX = 32-bit segment limit
.endpoint
.np
If the call succeeds, the carry flag is clear; if unsuccessful, the
carry flag is set.
.np
The call fails if the specified selector is invalid, or if the
specified limit cannot be set.
.np
Segment limits greater than 1MB must be page-aligned.
This means that limits greater than 1MB must have the low 12 bits set.
.np
To get the limit of a segment, use the 32-bit form of
.mono lsl
for segment limits greater than 64KB.
.*
.note Function 0009H
.*
This function sets the descriptor access rights.
Use this function to modify descriptors allocated with Function 0000H
only.
To examine the access rights of a descriptor, use the
.mono lar
instruction.
Pass the following information:
.begpoint $compact
.point AX = 0009H
.point BX = selector
.point CL = Access rights/type byte
.point CH = 386 extended access rights/type byte
.endpoint
.np
If the call succeeds, the carry flag is clear; if unsuccessful, the
carry flag is set.
If the selector you specify in BX is invalid, the call fails.
The call also fails if the access rights/type byte does not match the
format and meet the requirements shown in the figures below.
.cp 30
.np
The access rights/type byte passed in CL has the format shown in the
figure below.
:cmt. :image xoff='0.0i' depth='4.0i' file='RSI\MEM3' text='Access Rights/Type'.
.figure *depth='4.0' *scale=100 *file='mem3' Access Rights/Type
.cp 30
.np
The extended access rights/type byte passed in CH has the following
format.
:cmt. :image xoff='0.0i' depth='4.0i' file='RSI\MEM4' text='Extended Access Rights/Type'.
.figure *depth='4.0' *scale=100 *file='mem4' Extended Access Rights/Type
.*
.note Function 000AH
.*
This function creates an alias to a code segment.
This function creates a data descriptor that has the same base and
limit as the specified code segment descriptor.
Pass the following information:
.begpoint $compact
.point AX = 000AH
.point BX = code segment selector
.endpoint
.np
If the call succeeds, the carry flag is clear and the new data
selector is returned in AX.
If the call fails, the carry flag is set.
The call fails if the selector passed in BX is not a valid code
segment.
.np
To deallocate an alias to a code segment, use Function 0001H.
.np
After the alias is created, it does not change if the code segment
descriptor changes.
For example, if the base or limit of the code segment change later,
the alias descriptor stays the same.
.*
.note Function 000BH
.*
This function copies the descriptor table entry for a specified
descriptor.
The copy is written into an 8-byte buffer.
Pass the following information:
.begpoint $compact
.point AX = 000BH
.point BX = selector
.point ES:EDI = a pointer to the 8-byte buffer for the descriptor copy
.endpoint
.np
If the call succeeds, the carry flag is clear and ES:EDI contains a
pointer to the buffer that contains a copy of the descriptor.
If the call fails, the carry flag is set.
The call fails if the selector passed in BX is invalid or unallocated.
.*
.note Function 000CH
.*
This function copies an 8-byte buffer into the LDT for a specified
descriptor.
The descriptor must first have been allocated with Function 0000H.
Pass the following information:
.begpoint $compact
.point AX = 000CH
.point BX = selector
.point ES:EDI = a pointer to the 8-byte buffer containing the descriptor
.endpoint
.np
If the call succeeds, the carry flag is clear; if it fails, the carry
flag is set.
The call fails if the descriptor passed in BX is invalid.
.np
The type byte, byte 5, has the same format and requirements as the
access rights/type byte passed to Function 0009H in CL.
The format is shown in the first figure presented with the description
of Function 0009H.
.np
The extended type byte, byte 6, has the same format and requirements
as the extended access rights/type byte passed to Function 0009H in
CH, except that the limit field can have any value, and the low order
bits marked
.us reserved
are used to set the upper 4 bits of the descriptor limit.
The format is shown in the second figure presented with the
description of Function 0009H.
.*
.note Function 000DH
.*
This function allocates a specific LDT descriptor.
Pass the following information:
.begpoint $compact
.point AX = 000DH
.point BX = selector
.endpoint
.np
If the call succeeds, the carry flag is clear and the specified
descriptor is allocated.
If the call fails, the carry flag is set.
.np
The call fails if the specified selector is already in use, or if it
is not a valid LDT descriptor.
The first 10h (16 decimal) descriptors are reserved for this function,
and should not be used by the host.
Some of these descriptors may be in use, however, if another client
application is already loaded.
.np
To free the descriptor, use Function 0001H.
.endnote
.*
.section DOS Memory Management Services
.*
.begnote
.*
.note Function 0100H
.*
.ix 'DOS memory management'
.ix 'DPMI' 'allocate DOS memory block'
This function allocates memory from the DOS free memory pool.
This function returns both the real-mode segment and one or more
descriptors that can be used by protected-mode applications.
Pass the following information:
.begpoint $compact
.point AX = 0100H
.point BX = the number of paragraphs (16-byte blocks) requested
.endpoint
.np
If the call succeeds, the carry flag is clear.
AX contains the initial real-mode segment of the allocated block and
DX contains the base selector for the allocated block.
.np
If the call fails, the carry flag is set.
AX contains the DOS error code.
If memory is damaged, code 07H is returned.
If there is not enough memory to satisfy the request, code 08H is
returned.
BX contains the number of paragraphs in the largest available block of
DOS memory.
.np
If you request a block larger than 64KB, contiguous descriptors are
allocated.
Use Function 0003H to find the value of the increment to the next
descriptor.
The limit of the first descriptor is set to the entire block.
Subsequent descriptors have a limit of 64KB, except for the final
descriptor, which has a limit of
.mono blocksize MOD 64KB.
.np
You cannot modify or deallocate descriptors allocated with this
function.
Function 101H deallocates the descriptors automatically.
.*
.note Function 0101H
.*
.ix 'DPMI' 'free DOS memory block'
This function frees a DOS memory block allocated with function 0100H.
Pass the following information:
.begpoint $compact
.point AX = 0101H
.point DX = selector of the block to be freed
.endpoint
.np
If the call succeeds, the carry flag is clear.
.np
If the call fails, the carry flag is set and the DOS error code is
returned in AX.
If the incorrect segment was specified, code 09H is returned.
If memory control blocks are damaged, code 07H is returned.
.np
All descriptors allocated for the specified memory block are
deallocated automatically and cannot be accessed correctly after the
block is freed.
.*
.note Function 0102H
.*
.ix 'DPMI' 'resize DOS memory block'
This function resizes a DOS memory block allocated with function
0100H.
Pass the following information:
.begpoint $compact
.point AX = 0102H
.point BX = the number of paragraphs (16-byte blocks) in the resized block
.point DX = selector of block to resize
.endpoint
.np
If the call succeeds, the carry flag is clear.
.np
If the call fails, the carry flag is set, the maximum number of
paragraphs available is returned in BX, and the DOS error code is
returned in AX.
If memory code blocks are damaged, code 07H is returned.
If there isn't enough memory to increase the size as requested, code
08H is returned.
If the incorrect segment is specified, code 09h is returned.
.np
Because of the difficulty of finding additional contiguous memory or
descriptors, this function is not often used to increase the size of a
memory block.
Increasing the size of a memory block might well fail because other
DOS allocations have used contiguous space.
If the next descriptor in the LDT is not free, allocation also fails
when the size of a block grows over the 64KB boundary.
.np
If you shrink the size of a memory block, you may also free some
descriptors allocated to the block.
The initial selector remains unchanged, however; only the limits of
subsequent selectors will change.
.endnote
.*
.section Interrupt Services
.*
.begnote
.*
.note Function 0200H
.*
.ix 'DPMI' 'get real-mode interrupt vector'
.ix 'interrupt services'
This function gets the value of the current task's real-mode interrupt
vector for the specified interrupt.
Pass the following information:
.begpoint $compact
.point AX = 0200H
.point BL = interrupt number
.endpoint
.np
This call always succeeds.
All 100H (256 decimal) interrupt vectors are supported by the host.
When the call returns, the carry flag is clear, and the
.mono segment:offset
of the real-mode interrupt handler is returned in CX:DX.
.np
Because the address returned in CX is a segment, and not a selector,
you cannot put it into a protected-mode segment register.
If you do, a general protection fault may occur.
.*
.note Function 0201H
.*
.ix 'DPMI' 'set real-mode interrupt vector'
This function sets the value of the current task's real-mode interrupt
vector for the specified interrupt.
Pass the following information:
.begpoint $compact
.point AX = 0201H
.point BL = interrupt number
.point CX:DX = segment:offset of the real-mode interrupt handler
.endpoint
.np
If the call succeeds, the carry flag is clear; if it fails, the carry
flag is set.
.np
The address passed in CX:DX should be a real-mode
.mono segment:offset,
such as function 0200H returns.
For this reason, the interrupt handler must reside in DOS addressable
memory.
You can use Function 0100H to allocate DOS memory.
This version does not support the real-mode callback address
function.
.np
If you are hooking a hardware interrupt, you have to lock all segments
involved.
These segments include the segment in which the interrupt handler
runs, and any segment it may touch at interrupt time.
.*
.note Function 0202H
.*
.ix 'DPMI' 'get exception handler vector'
This function gets the processor exception handler vector.
This function returns the CS:EIP of the current protected-mode
exception handler for the specified exception number.
Pass the following information:
.begpoint $compact
.point AX = 0202H
.point BL = exception/fault number (00h - 1Fh)
.endpoint
.np
If the call succeeds, the carry flag is clear and the
.mono selector:offset
of the protected-mode exception handler is returned in CX:EDX.
If it fails, the carry flag is set.
.np
The value returned in CX is a valid protected-mode selector,
not a real-mode segment.
.*
.note Function 0203H
.*
.ix 'DPMI' 'set exception handler vector'
This function sets the processor exception handler vector.
This function allows protected-mode applications to intercept
processor exceptions that are not handled by the DPMI environment.
Programs may wish to handle exceptions such as "not present segment
faults" which would otherwise generate a fatal error.
Pass the following information:
.begpoint $compact
.point AX = 0203H
.point BL = exception/fault number (00h - 1Fh)
.point CX:EDX = selector:offset of the exception handler
.endpoint
.np
If the call succeeds, the carry flag is clear.
If it fails, the carry flag is set.
.np
The address passed in CX must be a valid protected-mode selector, such
as Function 204H returns, and not a real-mode segment.
A 32-bit implementation must supply a 32-bit offset in the EDX
register.
If the handler chains to the next handler, it must use a 32-bit
interrupt stack frame to do so.
.np
The handler should return using a far return instruction.
The original SS:ESP, CS:EIP and flags on the stack, including the
interrupt flag, will be restored.
.np
All fault stack frames have an error code.
However the error code is only valid for exceptions 08h, 0Ah, 0Bh,
0Ch, 0Dh, and 0Eh.
.np
The handler must preserve and restore all registers.
.np
The exception handler will be called on a locked stack with interrupts
disabled.
The original SS, ESP, CS, and EIP will be pushed on the exception
handler stack frame.
.np
The handler must either return from the call by executing a far return
or jump to the next handler in the chain (which will execute a far
return or chain to the next handler).
.np
The procedure can modify any of the values on the stack pertaining
to the exception before returning.
This can be used, for example, to jump to a procedure by modifying
the CS:EIP on the stack.
Note that the procedure must not modify the far return address on the
stack &mdash. it must return to the original caller.
The caller will then restore the flags, CS:EIP and SS:ESP from the
stack frame.
.np
If the DPMI client does not handle an exception, or jumps to the
default exception handler, the host will reflect the exception as an
interrupt for exceptions 0, 1, 2, 3, 4, 5 and 7.
Exceptions 6 and 8 - 1Fh will be treated as fatal errors and the
client will be terminated.
.np
Exception handlers will only be called for exceptions that occur in
protected mode.
.*
.note Function 0204H
.*
.ix 'DPMI' 'get protected-mode interrupt vector'
This function gets the CS:EIP
.mono selector:offset
of the current protected-mode interrupt handler for a specified
interrupt number.
Pass the following information:
.begpoint $compact
.point AX = 0204H
.point BL = interrupt number
.endpoint
.np
This call always succeeds.
All 100H (256 decimal) interrupt vectors are supported by the host.
When the call returns, the carry flag is clear and CX:EDX contains
the protected-mode
.mono selector:offset
of the exception handler.
.np
A 32-bit offset is returned in the EDX register.
.*
.note Function 0205H
.*
.ix 'DPMI' 'set protected-mode interrupt vector'
This function sets the address of the specified protected-mode
interrupt vector.
Pass the following information:
.begpoint $compact
.point AX = 0205H
.point BL = interrupt number
.point CX:EDX = selector:offset of the exception handler
.endpoint
.np
If the call succeeds, the carry flag is clear; if it fails, the carry
flag is set.
.np
The address passed in CX must be a valid protected-mode selector, such
as Function 204H returns, and not a real-mode segment.
A 32-bit implementation must supply a 32-bit offset in the EDX
register.
If the handler chains to the next handler, it must use a 32-bit
interrupt stack frame to do so.
.endnote
.*
.section Translation Services
.*
.np
.ix 'translation services'
These services are provided so that protected-mode programs can call
real-mode software that DPMI does not support directly.
The protected-mode program must set up a data structure with the
appropriate register values.
This "real-mode call structure" is shown below.
.np
.if '&format' eq '7x9' .do begin
.se *p0=&INDlvl-1+11
.se *c1=8
.se *c2=37
.do end
.el .do begin
.se *p0=&INDlvl-1+11
.se *c1=8
.se *c2=41
.do end
.cp 40
.tb set $
.tb &*p0 +&*c1 +&*c2
.bx on &*p0-3 +&*c1 +&*c2
$Offset$Register
.bx
$00H$EDI
.bx
$04H$ESI
.bx
$08H$EBP
.bx
$0CH$Reserved by system
.bx
$10H$EBX
.bx
$14H$EDX
.bx
$18H$ECX
.bx
$1CH$EAX
.bx
$20H$Flags
.bx
$22H$ES
.bx
$24H$DS
.bx
$26H$FS
.bx
$28H$GS
.bx
$2AH$IP
.bx
$2CH$CS
.bx
$2EH$SP
.bx
$30H$SS
.bx off
.tb set
.tb
.np
After the call or interrupt is complete, all real-mode registers and
flags except SS, SP, CS, and IP will be copied back to the real-mode
call structure so that the caller can examine the real-mode return
values.
.np
The values in the segment registers should be real-mode segments,
not protected-mode selectors.
.np
The translation services will provide a real-mode stack if the SS:SP
fields are zero.
However, the stack provided is relatively small.
If the real-mode procedure/interrupt routine uses more than 30 words
of stack space then you should provide your own real-mode stack.
.begnote
.*
.note Function 0300H
.*
.ix 'DPMI' 'simulate real-mode interrupt'
This function simulates a real-mode interrupt.
This function simulates an interrupt in real mode.
It will invoke the CS:IP specified by the real-mode interrupt
vector and the handler must return by executing an
.mono iret.
Pass the following information:
.begpoint $compact
.point AX = 0300H
.point BL = interrupt number
.point BH = flags
Bit 0 = 1 resets the interrupt controller and A20 line.
Other flags are reserved and must be 0.
.point CX = number of words to copy from protected-mode stack to real-mode stack
.point ES:EDI = the selector:offset of real-mode call structure
.endpoint
.np
If the call fails, the carry flag is set.
.np
If the call succeeds, the carry flag is clear and ES:EDI contains the
.mono selector:offset
of the modified real-mode call structure.
.np
The CS:IP in the real-mode call structure is ignored by this service.
The appropriate interrupt handler will be called based on the value
passed in BL.
.np
The flags specified in the real-mode call structure will be pushed on
the real-mode stack
.mono iret
frame.
The interrupt handler will be called with the interrupt and trace
flags clear.
.np
It is up to the caller to remove any parameters that were pushed on
the protected-mode stack.
.np
The flag to reset the interrupt controller and the A20 line is
ignored by DPMI implementations that run in Virtual 8086 mode.
It causes DPMI implementations that return to real mode to set the
interrupt controller and A20 address line hardware to its normal
real-mode state.
.*
.note Function 0301H
.*
.ix 'DPMI' 'simulate real-mode far call'
(&dos4gprd Professional only)
This function calls a real-mode procedure with a FAR return frame.
The called procedure must execute a FAR return when it completes.
Pass the following information:
.begpoint $compact
.point AX = 0301H
.point BH = flags
Bit 0 = 1 resets the interrupt controller and A20 line.
Other flags reserved and must be 0.
.point CX = Number of words to copy from protected-mode to real-mode stack
.point ES:EDI = selector:offset of real-mode call structure
.endpoint
.np
If the call succeeds, the carry flag is clear and ES:EDI contains the
.mono selector:offset
of modified real-mode call structure.
.np
If the call fails, the carry flag is set.
.autonote Notes:
.note
The CS:IP in the real-mode call structure specifies the address of the
real-mode procedure to call.
.note
The real-mode procedure must execute a FAR return when it has
completed.
.note
If the SS:SP fields are zero then a real-mode stack will be provided
by the DPMI host.
Otherwise, the real-mode SS:SP will be set to the specified values
before the procedure is called.
.note
When the Int 31h returns, the real-mode call structure will contain
the values that were returned by the real-mode procedure.
.note
It is up to the caller to remove any parameters that were pushed on
the protected-mode stack.
.note
The flag to reset the interrupt controller and A20 line is ignored by
DPMI implementations that run in Virtual 8086 mode.
It causes DPMI implementations that return to real mode to set the
interrupt controller and A20 address line hardware to its normal
real-mode state.
.endnote
.*
.note Function 0302H
.*
.ix 'DPMI' 'simulate real-mode iret call'
(&dos4gprd Professional only)
This function calls a real-mode procedure with an
.mono iret
frame.
The called procedure must execute an
.mono iret
when it completes.
Pass the following information:
.begpoint $compact
.point AX = 0302H
.point BH = flags
Bit 0 = 1 resets the interrupt controller and A20 line.
Other flags reserved and must be 0.
.point CX = Number of words to copy from protected-mode to real-mode stack
.point ES:EDI = selector:offset of real-mode call structure
.endpoint
.np
If the call succeeds, the carry flag is clear and ES:EDI contains the
.mono selector:offset
of modified real-mode call structure.
.np
If the call fails, the carry flag is set.
.autonote Notes:
.note
The CS:IP in the real-mode call structure specifies the address of the
real-mode procedure to call.
.note
The real-mode procedure must execute an
.mono iret
when it has completed.
.note
If the SS:SP fields are zero then a real-mode stack will be provided
by the DPMI host. Otherwise, the real-mode SS:SP will be set to the
specified values before the procedure is called.
.note
When the Int 31h returns, the real-mode call structure will contain
the values that were returned by the real-mode procedure.
.note
The flags specified in the real-mode call structure will be pushed the
real-mode stack
.mono iret
frame. The procedure will be called with the interrupt and trace flags
clear.
.note
It is up to the caller to remove any parameters that were pushed on
the protected-mode stack.
.note
The flag to reset the interrupt controller and A20 line is ignored by
DPMI implementations that run in Virtual 8086 mode. It causes DPMI
implementations that return to real mode to set the interrupt
controller and A20 address line hardware to its normal real-mode
state.
.endnote
.*
.note Function 0303H
.*
.ix 'DPMI' 'allocate real-mode callback address'
(&dos4gprd Professional only)
This function allocates a real-mode callback address.
This service is used to obtain a unique real-mode SEG:OFFSET that will
transfer control from real mode to a protected-mode procedure.
.np
At times it is necessary to hook a real-mode interrupt or device
callback in a protected-mode driver. For example, many mouse drivers
call an address whenever the mouse is moved. Software running in
protected mode can use a real-mode callback to intercept the mouse
driver calls.
Pass the following information:
.begpoint $compact
.point AX = 0303H
.point DS:ESI = selector:offset of procedure to call
.point ES:EDI = selector:offset of real-mode call structure
.endpoint
.np
If the call succeeds, the carry flag is clear and CX:DX contains the
.mono segment:offset
of real-mode callback address.
.np
If the call fails, the carry flag is set.
.begnote
.note Callback Procedure Parameters
.illust begin
Interrupts disabled
DS:ESI = selector:offset of real-mode SS:SP
ES:EDI = selector:offset of real-mode call structure
SS:ESP = Locked protected-mode API stack
All other registers undefined
.illust end
.note Return from Callback Procedure
.illust begin
Execute an IRET to return
ES:EDI =  selector:offset of real-mode call structure
to restore (see note)
.illust end
.endnote
.autonote Notes:
.note
Since the real-mode call structure is static, you must be careful when
writing code that may be reentered. The simplest method of avoiding
reentrancy is to leave interrupts disabled throughout the entire call.
However, if the amount of code executed by the callback is large then
you will need to copy the real-mode call structure into another
buffer. You can then return with ES:EDI pointing to the buffer you
copied the data to &mdash. it does not have to point to the original real
mode call structure.
.note
The called procedure is responsible for modifying the real-mode CS:IP
before returning. If the real-mode CS:IP is left unchanged then the
real-mode callback will be executed immediately and your procedure
will be called again. Normally you will want to pop a return address
off of the real-mode stack and place it in the real-mode CS:IP. The
example code in the next section demonstrates chaining to another
interrupt handler and simulating a real-mode
.mono iret.
.note
To return values to the real-mode caller, you must modify the
real-mode call structure.
.note
Remember that all segment values in the real-mode call structure will
contain real-mode segments, not selectors. If you need to examine data
pointed to by a real-mode seg:offset pointer, you should not use the
segment to selector service to create a new selector. Instead,
allocate a descriptor during initialization and change the
descriptor's base to 16 times the real-mode segment's value. This is
important since selectors allocated though the segment to selector
service can never be freed.
.note
DPMI hosts should provide a minimum of 16 callback addresses per
task.
.endnote
.np
The following code is a sample of a real-mode interrupt hook. It hooks
the DOS Int 21h and returns an error for the delete file function
(AH=41h). Other calls are passed through to DOS. This example is
somewhat silly but it demonstrates the techniques used to hook a real
mode interrupt. Note that since DOS calls are reflected from protected
mode to real mode, the following code will intercept all DOS calls
from both real mode and protected mode.
.code begin
;******************************************************
; This procedure gets the current Int 21h real-mode
; Seg:Offset, allocates a real-mode callback address,
; and sets the real-mode Int 21h vector to the call-
; back address.
;******************************************************
Initialization_Code:
;
; Create a code segment alias to save data in
;
        mov     ax, 000Ah
        mov     bx, cs
        int     31h
        jc      ERROR
        mov     ds, ax
        ASSUMES DS,_TEXT
;
; Get current Int 21h real-mode SEG:OFFSET
;
        mov     ax, 0200h
        mov     bl, 21h
        int     31h
        jc      ERROR
        mov     [Orig_Real_Seg], cx
        mov     [Orig_Real_Offset], dx
;
; Allocate a real-mode callback
;
        mov     ax, 0303h
        push    ds
        mov     bx, cs
        mov     ds, bx
        mov     si, OFFSET My_Int_21_Hook
        pop     es
        mov     di, OFFSET My_Real_Mode_Call_Struc
        int     31h
        jc      ERROR
;
; Hook real-mode int 21h with the callback address
;
        mov     ax, 0201h
        mov     bl, 21h
        int     31h
        jc      ERROR

;******************************************************
;
; This is the actual Int 21h hook code.  It will return
; an "access denied" error for all calls made in real
; mode to delete a file.  Other calls will be passed
; through to DOS.
;
; ENTRY:
;    DS:SI -> Real-mode SS:SP
;    ES:DI -> Real-mode call structure
;    Interrupts disabled
;
; EXIT:
;    ES:DI -> Real-mode call structure
;
;******************************************************

My_Int_21_Hook:
        cmp     es:[di.RealMode_AH], 41h
        jne     Chain_To_DOS
;
; This is a delete file call (AH=41h).  Simulate an
; iret on the real-mode stack, set the real-mode
; carry flag, and set the real-mode AX to 5 to indicate
; an access denied error.
;
        cld
        lodsw                   ; Get real-mode ret IP
        mov     es:[di.RealMode_IP], ax
        lodsw                   ; Get real-mode ret CS
        mov     es:[di.RealMode_CS], ax
        lodsw                   ; Get real-mode flags
        or      ax, 1           ; Set carry flag
        mov     es:[di.RealMode_Flags], ax
        add     es:[di.RealMode_SP], 6
        mov     es:[di.RealMode_AX], 5
        jmp     My_Hook_Exit
;
; Chain to original Int 21h vector by replacing the
; real-mode CS:IP with the original Seg:Offset.
;
Chain_To_DOS:
        mov     ax, cs:[Orig_Real_Seg]
        mov     es:[di.RealMode_CS], ax
        mov     ax, cs:[Orig_Real_Offset]
        mov     es:[di.RealMode_IP], ax

My_Hook_Exit:
        iret
.code end
.*
.note Function 0304H
.*
.ix 'DPMI' 'free real-mode callback address'
(&dos4gprd Professional only)
This function frees a real-mode callback address that was allocated
through the allocate real-mode callback address service.
Pass the following information:
.begpoint $compact
.point AX = 0304H
.point CX:DX = Real-mode callback address to free
.endpoint
.np
If the call succeeds, the carry flag is clear; if it fails, the carry
flag is set.
.autonote Notes:
.note
Real-mode callbacks are a limited resource. Your code should free any
break point that it is no longer using.
.endnote
.*
.endnote
.*
.section DPMI Version
.*
.begnote
.*
.note Function 0400H
.*
.ix 'DPMI' 'get DPMI version'
This function returns the version of DPMI services supported. Note
that this is not necessarily the version of any operating system that
supports DPMI. It should be used by programs to determine what calls
are legal in the current environment.
Pass the following information:
.begpoint $compact
.point AX = 0400H
.endpoint
.np
The information returned is:
.begpoint $compact
.point AH = Major version
.point AL = Minor version
.point BX = Flags
Bit 0 = 1 if running under an 80386 DPMI implementation.
Bit 1 = 1 if processor is returned to real mode for reflected
interrupts (as opposed to Virtual 8086 mode).
Bit 2 = 1 if virtual memory is supported.
Bit 3 is reserved and undefined.
All other bits are zero and reserved for later use.
.point CL = Processor type
.illust begin
02 = 80286
03 = 80386
04 = 80486
05 = Pentium
.illust end
.point DH = Current value of virtual master PIC base interrupt
.point DL = Current value of virtual slave PIC base interrupt
.point Carry flag clear (call cannot fail)
.endpoint
.*
.endnote
.*
.section Memory Management Services
.*
.begnote
.*
.note Function 0500H
.*
.ix 'memory management services'
.ix 'DPMI' 'get free memory information'
This function gets information about free memory.
Pass the following information:
.begpoint $compact
.point AX = 0500H
.point ES:EDI = the selector:offset of a 30H byte buffer.
.endpoint
.np
If the call fails, the carry flag is set.
.np
If the call succeeds, the carry flag is clear and ES:EDI contains the
.mono selector:offset
of a buffer with the structure shown in the figure below.
.np
.if '&format' eq '7x9' .do begin
.se *p0=&INDlvl-1+11+1
.se *c1=8
.se *c2=37
.do end
.el .do begin
.se *p0=&INDlvl-1+11+3
.se *c1=8
.se *c2=40
.do end
.cp 28
.tb set $
.tb &*p0. +&*c1. +&*c2.
.bx on &*p0.-3 +&*c1. +&*c2.
$Offset$Description
.bx
$00H$Largest available block, in bytes
.bx
$04H$Maximum unlocked page allocation
.bx
$08H$Largest block of memory (in pages) that could$
$   $be allocated and then locked
.bx
$0CH$Total linear address space size, in pages, including$
$   $already allocated pages
.bx
$10H$Total number of free pages and pages currently$
$   $unlocked and available for paging out
.bx
$14H$Number of physical pages not in use
.bx
$18H$Total number of physical pages managed by host
.bx
$1CH$Free linear address space, in pages
.bx
$20H$Size of paging/file partition, in pages
.bx
$24H -$Reserved
$2FH$
.bx off
.tb set
.tb
.np
Only the first field of the structure is guaranteed to contain a valid
value.
Any field that is not returned by &dos4g is set to -1 (0FFFFFFFFH).
.*
.note Function 0501H
.*
.ix 'DPMI' 'allocate memory block'
This function allocates and commits linear memory.
Pass the following information:
.begpoint $compact
.point AX = 0501H
.point BX:CX = size of memory to allocate, in bytes.
.endpoint
.np
If the call succeeds, the carry flag is clear, BX:CX contains the
linear address of the allocated memory, and SI:DI contains the memory
block handle used to free or resize the block.
If the call fails, the carry flag is set.
.np
No selectors are allocated for the memory block.
The caller must allocate and initialize selectors needed to access the
memory.
.np
If VMM is present, the memory is allocated as unlocked, page granular
blocks.
Because of the page granularity, memory should be allocated in
multiples of 4KB.
.*
.note Function 0502H
.*
.ix 'DPMI' 'free memory block'
This function frees a block of memory allocated through function
0501H.
Pass the following information:
.begpoint $compact
.point AX = 0502H
.point SI:DI = handle returned with function 0501H when memory was allocated
.endpoint
.np
If the call succeeds, the carry flag is clear; if it fails, the carry
flag is set.
You must also free any selectors allocated to point to the freed
memory block.
.*
.note Function 0503H
.*
.ix 'DPMI' 'resize memory block'
This function resizes a block of memory allocated through the 0501H
function.
If you resize a block of linear memory, it may have a new linear
address and a new handle.
Pass the following information:
.begpoint $compact
.point AX = 0503H
.point BX:CX = new size of memory block, in bytes
.point SI:DI = handle returned with function 0501H when memory was allocated
.endpoint
.np
If the call succeeds, the carry flag is clear, BX:CX contains the new
linear address of the memory block, and SI:DI contains the new handle
of the memory block.
If the call fails, the carry flag is set.
.np
If either the linear address or the handle has changed, update the
selectors that point to the memory block.
Use the new handle instead of the old one.
.np
You cannot resize a memory block to zero bytes.
.endnote
.*
.section Page Locking Services
.*
.np
.ix 'page locking services'
These services are only useful under DPMI implementations that support
virtual memory.
Although memory ranges are specified in bytes, the actual unit of
memory that will be locked will be one or more pages.
Page locks are maintained as a count.
When the count is decremented to zero, the page is unlocked and can be
swapped to disk.
This means that if a region of memory is locked three times then it
must be unlocked three times before the pages will be unlocked.
.begnote
.*
.note Function 0600H
.*
.ix 'DPMI' 'lock linear region'
This function locks a specified linear address range.
Pass the following information:
.begpoint $compact
.point AX = 0600H
.point BX:CX = starting linear address of memory to lock
.point SI:DI = size of region to lock (in bytes)
.endpoint
.np
If the call fails, the carry flag is set and none of the memory will
be locked.
.np
If the call succeeds, the carry flag is clear.
If the specified region overlaps part of a page at the beginning or
end of a region, the page(s) will be locked.
.*
.note Function 0601H
.*
.ix 'DPMI' 'unlock linear region'
This function unlocks a specified linear address range that was
previously locked using the "lock linear region" function (0600h).
Pass the following information:
.begpoint $compact
.point AX = 0601H
.point BX:CX = starting linear address of memory to unlock
.point SI:DI = size of region to unlock (in bytes)
.endpoint
.np
If the call fails, the carry flag is set and none of the memory will
be unlocked.
An error will be returned if the memory was not previously locked or
if the specified region is invalid.
.np
If the call succeeds, the carry flag is clear.
If the specified region overlaps part of a page at the beginning or
end of a region, the page(s) will be unlocked.
Even if the call succeeds, the memory will remain locked if the
lock count is not decremented to zero.
.*
.note Function 0604H
.*
.ix 'DPMI' 'get page size'
This function gets the page size for Virtual Memory (VM) only.
This function returns the size of a single memory page in bytes.
Pass the following information:
.begpoint $compact
.point AX = 0604H
.endpoint
.np
If the call succeeds, the carry flag is clear and BX:CX = Page size in
bytes.
.np
If the call fails, the carry flag is set.
.*
.endnote
.*
.section Demand Paging Performance Tuning Services
.*
.np
.ix 'DPMI' 'demand paging'
.ix 'page tuning services'
Some applications will discard memory objects or will not access
objects for long periods of time. These services can be used to
improve the performance of demand paging.
.np
Although these functions are only relevant for DPMI implementations
that support virtual memory, other implementations will ignore these
functions (it will always return carry clear). Therefore your code can
always call these functions regardless of the environment it is
running under.
.np
Since both of these functions are simply advisory functions, the
operating system may choose to ignore them. In any case, your code
should function properly even if the functions fail.
.begnote
.*
.note Function 0702H
.*
.ix 'DPMI' 'mark page'
(&dos4gprd Professional only)
This function marks a page as a demand paging candidate.
This function is used to inform the operating system that a range of
pages should be placed at the head of the page out candidate list.
This will force these pages to be swapped to disk ahead of other pages
even if the memory has been accessed recently. However, all memory
contents will be preserved.
.np
This is useful, for example, if a program knows that a given piece of
data will not be accessed for a long period of time. That data is
ideal for swapping to disk since the physical memory it now occupies
can be used for other purposes.
Pass the following information:
.begpoint $compact
.point AX = 0702H
.point BX:CX = Starting linear address of pages to mark
.point SI:DI = Number of bytes to mark as paging candidates
.endpoint
.np
If the call succeeds, the carry flag is clear; if it fails, the carry
flag is set.
.autonote Notes:
.note
This function does not force the pages to be swapped to disk
immediately.
.note
Partial pages will not be discarded.
.endnote
.*
.note Function 0703H
.*
.ix 'DPMI' 'discard page'
(&dos4gprd Professional only)
This function discards page contents.
This function discards the entire contents of a given linear memory
range. It is used after a memory object that occupied a given piece of
memory has been discarded.
.np
The contents of the region will be undefined the next time the memory
is accessed. All values previously stored in this memory will be lost.
Pass the following information:
.begpoint $compact
.point AX = 0703H
.point BX:CX = Starting linear address of pages to discard
.point SI:DI = Number of bytes to discard
.endpoint
.np
If the call succeeds, the carry flag is clear; if it fails, the carry
flag is set.
.autonote Notes:
.note
Partial pages will not be discarded.
.endnote
.*
.endnote
.*
.section Physical Address Mapping
.*
.np
Memory mapped devices such as network adapters and displays sometimes
have memory mapped at physical addresses that lie outside of the
normal 1Mb of memory that is addressable in real mode. Under many
implementations of DPMI, all addresses are linear addresses since they
use the paging mechanism of the 80386. This service can be used by
device drivers to convert a physical address into a linear address.
The linear address can then be used to access the device memory.
.begnote
.*
.note Function 0800H
.*
.ix 'DPMI' 'physical address mapping'
This function is used for Physical Address Mapping.
.np
Some implementations of DPMI may not support this call because it
could be used to circumvent system protection. This call should only
be used by programs that absolutely require direct access to a memory
mapped device.
.np
Pass the following information:
.begpoint $compact
.point AX = 0800H
.point BX:CX = Physical address of memory
.point SI:DI = Size of region to map in bytes
.endpoint
.np
If the call succeeds, the carry flag is clear and BX:CX = Linear
Address that can be used to access the physical memory.
.np
If the call fails, the carry flag is set.
.autonote Notes:
.note
Under DPMI implementations that do not use the 80386 paging mechanism,
the call will always succeed and the address returned will be
equal to the physical address parameter passed into this function.
.note
It is up to the caller to build an appropriate selector to access the
memory.
.note
Do not use this service to access memory that is mapped in the first
megabyte of address space (the real-mode addressable region).
.endnote
.*
.note Function 0801H
.*
.ix 'DPMI' 'free physical address mapping'
This function is used to free Physical Address Mapping.
Pass the following information:
.begpoint $compact
.point AX = 0801H
.point BX:CX = Linear address returned by Function 0800H.
.endpoint
.np
If the call succeeds, the carry flag is clear; if it fails, the carry
flag is set.
.autonote Notes:
.note
The client should call this function when it is finished using a
device previously mapped to linear addresses with the Physical Address
Mapping function (Function 0800H).
.endnote
.*
.endnote
.*
.section Virtual Interrupt State Functions
.*
.np
.ix 'DPMI' 'virtual interrupt state'
Under many implementations of DPMI, the interrupt flag in protected
mode will always be set (interrupts enabled). This is because the
program is running under a protected operating system that cannot
allow programs to disable physical hardware interrupts. However, the
operating system will maintain a "virtual" interrupt state for
protected-mode programs. When the program executes a CLI instruction,
the program's virtual interrupt state will be disabled, and the
program will not receive any hardware interrupts until it executes an
STI to reenable interrupts (or calls service 0901h).
.np
When a protected-mode program executes a PUSHF instruction, the real
processor flags will be pushed onto the stack. Thus, examining the
flags pushed on the stack is not sufficient to determine the state of
the program's virtual interrupt flag. These services enable programs
to get and modify the state of their virtual interrupt flag.
.np
The following sample code enters an interrupt critical section and
then restores the virtual interrupt state to it's previous state.
.code begin
;
; Disable interrupts and get previous interrupt state
;
        mov     ax, 0900h
        int     31h
;
; At this point AX = 0900h or 0901h
;
        .
        .
        .
;
; Restore previous state (assumes AX unchanged)
;
        int     31h
.code end
.begnote
.*
.note Function 0900H
.*
.ix 'DPMI' 'get and disable virtual interrupt state'
This function gets and disables Virtual Interrupt State.
This function will disable the virtual interrupt flag and return the
previous state of the virtual interrupt flag.
Pass the following information:
.begpoint $compact
.point AX = 0900H
.endpoint
.np
After the call, the carry flag is clear (this function always
succeeds) and virtual interrupts are disabled.
.illust begin
AL = 0 if virtual interrupts were previously disabled.
AL = 1 if virtual interrupts were previously enabled.
.illust end
.autonote Notes:
.note
AH will not be changed by this procedure. Therefore, to restore the
previous state, simply execute an Int 31h.
.endnote
.*
.note Function 0901H
.*
.ix 'DPMI' 'get and enable virtual interrupt state'
This function gets and enables the Virtual Interrupt State.
This function will enable the virtual interrupt flag and return the
previous state of the virtual interrupt flag.
Pass the following information:
.begpoint $compact
.point AX = 0901H
.endpoint
.np
After the call, the carry flag is clear (this function always
succeeds) and virtual interrupts are enabled.
.illust begin
AL = 0 if virtual interrupts were previously disabled.
AL = 1 if virtual interrupts were previously enabled.
.illust end
.autonote Notes:
.note
AH will not be changed by this procedure. Therefore, to restore the
previous state, simply execute an Int 31h.
.endnote
.*
.note Function 0902H
.*
.ix 'DPMI' 'get virtual interrupt state'
This function gets the Virtual Interrupt State.
This function will return the current state of the virtual interrupt
flag.
Pass the following information:
.begpoint $compact
.point AX = 0902H
.endpoint
.np
After the call, the carry flag is clear (this function always
succeeds).
.illust begin
AL = 0 if virtual interrupts are disabled.
AL = 1 if virtual interrupts are enabled.
.illust end
.*
.endnote
.*
.section Vendor Specific Extensions
.*
.np
.ix 'DPMI' 'vendor extensions'
Some DOS extenders provide extensions to the standard set of DPMI
calls. This call is used to obtain an address which must be called to
use the extensions. The caller points DS:ESI to a null terminated
string that specifies the vendor name or some other unique identifier
to obtain the specific extension entry point.
.begnote
.*
.note Function 0A00H
.*
.ix 'DPMI' 'get API entry point'
This function gets Tenberry Software's API Entry Point.
Pass the following information:
.begpoint $compact
.point AX = 0A00H
.point DS:ESI = Pointer to null terminated string "RATIONAL DOS/4G"
.endpoint
.np
If the call succeeds, the carry flag is clear and ES:EDI = Extended
API entry point.
DS, FS, GS, EAX, EBX, ECX, EDX, ESI, and EBP may be modified.
.np
If the call fails, the carry flag is set.
.autonote Notes:
.note
Execute a far call to call the API entry point.
.note
All extended API parameters are specified by the vendor.
.note
The string comparison used to return the API entry point is case
sensitive.
.endnote
.*
.endnote
.*
.section Coprocessor Status
.*
.begnote
.*
.note Function 0E00H
.*
.ix 'DPMI' 'get coprocessor status'
This function gets the coprocessor status.
Pass the following information:
.begpoint $compact
.point AX = 0E00H
.endpoint
.np
If the call succeeds, the carry flag is clear and AX contains the
coprocessor status.
.begnote $compact
:DTHD.Bit
:DDHD.Significance
.note 0
MPv (MP bit in the virtual MSW/CR0).
.br
0 = Numeric coprocessor is disabled for this client.
.br
1 = Numeric coprocessor is disabled for this client.
.note 1
EMv (EM bit in the virtual MSW/CR0).
.br
0 = Client is not emulating coprocessor instructions.
.br
1 = Client is emulating coprocessor instructions.
.note 2
MPr (MP bit from the actual MSW/CR0).
.br
0 = Numeric coprocessor is not present.
.br
1 = Numeric coprocessor is present.
.note 1
EMr (EM bit from the actual MSW/CR0).
.br
0 = Host is not emulating coprocessor instructions.
.br
1 = Host is emulating coprocessor instructions.
.note 4-7
Coprocessor type.
.illust begin
00H = no coprocessor.
02H = 80287
03H = 80387
04H = 80486 with numeric coprocessor
05H = Pentium
.illust end
.np
.note 8-15
Not applicable.
.endnote
.np
If the call fails, the carry flag is set.
.autonote Notes:
.note
If the real EM (EMr) bit is set, the host is supplying or is capable
of supplying floating-point emulation.
.note
If the MPv bit is not set, the host may not need to save the
coprocessor state for this virtual machine to improve system
performance.
.note
The MPr bit setting should be consistent with the setting of the
coprocessor type information. Ignore MPr bit information if it is in
conflict with the coprocessor type information.
.note
If the virtual EM (EMv) bit is set, the host delivers all coprocessor
exceptions to the client, and the client is performing its own
floating-point emulation (wether or not a coprocessor is present or
the host also has a floating-point emulator). In other words, if the
EMv bit is set, the host sets the EM bit in the real CR0 while the
virtual machine is active, and reflects coprocessor not present faults
(int 7) to the virtual machine.
.note
A client can determine the CPU type with int 31H Function 0400H, but a
client should not draw any conclusions about the presence or absence
of a coprocessor based on the CPU type alone.
.endnote
.*
.note Function 0E01H
.*
.ix 'DPMI' 'set coprocessor emulation'
This function sets coprocessor emulation.
Pass the following information:
.begpoint $compact
.point AX = 0E01H
.point BX = coprocessor bits
.begnote $compact
:DTHD.Bit
:DDHD.Significance
.note 0
New value of MPv bit for client's virtual CR0.
.br
0 = Disable numeric coprocessor for this client.
.br
1 = Enable numeric coprocessor for this client.
.note 1
New value of EMv bit for client's virtual CR0.
.br
0 = client will not supply coprocessor emulation.
.br
1 = client will supply coprocessor emulation.
.note 2-15
Not applicable.
.endnote
.endpoint
.np
If the call succeeds, the carry flag is clear; if it fails, the carry
flag is set.
.*
.endnote
.*
.endlevel
