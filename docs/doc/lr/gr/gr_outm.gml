.gfunc _outmem
.gsynop begin
.if '&lang' eq 'C' .do begin
void _FAR _outmem( char _FAR *text, short length );
.do end
.el .do begin
subroutine _outmem( text, length )
character*(*) text
integer*2 length
.do end
.gsynop end
.desc begin
The
.id &funcb.
&routine displays the character string indicated by the argument
.arg text
.period
The argument
.arg length
specifies the number of characters to be displayed.
Unlike the
.reffunc _outtext
&routine,
.id &funcb.
will display the graphical representation of characters
such as ASCII 10 and 0, instead of interpreting them as control characters.
.np
The text is displayed using the current text color
.seeref _settextcolor
.ct , starting at the current text position
.seeref _settextposition
.period
The text position is updated to follow the end of the displayed text.
.im gr_text
.desc end
.if '&lang' eq 'C' .do begin
.return begin
The
.id &funcb.
&routine does not return a value.
.return end
.do end
.see begin
.seelist _settextcolor _settextposition _settextwindow _grtext _outtext _outgtext
.see end
.grexam begin eg_outm.&langsuff
.grexam end
.class &grfun
.system
