.gfunc getfillmask
.gsynop begin 
.if '&lang' eq 'C' .do begin
unsigned char _FAR * _FAR
    _getfillmask( unsigned char _FAR *mask );
.do end
.el .do begin
subroutine _getfillmask( mask )
integer*1 mask(8)
.do end
.gsynop end
.desc begin
The
.id &func.
&routine copies the current fill mask into the area
located by the argument
.arg mask
.ct .li .
The fill mask is used by the
.kw _ellipse
.ct ,
.kw _floodfill
.ct ,
.kw _pie
.ct ,
.kw _polygon
and
.kw _rectangle
&routines that fill an area of the screen.
.pp
.im gr_fillm
.desc end
.if '&lang' eq 'C' .do begin
.return begin
If no fill mask has been set,
.id NULL
is returned; otherwise,
the
.id &func.
&routine returns
.arg mask
.ct .li .
.return end
.do end
.see begin
.seelist _floodfill _setfillmask _setplotaction
.see end
.grexam begin eg_getfm.&langsuff
.grexam end
.class &grfun
.system
