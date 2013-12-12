.gfunc pg_chartpie
.gsynop begin 
.if '&lang' eq 'C' .do begin
short _FAR _pg_chartpie( chartenv _FAR *env,
                         char _FAR * _FAR *cat,
                         float _FAR *values,
                         short _FAR *explode, short n );
.do end
.el .do begin
integer*2 function _pg_chartpie( env, cat, values, explode, n )
record /chartenv/ env
integer*4 cat(*)
real values(*)
integer*2 explode(*), n
.do end
.gsynop end
.desc begin
The
.id &func.
&routine displays a pie chart.
The chart is displayed using the options specified in the
.arg env
argument.
.pp
The pie chart is created from the data contained in the
.arg values
array.
The argument
.arg n
specifies the number of values to chart.
.pp
The argument
.arg cat
is an array of &strings..
These strings describe each of the pie slices and are used in the chart legend.
The argument
.arg explode
is an array of values corresponding to each of the pie slices.
For each non-zero element in the array, the corresponding pie slice
is drawn "exploded", or slightly offset from the rest of the pie.
.desc end
.return begin
The
.id &func.
&routine returns zero
if successful; otherwise, a non-zero value is returned.
.return end
.see begin
.seelist &function. _pg_defaultchart _pg_initchart _pg_chart _pg_chartscatter _pg_analyzechart _pg_analyzepie _pg_analyzescatter
.see end
.grexam begin eg_pgcp.&langsuff
.grexam output
.picture eg_pgcp
.grexam end
.class &grfun
.system
