.gfunc settextcolor
.gsynop begin 
.if '&lang' eq 'C' .do begin
short _FAR _settextcolor( short pixval );
.do end
.el .do begin
integer*2 function _settextcolor( pixval )
integer*2 pixval
.do end
.gsynop end
.desc begin
The
.id &func.
&routine sets the current text color to be the color
indicated by the pixel value of the
.arg pixval
argument.
This is the color value used for displaying text with the
.kw _outtext
and
.kw _outmem
&routines..
Use the
.kw _setcolor
&routine to change the color of graphics output.
The default text color value is set to 7 whenever a new video mode is selected.
.pp
The pixel value
.arg pixval
is a number in the range 0-31.
Colors in the range 0-15 are displayed normally.
In text modes, blinking colors are specified by adding 16 to the normal
color values.
The following table specifies the default colors in color text modes.
.millust begin
Pixel      Color        Pixel      Color
value                   value

  0       Black           8       Gray
  1       Blue            9       Light Blue
  2       Green          10       Light Green
  3       Cyan           11       Light Cyan
  4       Red            12       Light Red
  5       Magenta        13       Light Magenta
  6       Brown          14       Yellow
  7       White          15       Bright White
.millust end
.* .pp
.* .im gr_pixvl
.desc end
.return begin
The
.id &func.
&routine returns the pixel value of the previous text color.
.return end
.see begin
.seelist &function. _gettextcolor _outtext _outmem _setcolor
.see end
.grexam begin eg_gettc.&langsuff
.grexam end
.class &grfun
.system
