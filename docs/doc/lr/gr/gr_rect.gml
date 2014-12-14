.gfuncwxy rectangle
.gsynop begin 
.if '&lang' eq 'C' .do begin
short _FAR _rectangle( short fill,
                       short x1, short y1,
                       short x2, short y2 );

short _FAR _rectangle_w( short fill,
                         double x1, double y1,
                         double x2, double y2 );

short _FAR _rectangle_wxy( short fill,
                           struct _wxycoord _FAR *p1,
                           struct _wxycoord _FAR *p2 );
.do end
.el .do begin
integer*2 function _rectangle( fill, x1, y1, x2, y2 )
integer*2 fill
integer*2 x1, y1
integer*2 x2, y2

integer*2 function _rectangle_w( fill, x1, y1, x2, y2 )
integer*2 fill
double precision x1, y1
double precision x2, y2

integer*2 function _rectangle_wxy( fill, p1, p2 )
integer*2 fill
record /_wxycoord/ p1, p2
.do end
.gsynop end
.desc begin
The
.id &func.
&routines draw rectangles.
The
.id &func.
&routine uses the view coordinate system.
The
.id &func2.
and
.id &func3.
&routines use the window coordinate system.
.np
The rectangle is defined with opposite corners
established by the points
.coord x1 y1
and
.coord x2 y2 .
.im gr_fgfil fill rectangle
.* .pp
.* .im gr_lines
.desc end
.return begin
.im gr_retdr rectangle
.return end
.see begin
.seelist _setcolor _setfillmask _setlinestyle _setplotaction
.see end
.grexam begin eg_rect.&langsuff
.grexam output
.picture eg_rect
.grexam end
.class &grfun
.system
