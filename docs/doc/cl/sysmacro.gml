..if '&target' eq 'PC' or '&target' eq 'PC 370' ..th ..do begin
.pp
The &wcboth. compilers also provide the following predefined macros
for describing the memory model being used:
.pp
.mkwix &mkwCOMP_sp.
.mkwfont &mkwCOMPACT.
.discuss begin
The compact memory model is being used.
.ix 'memory model' 'compact'
.ix 'compact memory model'
.discuss end
.*
.pp
.mkwix &mkwFLAT_sp.
.mkwfont &mkwFLAT.
.discuss begin
The "flat" memory model is being used for the 80386 processor.
All segment registers refer to the same segment.
.discuss end
.*
.pp
.mkwix &mkwFUNC_sp.
.mkwfont &mkwFUNC.
.discuss begin
The name of the current function (a string literal).
.discuss end
.*
.pp
.mkwix &mkwHUGE_sp.
.mkwfont &mkwHUGE.
.discuss begin
The huge memory model is being used.
.ix 'memory model' 'huge'
.ix 'huge memory model'
.discuss end
.*
.pp
.mkwix &mkwLARG_sp.
.mkwfont &mkwLARGE.
.discuss begin
The large memory model is being used.
.ix 'memory model' 'large'
.ix 'large memory model'
.discuss end
.*
.pp
.mkwix &mkwMED_sp.
.mkwfont &mkwMEDIUM.
.discuss begin
The medium memory model is being used.
.ix 'memory model' 'medium'
.ix 'medium memory model'
.discuss end
.*
.pp
.mkwix &mkwSMAL_sp.
.mkwfont &mkwSMALL.
.discuss begin
The small memory model is being used.
.ix 'memory model' 'small'
.ix 'small memory model'
.discuss end
.*
.keep begin
.pp
The &wcboth. compilers also provide the following macros for
describing the target operating system:
.pp
.mkwix &mkwDOS_sp.
.mkwfont &mkwDOS.
.discuss begin
The program is being compiled for use on a DOS operating system.
.discuss end
.keep end
.*
.pp
.mkwix &mkwNET_sp.
.mkwfont &mkwNET.
.discuss begin
The program is being compiled for use on the Novell Netware 386
operating system.
.discuss end
.*
.pp
.mkwix &mkwNT_sp.
.mkwfont &mkwNT.
.discuss begin
The program is being compiled for use on the Windows NT operating
system.
.discuss end
.*
.pp
.mkwix &mkwOS2_sp.
.mkwfont &mkwOS2.
.discuss begin
The program is being compiled for use on the OS/2 operating system.
.discuss end
.*
.pp
.mkwix &mkwQNX_sp.
.mkwfont &mkwQNX.
.discuss begin
The program is being compiled for use on the QNX operating system.
.discuss end
.*
.pp
.mkwix &mkwWIND_sp.
.mkwfont &mkwWINDOWS.
.discuss begin
The program is being compiled for use with Microsoft Windows.
.discuss end
.*
.pp
.mkwix &mkwW386_sp.
.mkwfont &mkwWIN_386.
.discuss begin
The program is being compiled for use with Microsoft Windows,
using the &company. 32-bit Windows interface.
.discuss end
.*
.pp
The &wc286. compiler also provides the following miscellaneous macro:
.*
.pp
.mkwix &mkwCHPW_sp.
.mkwfont &mkwCHPW.
.discuss begin
The program is being compiled for use with Microsoft Windows using the
"zW" compiler option.
.discuss end
.*
.pp
.keep begin
The &wcboth. compilers also provide the following miscellaneous
macros:
.* .pp
.* .mkwix &mkwEXPR_sp.
.* .mkwfont &mkwEXPRESS.
.* .discuss begin
.* The compiler being used is the &loadgo. compiler.
.* .discuss end
.*
.pp
.mkwix &mkwCSGN_sp.
.mkwfont &mkwCSGN.
.discuss begin
The program is being compiled using the "j" compiler option.
The default
.mono char
type is treated as a signed quantity.
.discuss end
.keep end
.*
.pp
.mkwix &mkwFPI_sp.
.mkwfont &mkwFPI.
.discuss begin
The program is being compiled using in-line floating point instructions.
.discuss end
.*
.pp
.mkwix &mkwINLN_sp.
.mkwfont &mkwINLN.
.discuss begin
The program is being compiled using the "oi" compiler option.
.discuss end
.*
.pp
.keep begin
.mkwix &mkwWATC_sp.
.mkwfont &mkwWATCOMC.
.discuss begin
The compiler being used is the &wc286. or &wc386. compiler.
The value of the macro is the version number of the compiler times 100.
.discuss end
.keep end
.*
.pp
.mkwix &mkw386_sp.
.mkwfont &mkw386.
.discuss begin
The program is being compiled for the 80386 processor,
using the &wc386. compiler.
.discuss end
.*
.pp
The &wcboth. compilers also provide the following predefined macros
for compatibility with the Microsoft C compiler,
even though most of these macros do not begin with an underscore (_)
character:
.*
.pp
.mkwix &mkwMDOS_sp.
.mkwfont &mkwMSDOS.
.discuss begin
The program is being compiled for use on a DOS operating system.
.discuss end
.*
.pp
.mkwix &mkwMX86_sp.
.mkwfont &mkwM_IX86.
.discuss begin
The program is being compiled for a specific target architecture.
The macro is identically equal to 100 times the architecture compiler
option value (-0, -1, -2, -3, -4, -5, etc.).
If "-5" (Pentium instruction timings) was specified as a compiler
option, then the value of
.mkwfont &mkwM_IX86.
would be 500.
.discuss end
.*
.pp
.mkwix &mkwM86_sp.
.mkwfont &mkwM_I86.
.discuss begin
The program is being compiled for use on the Intel 80x86 processor.
.discuss end
.*
.pp
.mkwix &mkwM386_sp.
.mkwfont &mkwM_I386.
.discuss begin
The program is being compiled for use on the Intel 80386 processor.
.discuss end
.*
.pp
.mkwix &mkwM86C_sp.
.mkwfont &mkwM_I86CM.
.discuss begin
The compact memory model is being used.
.ix 'memory model' 'compact'
.ix 'compact memory model'
.discuss end
.*
.pp
.mkwix &mkwM86H_sp.
.mkwfont &mkwM_I86HM.
.discuss begin
The huge memory model is being used.
.ix 'memory model' 'huge'
.ix 'huge memory model'
.discuss end
.*
.pp
.mkwix &mkwM86L_sp.
.mkwfont &mkwM_I86LM.
.discuss begin
The large memory model is being used.
.ix 'memory model' 'large'
.ix 'large memory model'
.discuss end
.*
.pp
.mkwix &mkwM86M_sp.
.mkwfont &mkwM_I86MM.
.discuss begin
The medium memory model is being used.
.ix 'memory model' 'medium'
.ix 'medium memory model'
.discuss end
.*
.pp
.mkwix &mkwM86S_sp.
.mkwfont &mkwM_I86SM.
.discuss begin
The small memory model is being used.
.ix 'memory model' 'small'
.ix 'small memory model'
.discuss end
.*
.pp
.keep begin
.mkwix &mkwNKEY_sp.
.mkwfont &mkwNKEY.
.discuss begin
The program is being compiled for ISO/ANSI conformance using the "za"
(no extended keywords) compiler option.
.discuss end
.keep end
.*
..do end
