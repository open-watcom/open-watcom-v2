.gfunc remappalette
.if '&lang' eq 'C' .do begin
long _FAR _remappalette( short pixval, long color );
.do end
.el .do begin
integer*4 function _remappalette( pixval, color )
integer*2 pixval
integer*4 color
.do end
.gfuncend
.desc begin
The &func &routine sets (or remaps) the palette color
.arg pixval
to be the color
.arg color
.ct .li .
This &routine is supported in all video modes, but only works
with EGA, MCGA and VGA adapters.
.pp
The argument
.arg pixval
is an index in the color palette of the current video mode.
The argument
.arg color
specifies the actual color displayed on the screen by pixels with
pixel value
.arg pixval
.ct .li .
Color values are selected by specifying the red, green and
blue intensities that make up the color.
Each intensity can be in the range from 0 to 63, resulting in 262144
possible different colors.
A given color value can be conveniently specified as a value of type
.mono &long..
The color value is of the form
.hex 00bbggrr ,
where
.mono bb
is the blue intensity,
.mono gg
is the green intensity
and
.mono rr
is the red intensity of the selected color.
The file
.filename graph.&hext
defines constants containing the color intensities of each
of the 16 default colors.
.pp
The &func &routine takes effect immediately.
All pixels on the complete screen which have a pixel value equal to the value of
.arg pixval
will now have the color indicated by the argument
.arg color
.ct .li .
.desc end
.return begin
The &func &routine returns the previous color for the pixel value
if the palette is remapped successfully; otherwise, (-1) is returned.
.return end
.see begin
.seelist &function. _remapallpalette _setvideomode
.see end
.grexam begin eg_mappc.&langsuff
.grexam end
.class &grfun
.system
