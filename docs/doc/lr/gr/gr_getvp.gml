.gfunc getvisualpage
.if '&lang' eq 'C' .do begin
short _FAR _getvisualpage( void );
.do end
.el .do begin
integer*2 function _getvisualpage()
.do end
.gfuncend
.desc begin
The &func &routine returns the number of the currently selected
visual graphics page.
.im gr_pages
.desc end
.return begin
The &func &routine returns the number of the currently selected
visual graphics page.
.return end
.see begin
.seelist &function. _setvisualpage _setactivepage _getactivepage _getvideoconfig
.see end
.grexam begin eg_setap.&langsuff
.grexam end
.class &grfun
.system
