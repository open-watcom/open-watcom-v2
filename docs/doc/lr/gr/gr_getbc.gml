.gfunc getbkcolor
.gsynop begin 
.if '&lang' eq 'C' .do begin
long _FAR _getbkcolor( void );
.do end
.el .do begin
integer*4 function _getbkcolor()
.do end
.gsynop end
.desc begin
The
.id &func.
&routine returns the current background color.
In text modes, the background color controls the area behind each
individual character.
In graphics modes, the background refers to the entire screen.
The default background color is 0.
.* .pp
.* .im gr_pixvl
.desc end
.return begin
The
.id &func.
&routine returns the current background color.
.return end
.see begin
.seelist &function. _setbkcolor _remappalette
.see end
.grexam begin eg_getbc.&langsuff
.grexam end
.class &grfun
.system
