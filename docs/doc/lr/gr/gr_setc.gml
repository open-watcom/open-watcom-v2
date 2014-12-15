.gfunc setcolor
.gsynop begin 
.if '&lang' eq 'C' .do begin
short _FAR _setcolor( short pixval );
.do end
.el .do begin
integer*2 function _setcolor( pixval )
integer*2 pixval
.do end
.gsynop end
.desc begin
The
.id &funcb.
&routine sets the pixel value for the
current color to be that indicated by the
.arg pixval
argument.
The current color is only used by the &routines that produce graphics
output; text output with
.kw _outtext
uses the current text color (see the
.kw _settextcolor
&routine).
The default color value is one less than the maximum number of colors
in the current video mode.
.* .pp
.* .im gr_pixvl
.desc end
.return begin
The
.id &funcb.
&routine returns the previous value of the
current color.
.return end
.see begin
.seelist _getcolor _settextcolor
.see end
.grexam begin eg_getc.&langsuff
.grexam end
.class &grfun
.system
