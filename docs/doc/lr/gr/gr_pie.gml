.gfuncwxy pie
.if '&lang' eq 'C' .do begin
short _FAR _pie( short fill, short x1, short y1,
                             short x2, short y2,
                             short x3, short y3,
                             short x4, short y4 );

short _FAR _pie_w( short fill, double x1, double y1,
                               double x2, double y2,
                               double x3, double y3,
                               double x4, double y4 );

short _FAR _pie_wxy( short fill,
                     struct _wxycoord _FAR *p1,
                     struct _wxycoord _FAR *p2,
                     struct _wxycoord _FAR *p3,
                     struct _wxycoord _FAR *p4 );
.do end
.el .do begin
integer*2 function _pie( fill, x1, y1, x2, y2,
                               x3, y3, x4, y4 )
integer*2 fill
integer*2 x1, y1
integer*2 x2, y2
integer*2 x3, y3
integer*2 x4, y4

integer*2 function _pie_w( fill, x1, y1, x2, y2,
                                 x3, y3, x4, y4 )
integer*2 fill
double precision x1, y1
double precision x2, y2
double precision x3, y3
double precision x4, y4

integer*2 function _pie_wxy( fill, p1, p2, p3, p4 )
integer*2 fill,
record /_wxycoord/ p1, p2
record /_wxycoord/ p3, p4
.do end
.gfuncend
.desc begin
The &func &routines draw pie-shaped wedges.
The &func &routine uses the view coordinate system.
The &func2 and &func3 &routines use the window coordinate system.
.np
The pie wedges are drawn by drawing an elliptical
arc (in the way described for the
.kw _arc
&routines) and then joining the center of the rectangle that
contains the ellipse to the two endpoints of the arc.
.pp
The elliptical arc is drawn with its center at the center of the
rectangle established by the points
.coord x1 y1
and
.coord x2 y2 .
.im gr_vect
.picture pic2
.im gr_noarc
.im gr_fgfil fill figure
.desc end
.return begin
.im gr_retdr figure
.return end
.see begin
.seelist &function. _arc _ellipse _setcolor _setfillmask _setlinestyle _setplotaction
.see end
.grexam begin eg_pie.&langsuff
.grexam output
.picture eg_pie
.grexam end
.class &grfun
.system
