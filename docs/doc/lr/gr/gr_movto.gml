.gfuncw moveto
.gsynop begin 
.if '&lang' eq 'C' .do begin
struct xycoord _FAR _moveto( short x, short y );

struct _wxycoord _FAR _moveto_w( double x, double y );
.do end
.el .do begin
record /xycoord/ function _moveto( x, y )
integer*2 x, y

record /_wxycoord/ function _moveto_w( x, y )
double precision x, y
.do end
.gsynop end
.desc begin
The
.id &func.
&routines set the current output position for graphics.
The
.id &func.
&routine uses the view coordinate system.
The
.id &func2.
&routine uses the window coordinate system.
.np
The current output position is set to be
the point at the coordinates
.coord x y .
Nothing is drawn by the &routine..
The
.kw _lineto
&routine uses the current output position as the starting point
when a line is drawn.
.pp
Note that the output position for graphics output differs from that
for text output.
The output position for text output can be set by use of the
.kw _settextposition
&routine..
.desc end
.return begin
The
.id &func.
&routines return the previous value of the
output position for graphics.
.return end
.see begin
.seelist _getcurrentposition _lineto _settextposition
.see end
.grexam begin eg_movto.&langsuff
.grexam end
.class &grfun
.system
