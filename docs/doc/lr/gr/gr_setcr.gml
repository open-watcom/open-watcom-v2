.gfunc setcliprgn
.if '&lang' eq 'C' .do begin
void _FAR _setcliprgn( short x1, short y1,
                       short x2, short y2 );
.do end
.el .do begin
subroutine _setcliprgn( x1, y1, x2, y2 )
integer*2 x1, y1
integer*2 x2, y2
.do end
.gfuncend
.desc begin
The &func &routine restricts the display of graphics output to the
clipping region.
This region is a rectangle whose opposite corners are established
by the physical points
.coord x1 y1
and
.coord x2 y2 .
.pp
The &func &routine does not affect text output using the
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
The &func &routine does not return a value.
.return end
.do end
.see begin
.seelist &function. _settextwindow _setvieworg _setviewport
.see end
.grexam begin eg_setcr.&langsuff
.grexam end
.class &grfun
.system
