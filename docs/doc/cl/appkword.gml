.pp
The following topics are discussed:
.begbull
.bull
Standard Keywords
.bull
&wcboth. Keywords
.endbull
.*
.section Standard Keywords
.*
.pp
The following is the list of keywords reserved by the
C language:
..sk 1 c
.im kwlist
.*
.************************************************************************
.*
..if '&target' eq 'PC' or '&target' eq 'PC 370' ..th ..do begin
.*
.section &company. Extended Keywords
.*
.pp
The &company. compilers also reserve the following extended keywords:
.sk 1
.im kwwatcom
.sk 1
.*
The keywords
.kw &kwbased.
.ct ,
.kw &kwsegment.
.ct ,
.kw &kwsegname.
and
.kw &kwself.
are described in the section "&basedptr.".
.*
&wcboth. provide the predefined macro
.mkw _based
for convenience and compatibility with the Microsoft C compiler.
It may be used in place of
.kw &kwbased.
.period
.*
&wcboth. provide the predefined macro
.mkw _segment
for convenience and compatibility with the Microsoft C compiler.
It may be used in place of
.kw &kwsegment.
.period
.*
&wcboth. provide the predefined macro
.mkw _segname
for convenience and compatibility with the Microsoft C compiler.
It may be used in place of
.kw &kwsegname.
.period
.*
&wcboth. provide the predefined macro
.mkw _self
for convenience and compatibility with the Microsoft C compiler.
It may be used in place of
.kw &kwself.
.period
.sk 1
.*
The keywords
.kw &kwfar.
.ct ,
.kw &kwhuge.
and
.kw &kwnear.
are described in the sections "&ptr86." and "&ptr386.".
.*
&wcboth. provide the predefined macros
.mkw far
and
.mkw _far
for convenience and compatibility with the Microsoft C compiler.
They may be used in place of
.kw &kwfar.
.period
.*
&wcboth. provide the predefined macros
.mkw huge
and
.mkw _huge
for convenience and compatibility with the Microsoft C compiler.
They may be used in place of
.kw &kwhuge.
.period
.*
&wcboth. provide the predefined macros
.mkw near
and
.mkw _near
for convenience and compatibility with the Microsoft C compiler.
They may be used in place of
.kw &kwnear.
.period
.pp
The keywords
.kw &kwfar16.
,
.kw &kwifar16.
and
.kw &kwiseg16.
are described in the section "&ptr386.".
.*
&wcboth. provide the predefined macro
.mkw _far16
for convenience and compatibility with the Microsoft C compiler.
It may be used in place of
.kw &kwfar16.
.period
.pp
The
.kw &kwipacked.
keyword is described in the section
"&struct".
.pp
The
.kw &kwcdecl.
and
.kw &kwicdecl.
keywords may be used with function definitions, and indicates that the
calling convention for the function is the same as that used by
Microsoft C.
All parameters are pushed onto the stack, instead of being passed
in registers.
This calling convention may be controlled by a
.kwpp #pragma
directive.
See the &userguide..
&wcboth. provide the predefined macros
.mkw cdecl
and
.mkw _cdecl
for convenience and compatibility with the Microsoft C compiler.
They may be used in place of
.kw &kwcdecl.
.period
.pp
.ix 'Win32 convention'
The
.kw &kwfastcall.
and
.kw &kwifastcal.
keywords may be used with function definitions, and indicates that the
calling convention used is compatible with Microsoft C compiler.
This calling convention may be controlled by a
.kwpp #pragma
directive.
&wcboth. provide the predefined macro
.mkw _fastcall,
for convenience and compatibility with the Microsoft C compiler.
It may be used in place of
.kw &kwfastcall.
See the &userguide..
.pp
The
.kw &kwfortran.
keyword
may be used with function definitions, and indicates that the
calling convention for the function
is suitable for calling a function written in FORTRAN.
By default, this keyword has no effect.
This calling convention may be controlled by a
.kwpp #pragma
directive.
See the &userguide..
&wcboth. provide the predefined macros
.mkw fortran
and
.mkw _fortran
for convenience and compatibility with the Microsoft C compiler.
They may be used in place of
.kw &kwfortran.
.period
.pp
The
.kw &kwpascal.
and
.kw &kwipascal.
keywords
may be used with function definitions, and indicates that the
calling convention for the function is suitable for
calling a function written in Pascal.
All parameters are pushed onto the stack, but in reverse order to the
order specified by
.kw &kwcdecl.
.period
This calling convention may be controlled by a
.kwpp #pragma
directive.
See the &userguide..
&wcboth. provide the predefined macros
.mkw pascal
and
.mkw _pascal
for convenience and compatibility with the Microsoft C compiler.
They may be used in place of
.kw &kwpascal.
.period
.pp
.ix 'OS/2 convention'
The
.kw &kwsyscall.
,
.kw &kwisyscall.
and
.kw &kwisystem.
keywords may be used with function definitions, and indicates that the
calling convention used is compatible with OS/2 (version 2.0 or
higher).
This calling convention may be controlled by a
.kwpp #pragma
directive.
See the &userguide..
&wcboth. provide the predefined macro
.mkw _syscall
for convenience and compatibility with the Microsoft C compiler.
It may be used in place of
.kw &kwsyscall.
.period
.pp
.ix 'Win32 convention'
The
.kw &kwstdcall.
keyword may be used with function definitions, and indicates that the
calling convention used is compatible with Win32.
This calling convention may be controlled by a
.kwpp #pragma
directive.
&wcboth. provide the predefined macro
.mkw _stdcall,
for convenience and compatibility with the Microsoft C compiler.
It may be used in place of
.kw &kwstdcall.
See the &userguide..
.pp
The
.kw &kwwatcall.
keyword may be used with function definitions, and indicates that the
&company default calling convention is used.
This calling convention may be controlled by a
.kwpp #pragma
directive.
See the &userguide..
.pp
.ix 'storage duration' 'static'
.ix 'static storage duration'
The
.kw &kwexport.
and
.kw &kwiexport.
keywords may be used with objects with static storage duration (global
objects) and with functions, and describes that object or function as
being a known object or entry point within a Dynamic Link Library in
OS/2 or Microsoft Windows.
.ix 'linkage' 'external'
.ix 'external linkage'
The object or function must also be declared as having external
linkage (using the
.kw extern
keyword).
.ix 'function' 'call back'
.ix 'call back function'
In addition, any
.ul call back
function whose address is passed to Windows (and which Windows will
"call back") must be defined with the
.kw &kwexport.
keyword, otherwise the call will fail and cause unpredictable results.
The
.kw &kwexport.
keyword may be omitted if the object or function is exported by an
option specified using the linker.
See the &linkref..
&wcboth. provide the predefined macro
.mkw _export
for convenience and compatibility with the Microsoft C compiler.
It may be used in place of
.kw &kwexport.
.period
.pp
The
.kw &kwintrpt.
keyword may be used with function definitions for functions that
handle computer interrupts.
All registers are saved before the function begins execution and
restored prior to returning from the interrupt.
The machine language return instruction for the function is changed to
.mono iret
(interrupt return).
Functions written using
.kw &kwintrpt.
are suitable for attaching to the
.ix 'interrupt'
interrupt vector using the library
function
.libfn _dos_setvect
.period
&wcboth. provide the predefined macros
.mkw interrupt
and
.mkw _interrupt
for convenience and compatibility with the Microsoft C compiler.
They may be used in place of
.kw &kwintrpt.
.period
.pp
The
.kw &kwloadds.
keyword may be used with functions, and causes the compiler to
generate code that will force the DS register to be set to the default
data segment (DGROUP) so that near pointers will refer to that
segment.
This keyword is normally used with functions written for Dynamic Link
Libraries in Windows and OS/2.
&wcboth. provide the predefined macro
.mkw _loadds
for convenience and compatibility with the Microsoft C compiler.
It may be used in place of
.kw &kwloadds.
.period
.pp
The
.kw &kwsaveregs.
keyword may be used with functions.
It is provided for compatibility with Microsoft C, and is used
to save and restore all segment registers in &wcboth..
&wcboth. provide the predefined macro
.mkw _saveregs
for convenience and compatibility with the Microsoft C compiler.
It may be used in place of
.kw &kwsaveregs.
.period
.pp
The
.kw &kwtry.
,
.kw &kwitry.
,
.kw &kwexcept.
,
.kw &kwiexcept.
,
.kw &kwfinally.
,
.kw &kwifinally.
,
.kw &kwleave.
and
.kw &kwileave.
keywords may be used for exception handling, See the
"Structured Exception Handling" in &userguide..
&wcboth. provide the predefined macro
.mkw _try
for convenience and compatibility with the Microsoft C compiler.
It may be used in place of
.kw &kwtry.
.period
&wcboth. provide the predefined macro
.mkw _except
for convenience and compatibility with the Microsoft C compiler.
It may be used in place of
.kw &kwexcept.
.period
&wcboth. provide the predefined macro
.mkw _finally
for convenience and compatibility with the Microsoft C compiler.
It may be used in place of
.kw &kwfinally.
.period
&wcboth. provide the predefined macro
.mkw _leave
for convenience and compatibility with the Microsoft C compiler.
It may be used in place of
.kw &kwleave.
.period
.pp
The
.kw &kwimagunit.
keyword may be used as _Imaginary constant 1.0.
.pp
The
.kw &kwbldiflt.
keyword may be used as function for testing symbol type.
..do end
.*
.************************************************************************
.*
