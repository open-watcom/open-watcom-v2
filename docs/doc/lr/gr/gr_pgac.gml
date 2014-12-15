.gfuncms pg_analyzechart
.gsynop begin
.if '&lang' eq 'C' .do begin
short _FAR _pg_analyzechart( chartenv _FAR *env,
                             char _FAR * _FAR *cat,
                             float _FAR *values, short n );

short _FAR _pg_analyzechartms( chartenv _FAR *env,
                               char _FAR * _FAR *cat,
                               float _FAR *values,
                               short nseries,
                               short n, short dim,
                               char _FAR * _FAR *labels );
.do end
.el .do begin
integer*2 function _pg_analyzechart( env, cat, values, n )
record /chartenv/ env
integer*4 cat(*)
real values(*)
integer*2 n

integer*2 function _pg_analyzechartms( env, cat, values,
                                       nseries, n,
                                       dim, labels )
record /chartenv/ env
integer*4 cat(*)
real values(*)
integer*2 nseries, n, dim
integer*4 labels(*)
.do end
.gsynop end
.desc begin
The
.id &funcb.
&routines analyze either a single-series or a multi-series
bar, column or line chart.
These &routines calculate default values for chart elements without
actually displaying the chart.
.pp
The
.id &funcb.
&routine analyzes a single-series bar, column or line chart.
The chart environment structure
.arg env
is filled with default values based on the type of chart and the values
of the
.arg cat
and
.arg values
arguments.
The arguments are the same as for the
.kw _pg_chart
&routine..
.pp
The
.id &func1.
&routine analyzes a multi-series bar, column or line chart.
The chart environment structure
.arg env
is filled with default values based on the type of chart and the values
of the
.arg cat
.ct ,
.arg values
and
.arg labels
arguments.
The arguments are the same as for the
.kw _pg_chartms
&routine..
.desc end
.return begin
The
.id &funcb.
&routines return zero
if successful; otherwise, a non-zero value is returned.
.return end
.see begin
.seelist _pg_defaultchart _pg_initchart _pg_chart _pg_chartpie _pg_chartscatter _pg_analyzepie _pg_analyzescatter
.see end
.grexam begin eg_pgac.&langsuff
.grexam end
.class &grfun
.system
