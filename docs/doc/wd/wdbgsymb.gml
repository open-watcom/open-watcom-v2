.chap *refid=vidsymb Predefined Symbols
.*
.np
.ix 'symbols' 'predefined'
The &dbgname defines a number of symbols which have special meaning.
Each of the registers is designated by a special name. Note that the
registers listed here are applicable when the target is an x86 platorm.
For other platforms, the register set is different.
.begnote $compact
.mnote eax
32-bit EAX register (32-bit mode only)
.mnote ax
16-bit AX register
.mnote al
8-bit AL register
.mnote ah
8-bit AH register
.mnote ebx
32-bit EBX register (32-bit mode only)
.mnote bx
16-bit BX register
.mnote bl
8-bit BL register
.mnote bh
8-bit BH register
.mnote ecx
32-bit ECX register (32-bit mode only)
.mnote cx
16-bit CX register
.mnote cl
8-bit CL register
.mnote ch
8-bit CH register
.mnote edx
32-bit EDX register (32-bit mode only)
.mnote dx
16-bit DX register
.mnote dl
8-bit DL register
.mnote dh
8-bit DH register
.mnote eip
Instruction pointer register (32-bit mode only)
.mnote ip
Instruction pointer register
.mnote esi
Source index register (32-bit mode only)
.mnote si
Source index register
.mnote edi
Destination index register (32-bit mode only)
.mnote di
Destination index register
.mnote esp
Stack pointer register (32-bit mode only)
.mnote sp
Stack pointer register
.mnote ebp
Base pointer register (32-bit mode only)
.mnote bp
Base pointer register
.mnote cs
Code segment register
.mnote ds
Data segment register
.mnote es
Extra segment register
.mnote fs
Segment register (32-bit mode only)
.mnote gs
Segment register (32-bit mode only)
.mnote ss
Stack segment register
.mnote fl
Flags register
.mnote efl
Flags register (32-bit mode only)
.mnote fl.flg_bit_name
Individual bits in Flags register
.syntax
flg_bit_name ::= "c" | "p" | "a" | "z" | "s" | "i" | "d" | "o"
.esyntax
.pc
.mnote efl.flg_bit_name
Individual bits in Flags register
.syntax
flg_bit_name ::= "c" | "p" | "a" | "z" | "s" | "i" | "d" | "o"
.esyntax
.np
The following table lists the full name for each of the flags register
bits:
.begnote $compact
.ix 'registers' 'flags'
.mnote fl.o, efl.o
overflow flag
.mnote fl.d, efl.d
direction flag
.mnote fl.i, efl.i
interrupt flag
.mnote fl.s, efl.s
sign flag
.mnote fl.z, efl.z
zero flag
.mnote fl.a, efl.a
auxiliary carry flag
.mnote fl.p, efl.p
parity flag
.mnote fl.c, efl.c
carry flag
.endnote
.pc
.ix 'registers' 'floating point'
.ix 'registers' 'st0 - st7'
.mnote st0 - st7
Numeric Data Processor registers (math coprocessor registers)
.ix 'registers' 'control word'
.ix 'registers' 'cw'
.ix '8087' 'registers'
.mnote cw
8087 control word (math coprocessor control word)
.mnote cw.cw_bit_name
Individual bits in the control word
.syntax
cw_bit_name ::= "ic" | "rc" | "pc" | "iem" | "pm" |
                        "um" | "om" | "zm" | "dm" | "im"
.esyntax
.np
The following table lists the full name for each of the control word
bits:
.begnote $compact
.mnote cw.ic
infinity control
.illust begin
0 = projective
1 = affine
.illust end
.pc
.mnote cw.rc
rounding control (2 bits)
.illust begin
00 = round to nearest or even
01 = round down (towards negative infinity)
10 = round up (towards positive infinity)
11 = chop (truncate toward zero)
.illust end
.pc
.mnote cw.pc
precision control (2 bits)
.illust begin
00 = 24 bits
01 = reserved
10 = 53 bits
11 = 64 bits
.illust end
.pc
.mnote cw.iem
interrupt enable mask (8087 only)
.illust begin
0 = interrupts enabled
1 = interrupts disabled (masked)
.illust end
.pc
.mnote cw.pm
precision (inexact result) mask
.mnote cw.um
underflow mask
.mnote cw.om
overflow mask
.mnote cw.zm
zero-divide mask
.mnote cw.dm
denormalized operand mask
.mnote cw.im
invalid operand mask
.endnote
.pc
.ix 'registers' 'status word'
.ix 'registers' 'sw'
.ix '8087' 'registers'
.mnote sw
8087 status word (math coprocessor status word)
.mnote sw.sw_bit_name
Individual bits in the status word
.syntax
sw_bit_name ::=  "b" | "c3" | "st" | "c2" | "c1" |
                        "c0" | "es" | "sf" | "pe" | "ue" |
                        "oe" | "ze" | "de" | "ie"
.esyntax
.np
The following table lists the full name for each of the status word
bits:
.begnote $compact
.mnote sw.b
busy
.mnote sw.c3
condition code bit 3
.mnote sw.st
stack stop pointer (3 bits)
.illust begin
000 = register 0 is stack top
001 = register 1 is stack top
010 = register 2 is stack top
    .
    .
    .
111 = register 7 is stack top
.illust end
.pc
.mnote sw.c2
condition code bit 2
.mnote sw.c1
condition code bit 1
.mnote sw.c0
condition code bit 0
.mnote sw.es
error summary (287, 387 only)
.mnote sw.sf
stack fault (387 only)
.mnote sw.pe
precision (inexact result) exception
.mnote sw.ue
underflow exception
.mnote sw.oe
overflow exception
.mnote sw.ze
zero-divide exception
.mnote sw.de
denormalized operand exception
.mnote sw.ie
invalid operation exception
.endnote
.pc
.ix 'MMX registers'
.ix 'registers' 'MMX'
.ix 'registers' 'mm0 - mm7'
.mnote mm0 - mm7
MMX registers
.begnote $compact
.mnote mm0.b0 - mm0.b7
MMX register component bytes
.mnote mm0.w0 - mm0.w3
MMX register component words
.mnote mm0.d0 - mm0.d1
MMX register component doublewords
.endnote
.pc
.ix 'XMM registers'
.ix 'registers' 'XMM'
.ix 'registers' 'xmm0 - xmm7'
.mnote xmm0 - xmm7
XMM registers (SSE registers)
.begnote $compact
.mnote xmm0.b0 - xmm0.b15
XMM register component bytes
.mnote xmm0.w0 - xmm0.w7
XMM register component words
.mnote xmm0.d0 - xmm0.d3
XMM register component doublewords
.mnote xmm0.q0 - xmm0.q1
XMM register component quadwords
.endnote
.endnote
.np
The debugger permits the manipulation of register contents using any
of the operators described in the following chapter.
By default, these predefined names are accessed just like any other
variables defined by the user or the application.
.ix '_dbg'
Should the situation ever arise where the application defines a
variable whose name conflicts with that of one of these debugger
variables, the module specifier
.id _dbg
may be used to resolve the ambiguity.
.ix '_dbg@'
For example, if the application defines a variable called
.id cs
then
.id _dbg@cs
can be specified to resolve the ambiguity.
The "_dbg@" prefix indicates that we are referring to a debugger
defined symbol rather than an application defined symbol.
.np
The flags register, the 8087 control word, and the 8087 status word
can be accessed as a whole or by its component status bits.
.exam begin
/fl.c=0
/cw.um=0
?sw.oe
.exam end
.pc
In the above example, the "carry" flag is cleared, the 8087 underflow
mask of the control word is cleared, and the 8087 overflow exception
bit of the status word is printed.
.np
The low order bit of the expression result is used to set or
clear the specified flag.
.exam begin
fl.c=0x03a6
.exam end
.pc
In the above example, the "carry" flag is cleared since the
low order bit of the result is 0.
.np
Similarly, the MMX and XMM registers can be accessed as a whole or by
their component bytes, words, doublewords and quadwords (in the case
of the 128-bit XMM registers).
.exam begin
/mm0.b1=1
?mm0.d0
.exam end
.pc
In the above example, the second byte of the first MMX register is
set to 1, then the first doubleword of the same register is printed.
.np
The debugger also defines some other special names.
.begnote
.ix 'predefined symbol' 'dbg$32'
.mnote dbg$32
This debugger symbol represents the mode in which the processor is
running.
.begnote $compact
.note 0
16-bit mode
.note 1
32-bit mode
.endnote
.ix 'predefined symbol' 'dbg$bp'
.mnote dbg$bp
This debugger symbol represents the register pair SS:BP (16-bit mode)
or SS:EBP (32-bit mode).
.exam begin
? dbg$bp
.exam end
.ix 'predefined symbol' 'dbg$code'
.mnote dbg$code
This debugger symbol represents the current code location under
examination.
The dot address "." is either set to dbg$code or dbg$data, depending
on whether you were last looking at code or data.
.ix 'predefined symbol' 'dbg$cpu'
.mnote dbg$cpu
This debugger symbol represents the type of central processing unit
which is in your personal computer system.
.begnote $compact
.note 0
Intel 8088, 8086 or compatible processor
.note 1
Intel 80188, 80186 or compatible processor
.note 2
Intel 80286 or compatible processor
.note 3
Intel 80386 or compatible processor
.note 4
Intel 80486 or compatible processor
.note 5
Intel Pentium processor
.note 6
Intel Pentium Pro/II/III processor
.note 15
Intel Pentium 4 processor
.endnote
.ix 'predefined symbol' 'dbg$ctid'
.mnote dbg$ctid
This debugger symbol represents the identification number of the
current execution thread.
Under environments which do not support threading, the current thread
ID is always 1. The current execution thread can be selected using the
Thread window or the Thread command.
.ix 'predefined symbol' 'dbg$data'
.mnote dbg$data
This debugger symbol represents the current data location under
examination.
The dot address "." is either set to dbg$code or dbg$data, depending
on whether you were last looking at code or data.
.ix 'predefined symbol' 'dbg$etid'
.mnote dbg$etid
This debugger symbol represents the identification number of the
thread that was executing when the debugger was entered.
Under environments which do not support threading, the executing thread
ID is always 1.
.ix 'predefined symbol' 'dbg$fpu'
.mnote dbg$fpu
This debugger symbol represents the type of numeric data processor
(math coprocessor) that is installed in your personal computer system.
.begnote $compact
.note -1
An 80x87 emulator is installed
.note 0
No coprocessor is installed
.note 1
An Intel 8087 is installed
.note 2
An Intel 80287 is installed
.note 3
An Intel 80387 is installed
.note 4
An Intel 80486 processor, supporting coprocessor instructions, is
installed
.note 5
An Intel Pentium processor integrated FPU is installed
.note 6
An Intel Pentium Pro/II/III processor integrated FPU is installed
.note 15
An Intel Pentium 4 processor integrated FPU is installed
.endnote
.ix 'predefined symbol' 'dbg$ip'
.mnote dbg$ip
This debugger symbol represents the register pair CS:IP (16-bit mode)
or CS:EIP (32-bit mode).
.exam begin
? dbg$ip
.exam end
.ix 'predefined symbol' 'dbg$monitor'
.mnote dbg$monitor
This debugger symbol represents the type of monitor adapter
which is in use.
.begnote $compact
.note 0
IBM Monochrome Adapter
.note 1
IBM Colour Graphics Adapter (CGA)
.note 2
IBM Enhanced Graphics Adapter (EGA)
.note 3
IBM Video Graphics Array (VGA)
.endnote

.ix 'predefined symbol' 'dbg$ntid'
.mnote dbg$ntid
This debugger symbol represents the identification number of the
next execution thread. To iterate through all of the threads in a process,
you can execute
.id thread dbg$ntid
repetitively until you are back to the original thread.
Under environments which do not support threading, the next thread
ID is always 1. To show the execution stack for all threads (in the Log window),
you can execute the following commands:
.exam begin
/orig_tid = dbg$ctid
/curr_tid = dbg$ctid
while curr_tid != 0 {
    print {----- Next Thread %x -----} curr_tid;
    show calls;
    /curr_tid = dbg$ntid;
    thread curr_tid;
    if( curr_tid == orig_tid ) {
        /curr_tid = 0;
    };
}
.exam end
.ix 'predefined symbol' 'dbg$os'
.mnote dbg$os
This debugger symbol represents the operating system that is currently
running the application.
.begnote $compact
.note 0
Unknown Operating System
.note 1
DOS
.note 2
OS/2
.note 3
.ix 'DOS extenders' '386|DOS-Extender'
.ix '386|DOS-Extender'
.ix 'Phar Lap Software, Inc.'
386|DOS-Extender from Phar Lap Software, Inc.
.note 5
.ix 'NetWare 386'
.ix 'Novell'
NetWare 386 from Novell, Inc.
.note 6
.ix 'QNX'
.ix 'QNX Software Systems'
QNX from QNX Software Systems.
.note 7
.ix 'DOS extenders' 'DOS/4GW'
.ix 'DOS/4GW'
.ix 'Tenberry Software, Inc.'
.ix 'DOS extenders' 'CauseWay'
.ix 'CauseWay'
DOS/4GW from Tenberry Software, Inc., or CauseWay (both included in the
&watc32 and &watf32 packages)
.note 8
.ix 'Windows 3.x'
.ix 'Microsoft Corp.'
Windows 3.x from Microsoft Corporation
.note 10
.ix 'Windows NT'
.ix 'Windows 95'
.ix 'Microsoft Corp.'
Windows NT/2000/XP or Windows 9x from Microsoft Corporation
.note 12
QNX 6.x 'Neutrino'
.note 13
.ix 'Linux'
.ix 'GNU/Linux'
GNU/Linux
.note 14
FreeBSD
.endnote
.ix 'predefined symbol' 'dbg$pid'
.mnote dbg$pid
(OS/2, NetWare 386, Linux, QNX, Windows NT, Windows 95 only)
This debugger symbol contains the process identification value for the
program being debugged.
.ix 'predefined symbol' 'dbg$psp'
.mnote dbg$psp
(DOS only) This debugger symbol contains the segment value for the DOS
"program segment prefix" of the program being debugged.
.ix 'predefined symbol' 'dbg$radix'
.mnote dbg$radix
This debugger symbol represents the current default numeric
radix.
.ix 'predefined symbol' 'dbg$remote'
.mnote dbg$remote
This debugger symbol is 1 if the "REMotefiles" option was specified
and 0 otherwise.
.ix 'predefined symbol' 'dbg$sp'
.mnote dbg$sp
This debugger symbol represents the register pair SS:SP (16-bit mode)
or SS:ESP (32-bit mode).
.exam begin
? dbg$sp
.exam end
.ix 'predefined symbol' 'dbg$loaded'
.mnote dbg$loaded
This debugger symbol is 1 if a program is loaded. Otherwise, it is 0.
.ix 'predefined symbol' 'dbg$nil'
.mnote dbg$nil
This debugger symbol is the null pointer value.
.ix 'predefined symbol' 'dbg$src'
.mnote dbg$src
This debugger symbol is 1 if you are currently debugging in an area that
contains debugging information.
.endnote
