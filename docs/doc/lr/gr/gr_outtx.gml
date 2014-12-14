.gfunc outtext
.gsynop begin 
.if '&lang' eq 'C' .do begin
void _FAR _outtext( char _FAR *text );
.do end
.el .do begin
subroutine _outtext( text )
character*(*) text
.do end
.gsynop end
.desc begin
The
.id &func.
&routine displays the character string indicated by the argument
.arg text
.ct .li .
The string must be terminated by a null character
.if '&lang' eq 'C' .do begin
('\0').
.do end
.el .do begin
(char(0)).
.do end
When a line-feed character
.if '&lang' eq 'C' .do begin
('\n')
.do end
.el .do begin
(char(10))
.do end
is encountered in the string, the characters following will be
displayed on the next row of the screen.
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
The
.id &func.
&routine does not return a value.
.return end
.do end
.see begin
.seelist _settextcolor _settextposition _settextwindow _grtext _outmem _outgtext
.see end
.grexam begin eg_outtx.&langsuff
.grexam end
.class &grfun
.system
