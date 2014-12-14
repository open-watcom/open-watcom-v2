.gfunc remapallpalette
.gsynop begin 
.if '&lang' eq 'C' .do begin
short _FAR _remapallpalette( long _FAR *colors );
.do end
.el .do begin
integer*2 function _remapallpalette( colors )
integer*4 colors(*)
.do end
.gsynop end
.desc begin
The
.id &func.
&routine sets (or remaps) all of the colors in the palette.
The color values in the palette are replaced by the array of
color values given by the argument
.arg colors
.ct .li .
This &routine is supported in all video modes, but only works
with EGA, MCGA and VGA adapters.
.np
The array
.arg colors
must contain at least as many elements as there are supported colors.
The newly mapped palette will cause the complete screen to change color
wherever there is a pixel value of a changed color in the palette.
.pp
The representation of colors depends upon the hardware being used.
The number of colors in the palette can be determined by using the
.kw _getvideoconfig
&routine..
.desc end
.return begin
The
.id &func.
&routine returns (-1) if the palette is remapped successfully
and zero otherwise.
.return end
.see begin
.seelist _remappalette _getvideoconfig
.see end
.grexam begin eg_mappa.&langsuff
.grexam end
.class &grfun
.system
