.gfunc gettextwindow
.gsynop begin 
.if '&lang' eq 'C' .do begin
void _FAR _gettextwindow(
                short _FAR *row1, short _FAR *col1,
                short _FAR *row2, short _FAR *col2 );
.do end
.el .do begin
subroutine _gettextwindow( row1, col1, row2, col2 )
integer*2 row1, col1
integer*2 row2, col2
.do end
.gsynop end
.desc begin
The
.id &funcb.
&routine returns the location of the current text window.
A text window is defined with the
.kw _settextwindow
&routine..
By default, the text window is the entire screen.
.np
The current text window is a rectangular area of the screen.
Text display is restricted to be within this window.
The top left corner of the text window is placed in the arguments
.coord row1 col1 .
The bottom right corner of the text window is placed in
.coord row2 col2 .
.desc end
.if '&lang' eq 'C' .do begin
.return begin
The
.id &funcb.
&routine returns the location of the current text window.
.return end
.do end
.see begin
.seelist _settextwindow _outtext _outmem _settextposition _scrolltextwindow
.see end
.grexam begin eg_settw.&langsuff
.grexam end
.class &grfun
.system
