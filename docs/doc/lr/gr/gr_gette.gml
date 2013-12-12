.gfunc gettextextent
.gsynop begin 
.if '&lang' eq 'C' .do begin
void _FAR _gettextextent( short x, short y,
                          char _FAR *text,
                          struct xycoord _FAR *concat,
                          struct xycoord _FAR *extent );
.do end
.el .do begin
subroutine _gettextextent( x, y, text, concat, extent )
integer*2 x, y
character*(*) text
record /xycoord/ concat
record /xycoord/ extent(4)
.do end
.gsynop end
.desc begin
The
.id &func.
&routine simulates the effect of using the
.kw _grtext
&routine to display the text string
.arg text
at the position
.coord x y ,
using the current text settings.
The concatenation point is returned in the argument
.arg concat
.ct .li .
The text extent parallelogram is returned in the array
.arg extent
.ct .li .
.pp
The concatenation point is the position to use to output text after
the given string.
The text extent parallelogram outlines the area where the text string
would be displayed.
The four points are returned in counter-clockwise order, starting
at the upper-left corner.
.desc end
.if '&lang' eq 'C' .do begin
.return begin
The
.id &func.
&routine does not return a value.
.return end
.do end
.see begin
.seelist &function. _grtext _gettextsettings
.see end
.grexam begin eg_gette.&langsuff
.grexam output
.picture eg_gette
.grexam end
.class &grfun
.system
