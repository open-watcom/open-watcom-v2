.gfuncw getcurrentposition
.if '&lang' eq 'C' .do begin
struct xycoord _FAR _getcurrentposition( void );

struct _wxycoord _FAR _getcurrentposition_w( void );
.do end
.el .do begin
record /xycoord/ function _getcurrentposition()

record /_wxycoord/ function _getcurrentposition_w()
.do end
.gfuncend
.desc begin
The &func &routines return the current
output position for graphics.
The &func &routine returns the point in view coordinates.
The &func2 &routine returns the point in window coordinates.
.pp
The current position defaults to the origin,
.coord 0 0 ,
when a new video
mode is selected.
It is changed
by successful calls to the
.kw _arc
.ct ,
.kw _moveto
and
.kw _lineto
&routines as well as the
.kw _setviewport
&routine..
.pp
Note that the output position for graphics output differs from that
for text output.
The output position for text output can be set by use of the
.kw _settextposition
&routine..
.desc end
.return begin
The &func &routines return the current output
position for graphics.
.return end
.see begin
.seelist &function. _moveto _settextposition
.see end
.grexam begin eg_getcp.&langsuff
.grexam end
.class &grfun
.system
