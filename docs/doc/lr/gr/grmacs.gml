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
.  :GRAPHIC depth='3.6i' file='&*1..ps'.
.do end
.el .do begin
.  :HBMP '&*1..bmp' c
:cmt. The graphical image cannot be reproduced here.  See the printed
:cmt. version of the manual.
.do end
.dm picture end
.*
.dm gfuncinit begin
.  .funcinit
.dm gfuncinit end
.*
.dm gfunc begin
.gfuncinit
.topsect _&*
:set symbol="function" value="_&*".
:set symbol="funcb" value="_&*".
:set symbol="headtxt0$" value="_&*".
:set symbol="headtxt1$" value="_&*".
.ixm '&funcb.'
.se __fnx=1
.se $$fnc(1)=&funcb.
.dm gfunc end
.*
.dm gfuncw begin
.gfuncinit
.topsect _&*, _&*._w
:set symbol="function" value="_&*".
:set symbol="funcb" value="_&*".
:set symbol="func2" value="_&*._w".
:set symbol="headtxt0$" value="_&* &rroutines".
:set symbol="headtxt1$" value="_&* &rroutines".
.ixm '&funcb.'
.ixm '&funcb._w'
.se __fnx=2
.se $$fnc(1)=&funcb.
.se $$fnc(2)=&funcb._w
.dm gfuncw end
.*
.dm gfuncwxy begin
.gfuncinit
.topsect _&*, _&*._w, _&*._wxy
:set symbol="function" value="_&*".
:set symbol="funcb" value="_&*".
:set symbol="func2" value="_&*._w".
:set symbol="func3" value="_&*._wxy".
:set symbol="headtxt0$" value="_&* &rroutines".
:set symbol="headtxt1$" value="_&* &rroutines".
.ixm '&funcb.'
.ixm '&funcb._w'
.ixm '&funcb._wxy'
.se __fnx=3
.se $$fnc(1)=&funcb.
.se $$fnc(2)=&funcb._w
.se $$fnc(3)=&funcb._wxy
.dm gfuncwxy end
.*
.dm gfuncms begin
.gfuncinit
.topsect _&*, _&*.ms
:set symbol="function" value="_&*".
:set symbol="funcb" value="_&*".
:set symbol="func1" value="_&*.ms".
:set symbol="headtxt0$" value="_&* &rroutines".
:set symbol="headtxt1$" value="_&* &rroutines".
.ixm '&funcb.'
.ixm '&funcb.ms'
.se __fnx=2
.se $$fnc(1)=&funcb.
.se $$fnc(2)=&funcb.ms
.dm gfuncms end
.*
.dm gsynop begin
.if '&*' eq 'begin' .do begin
.  .newcode Synopsis:
.  .if '&lang' eq 'C' .do begin
.  .  .if '&'substr(&function.,1,4)' eq '_pg_' .do begin
#include <pgchart.h>
.  .  .do end
.  .  .el .do begin
#include <graph.h>
.  .  .do end
.  .do end
.do end
.el .if '&*' eq 'end' .do begin
.   .endcode
.do end
.dm gsynop end
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
