.gfunc setwindow
.gsynop begin 
.if '&lang' eq 'C' .do begin
short _FAR _setwindow( short invert,
                       double x1, double y1,
                       double x2, double y2 );
.do end
.el .do begin
integer*2 function _setwindow( invert, x1, y1, x2, y2 )
logical invert
double precision x1, y1
double precision x2, y2
.do end
.gsynop end
.desc begin
The
.id &func.
&routine defines a window for the window coordinate system.
Window coordinates are specified as a user-defined range of values.
This allows for consistent pictures regardless of the video mode.
.np
The window is defined as the region with opposite corners
established by the points
.coord x1 y1
and
.coord x2 y2 .
The argument
.arg invert
specifies the direction of the y-axis.
If the value is
.if '&lang' eq 'C' .do begin
non-zero,
.do end
.el .do begin
.mono .TRUE.,
.do end
the y values increase from the bottom of the screen to the top,
otherwise, the y values increase as you move down the screen.
.np
The window defined by the
.id &func.
&routine is displayed in
the current viewport.
A viewport is defined by the
.kw _setviewport
&routine..
.np
By default, the window coordinate system is defined with the point
.coord 0.0 0.0
located at the lower left corner of the screen, and the point
.coord 1.0 1.0
at the upper right corner.
.desc end
.return begin
The
.id &func.
&routine returns a non-zero value when the window is set
successfully; otherwise, zero is returned.
.return end
.see begin
.seelist _setviewport
.see end
.grexam begin eg_setw.&langsuff
.grexam end
.class &grfun
.system
