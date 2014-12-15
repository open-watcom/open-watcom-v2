.gfunc displaycursor
.gsynop begin 
.if '&lang' eq 'C' .do begin
short _FAR _displaycursor( short mode );
.do end
.el .do begin
integer*2 function _displaycursor( mode )
integer*2 mode
.do end
.gsynop end
.desc begin
The
.id &funcb.
&routine is used to establish whether the text cursor is to be
displayed when graphics &routines complete.
On entry to a graphics &routine, the text cursor is turned off.
When the &routine completes, the
.arg mode
setting determines whether the cursor is turned back on.
The
.arg mode
argument can have one of the following values:
.begterm 15
.uterm GCURSORON
the cursor will be displayed
.uterm GCURSOROFF
the cursor will not be displayed
.endterm
.desc end
.return begin
The
.id &funcb.
&routine returns the previous setting for
.arg mode
.ct .li .
.return end
.see begin
.seelist _gettextcursor _settextcursor
.see end
.grexam begin eg_disp.&langsuff
.grexam end
.class &grfun
.system
