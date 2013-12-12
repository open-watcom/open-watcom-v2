.gfunc setactivepage
.gsynop begin 
.if '&lang' eq 'C' .do begin
short _FAR _setactivepage( short pagenum );
.do end
.el .do begin
integer*2 function _setactivepage( pagenum )
integer*2 pagenum
.do end
.gsynop end
.desc begin
The
.id &func.
&routine selects the page (in memory) to which graphics output
is written.
The page to be selected is given by the
.arg pagenum
argument.
.im gr_pages
.desc end
.return begin
The
.id &func.
&routine returns the number of the previous page when the
active page is set successfully; otherwise, a negative number
is returned.
.return end
.see begin
.seelist &function. _getactivepage _setvisualpage _getvisualpage _getvideoconfig
.see end
.grexam begin eg_setap.&langsuff
.grexam end
.class &grfun
.system
