.gfunc pg_initchart
.gsynop begin 
.if '&lang' eq 'C' .do begin
short _FAR _pg_initchart( void );
.do end
.el .do begin
integer*2 function _pg_initchart()
.do end
.gsynop end
.desc begin
The
.id &func.
&routine initializes the presentation graphics system.
This includes initializing the internal palette and style-set used
when drawing charts.
This &routine must be called before any of the other presentation graphics
&routines..
.pp
The initialization of the presentation graphics system requires that
a valid graphics mode has been selected.
For this reason the
.kw _setvideomode
&routine must be called before
.id &func.
is called.
If a font has been selected (with the
.kw _setfont
&routine), that font will be used when text is displayed in a chart.
Font selection should also be done before initializing
the presentation graphics system.
.desc end
.return begin
The
.id &func.
&routine returns zero
if successful; otherwise, a non-zero value is returned.
.return end
.see begin
.seelist _pg_defaultchart _pg_chart _pg_chartpie _pg_chartscatter _setvideomode _setfont _registerfonts
.see end
.grexam begin eg_pgc.&langsuff
.grexam end
.class &grfun
.system
