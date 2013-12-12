.gfunc getfontinfo
.gsynop begin 
.if '&lang' eq 'C' .do begin
short _FAR _getfontinfo( struct _fontinfo _FAR *info );
.do end
.el .do begin
integer*2 function _getfontinfo( info )
record /_fontinfo/ info
.do end
.gsynop end
.desc begin
The
.id &func.
&routine returns information about the currently selected font.
Fonts are selected with the
.kw _setfont
&routine..
The font information is returned in the
.id _fontinfo
structure indicated by the argument
.arg info
.ct .li .
The structure contains the following fields:
.begterm 15
.note type
1 for a vector font, 0 for a bit-mapped font
.note ascent
distance from top of character to baseline in pixels
.note pixwidth
character width in pixels (0 for a proportional font)
.note pixheight
character height in pixels
.note avgwidth
average character width in pixels
.note filename
name of the file containing the current font
.note facename
name of the current font
.endterm
.desc end
.return begin
The
.id &func.
&routine returns zero if the font information is
returned successfully; otherwise a negative value is returned.
.return end
.see begin
.seelist &function. _registerfonts _unregisterfonts _setfont _outgtext _getgtextextent _setgtextvector _getgtextvector
.see end
.grexam begin eg_getfi.&langsuff
.grexam end
.class &grfun
.system
