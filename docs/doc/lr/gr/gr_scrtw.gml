.gfunc scrolltextwindow
.gsynop begin 
.if '&lang' eq 'C' .do begin
void _FAR _scrolltextwindow( short rows );
.do end
.el .do begin
subroutine _scrolltextwindow( rows )
integer*2 rows
.do end
.gsynop end
.desc begin
The
.id &func.
&routine scrolls the lines in the current text window.
A text window is defined with the
.kw _settextwindow
&routine..
By default, the text window is the entire screen.
.np
The argument
.arg rows
specifies the number of rows to scroll.
A positive value means to scroll the text window up or towards the top
of the screen.
A negative value means to scroll the text window down or towards the bottom
of the screen.
Specifying a number of rows greater than the height of the text window
is equivalent to clearing the text window with the
.kw _clearscreen
&routine..
.np
Two constants are defined that can be used with the
.id &func.
&routine:
.begterm 15
.uterm GSCROLLUP
the contents of the text window are scrolled up (towards the top of the
screen) by one row
.uterm GSCROLLDOWN
the contents of the text window are scrolled down (towards the bottom of the
screen) by one row
.endterm
.desc end
.if '&lang' eq 'C' .do begin
.return begin
The
.id &func.
&routine does not return a value.
.return end
.do end
.see begin
.seelist &function. _settextwindow _clearscreen _outtext _outmem _settextposition
.see end
.grexam begin eg_scrtw.&langsuff
.grexam end
.class &grfun
.system
