.dm begflist begin
.cp 3
:DL termhi=2 tsize=10 break compact.
:set symbol="NTEpr2" value="&NTEpr1".
:set symbol="NTEpr1" value="&NTEset".
:set symbol="NTEset" value="dl".
.dm begflist end
.*
.dm disklist begin
.if '&*' ne '' .do begin
.cp 5
:P.:HP1.&*:eHP1.
.do end
:OL compact.
:set symbol="NTEpr2" value="&NTEpr1".
:set symbol="NTEpr1" value="&NTEset".
:set symbol="NTEset" value="ol".
.dm disklist end
.*
.dm kw begin
.ix &*
.ct
:SF font=3.&*:eSF.
.dm kw end
