.gfunc getgtextvector
.gsynop begin 
.if '&lang' eq 'C' .do begin
struct xycoord _FAR _getgtextvector( void );
.do end
.el .do begin
record /xycoord/ function _getgtextvector()
.do end
.gsynop end
.desc begin
The
.id &func.
&routine returns the current value of the text orientation vector.
This is the direction used when text is displayed by the
.kw _outgtext
&routine..
.desc end
.return begin
The
.id &func.
&routine returns, as an
.id xycoord
structure, the current value of the text orientation vector.
.return end
.see begin
.seelist _registerfonts _unregisterfonts _setfont _getfontinfo _outgtext _getgtextextent _setgtextvector
.see end
.grexam begin eg_gettv.&langsuff
.grexam end
.class &grfun
.system
