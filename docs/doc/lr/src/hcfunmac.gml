.* MACROS USED WITH LIBRARY DESCRIPTIONS (C AND GRAPHICS LIBRARIES)
.*
.* fd... macros are used for function definitions
.* df... macros are used for header file definitions
.* sf... macros are used for structure/union definitions
.*
.dm fdbeg begin
The following functions are defined:
:ZDL compact termhi=1 tsize=14.
.dm fdbeg end
.*
.dm fd begin
.se *fnd=&'vecpos(&*fun.,fnclst)
.if &*fnd. ne 0 .do begin
.* .if &*0 ge 30 .ty **** not enough macro args (&*0) *****
.sr *ent=&*fun
:ZDT.&*ent.
:ZDD.&*1 &*2 &*3 &*4 &*5 &*6 &*7 &*8 &*9 &*10 &*11 &*12 &*13 &*14 &*15 &*16 &*17 &*18 &*19 &*20 &*21 &*22 &*23 &*24 &*25 &*26 &*27 &*28 &*29 &*30
.do end
.dm fd end
.*
.dm fdend begin
:ZeDL.
.dm fdend end
.*
.dm dfbeg begin
:ZDL break termhi=3.
.dm dfbeg end
.*
.dm df begin
.ix '&*'
:cmt. .ix 'data' '&*'
:ZDT.&*
:ZDD.
.dm df end
.*
.dm dfend begin
:ZeDL.
.dm dfend end
.*
.dm sfbeg begin
:ZDL termhi=3.
.dm sfbeg end
.*
.dm sf begin
.ix '&*'
.ix 'structure' '&*'
:ZDT.&*
:ZDD.
.dm sf end
.*
.dm sfend begin
:ZeDL.
.dm sfend end
.*
