.gfunc setplotaction
.if '&lang' eq 'C' .do begin
short _FAR _setplotaction( short action );
.do end
.el .do begin
integer*2 function _setplotaction( action )
integer*2 action
.do end
.gfuncend
.desc begin
The &func &routine sets the current plotting action to the value of the
.arg action
argument.
.pp
.im gr_plot
.desc end
.return begin
The previous value of the plotting action is returned.
.return end
.see begin
.seelist &function. _getplotaction
.see end
.grexam begin eg_getpa.&langsuff
.grexam end
.class &grfun
.system
