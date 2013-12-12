.gfunc setfont
.gsynop begin 
.if '&lang' eq 'C' .do begin
short _FAR _setfont( char _FAR *opt );
.do end
.el .do begin
integer*2 function _setfont( opt )
character*(*) opt
.do end
.gsynop end
.desc begin
The
.id &func.
&routine selects a font from the list of registered fonts
(see the
.kw _registerfonts
&routine).
The font selected becomes the current font and is used whenever text
is displayed with the
.kw _outgtext
&routine..
The &routine will fail if no fonts have been registered, or if a font
cannot be found that matches the given characteristics.
.pp
The argument
.arg opt
is a string of characters specifying the characteristics of the desired font.
These characteristics determine which font is selected.
The options may be separated by blanks and are not case-sensitive.
Any number of options may be specified and in any order.
The available options are:
.begterm 15
.note hX
character height X (in pixels)
.note wX
character width X (in pixels)
.note f
choose a fixed-width font
.note p
choose a proportional-width font
.note r
choose a raster (bit-mapped) font
.note v
choose a vector font
.note b
choose the font that best matches the options
.note nX
choose font number X (the number of fonts is returned by the
.kw _registerfonts
&routine)
.note t'facename'
choose a font with specified facename
.endterm
.pp
The facename option is specified as a "t" followed by a facename enclosed
in single quotes.
The available facenames are:
.begterm 15
.note Courier
fixed-width raster font with serifs
.note Helv
proportional-width raster font without serifs
.note Tms Rmn
proportional-width raster font with serifs
.note Script
proportional-width vector font that appears similar to hand-writing
.note Modern
proportional-width vector font without serifs
.note Roman
proportional-width vector font with serifs
.endterm
.pp
When "nX" is specified to select a particular font, the other options
are ignored.
.pp
If the best fit option ("b") is specified,
.id &func.
will always be able
to select a font.
The font chosen will be the one that best matches the options specified.
The following precedence is given to the options when selecting a font:
.autonote
.note
Pixel height (higher precedence is given to heights less than
the specified height)
.note
Facename
.note
Pixel width
.note
Font type (fixed or proportional)
.endnote
.pp
When a pixel height or width does not match exactly and a vector font
has been selected, the font will be stretched appropriately to match the
given size.
.desc end
.return begin
The
.id &func.
&routine returns zero
if successful; otherwise, (-1) is returned.
.return end
.see begin
.seelist &function. _registerfonts _unregisterfonts _getfontinfo _outgtext _getgtextextent _setgtextvector _getgtextvector
.see end
.grexam begin eg_reg.&langsuff
.grexam end
.class &grfun
.system
