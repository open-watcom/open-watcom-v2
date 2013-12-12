.gfunc setviewport
.gsynop begin 
.if '&lang' eq 'C' .do begin
void _FAR _setviewport( short x1, short y1,
                        short x2, short y2 );
.do end
.el .do begin
subroutine _setviewport( x1, y1, x2, y2 )
integer*2 x1, y1
integer*2 x2, y2
.do end
.gsynop end
.desc begin
The
.id &func.
&routine restricts the display of graphics output to the
clipping region and then sets the origin of the view coordinate system
to be the top left corner of the region.
This region is a rectangle whose opposite corners are established
by the physical points
.coord x1 y1
and
.coord x2 y2 .
.pp
The
.id &func.
&routine does not affect text output using the
.kw _outtext
and
.kw _outmem
&routines..
To control the location of text output, see the
.kw _settextwindow
&routine..
.desc end
.if '&lang' eq 'C' .do begin
.return begin
The
.id &func.
&routine does not return a value.
.return end
.do end
.see begin
.seelist &function. _setcliprgn _setvieworg _settextwindow _setwindow
.see end
.grexam begin eg_setvp.&langsuff
.grexam end
.class &grfun
.system
