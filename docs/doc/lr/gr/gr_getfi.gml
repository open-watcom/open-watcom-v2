.gfunc _getfontinfo
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
.id &funcb.
&routine returns information about the currently selected font.
Fonts are selected with the
.reffunc _setfont
&routine..
The font information is returned in the
.id _fontinfo
structure indicated by the argument
.arg info
.period
The structure contains the following fields:
.begterm 15
.termnx type
1 for a vector font, 0 for a bit-mapped font
.termnx ascent
distance from top of character to baseline in pixels
.termnx pixwidth
character width in pixels (0 for a proportional font)
.termnx pixheight
character height in pixels
.termnx avgwidth
average character width in pixels
.termnx filename
name of the file containing the current font
.termnx facename
name of the current font
.endterm
.desc end
.return begin
The
.id &funcb.
&routine returns zero if the font information is
returned successfully; otherwise a negative value is returned.
.return end
.see begin
.seelist _registerfonts _unregisterfonts _setfont _outgtext _getgtextextent _setgtextvector _getgtextvector
.see end
.grexam begin eg_getfi.&langsuff
.grexam end
.class &grfun
.system
