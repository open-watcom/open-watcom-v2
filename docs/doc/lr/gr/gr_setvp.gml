.gfunc setvisualpage
.gsynop begin 
.if '&lang' eq 'C' .do begin
short _FAR _setvisualpage( short pagenum );
.do end
.el .do begin
integer*2 function _setvisualpage( pagenum )
integer*2 pagenum
.do end
.gsynop end
.desc begin
The
.id &funcb.
&routine selects the page (in memory)
from which graphics output is displayed.
The page to be selected is given by the
.arg pagenum
argument.
.im gr_pages
.desc end
.return begin
The
.id &funcb.
&routine returns the number of the previous page when the
visual page is set successfully; otherwise, a negative number
is returned.
.return end
.see begin
.seelist _getvisualpage _setactivepage _getactivepage _getvideoconfig
.see end
.grexam begin eg_setap.&langsuff
.grexam end
.class &grfun
.system
