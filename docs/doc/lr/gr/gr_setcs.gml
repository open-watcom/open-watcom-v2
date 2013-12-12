.gfuncw setcharsize
.gsynop begin 
.if '&lang' eq 'C' .do begin
void _FAR _setcharsize( short height, short width );

void _FAR _setcharsize_w( double height, double width );
.do end
.el .do begin
subroutine _setcharsize( height, width )
integer*2 height, width

subroutine _setcharsize_w( height, width )
double precision height, width
.do end
.gsynop end
.desc begin
The
.id &func.
&routines set the character height and width to the values
specified by the arguments
.arg height
and
.arg width
.ct .li .
For the
.id &func.
&routine, the arguments
.arg height
and
.arg width
represent a number of pixels.
For the
.id &func2.
&routine, the arguments
.arg height
and
.arg width
represent lengths along the y-axis and x-axis in the window coordinate system.
.np
These sizes are used when displaying text with the
.kw _grtext
&routine..
The default character sizes are dependent on the graphics mode selected,
and can be determined by the
.kw _gettextsettings
&routine..
.desc end
.if '&lang' eq 'C' .do begin
.return begin
The
.id &func.
&routines do not return a value.
.return end
.do end
.see begin
.seelist &function. _grtext _gettextsettings
.see end
.grexam begin eg_getts.&langsuff
.grexam output
.picture eg_getts
.grexam end
.class &grfun
.system
