:set symbol="ipfname"   value="&company OS/2 Help Compiler".
:set symbol="ipfcmd"    value="wipfc".
:set symbol="colon"     value=":"
.*
.if '&target' eq 'QNX' .do begin
:set symbol="ipfpath"   value="$(WATCOM)/wipfc".
.do end
.el .do begin
:set symbol="ipfpath"   value="%watcom%\wipfc".
.do end
.*
.dm keyword begin
:SF font=2.&*:eSF.
.dm keyword end
.*
.dm var begin
:SF font=2.&*.:eSF.
.dm var end
.*
.dm em begin
:SF font=1.&*.:eSF.
.dm em end
.*
.* tag tagname begin/end
.dm tag begin
.if '&*' eq 'begin' .do begin
.   .if &e'&dohelp eq 0 .do begin
:DL break.
.   .do end
.   .el .do begin
:ZDL.
.   .do end
.do end
.el .if '&*' eq 'end' .do begin
.   .endDL
.do end
.dm tag end
.*
.dm tattrbs begin
.if &e'&dohelp eq 0 .do begin
:DT.Attributes:
:DD.~b
.do end
.el .do begin
:ZDT.Attributes
:ZDD.~b
.do end
.dm tattrbs end
.*
.* tattr attribname
.* description
.dm tattr begin
.if &e'&dohelp eq 0 .do begin
:DT.&*
:DD.
.do end
.el .do begin
:ZDT.&*
:ZDD.
.do end
.dm tattr end
.*
.* tclass classes...
.dm tclass begin
.if &e'&dohelp eq 0 .do begin
:DT.Classification:
:DD.&*
.do end
.el .do begin
:ZDT.Classification:
:ZDD.&*
.do end
.dm tclass end
.*
.* tcont children...
.dm tcont begin
.if &e'&dohelp eq 0 .do begin
:DT.May contain:
:DD.&*
.do end
.el .do begin
:ZDT.May contain:
:ZDD.&*
.do end
.dm tcont end
.*
.* tno warning
.* description
.dm tno begin
.if &e'&dohelp eq 0 .do begin
:DT.&*
:DD.
.do end
.el .do begin
:ZDT.&*
:ZDD.
.do end
.dm tno end
.*
.* tdesc
.* description
.dm tdesc begin
.if &e'&dohelp eq 0 .do begin
:DT.Description:
:DD.
.do end
.el .do begin
:ZDT.Description:
:ZDD.
.do end
.dm tdesc end
.*
.* seealso
.* references
.dm tseealso begin
.if &e'&dohelp eq 0 .do begin
:DT.See also:
:DD.
.do end
.el .do begin
:ZDT.See also:
:ZDD.
.do end
.dm tseealso end
.*
.* tclose etag
.dm tclose begin
.if &e'&dohelp eq 0 .do begin
:DT.Closing tag:
:DD.The closing &* is required.
.do end
.el .do begin
:ZDT.Closing tag:
:ZDD.The closing &* is required.
.do end
.dm tclose end
