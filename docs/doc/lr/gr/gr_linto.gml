.gfuncw lineto
.if '&lang' eq 'C' .do begin
short _FAR _lineto( short x, short y );

short _FAR _lineto_w( double x, double y );
.do end
.el .do begin
integer*2 function _lineto( x, y )
integer*2 x, y

integer*2 function _lineto_w( x, y )
double precision x, y
.do end
.gfuncend
.desc begin
The &func &routines draw straight lines.
The &func &routine uses the view coordinate system.
The &func2 &routine uses the window coordinate system.
.np
The line is drawn from the current position to
the point at the coordinates
.coord x y .
The point
.coord x y
becomes the new current position.
The line is drawn with the current plotting action using
the current line style and the current color.
.desc end
.return begin
.im gr_retdr line
.return end
.see begin
.seelist &function. _moveto _setcolor _setlinestyle _setplotaction
.see end
.grexam begin eg_movto.&langsuff
.grexam output
.picture eg_movto
.grexam end
.class &grfun
.system
