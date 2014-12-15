.gfunc settextwindow
.gsynop begin 
.if '&lang' eq 'C' .do begin
void _FAR _settextwindow( short row1, short col1,
                          short row2, short col2 );
.do end
.el .do begin
subroutine _settextwindow( row1, col1, row2, col2 )
integer*2 row1, col1
integer*2 row2, col2
.do end
.gsynop end
.desc begin
The
.id &funcb.
&routine sets the text window to be the rectangle with a top
left corner at
.coord row1 col1
and a bottom right corner at
.coord row2 col2 .
These coordinates are in terms of characters not pixels.
.pp
The initial text output position is
.coord 1 1 .
Subsequent text positions are reported (by the
.kw _gettextposition
&routine) and set (by the
.kw _outtext
.ct ,
.kw _outmem
and
.kw _settextposition
&routines) relative to this rectangle.
.pp
Text is displayed from the current output position for text
proceeding along the current row and then downwards.
When the window is full, the lines scroll upwards one line and
then text is displayed on the last line of the window.
.desc end
.if '&lang' eq 'C' .do begin
.return begin
The
.id &funcb.
&routine does not return a value.
.return end
.do end
.see begin
.seelist _gettextposition _outtext _outmem _settextposition
.see end
.grexam begin eg_settw.&langsuff
.grexam end
.class &grfun
.system
