.gfuncms pg_chart
.if '&lang' eq 'C' .do begin
short _FAR _pg_chart( chartenv _FAR *env,
                      char _FAR * _FAR *cat,
                      float _FAR *values, short n );

short _FAR _pg_chartms( chartenv _FAR *env,
                        char _FAR * _FAR *cat,
                        float _FAR *values, short nseries,
                        short n, short dim,
                        char _FAR * _FAR *labels );
.do end
.el .do begin
integer*2 function _pg_chart( env, cat, values, n )
record /chartenv/ env
integer*4 cat(*)
real values(*)
integer*2 n

integer*2 function _pg_chartms( env, cat, values, nseries,
                                n, dim, labels )
record /chartenv/ env
integer*4 cat(*)
real values(*)
integer*2 nseries, n, dim
integer*4 labels(*)
.do end
.gfuncend
.desc begin
The &func &routines display either a single-series or a multi-series
bar, column or line chart.
The type of chart displayed and other chart options are contained in the
.arg env
argument.
The argument
.arg cat
is an array of &strings..
These strings describe the categories against which
the data in the
.arg values
array is charted.
.pp
The &func &routine displays a bar, column or line chart from the single series
of data contained in the
.arg values
array.
The argument
.arg n
specifies the number of values to chart.
.pp
The &func2 &routine displays a multi-series bar, column or line chart.
The argument
.arg nseries
specifies the number of series of data to chart.
The argument
.arg values
is assumed to be a two-dimensional array defined as follows:
.millust begin
.if '&lang' eq 'C' .do begin
float values[ nseries ][ dim ];
.do end
.el .do begin
real values( nseries, dim )
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
.seelist &function. _pg_defaultchart _pg_initchart _pg_chartpie _pg_chartscatter _pg_analyzechart _pg_analyzepie _pg_analyzescatter
.see end
.grexam begin eg_pgc.&langsuff
.grexam output
.picture eg_pgc
.grexam end
.class &grfun
.system
