.gfuncw setpixel
.gsynop begin 
.if '&lang' eq 'C' .do begin
short _FAR _setpixel( short x, short y );

short _FAR _setpixel_w( double x, double y );
.do end
.el .do begin
integer*2 function _setpixel( x, y )
integer*2 x, y

integer*2 function _setpixel_w( x, y )
double precision x, y
.do end
.gsynop end
.desc begin
The
.id &funcb.
&routine sets the pixel value of the point
.coord x y
using the current plotting action with the current color.
The
.id &funcb.
&routine uses the view coordinate system.
The
.id &func2.
&routine uses the window coordinate system.
.pp
.im gr_pixvl
.desc end
.return begin
The
.id &funcb.
&routines return the previous value of the indicated pixel
if the pixel value can be set; otherwise, (-1) is returned.
.return end
.see begin
.seelist _getpixel _setcolor _setplotaction
.see end
.grexam begin eg_getpx.&langsuff
.grexam end
.class &grfun
.system
