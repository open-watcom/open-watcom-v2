.gfunc getcolor
.gsynop begin 
.if '&lang' eq 'C' .do begin
short _FAR _getcolor( void );
.do end
.el .do begin
integer*2 function _getcolor()
.do end
.gsynop end
.desc begin
The
.id &func.
&routine returns the pixel value for the current color.
This is the color used for displaying graphics output.
The default color value is one less than the maximum number of colors
in the current video mode.
.* .pp
.* .im gr_pixvl
.desc end
.return begin
The
.id &func.
&routine returns the pixel value for the current color.
.return end
.see begin
.seelist &function. _setcolor
.see end
.grexam begin eg_getc.&langsuff
.grexam end
.class &grfun
.system
