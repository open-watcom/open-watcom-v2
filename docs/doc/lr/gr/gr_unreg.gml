.gfunc unregisterfonts
.gsynop begin 
.if '&lang' eq 'C' .do begin
void _FAR _unregisterfonts( void );
.do end
.el .do begin
subroutine _unregisterfonts()
.do end
.gsynop end
.desc begin
The
.id &func.
&routine frees the memory previously allocated by the
.kw _registerfonts
&routine..
The currently selected font is also unloaded.
.pp
Attempting to use the
.kw _setfont
&routine after calling
.id &func.
will result in an error.
.desc end
.if '&lang' eq 'C' .do begin
.return begin
The
.id &func.
&routine does not return a value.
.return end
.do end
.see begin
.seelist &function. _registerfonts _setfont _getfontinfo _outgtext _getgtextextent _setgtextvector _getgtextvector
.see end
.grexam begin eg_reg.&langsuff
.grexam end
.class &grfun
.system
