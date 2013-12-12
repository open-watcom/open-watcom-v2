.gfunc getlinestyle
.gsynop begin 
.if '&lang' eq 'C' .do begin
unsigned short _FAR _getlinestyle( void );
.do end
.el .do begin
integer*2 function _getlinestyle()
.do end
.gsynop end
.desc begin
The
.id &func.
&routine returns the current line-style mask.
.pp
.im gr_lines
.pp
The default line style mask is
.hex FFFF .
.desc end
.return begin
The
.id &func.
&routine returns the current line-style mask.
.return end
.see begin
.seelist &function. _lineto _pie _rectangle _polygon _setlinestyle
.see end
.grexam begin eg_getls.&langsuff
.grexam end
.class &grfun
.system
