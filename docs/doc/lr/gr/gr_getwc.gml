.gfunc getwindowcoord
.gsynop begin 
.if '&lang' eq 'C' .do begin
struct _wxycoord _FAR _getwindowcoord( short x, short y );
.do end
.el .do begin
record /_wxycoord/ function _getwindowcoord( x, y )
integer*2 x, y
.do end
.gsynop end
.desc begin
The
.id &func.
&routine returns the window coordinates
of the position with view coordinates
.coord x y .
Window coordinates are defined by the
.kw _setwindow
&routine..
.desc end
.return begin
The
.id &func.
&routine returns the window coordinates,
as a
.id _wxycoord
structure,
of the given point.
.return end
.see begin
.seelist &function. _setwindow _getviewcoord
.see end
.grexam begin eg_getwc.&langsuff
.grexam end
.class &grfun
.system
