.gfunc settextrows
.gsynop begin 
.if '&lang' eq 'C' .do begin
short _FAR _settextrows( short rows );
.do end
.el .do begin
integer*2 function _settextrows( rows )
integer*2 rows
.do end
.gsynop end
.desc begin
The
.id &func.
&routine selects the number of rows of text displayed on the screen.
The number of rows is specified by the argument
.arg rows
.ct .li .
Computers equipped with EGA, MCGA and VGA adapters can support
different numbers of text rows.
The number of rows that can be selected depends on the current
video mode and the type of monitor attached.
.np
If the argument
.arg rows
has the value
.arg _MAXTEXTROWS
.ct , the maximum number of text rows will be selected for the current
video mode and hardware configuration.
In text modes the maximum number of rows is 43 for EGA adapters,
and 50 for MCGA and VGA adapters.
Some graphics modes will support 43 rows for EGA adapters and
60 rows for MCGA and VGA adapters.
.desc end
.return begin
The
.id &func.
&routine returns the number of screen rows when the number of
rows is set successfully; otherwise, zero is returned.
.return end
.see begin
.seelist _getvideoconfig _setvideomode _setvideomoderows
.see end
.grexam begin eg_settr.&langsuff
.grexam end
.class &grfun
.system
