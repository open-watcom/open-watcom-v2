.gfunc pg_getchardef
.gsynop begin 
.if '&lang' eq 'C' .do begin
short _FAR _pg_getchardef( short ch,
                           unsigned char _FAR *def );
.do end
.el .do begin
integer*2 function _pg_getchardef( ch, def )
integer*2 ch
integer*1 def(8)
.do end
.gsynop end
.desc begin
The
.id &func.
&routine retrieves the current bit-map definition
for the character
.arg ch
.ct .li .
The bit-map is placed in the array
.arg def
.ct .li .
The current font must be an 8-by-8 bit-mapped font.
.desc end
.return begin
The
.id &func.
&routine returns zero
if successful; otherwise, a non-zero value is returned.
.return end
.see begin
.seelist &function. _pg_defaultchart _pg_initchart _pg_chart _pg_chartpie _pg_chartscatter _pg_setchardef
.see end
.grexam begin eg_pggc.&langsuff
.grexam end
.class &grfun
.system
