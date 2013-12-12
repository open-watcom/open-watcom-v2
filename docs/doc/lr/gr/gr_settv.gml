.gfunc setgtextvector
.gsynop begin 
.if '&lang' eq 'C' .do begin
struct xycoord _FAR _setgtextvector( short x, short y );
.do end
.el .do begin
record /xycoord/ function _setgtextvector( x, y )
integer*2 x, y
.do end
.gsynop end
.desc begin
The
.id &func.
&routine sets the orientation for text output used by the
.kw _outgtext
&routine to the vector specified by the arguments
.coord x y .
Each of the arguments can have a value of -1, 0 or 1, allowing for
text to be displayed at any multiple of a 45-degree angle.
The default text orientation, for normal left-to-right text,
is the vector
.coord 1 0 .
.desc end
.return begin
The
.id &func.
&routine returns, as an
.id xycoord
structure, the previous value of the text orientation vector.
.return end
.see begin
.seelist &function. _registerfonts _unregisterfonts _setfont _getfontinfo _outgtext _getgtextextent _getgtextvector
.see end
.grexam begin eg_gettv.&langsuff
.grexam end
.class &grfun
.system
