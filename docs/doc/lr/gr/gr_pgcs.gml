.gfuncms pg_chartscatter
.if '&lang' eq 'C' .do begin
short _FAR _pg_chartscatter( chartenv _FAR *env,
                             float _FAR *x,
                             float _FAR *y, short n );

short _FAR _pg_chartscatterms( chartenv _FAR *env,
                               float _FAR *x,
                               float _FAR *y,
                               short nseries,
                               short n, short dim,
                               char _FAR * _FAR *labels );
.do end
.el .do begin
integer*2 function _pg_chartscatter( env, x, y, n )
record /chartenv/ env
float x(*), y(*)
integer*2 n

integer*2 function _pg_chartscatterms( env, x, y, nseries,
                                       n, dim, labels )
record /chartenv/ env
real x(*), y(*)
integer*2 nseries, n, dim
integer*4 labels(*)
.do end
.gfuncend
.desc begin
The &func &routines display either a single-series or a multi-series
scatter chart.
The chart is displayed using the options specified in the
.arg env
argument.
.pp
The &func &routine displays a scatter chart from the single series
of data contained in the arrays
.arg x
and
.arg y
.ct .li .
The argument
.arg n
specifies the number of values to chart.
.pp
The &func2 &routine displays a multi-series scatter chart.
The argument
.arg nseries
specifies the number of series of data to chart.
The arguments
.arg x
and
.arg y
are assumed to be two-dimensional arrays defined as follows:
.millust begin
.if '&lang' eq 'C' .do begin
float x[ nseries ][ dim ];
.do end
.el .do begin
real x( nseries, dim )
.do end
.millust end
.pp
The number of values used from each series is given by the argument
.arg n
.ct , where
.arg n
is less than or equal to
.arg dim
.ct .li .
The argument
.arg labels
is an array of &strings..
These strings describe each of the series and are used in the chart legend.
.desc end
.return begin
The &func &routines return zero
if successful; otherwise, a non-zero value is returned.
.return end
.see begin
.seelist &function. _pg_defaultchart _pg_initchart _pg_chart _pg_chartpie _pg_analyzechart _pg_analyzepie _pg_analyzescatter
.see end
.grexam begin eg_pgcs.&langsuff
.grexam output
.picture eg_pgcs
.grexam end
.class &grfun
.system
