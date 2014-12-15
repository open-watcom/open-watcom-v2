.gfunc pg_resetpalette
.gsynop begin 
.if '&lang' eq 'C' .do begin
short _FAR _pg_resetpalette( void );
.do end
.el .do begin
integer*2 function _pg_resetpalette()
.do end
.gsynop end
.desc begin
The
.id &funcb.
&routine resets the internal palette of the presentation
graphics system to default values.
The palette controls the colors, line styles, fill patterns
and plot characters used to display each series of data in a chart.
The default palette chosen is dependent on the current video mode.
.desc end
.return begin
The
.id &funcb.
&routine returns zero
if successful; otherwise, a non-zero value is returned.
.return end
.see begin
.seelist _pg_defaultchart _pg_initchart _pg_chart _pg_chartpie _pg_chartscatter _pg_getpalette _pg_setpalette
.see end
.grexam begin eg_pggp.&langsuff
.grexam end
.class &grfun
.system
