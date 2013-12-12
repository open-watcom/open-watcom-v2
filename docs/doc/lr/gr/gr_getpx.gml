.gfuncw getpixel
.gsynop begin 
.if '&lang' eq 'C' .do begin
short _FAR _getpixel( short x, short y );

short _FAR _getpixel_w( double x, double y );
.do end
.el .do begin
integer*2 function _getpixel( x, y )
integer*2 x, y

integer*2 function _getpixel_w( x, y )
double precision x, y
.do end
.gsynop end
.desc begin
The
.id &func.
&routines return the pixel value for the point
with coordinates
.coord x y .
The
.id &func.
&routine uses the view coordinate system.
The
.id &func2.
&routine uses the window coordinate system.
.* .pp
.* .im gr_pixvl
.desc end
.return begin
The
.id &func.
&routines return the pixel value for the given point
when the point lies within the clipping region; otherwise, (-1) is
returned.
.return end
.see begin
.seelist &function. _setpixel
.see end
.grexam begin eg_getpx.&langsuff
.grexam end
.class &grfun
.system
