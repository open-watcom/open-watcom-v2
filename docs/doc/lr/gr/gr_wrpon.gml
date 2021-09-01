.gfunc _wrapon
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
.id &funcb.
&routine is used to control the display of text when
the text output reaches the right side of the text window.
This is text displayed with the
.reffunc _outtext
and
.reffunc _outmem
&routines..
The
.arg wrap
argument can take one of the following values:
.begterm 15
.uterm _GWRAPON
causes lines to wrap at the window border
.uterm _GWRAPOFF
causes lines to be truncated at the window border
.endterm
.desc end
.return begin
The
.id &funcb.
&routine returns the previous setting for wrapping.
.return end
.see begin
.seelist _outtext _outmem _settextwindow
.see end
.grexam begin eg_wrap.&langsuff
.grexam end
.class &grfun
.system
