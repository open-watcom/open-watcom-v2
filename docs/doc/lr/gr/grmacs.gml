.* MACROS USED WITH DESCRIPTIONS OF GRAPHIC FUNCTIONS
.*
.sr grfun = "PC Graphics"
.sr grindex = "PC Graphics Functions"
.sr graphics = 1
.*
.dm coord begin
.  .mono (&*1,&*2)&*3
.dm coord end
.*
.dm picture begin
.sk
.if '&XMPset' eq 'on' .xmpoff
.if &e'&dohelp eq 0 .do begin
:GRAPHIC depth='3.6i' file='&*1..ps'.
.do end
.el .do begin
:HBMP '&*1..bmp' c
:cmt. The graphical image cannot be reproduced here.  See the printed
:cmt. version of the manual.
.do end
.dm picture end
.*
.dm gfunc begin
.topsect _&*
:set symbol="function" value="_&*".
:set symbol="func" value="_&*".
:set symbol="headtxt0$" value="_&*".
:set symbol="headtxt1$" value="_&*".
.ixm '&func.'
.se __fnx=1
.se $$fnc(1)=&func.
.newcode Synopsis:
.if '&lang' eq 'C' .do begin
.if '&'substr(&*,1,2)' eq 'pg' .do begin
#include <pgchart.h>
.do end
.el .do begin
#include <graph.h>
.do end
.do end
.dm gfunc end
.*
.dm gfuncw begin
.topsect _&*, _&*._w
:set symbol="function" value="_&*".
:set symbol="func" value="_&*".
:set symbol="func2" value="_&*._w".
:set symbol="headtxt0$" value="_&* &rroutines".
:set symbol="headtxt1$" value="_&* &rroutines".
.ixm '&func.'
.ixm '&func._w'
.se __fnx=2
.se $$fnc(1)=&func.
.se $$fnc(2)=&func._w
.newcode Synopsis:
.if '&lang' eq 'C' .do begin
#include <graph.h>
.do end
.dm gfuncw end
.*
.dm gfuncwxy begin
.topsect _&*, _&*._w, _&*._wxy
:set symbol="function" value="_&*".
:set symbol="func" value="_&*".
:set symbol="func2" value="_&*._w".
:set symbol="func3" value="_&*._wxy".
:set symbol="headtxt0$" value="_&* &rroutines".
:set symbol="headtxt1$" value="_&* &rroutines".
.ixm '&func.'
.ixm '&func._w'
.ixm '&func._wxy'
.se __fnx=3
.se $$fnc(1)=&func.
.se $$fnc(2)=&func._w
.se $$fnc(3)=&func._wxy
.newcode Synopsis:
.if '&lang' eq 'C' .do begin
#include <graph.h>
.do end
.dm gfuncwxy end
.*
.dm gfuncms begin
.topsect _&*, _&*.ms
:set symbol="function" value="_&*".
:set symbol="func" value="_&*".
:set symbol="func2" value="_&*.ms".
:set symbol="headtxt0$" value="_&* &rroutines".
:set symbol="headtxt1$" value="_&* &rroutines".
.ixm '&func.'
.ixm '&func.ms'
.se __fnx=2
.se $$fnc(1)=&func.
.se $$fnc(2)=&func.ms
.newcode Synopsis:
.if '&lang' eq 'C' .do begin
#include <pgchart.h>
.do end
.dm gfuncms end
.*
.dm gfuncend begin
.endcode
:set symbol="func" value=";.sf4 &function;.esf ".
.if '&'substr(&function,1,3)' eq '_pg' .do begin
:set symbol="func2" value=";.sf4 &function.ms;.esf ".
.do end
.el .do begin
:set symbol="func2" value=";.sf4 &function._w;.esf ".
:set symbol="func3" value=";.sf4 &function._wxy;.esf ".
.do end
.dm gfuncend end
.*
.dm hex begin
.if '&lang' eq 'C' .do begin
.mono 0x&*1&*2
.do end
.el .do begin
.mono '&*1'x&*2
.do end
.dm hex end
.*
.dm grexam begin
.if '&*1' eq 'begin' .do begin
. .im gr_sizes &*2
. .newcode Example:
. .im &*2
.do end
.el .if '&*' eq 'end' .endcode
.el .if '&*' eq 'break' .brkcode
.el .if '&*' eq 'output' .outcode
.dm grexam end
.*
.dm gfuncref begin
.ix '&*1'
(see the
.kw _&*1
&routine)&*2
.dm gfuncref end
.*
.dm gfuncsref begin
.ix '&*1'
(see the
.kw _&*1
&routines)&*2
.dm gfuncsref end
.*
.dm uterm begin
.note _&*
.ix '&*'
.dm uterm end
.*
.dm uindex begin
uindex=2
.ix '&*'
.dm uindex end
.*
