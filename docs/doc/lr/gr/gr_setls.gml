.gfunc setlinestyle
.gsynop begin 
.if '&lang' eq 'C' .do begin
void _FAR _setlinestyle( unsigned short style );
.do end
.el .do begin
subroutine _setlinestyle( style )
integer*2 style
.do end
.gsynop end
.desc begin
The
.id &func.
&routine sets the current line-style mask to the value of the
.arg style
argument.
.pp
.im gr_lines
.pp
The default line style mask is
.hex FFFF .
.desc end
.if '&lang' eq 'C' .do begin
.return begin
The
.id &func.
&routine does not return a value.
.return end
.do end
.see begin
.seelist &function. _getlinestyle _lineto _rectangle _polygon _setplotaction
.see end
.grexam begin eg_getls.&langsuff
.grexam output
.picture eg_getls
.grexam end
.class &grfun
.system
