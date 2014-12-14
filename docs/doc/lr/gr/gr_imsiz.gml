.gfuncwxy imagesize
.gsynop begin 
.if '&lang' eq 'C' .do begin
long _FAR _imagesize( short x1, short y1,
                      short x2, short y2 );

long _FAR _imagesize_w( double x1, double y1,
                        double x2, double y2 );

long _FAR _imagesize_wxy( struct _wxycoord _FAR *p1,
                          struct _wxycoord _FAR *p2 );
.do end
.el .do begin
integer*4 function _imagesize( x1, y1, x2, y2 )
integer*2 x1, y1
integer*2 x2, y2

integer*4 function _imagesize_w( x1, y1, x2, y2 )
double precision x1, y1
double precision x2, y2

integer*4 function _imagesize_wxy( p1, p2 )
record /_wxycoord/ p1, p2
.do end
.gsynop end
.desc begin
The
.id &func.
&routines compute the number of bytes required to store
a screen image.
The
.id &func.
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
The storage area used by the
.kw _getimage
&routines must be at least this large (in bytes).
.desc end
.return begin
The
.id &func.
&routines return the size of a screen image.
.return end
.see begin
.seelist _getimage _putimage
.see end
.grexam begin eg_getim.&langsuff
.grexam end
.class &grfun
.system
