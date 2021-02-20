..if '&target' eq 'PC' or '&target' eq 'PC 370' ..th ..do begin
.pp
The &wcboth. compilers also provide the following predefined macros
for describing the memory model being used:
.pp
.mkw &mkwCOMPACT.
.discuss begin
The compact memory model is being used.
.ix 'memory model' 'compact'
.ix 'compact memory model'
.discuss end
.*
.pp
.mkw &mkwFLAT.
.discuss begin
The "flat" memory model is being used for the 80386 processor.
All segment registers refer to the same segment.
.discuss end
.*
.pp
.mkw &mkwFUNC.
.discuss begin
The name of the current function (a string literal).
.discuss end
.*
.pp
.mkw &mkwHUGE.
.discuss begin
The huge memory model is being used.
.ix 'memory model' 'huge'
.ix 'huge memory model'
.discuss end
.*
.pp
.mkw &mkwLARGE.
.discuss begin
The large memory model is being used.
.ix 'memory model' 'large'
.ix 'large memory model'
.discuss end
.*
.pp
.mkw &mkwMEDIUM.
.discuss begin
The medium memory model is being used.
.ix 'memory model' 'medium'
.ix 'medium memory model'
.discuss end
.*
.pp
.mkw &mkwSMALL.
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
.mkw &mkwDOS.
.discuss begin
The program is being compiled for use on a DOS operating system.
.discuss end
.keep end
.*
.pp
.mkw &mkwNET.
.discuss begin
The program is being compiled for use on the Novell Netware 386
operating system.
.discuss end
.*
.pp
.mkw &mkwNT.
.discuss begin
The program is being compiled for use on the Windows NT operating
system.
.discuss end
.*
.pp
.mkw &mkwOS2.
.discuss begin
The program is being compiled for use on the OS/2 operating system.
.discuss end
.*
.pp
.mkw &mkwQNX.
.discuss begin
The program is being compiled for use on the QNX operating system.
.discuss end
.*
.pp
.mkw &mkwWINDOWS.
.discuss begin
The program is being compiled for use with Microsoft Windows.
.discuss end
.*
.pp
.mkw &mkwWIN_386.
.discuss begin
The program is being compiled for use with Microsoft Windows,
using the &company. 32-bit Windows interface.
.discuss end
.*
.pp
The &wc286. compiler also provides the following miscellaneous macro:
.*
.pp
.mkw &mkwCHPW.
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
.* .mkw &mkwEXPRESS.
.* .discuss begin
.* The compiler being used is the &loadgo. compiler.
.* .discuss end
.*
.pp
.mkw &mkwCSGN.
.discuss begin
The program is being compiled using the "j" compiler option.
The default
.mono char
type is treated as a signed quantity.
.discuss end
.keep end
.*
.pp
.mkw &mkwFPI.
.discuss begin
The program is being compiled using in-line floating point instructions.
.discuss end
.*
.pp
.mkw &mkwINLN.
.discuss begin
The program is being compiled using the "oi" compiler option.
.discuss end
.*
.pp
.keep begin
.mkw &mkwWATCOMC.
.discuss begin
The compiler being used is the &wc286. or &wc386. compiler.
The value of the macro is the version number of the compiler times 100.
.discuss end
.keep end
.*
.pp
.mkw &mkw386.
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
.mkw &mkwMSDOS.
.discuss begin
The program is being compiled for use on a DOS operating system.
.discuss end
.*
.pp
.mkw &mkwM_IX86.
.discuss begin
The program is being compiled for a specific target architecture.
The macro is identically equal to 100 times the architecture compiler
option value (-0, -1, -2, -3, -4, -5, etc.).
If "-5" (Pentium instruction timings) was specified as a compiler
option, then the value of
.mkw &mkwM_IX86.
would be 500.
.discuss end
.*
.pp
.mkw &mkwM_I86.
.discuss begin
The program is being compiled for use on the Intel 80x86 processor.
.discuss end
.*
.pp
.mkw &mkwM_I386.
.discuss begin
The program is being compiled for use on the Intel 80386 processor.
.discuss end
.*
.pp
.mkw &mkwM_I86CM.
.discuss begin
The compact memory model is being used.
.ix 'memory model' 'compact'
.ix 'compact memory model'
.discuss end
.*
.pp
.mkw &mkwM_I86HM.
.discuss begin
The huge memory model is being used.
.ix 'memory model' 'huge'
.ix 'huge memory model'
.discuss end
.*
.pp
.mkw &mkwM_I86LM.
.discuss begin
The large memory model is being used.
.ix 'memory model' 'large'
.ix 'large memory model'
.discuss end
.*
.pp
.mkw &mkwM_I86MM.
.discuss begin
The medium memory model is being used.
.ix 'memory model' 'medium'
.ix 'medium memory model'
.discuss end
.*
.pp
.mkw &mkwM_I86SM.
.discuss begin
The small memory model is being used.
.ix 'memory model' 'small'
.ix 'small memory model'
.discuss end
.*
.pp
.keep begin
.mkw &mkwNKEY.
.discuss begin
The program is being compiled for ISO/ANSI conformance using the "za"
(no extended keywords) compiler option.
.discuss end
.keep end
.*
..do end
