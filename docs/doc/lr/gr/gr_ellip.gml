.gfuncwxy ellipse
.gsynop begin 
.if '&lang' eq 'C' .do begin
short _FAR _ellipse( short fill, short x1, short y1,
                                 short x2, short y2 );

short _FAR _ellipse_w( short fill, double x1, double y1,
                                   double x2, double y2 );

short _FAR _ellipse_wxy( short fill,
                         struct _wxycoord _FAR *p1,
                         struct _wxycoord _FAR *p2 );
.do end
.el .do begin
integer*2 function _ellipse( fill, x1, y1, x2, y2 )
integer*2 fill
integer*2 x1, y1
integer*2 x2, y2

integer*2 function _ellipse_w( fill, x1, y1, x2, y2 )
integer*2 fill,
double precision x1, y1
double precision x2, y2

integer*2 function _ellipse_wxy( fill, p1, p2 )
integer*2 fill,
record /_wxycoord/ p1, p2
.do end
.gsynop end
.desc begin
The
.id &func.
&routines draw ellipses.
The
.id &func.
&routine uses the view coordinate system.
The
.id &func2.
and
.id &func3.
&routines use the window coordinate system.
.np
The center of the ellipse is the center of the
rectangle established by the points
.coord x1 y1
and
.coord x2 y2 .
.im gr_fgfil fill ellipse
.im gr_noarc
.desc end
.return begin
.im gr_retdr ellipse
.return end
.see begin
.seelist &function. _arc _rectangle _setcolor _setfillmask _setlinestyle _setplotaction
.see end
.grexam begin eg_ellip.&langsuff
.grexam output
.picture eg_ellip
.grexam end
.class &grfun
.system
