.gfunc pg_resetstyleset
.if '&lang' eq 'C' .do begin
void _FAR _pg_resetstyleset( void );
.do end
.el .do begin
subroutine _pg_resetstyleset()
.do end
.gfuncend
.desc begin
The &func &routine resets the internal style-set of the
presentation graphics system to default values.
The style-set is a set of line styles used for drawing
window borders and grid-lines.
.desc end
.if '&lang' eq 'C' .do begin
.return begin
The &func &routine does not return a value.
.return end
.do end
.see begin
.seelist &function. _pg_defaultchart _pg_initchart _pg_chart _pg_chartpie _pg_chartscatter _pg_getstyleset _pg_setstyleset
.see end
.grexam begin eg_pggy.&langsuff
.grexam end
.class &grfun
.system
