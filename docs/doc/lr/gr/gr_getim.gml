.gfuncwxy getimage
.gsynop begin 
.if '&lang' eq 'C' .do begin
void _FAR _getimage( short x1, short y1,
                     short x2, short y2,
                     char _HUGE *image );

void _FAR _getimage_w( double x1, double y1,
                       double x2, double y2,
                       char _HUGE *image );

void _FAR _getimage_wxy( struct _wxycoord _FAR *p1,
                         struct _wxycoord _FAR *p2,
                         char _HUGE *image );
.do end
.el .do begin
subroutine _getimage( x1, y1, x2, y2, image )
integer*2 x1, y1
integer*2 x2, y2
integer*1 image(*)

subroutine _getimage_w( x1, y1, x2, y2, image )
double precision x1, y1
double precision x2, y2
integer*1 image(*)

subroutine _getimage_wxy( p1, p2, image )
record /_wxycoord/ p1, p2
integer*1 image(*)
.do end
.gsynop end
.desc begin
The
.id &funcb.
&routines store a copy of an area of the screen into the buffer
indicated by the
.arg image
argument.
The
.id &funcb.
&routine uses the view coordinate system.
The
.id &func2.
and
.id &func3.
&routines use the window coordinate system.
.np
The screen image is the rectangular area defined by the points
.coord x1 y1
and
.coord x2 y2 .
The buffer
.arg image
must be large enough to contain the image (the size of the image
can be determined by using the
.kw _imagesize
&routine).
The image may be displayed upon the screen at some later time
by using the
.kw _putimage
&routines..
.desc end
.if '&lang' eq 'C' .do begin
.return begin
The
.id &funcb.
&routines do not return a value.
.return end
.do end
.see begin
.seelist _imagesize _putimage
.see end
.grexam begin eg_getim.&langsuff
.grexam end
.class &grfun
.system
