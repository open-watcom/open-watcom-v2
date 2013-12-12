.gfunc getcliprgn
.gsynop begin 
.if '&lang' eq 'C' .do begin
void _FAR _getcliprgn( short _FAR *x1, short _FAR *y1,
                       short _FAR *x2, short _FAR *y2 );
.do end
.el .do begin
subroutine _getcliprgn( x1, y1, x2, y2 )
integer*2 x1, y1
integer*2 x2, y2
.do end
.gsynop end
.desc begin
The
.id &func.
&routine returns the location of the current clipping region.
A clipping region is defined with the
.kw _setcliprgn
or
.kw _setviewport
&routines..
By default, the clipping region is the entire screen.
.np
The current clipping region is a rectangular area of the screen
to which graphics output is restricted.
The top left corner of the clipping region is placed in the arguments
.coord x1 y1 .
The bottom right corner of the clipping region is placed in
.coord x2 y2 .
.desc end
.if '&lang' eq 'C' .do begin
.return begin
The
.id &func.
&routine returns the location of the current clipping region.
.return end
.do end
.see begin
.seelist &function. _setcliprgn _setviewport
.see end
.grexam begin eg_setcr.&langsuff
.grexam end
.class &grfun
.system
