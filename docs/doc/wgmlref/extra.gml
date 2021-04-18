:SET symbol='ibmpc' value='IBM PC/DOS'
:SET symbol='ibmvm' value='IBM VM/CMS'
:SET symbol='decvax' value='DEC VAX/VMS'
:SET symbol='isbn' value=''.
:SET symbol='wgml' value='WATCOM Script/GML'

:INCLUDE file='wbsymbol'.

.dm hp2 begin
:HP2.
.dm hp2 end

.dm ehp2 begin
:eHP2.
.dm ehp2 end

.dm choose begin
.in +3
.dm choose end

.dm echoose begin
.sk 2
.in -3
.dm echoose end

.dm mouse begin
.sk 2
.cp 6
.in -3
:HP2.Mouse::eHP2.
.sk
.in +3
.dm mouse end

.dm keyboard begin
.sk 2
.cp 6
.in -3
:HP2.Keyboard::eHP2.
.sk
.in +3
.dm keyboard end

.dm screen begin
:FIG frame=none.
.if &e'&dohelp ne 0 .do begin
:HBMP '&*1..bmp' i
.do end
.el .do begin
:GRAPHIC file='&*1..ps' depth='2.5i' scale=45.
.do end
:FIGCAP.&*2.
:eFIG.
.dm screen end


:SET symbol='hp2' value=';.hp2;.ct '
:SET symbol='ehp2' value=';.ehp2;.ct '
:SET symbol='menu' value=';.sf1;.ct '
:SET symbol='emenu' value=';.esf;.ct '
:SET symbol='mono' value=';.sf8;.ct '
:SET symbol='emono' value=';.esf;.ct '
