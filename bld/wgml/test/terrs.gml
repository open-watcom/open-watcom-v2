:GDOC.
:BODY.

:P.The output file contents should be ignored or used very carefully, since it
is all (except this sentence) the result of our wgml not halting at the first 
error found. 

.ty gscofoju.c
.co of
.co sam
.co fred
.ju of
.ju sam
.ju fred
.ju sally
.ju robert
.ju richard
.fo of
.fo sam
.fo fred
.fo sally
.fo robert
.fo richard

.ty gspe.c
.pe of
.pe fred
.pe robert

.ty gsdccw.c
.cw fred
.dc fred
.dc cw sam
.dc cw fred
.dc gml sam
.dc gml fred
.dc tb sam
.dc tb fred

.ty gstitr.c
.ti sam
.ti fred
.ti set fred
.ti mm
.ti k mm
.tr sam
.tr mm
.tr k mm

.ty guseropt.c
.dm hbmp begin
&rsbmp.&*2.&rsbmp.&*1.&rsbmp
.dm hbmp end
.dm zhbmp begin
.hbmp &*
.dm zhbmp end
.gt hbmp add zhbmp cont texte
.gt hbmp1 add zhbmp cont textr

:hbmp.
.br
:hbmp;
.br;
:HBMP testbx.gml c
.br
:hbmp testbx.gml c; more text

:hbmp1.
.br
:hbmp1;
.br;
:HBMP1 testbx.gml c
.br
:hbmp1 testbx.gml c; more text

:eGDOC.
