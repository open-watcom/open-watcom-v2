:cmt. Processing if macros

:include file='process.gml'.

:cmt.   Symbols used in SQL Tutorial

:cmt.   Device lists

:cmt. .dm ifhelp begin
:cmt. :psc proc='help'.
:cmt. .dm ifhelp end

:cmt. .dm ifnhelp begin
:cmt. :psc proc='ps tty asa'.
:cmt. .dm ifnhelp end

:cmt. .dm endif begin
:cmt. :epsc.
:cmt. .dm endif end

:cmt.    Font selection

.dm sf3 begin
.ct :SF font=3.
.dm sf3 end

.dm sf21 begin
.ct :SF font=21.
.dm sf21 end

:set symbol='menu'      value=';.sf2;.ct '.
:set symbol='emenu'     value=';.esf;.ct '.
:set symbol='fn'        value=''.
:set symbol='efn'       value=''.
:set symbol='bold'      value=';.sf2;.ct '.
:set symbol='ebold'     value=';.esf;.ct '.
:set symbol='dtbold'    value=''.
:set symbol='edtbold'   value=''.
:set symbol='mono'      value=';.sf8;.ct '.
:set symbol='emono'     value=';.esf;.ct '.
:set symbol='nt'        value=';.sf8;.ct '.
:set symbol='ent'       value=';.esf;.ct '
:set symbol='keyword'   value=''.
:set symbol='ekeyword'  value=''.

:cmt.   Our manuals

:set symbol='br'        value=';.br'
:set symbol='acmetut'   value=';.sf1;.ct ACME Tutorial;.esf;.ct '
:set symbol='acmeref'   value=';.sf1;.ct ACME Reference;.esf;.ct '
:set symbol='acmedoc'   value=';.sf1;.ct Application Creation Made Easy Tutorial and Reference;.esf;.ct '
:set symbol='langref'   value=";.sf1;.ct WATCOM SQL Language Reference;.esf;.ct "
:set symbol='wsqldoc'   value=";.sf1;.ct WATCOM SQL User's Guide;.esf;.ct "
:set symbol='isqldoc'   value=";.sf1;.ct WATCOM SQL User's Guide;.esf;.ct "
:set symbol='esqldoc'   value=";.sf1;.ct WATCOM SQL Embedded SQL Developer's Reference;.esf;.ct "
:set symbol='readme'    value=";.sf1;.ct WATCOM SQL Read Me First;.esf;.ct "

:cmt.   Names
:set symbol = 'redo'      value = 'transaction log'.
:set symbol = 'hdredo'    value = 'Transaction&b.Log'.
:set symbol = 'undo'      value = 'rollback log'.
:set symbol = 'hdundo'    value = 'Rollback&b.Log'.
:set symbol = 'pages'     value = 'checkpoint log'.
:set symbol = 'hdpages'   value = 'Checkpoint&b.Log'.
:cmt.:set symbol = 'dbname'    value = ';.sf1;.ct WATCOM ;.esf;.ct SQL'.
:set symbol = 'dbname'    value = 'WATCOM SQL'.
:set symbol = 'hddbname'  value = 'WATCOM&b.SQL'.
:set symbol = 'dbstart'   value = 'DBSTART'
:set symbol = 'dbstartr'  value = 'RTSTART'
:set symbol = 'db386'     value = 'DB386'
:set symbol = 'db386r'    value = 'RT386'
:set symbol = 'acmer'     value = 'RTACME'
:set symbol = 'engine'    value = 'WATCOM SQL database engine'.
:set symbol = 'dbdir'     value=';.sf8;.ct c:\\wsql;.esf;.ct '
:set symbol = 'watcomsys' value='WATCOM Systems Inc.'.
:set symbol = 'sellist'   value='select list'

:psc proc='novell'.
:set symbol = 'dbname'    value = 'XQL'.
:set symbol = 'hddbname'  value = 'XQL'.
:set symbol = 'engine'    value = 'XQL database engine'.
:set symbol = 'novcc'     value = 'Novell/WATCOM C compiler'.
:set symbol = 'novcpp'    value = 'Novell/WATCOM C preprocessor'.
:set symbol = 'nov386'    value = 'Novell/WATCOM 386 C compiler'.
:set symbol = 'novlink'   value = 'Novell/WATCOM Linker'.
:set symbol = 'xqlpdoc'   value = ';.sf1;.ct XQL Relational Functions Interface;.esf;.ct '.
:set symbol = 'xqlmdoc'   value = ';.sf1;.ct XQL Operations and SQL Interface;.esf;.ct '.
:set symbol = 'btrvdoc'   value = ';.sf1;.ct Btrieve Reference Manual;.esf;.ct '.
:set symbol = 'netsql'    value = ";.sf1;.ct NetWare SQL User's Manual;.esf;.ct ".
:epsc.


.dm semi begin
.'ct ;
.dm semi end

:set symbol='semi'      value=';.semi;.ct '
:set symbol='cont'      value=';.ct '

:cmt.    Menu selection

:set symbol='choose'   value='choose'
:set symbol='uchoose'  value='Choose'

:cmt. Things that might change
:set symbol=mvleft value=';.sf2;.ct F5;.esf;.ct '.
:set symbol=mvright value=';.sf2;.ct F6;.esf;.ct '.
:set symbol=mvltch value=';.sf2;.ct Shift+F5;.esf;.ct '.
:set symbol=mvrtch value=';.sf2;.ct Shift+F6;.esf;.ct '.
:set symbol=doit value=';.sf2;.ct F9;.esf;.ct '.

:set symbol=bup value=';.sf21;.ct s;.esf;.ct '.
:set symbol=bdown value=';.sf21;.ct t;.esf;.ct '.

:cmt. check and cross tags are used in locking chapter to describe
:cmt. the behaviour of various isolation levels
.dm check begin
:SF font=21.4:ESF.&*.
.dm check end
.gt check add check continue

.dm cross begin
:SF font=21.8:ESF.&*.
.dm cross end
.gt cross add cross continue

:cmt. sqlref is used for any reference to the language reference
:cmt. book so that we can change to hdref if the books are ever
:cmt. combined again
.if &lang or &develop .do begin
.dm sqlref begin
:hdref refid='&*refname.'.
.dm sqlref end
.gt sqlref add sqlref attribute continue
.ga * refname any
.do end
.el .do begin
.dm sqlref begin
&*refname. in the &langref.
.dm sqlref end
.gt sqlref add sqlref attribute continue
.ga * refname uppercase any
.do end




:cmt.:set symbol='pg'   value='&gml.h6.      '
:cmt.:set symbol='pg'   value=';.pa;'
:set symbol='pg'   value=''

.dm beghint begin
:h6.SUGGESTION:
:pc.
.dm beghint end

.dm endhint begin
.dm endhint end


