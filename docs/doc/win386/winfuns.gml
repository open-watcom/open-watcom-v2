:set symbol="ohtxt$" value="&headtxt0$".
.*
.sr machsys='DOS'
.*
.im libmacs
.im cmanmac
.*
.if '&lang' eq 'FORTRAN 77' .do begin
.chap WIN386 Library Subprograms
:set symbol="fn_or_mac" value="subprogram".
:set symbol="fn_or_macs" value="subprograms".
:set symbol="function" value="subprogram".
:set symbol="header" value="include".
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.chap WIN386 Library Functions and Macros
:set symbol="fn_or_mac" value="function or macro".
:set symbol="fn_or_macs" value="functions or macros".
:set symbol="function" value="function".
:set symbol="header" value="header".
.do end
.*
.np
.ix 'WIN386 library routines'
Each special Windows &fn_or_mac in the &cmpname library is described
in this chapter.
Each description consists of a number of subsections:
.in 0
.begpoint $setptnt &INDlvl
.point Synopsis:
This subsection gives the &header files that should be included within
a source file that references the &fn_or_mac..
It also shows an appropriate declaration for the function or for a
function that could be substituted for a macro.
This declaration is not included in your program; only the &header
file(s) should be included.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.np
When a pointer argument is passed to a function and that function does
not modify the item indicated by that pointer, the argument is shown
with
.kw const
before the argument.
For example,
.millust begin
const char *string
.millust end
.pc
indicates that the array pointed at by
.arg string
is not changed.
.do end
.point Description:
This subsection is a description of the &fn_or_mac..
.point Returns:
This subsection describes the return value (if any)
for the &fn_or_mac..
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.point Errors:
This subsection describes the possible
.kw errno
values.
.do end
.point See Also:
This optional subsection provides a list of related &fn_or_macs..
.point Example:
This optional subsection consists of one or more examples of
the use of the &function..
The examples are often just fragments of code (not complete programs)
for illustration purposes.
.point Classification:
This subsection provides an indication of where the &fn_or_mac
is commonly found.
The &fn_or_macs in this section are all classified as "WIN386"
(i.e., they pertain to 32-bit Windows programming).
.endpoint
.in &INDlvl
.*
.fnc AllocAlias16                   aalias.gml   DOS
.fnc AllocHugeAlias16               ahalias.gml  DOS
.fnc _Call16                        call16.gml   DOS
.fnc DefineDLLEntry                 ddllent.gml  DOS
.fnc DefineUserProc16               defp16.gml   DOS
.fnc FreeAlias16                    falias.gml   DOS
.fnc FreeHugeAlias16                fhalias.gml  DOS
.fnc FreeIndirectFunctionHandle     freeindr.gml DOS
.* .fnc GetCallbackRoutine
.fnc GetIndirectFunctionHandle      getindr.gml  DOS
.fnc GetProc16                      getp16.gml   DOS
.* .fnc Invoke16BitFunction
.fnc InvokeIndirectFunction         invindr.gml  DOS
.fnc MapAliasToFlat                 mapa2f.gml   DOS
.fnc MK_FP16                        mkfp16.gml   DOS
.fnc MK_FP32                        mkfp32.gml   DOS
.fnc MK_LOCAL32                     mklcl32.gml  DOS
.fnc PASS_WORD_AS_POINTER           pwap.gml     DOS
.* .fnc ReleaseCallbackRoutine
.fnc ReleaseProc16                  relp16.gml   DOS
.* .fnc SetProc
.* .fnc TryAlias
.* .fnc vGetCallbackRoutine
.*
.sys AllocAlias16 WIN386
.sys AllocHugeAlias16 WIN386
.sys _Call16 WIN386
.sys DefineDLLEntry WIN386
.sys DefineUserProc16 WIN386
.sys FreeAlias16 WIN386
.sys FreeHugeAlias16 WIN386
.sys FreeIndirectFunctionHandle WIN386
.* .sys GetCallbackRoutine WIN386
.sys GetIndirectFunctionHandle WIN386
.sys GetProc16 WIN386
.* .sys Invoke16BitFunction WIN386
.sys InvokeIndirectFunction WIN386
.sys MapAliasToFlat WIN386
.sys MK_FP16 WIN386
.sys MK_FP32 WIN386
.sys MK_LOCAL32 WIN386
.sys PASS_WORD_AS_POINTER WIN386
.* .sys ReleaseCallbackRoutine WIN386
.sys ReleaseProc16 WIN386
.* .sys SetProc WIN386
.* .sys TryAlias WIN386
.* .sys vGetCallbackRoutine WIN386
.*
:INCLUDE file='AALIAS'.
:INCLUDE file='AHALIAS'.
:INCLUDE file='CALL16'.
:INCLUDE file='DDLLENT'.
:INCLUDE file='DEFP16'.
:INCLUDE file='FALIAS'.
:INCLUDE file='FHALIAS'.
:INCLUDE file='FREEINDR'.
:INCLUDE file='GETINDR'.
:INCLUDE file='GETP16'.
:INCLUDE file='INVINDR'.
:INCLUDE file='MAPA2F'
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
:INCLUDE file='MKFP16'.
:INCLUDE file='MKFP32'.
:INCLUDE file='MKLCL32'.
.do end
:INCLUDE file='PWAP'.
:INCLUDE file='RELP16'.
.*
.pa
:set symbol="headtxt0$" value="&ohtxt$".
