.if &'length(&mfunc.) ne 0 .do begin
.np
The &mfunc function is a multibyte character version of &func that
operates with multibyte character strings.
.if &farfnc eq 1 .do begin
.np
The &fmfunc function is a data model independent form of the &mfunc
function that accepts far pointer arguments.
It is most useful in mixed memory model applications.
.do end
.do end
