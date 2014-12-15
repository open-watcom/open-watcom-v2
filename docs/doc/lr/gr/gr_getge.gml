.gfunc getgtextextent
.gsynop begin 
.if '&lang' eq 'C' .do begin
short _FAR _getgtextextent( char _FAR *text );
.do end
.el .do begin
integer*2 function _getgtextextent( text )
character*(*) text
.do end
.gsynop end
.desc begin
The
.id &funcb.
&routine returns the length in pixels of the argument
.arg text
as it would be displayed in the current font by the &routine
.kw _outgtext
.ct .li .
Note that the text is not displayed on the screen, only its length
is determined.
.desc end
.return begin
The
.id &funcb.
&routine returns the length in pixels of a string.
.return end
.see begin
.seelist _registerfonts _unregisterfonts _setfont _getfontinfo _outgtext _setgtextvector _getgtextvector
.see end
.grexam begin eg_getfi.&langsuff
.grexam end
.class &grfun
.system
