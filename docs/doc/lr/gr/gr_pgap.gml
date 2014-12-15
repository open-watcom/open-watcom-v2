.gfunc pg_analyzepie
.gsynop begin 
.if '&lang' eq 'C' .do begin
short _FAR _pg_analyzepie( chartenv _FAR *env,
                           char _FAR * _FAR *cat,
                           float _FAR *values,
                           short _FAR *explode, short n );
.do end
.el .do begin
integer*2 function _pg_analyzepie( env, cat, values,
                                   explode, n )
record /chartenv/ env
integer*4 cat(*)
real values(*)
integer*2 explode(*), n
.do end
.gsynop end
.desc begin
The
.id &funcb.
&routine analyzes a pie chart.
This &routine calculates default values for chart elements without
actually displaying the chart.
.pp
The chart environment structure
.arg env
is filled with default values based on the values of the
.arg cat
.ct ,
.arg values
and
.arg explode
arguments.
The arguments are the same as for the
.kw _pg_chartpie
&routine..
.desc end
.return begin
The
.id &funcb.
&routine returns zero
if successful; otherwise, a non-zero value is returned.
.return end
.see begin
.seelist _pg_defaultchart _pg_initchart _pg_chart _pg_chartpie _pg_chartscatter _pg_analyzechart _pg_analyzescatter
.see end
.grexam begin eg_pgap.&langsuff
.grexam end
.class &grfun
.system
