.if &farfnc ne 0 .do begin
.np
The
.sr __f=&ffunc
.if '&__f.' eq '' .sr __f=&fmfunc
&__f.
function is a data model independent form of the &functiong.
function that accepts far pointer arguments.
It is most useful in mixed memory model applications.
.do end
