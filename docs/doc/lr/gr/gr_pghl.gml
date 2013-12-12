.gfunc pg_hlabelchart
.gsynop begin 
.if '&lang' eq 'C' .do begin
short _FAR _pg_hlabelchart( chartenv _FAR *env,
                            short x, short y,
                            short color,
                            char _FAR *label );
.do end
.el .do begin
integer*2 function _pg_hlabelchart( env, x, y,
                                    color, label )
record /chartenv/ env
integer*2 x, y, color
character*(*) label
.do end
.gsynop end
.desc begin
The
.id &func.
&routine displays the text string
.arg label
on the chart described by the
.arg env
chart structure.
The string is displayed horizontally starting at the point
.coord x y ,
relative to the upper left corner of the chart.
The
.arg color
specifies the palette color used to display the string.
.desc end
.return begin
The
.id &func.
&routine returns zero
if successful; otherwise, a non-zero value is returned.
.return end
.see begin
.seelist &function. _pg_defaultchart _pg_initchart _pg_chart _pg_chartpie _pg_chartscatter _pg_vlabelchart
.see end
.grexam begin eg_pghl.&langsuff
.grexam end
.class &grfun
.system
