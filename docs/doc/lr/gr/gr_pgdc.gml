.gfunc pg_defaultchart
.gsynop begin 
.if '&lang' eq 'C' .do begin
short _FAR _pg_defaultchart( chartenv _FAR *env,
                             short type, short style );
.do end
.el .do begin
integer*2 function _pg_defaultchart( env, type, style )
record /chartenv/ env
integer*2 type, style
.do end
.gsynop end
.desc begin
The
.id &func.
&routine initializes the chart structure
.arg env
to contain default values before a chart is drawn.
All values in the chart structure are initialized, including
blanking of all titles.
The chart type in the structure is initialized to the value
.arg type
.ct , and the chart style is initialized to
.arg style
.ct .li .
.pp
The argument
.arg type
can have one of the following values:
.begterm 20
.uterm PG_BARCHART
Bar chart (horizontal bars)
.uterm PG_COLUMNCHART
Column chart (vertical bars)
.uterm PG_LINECHART
Line chart
.uterm PG_SCATTERCHART
Scatter chart
.uterm PG_PIECHART
Pie chart
.endterm
.pp
Each type of chart can be drawn in one of two styles.
For each chart type the argument
.arg style
can have one of the following values:
.uindex PG_PLAINBARS
.uindex PG_STACKEDBARS
.uindex PG_POINTANDLINE
.uindex PG_POINTONLY
.uindex PG_PERCENT
.uindex PG_NOPERCENT
.millust begin
Type            Style 1                 Style 2

Bar             _PG_PLAINBARS           _PG_STACKEDBARS
Column          _PG_PLAINBARS           _PG_STACKEDBARS
Line            _PG_POINTANDLINE        _PG_POINTONLY
Scatter         _PG_POINTANDLINE        _PG_POINTONLY
Pie             _PG_PERCENT             _PG_NOPERCENT
.millust end
.pp
For single-series bar and column charts, the chart style is ignored.
The "plain" (clustered) and "stacked" styles only apply when there is more than one
series of data.
The "percent" style for pie charts causes percentages to be displayed
beside each of the pie slices.
.desc end
.return begin
The
.id &func.
&routine returns zero
if successful; otherwise, a non-zero value is returned.
.return end
.see begin
.seelist &function. _pg_initchart _pg_chart _pg_chartpie _pg_chartscatter
.see end
.grexam begin eg_pgc.&langsuff
.grexam end
.class &grfun
.system
