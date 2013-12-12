.gfuncwxy arc
.gsynop begin 
.if '&lang' eq 'C' .do begin
short _FAR _arc( short x1, short y1,
                 short x2, short y2,
                 short x3, short y3,
                 short x4, short y4 );

short _FAR _arc_w( double x1, double y1,
                   double x2, double y2,
                   double x3, double y3,
                   double x4, double y4 );

short _FAR _arc_wxy( struct _wxycoord _FAR *p1,
                     struct _wxycoord _FAR *p2,
                     struct _wxycoord _FAR *p3,
                     struct _wxycoord _FAR *p4 );
.do end
.el .do begin
integer*2 function _arc( x1, y1, x2, y2,
                         x3, y3, x4, y4 )
integer*2 x1, y1
integer*2 x2, y2
integer*2 x3, y3
integer*2 x4, y4

integer*2 function _arc_w( x1, y1, x2, y2,
                           x3, y3, x4, y4 )
double precision x1, y1
double precision x2, y2
double precision x3, y3
double precision x4, y4

integer*2 function _arc_wxy( p1, p2, p3, p4 )
record /_wxycoord/ p1
record /_wxycoord/ p2
record /_wxycoord/ p3
record /_wxycoord/ p4
.do end
.gsynop end
.desc begin
The
.id &func.
&routines draw elliptical arcs.
The
.id &func.
&routine uses the view coordinate system.
The
.id &func2.
and
.id &func3.
&routines use the window coordinate system.
.np
The center of the arc is the center of the
rectangle established by the points
.coord x1 y1
and
.coord x2 y2 .
.im gr_vect
.picture pic1
.im gr_noarc
.pp
The current output position for graphics output is set to be
the point at the end of the arc that was drawn.
.desc end
.return begin
.im gr_retdr arc
.return end
.see begin
.seelist &function. _ellipse _pie _rectangle _getarcinfo _setcolor _setlinestyle _setplotaction
.see end
.grexam begin eg_arc.&langsuff
.grexam output
.picture eg_arc
.grexam end
.class &grfun
.system
