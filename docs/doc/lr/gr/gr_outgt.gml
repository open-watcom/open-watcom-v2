.gfunc _outgtext
.gsynop begin
.if '&lang' eq 'C' .do begin
void _FAR _outgtext( char _FAR *text );
.do end
.el .do begin
subroutine _outgtext( text )
character*(*) text
.do end
.gsynop end
.desc begin
The
.id &funcb.
&routine displays the character string indicated by the argument
.arg text
.period
The string must be terminated by a null character
.if '&lang' eq 'C' .do begin
('\0').
.do end
.el .do begin
(char(0)).
.do end
.pp
The string is displayed starting at the current position 
.seeref _moveto
in the current color and in the currently selected font
.seeref _setfont
.period
The current position is updated to follow the displayed text.
.pp
When no font has been previously selected with
.reffunc _setfont
.ct , a default font will be used.
The default font is an 8-by-8 bit-mapped font.
.im gr_text
.desc end
.if '&lang' eq 'C' .do begin
.return begin
The
.id &funcb.
&routine does not return a value.
.return end
.do end
.see begin
.seelist _registerfonts _unregisterfonts _setfont _getfontinfo _getgtextextent _setgtextvector _getgtextvector _outtext _outmem _grtext
.see end
.grexam begin eg_reg.&langsuff
.grexam end
.class &grfun
.system
