.gfunc xxxx
.if '&lang' eq 'C' .do begin
long _FAR _xxxx
.do end
.el .do begin
integer*4 function _xxxx
.do end
.gfuncend
.desc begin
The &func &routine
.desc end
.return begin
The &func &routine returns
.return end
.see begin
.seelist &function. _
.see end
.grexam begin eg_temp.&langsuff
.grexam output
.picture eg_temp
.grexam end
.class &grfun
.system
