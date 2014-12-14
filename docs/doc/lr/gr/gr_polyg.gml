.gfuncwxy polygon
.gsynop begin 
.if '&lang' eq 'C' .do begin
short _FAR _polygon( short fill, short numpts,
                     struct xycoord _FAR *points );

short _FAR _polygon_w( short fill, short numpts,
                       double _FAR *points );

short _FAR _polygon_wxy( short fill, short numpts,
                         struct _wxycoord _FAR *points );
.do end
.el .do begin
integer*2 function _polygon( fill, numpts, points )
integer*2 fill
integer*2 numpts
record /xycoord/ points(*)

integer*2 function _polygon_w( fill, numpts, points )
integer*2 fill
integer*2 numpts
double precision points(*)

integer*2 function _polygon_wxy( fill, numpts, points )
integer*2 fill
integer*2 numpts
record /_wxycoord/ points(*)
.do end
.gsynop end
.desc begin
The
.id &func.
&routines draw polygons.
The
.id &func.
&routine uses the view coordinate system.
The
.id &func2.
and
.id &func3.
&routines use the window coordinate system.
.np
The polygon is defined as containing
.arg numpts
points whose coordinates are given in the array
.arg points
.ct .li .
.im gr_fgfil fill polygon
.* .pp
.* When the coordinates
.* establish a line or a point (this happens when all of the
.* coordinates and/or all the y-coordinates are equal), nothing is drawn.
.* .pp
.* .im gr_lines
.desc end
.return begin
.im gr_retdr polygon
.return end
.see begin
.seelist _setcolor _setfillmask _setlinestyle _setplotaction
.see end
.grexam begin eg_polyg.&langsuff
.grexam output
.picture eg_polyg
.grexam end
.class &grfun
.system
