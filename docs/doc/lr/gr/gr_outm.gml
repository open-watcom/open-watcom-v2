.gfunc outmem
.if '&lang' eq 'C' .do begin
void _FAR _outmem( char _FAR *text, short length );
.do end
.el .do begin
subroutine _outmem( text, length )
character*(*) text
integer*2 length
.do end
.gfuncend
.desc begin
The &func &routine displays the character string indicated by the argument
.arg text
.ct .li .
The argument
.arg length
specifies the number of characters to be displayed.
Unlike the
.kw _outtext
&routine, &func will display the graphical representation of characters
such as ASCII 10 and 0, instead of interpreting them as control characters.
.np
The text is displayed using the current text color (see the
.kw _settextcolor
&routine), starting at the current text position (see the
.kw _settextposition
&routine).
The text position is updated to follow the end of the displayed text.
.im gr_text
.desc end
.if '&lang' eq 'C' .do begin
.return begin
The &func &routine does not return a value.
.return end
.do end
.see begin
.seelist &function. _settextcolor _settextposition _settextwindow _grtext _outtext _outgtext
.see end
.grexam begin eg_outm.&langsuff
.grexam end
.class &grfun
.system
