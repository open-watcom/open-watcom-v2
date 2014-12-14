.gfuncms pg_analyzescatter
.gsynop begin
.if '&lang' eq 'C' .do begin
short _FAR _pg_analyzescatter( chartenv _FAR *env,
                               float _FAR *x,
                               float _FAR *y, short n );

short _FAR _pg_analyzescatterms(
                      chartenv _FAR *env,
                      float _FAR *x, float _FAR *y,
                      short nseries, short n, short dim,
                      char _FAR * _FAR *labels );
.do end
.el .do begin
integer*2 function _pg_analyzescatter( env, x, y, n )
record /chartenv/ env
real x(*), y(*)
integer*2 n

integer*2 function _pg_analyzescatterms( env, x, y,
                                         nseries, n,
                                         dim, labels )
record /chartenv/ env
real x(*), y(*)
integer*2 nseries, n, dim
integer*4 labels(*)
.do end
.gsynop end
.desc begin
The
.id &func.
&routines analyze either a single-series or a multi-series
scatter chart.
These &routines calculate default values for chart elements without
actually displaying the chart.
.pp
The
.id &func.
&routine analyzes a single-series scatter chart.
The chart environment structure
.arg env
is filled with default values based on the values
of the
.arg x
and
.arg y
arguments.
The arguments are the same as for the
.kw _pg_chartscatter
&routine..
.pp
The
.id &func1.
&routine analyzes a multi-series scatter chart.
The chart environment structure
.arg env
is filled with default values based on the values
of the
.arg x
.ct ,
.arg y
and
.arg labels
arguments.
The arguments are the same as for the
.kw _pg_chartscatterms
&routine..
.desc end
.return begin
The
.id &func.
&routines return zero
if successful; otherwise, a non-zero value is returned.
.return end
.see begin
.seelist _pg_defaultchart _pg_initchart _pg_chart _pg_chartpie _pg_chartscatter _pg_analyzechart _pg_analyzepie
.see end
.grexam begin eg_pgas.&langsuff
.grexam end
.class &grfun
.system
