.gfunc setvideomoderows
.if '&lang' eq 'C' .do begin
short _FAR _setvideomoderows( short mode, short rows );
.do end
.el .do begin
integer*2 function _setvideomoderows( mode, rows )
integer*2 mode
integer*2 rows
.do end
.gfuncend
.desc begin
The &func &routine selects a video mode and the number of rows
of text displayed on the screen.
The video mode is specified by the argument
.arg mode
and is selected with the
.kw _setvideomode
&routine..
The number of rows is specified by the argument
.arg rows
and is selected with the
.kw _settextrows
&routine..
.np
Computers equipped with EGA, MCGA and VGA adapters can support
different numbers of text rows.
The number of rows that can be selected depends on the
video mode and the type of monitor attached.
.desc end
.return begin
The &func &routine returns the number of screen rows when the
mode and number of rows are set successfully; otherwise, zero is returned.
.return end
.see begin
.seelist &function. _getvideoconfig _setvideomode _settextrows
.see end
.grexam begin eg_setvr.&langsuff
.grexam end
.class &grfun
.system
