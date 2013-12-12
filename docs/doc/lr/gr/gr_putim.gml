.gfuncw putimage
.gsynop begin 
.if '&lang' eq 'C' .do begin
void _FAR _putimage( short x, short y,
                     char _HUGE *image, short mode );

void _FAR _putimage_w( double x, double y,
                       char _HUGE *image, short mode );
.do end
.el .do begin
subroutine _putimage( x, y, image, mode )
integer*2 x, y
integer*1 image(*)
integer*2 mode

subroutine _putimage_w( x, y, image, mode )
double precision x, y
integer*1 image(*)
integer*2 mode
.do end
.gsynop end
.desc begin
The
.id &func.
&routines display the screen image indicated by the argument
.arg image
.ct .li .
The
.id &func.
&routine uses the view coordinate system.
The
.id &func2.
&routine uses the window coordinate system.
.np
The image is displayed upon the screen with its top left corner
located at the point with coordinates
.coord x y .
The image was previously saved using the
.kw _getimage
&routines..
The image is displayed in a rectangle whose size
is the size of the rectangular image saved by the
.kw _getimage
&routines..
.pp
The image can be displayed in a number of ways, depending upon the
value of the
.arg mode
argument.
This argument can have the following values:
.begterm 15
.uterm GPSET
replace the rectangle on the screen by the saved image
.uterm GPRESET
replace the rectangle on the screen with the pixel values of the
saved image inverted; this produces a negative image
.uterm GAND
produce a new image on the screen by ANDing together the pixel values
from the screen with those from the saved image
.uterm GOR
produce a new image on the screen by ORing together the pixel values
from the screen with those from the saved image
.uterm GXOR
produce a new image on the screen by exclusive ORing together
the pixel values from the screen with those from the saved image;
the original screen is restored by two successive calls to the &func
&routine with this value, providing an efficient method to produce
animated effects
.endterm
.desc end
.if '&lang' eq 'C' .do begin
.return begin
The
.id &func.
&routines do not return a value.
.return end
.do end
.see begin
.seelist &function. _getimage _imagesize
.see end
.grexam begin eg_getim.&langsuff
.grexam end
.class &grfun
.system
