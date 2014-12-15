.gfunc pg_setstyleset
.gsynop begin 
.if '&lang' eq 'C' .do begin
void _FAR _pg_setstyleset( unsigned short _FAR *style );
.do end
.el .do begin
subroutine _pg_setstyleset( style )
integer*2 style(*)
.do end
.gsynop end
.desc begin
The
.id &funcb.
&routine retrieves the internal style-set of the
presentation graphics system.
The style-set is a set of line styles used for drawing
window borders and grid-lines.
The argument
.arg style
is an array containing the new style-set.
.desc end
.if '&lang' eq 'C' .do begin
.return begin
The
.id &funcb.
&routine does not return a value.
.return end
.do end
.see begin
.seelist _pg_defaultchart _pg_initchart _pg_chart _pg_chartpie _pg_chartscatter _pg_getstyleset _pg_resetstyleset
.see end
.grexam begin eg_pggy.&langsuff
.grexam end
.class &grfun
.system
