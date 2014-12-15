.gfunc setvieworg
.gsynop begin 
.if '&lang' eq 'C' .do begin
struct xycoord _FAR _setvieworg( short x, short y );
.do end
.el .do begin
record /xycoord/ function _setvieworg( x, y )
integer*2 x, y
.do end
.gsynop end
.desc begin
The
.id &funcb.
&routine sets the origin of the view coordinate system,
.coord 0 0 ,
to be located at the physical point
.coord x y .
This causes subsequently drawn images to be translated by the amount
.coord x y .
.if '&lang' eq 'C' .do begin
.np
.bd Note:
In previous versions of the software, the
.id &funcb.
&routine was called
.mono _setlogorg.
.uindex setlogorg
.do end
.desc end
.return begin
The
.id &funcb.
&routine returns, as an
.id xycoord
structure, the physical coordinates of
the previous origin.
.return end
.see begin
.seelist _getviewcoord _getphyscoord _setcliprgn _setviewport
.see end
.grexam begin eg_setlg.&langsuff
.grexam end
.class &grfun
.system
