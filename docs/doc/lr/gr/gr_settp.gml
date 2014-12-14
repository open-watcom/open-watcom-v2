.gfunc settextposition
.gsynop begin 
.if '&lang' eq 'C' .do begin
struct rccoord _FAR _settextposition( short row,
                                      short col );
.do end
.el .do begin
record /rccoord/ function _settextposition( row, col )
integer*2 row, col
.do end
.gsynop end
.desc begin
The
.id &func.
&routine sets the current output position for text to be
.coord row col
where this position is in terms of characters, not pixels.
.np
The text position is relative to the current text window.
It defaults to the top left corner of the screen,
.coord 1 1 ,
when a new video mode is selected, or when a new text window is set.
The position is updated as text is drawn with the
.kw _outtext
and
.kw _outmem
&routines..
.np
Note that the output position for graphics output differs from that
for text output.
The output position for graphics output can be set by use of the
.kw _moveto
&routine..
.if '&lang' eq 'C' .do begin
.np
Also note that output to the standard output file,
.kw stdout
.ct , is line buffered by default.
It may be necessary to flush the output stream using
.mono fflush( stdout )
after a
.kw printf
call if your output does not contain a newline character.
Mixing of calls to
.kw _outtext
and
.kw printf
may cause overlapped text since
.kw _outtext
uses the output position that was set by &func..
.do end
.desc end
.return begin
The
.id &func.
&routine returns, as an
.id rccoord
structure, the previous output position for text.
.return end
.see begin
.seelist _gettextposition _outtext _outmem _settextwindow _moveto
.see end
.grexam begin eg_gettp.&langsuff
.grexam end
.class &grfun
.system
