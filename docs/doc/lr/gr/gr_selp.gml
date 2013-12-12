.gfunc selectpalette
.gsynop begin 
.if '&lang' eq 'C' .do begin
short _FAR _selectpalette( short palnum );
.do end
.el .do begin
integer*2 function _selectpalette( palnum )
integer*2 palnum
.do end
.gsynop end
.desc begin
The
.id &func.
&routine selects the palette indicated by the argument
.arg palnum
from the color palettes available.
This &routine is only supported by the video modes
.mono _MRES4COLOR
and
.mono _MRESNOCOLOR.
.pp
Mode
.mono _MRES4COLOR
supports four palettes of four colors.
In each palette, color 0, the background color, can be any of the 16
possible colors.
The color values associated with the other three pixel values, (1, 2
and 3), are determined by the selected palette.
.pp
The following table outlines the available color palettes:
.millust begin
Palette              Pixel Values
Number    1               2               3

   0     green           red             brown
   1     cyan            magenta         white
   2     light green     light red       yellow
   3     light cyan      light magenta   bright white
.millust end
.desc end
.return begin
The
.id &func.
&routine returns the number of the previously selected palette.
.return end
.see begin
.seelist &function. _setvideomode _getvideoconfig
.see end
.grexam begin eg_selpa.&langsuff
.grexam end
.class &grfun
.system
