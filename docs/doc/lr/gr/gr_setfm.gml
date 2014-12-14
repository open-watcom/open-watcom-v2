.gfunc setfillmask
.gsynop begin 
.if '&lang' eq 'C' .do begin
void _FAR _setfillmask( char _FAR *mask );
.do end
.el .do begin
subroutine _setfillmask( mask )
integer*1 mask(8)
.do end
.gsynop end
.desc begin
The
.id &func.
&routine sets the current fill mask to the value of the argument
.arg mask
.ct .li .
.if '&lang' eq 'C' .do begin
When the value of the
.arg mask
argument is
.id NULL,
there will be no fill mask set.
.do end
.pp
.im gr_fillm
By default, no fill mask is set.
.desc end
.if '&lang' eq 'C' .do begin
.return begin
The
.id &func.
&routine does not return a value.
.return end
.do end
.see begin
.seelist _getfillmask _ellipse _floodfill _rectangle _polygon _pie _setcolor _setplotaction
.see end
.grexam begin eg_getfm.&langsuff
.grexam output
.picture eg_getfm
.grexam end
.class &grfun
.system
