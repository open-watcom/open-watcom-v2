.gfunc getphyscoord
.gsynop begin 
.if '&lang' eq 'C' .do begin
struct xycoord _FAR _getphyscoord( short x, short y );
.do end
.el .do begin
record /xycoord/ function _getphyscoord( x, y )
integer*2 x, y
.do end
.gsynop end
.desc begin
The
.id &funcb.
&routine returns the physical coordinates
of the position with view coordinates
.coord x y .
View coordinates are defined by the
.kw _setvieworg
and
.kw _setviewport
&routines..
.desc end
.return begin
The
.id &funcb.
&routine returns the physical coordinates,
as an
.id xycoord
structure,
of the given point.
.return end
.see begin
.seelist _getviewcoord _setvieworg _setviewport
.see end
.grexam begin eg_getpc.&langsuff
.grexam end
.class &grfun
.system
