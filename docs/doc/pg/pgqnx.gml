.* ------- 16-bit and 32-bit QNX
.sr bldsys = 'QNX'
.sr bldos  = 'QNX'
.sr bldnam = 'qnx'
.sr bldswt = ''
.sr bldmsg = 'a QNX 16-bit executable'
.sr bldexe = ''
.sr bldrun = ''
.sr blddbg = '&dbgcmd'
.sr bldhost = 'QNX'
.sr wclcmd = 'cc'
.sr wclcmdup = 'cc'
.im pgbuild
.*
.section Choice of Compile-time Options
.*
.np
In some instances, you want may want to change the behavior of a
particular parser.
You can do so by invoking cc/CC with the "&sw.Wc" option.
For example, to disable stack overflow checking the following would be
used to pass the "&sw.s" option to the C compiler.
.exam begin 1
cc -Wc,-s  ...
.exam end
.np
For detailed information on the C/C++ compilers and the linker, see
the relevant chapters in this guide.
.np
Some options to &cmpname. should not be used to generate QNX
executables.
Others may be used, but with caution.
.np
Do not use the following options since the QNX libraries do not
support the stack calling conventions.
.begnote
.note &sw.3s
386 stack calling conventions
.note &sw.4s
486 stack calling conventions
.note &sw.5s
Pentium stack calling conventions
.endnote
.np
Do not use the
.begnote
.note &sw.sg
.endnote
.pc
option to generate calls to grow the stack since QNX does not support
stack growing.
.np
Use the following option with caution.
.begnote
.note &sw.zu
SS != DGROUP
.endnote
.pc
This option makes all stack references FAR.
Because the QNX library routines are small model, you shouldn't use
"zu" with any "single data segment" model (i.e., 16-bit small &
medium, 32-bit small).
However the "zu" option
.bd is
necessary when compiling interrupt handlers (see
.us qnx_hint_attach
.ct ).
.np
Do not use the following memory model options for 32-bit applications
.begnote
.note &sw.m{m,c,l}
memory models (Medium,Compact,Large)
.endnote
.pc
since 32-bit QNX only supports small and flat models.
.np
Use the following options with caution since they make assumptions
about the data segment and may generate code which is incompatible
with future 32-bit versions of QNX.
.begnote
.note &sw.zdf
DS floats, i.e., it is not fixed to DGROUP
.note &sw.zt<number>
set data threshold
.note &sw.zdl
load DS directly from DGROUP
.endnote
