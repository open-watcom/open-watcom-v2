.gfunc setbkcolor
.gsynop begin 
.if '&lang' eq 'C' .do begin
long _FAR _setbkcolor( long color );
.do end
.el .do begin
integer*4 function _setbkcolor( color )
integer*4 color
.do end
.gsynop end
.desc begin
The
.id &func.
&routine sets the current background color to be that of the
.arg color
argument.
In text modes, the background color controls the area behind each
individual character.
In graphics modes, the background refers to the entire screen.
The default background color is 0.
.pp
When the current video mode is a graphics mode,
any pixels with a zero pixel
value will change to the color of the
.arg color
argument.
When the current video mode is a text mode,
nothing will immediately change;
only subsequent output is affected.
.* .pp
.* .im gr_pixvl
.desc end
.return begin
The
.id &func.
&routine returns the previous background color.
.return end
.see begin
.seelist &function. _getbkcolor
.see end
.grexam begin eg_getbc.&langsuff
.grexam end
.class &grfun
.system
