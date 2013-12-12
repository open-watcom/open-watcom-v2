.gfunc gettextcursor
.gsynop begin 
.if '&lang' eq 'C' .do begin
short _FAR _gettextcursor( void );
.do end
.el .do begin
integer*2 function _gettextcursor()
.do end
.gsynop end
.desc begin
The
.id &func.
&routine returns the current cursor attribute, or shape.
The cursor shape is set with the
.kw _settextcursor
&routine..
See the
.kw _settextcursor
&routine for a description of the value returned by the
.id &func.
&routine..
.desc end
.return begin
The
.id &func.
&routine returns the current cursor shape
when successful; otherwise, (-1) is returned.
.return end
.see begin
.seelist &function. _settextcursor _displaycursor
.see end
.grexam begin eg_gettu.&langsuff
.grexam end
.class &grfun
.system
