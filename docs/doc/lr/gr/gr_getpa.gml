.gfunc getplotaction
.gsynop begin 
.if '&lang' eq 'C' .do begin
short _FAR _getplotaction( void );
.do end
.el .do begin
integer*2 function _getplotaction()
.do end
.gsynop end
.desc begin
The
.id &func.
&routine returns the current plotting action.
.pp
.im gr_plot
.desc end
.return begin
The
.id &func.
&routine returns the current plotting action.
.return end
.see begin
.seelist &function. _setplotaction
.see end
.grexam begin eg_getpa.&langsuff
.grexam end
.class &grfun
.system
