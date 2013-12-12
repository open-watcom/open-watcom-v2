.gfunc wrapon
.gsynop begin 
.if '&lang' eq 'C' .do begin
short _FAR _wrapon( short wrap );
.do end
.el .do begin
integer*2 function _wrapon( wrap )
integer*2 wrap
.do end
.gsynop end
.desc begin
The
.id &func.
&routine is used to control the display of text when
the text output reaches the right side of the text window.
This is text displayed with the
.kw _outtext
and
.kw _outmem
&routines..
The
.arg wrap
argument can take one of the following values:
.begterm 15
.uterm GWRAPON
causes lines to wrap at the window border
.uterm GWRAPOFF
causes lines to be truncated at the window border
.endterm
.desc end
.return begin
The
.id &func.
&routine returns the previous setting for wrapping.
.return end
.see begin
.seelist &function. _outtext _outmem _settextwindow
.see end
.grexam begin eg_wrap.&langsuff
.grexam end
.class &grfun
.system
