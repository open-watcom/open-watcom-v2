.gfunc getarcinfo
.if '&lang' eq 'C' .do begin
short _FAR _getarcinfo( struct xycoord _FAR *start_pt,
                        struct xycoord _FAR *end_pt,
                        struct xycoord _FAR *inside_pt );
.do end
.el .do begin
integer*2 function _getarcinfo( start_pt, end_pt,
                                inside_pt )
record /xycoord/ start_pt
record /xycoord/ end_pt
record /xycoord/ inside_pt
.do end
.gfuncend
.desc begin
The &func &routine returns information about the arc most recently
drawn by the
.kw _arc
or
.kw _pie
&routines..
The arguments
.arg start_pt
and
.arg end_pt
are set to contain the endpoints of the arc.
The argument
.arg inside_pt
will contain the coordinates of a point within the pie.
The points are all specified in the view coordinate system.
.np
The endpoints of the arc can be used to connect other lines to the arc.
The interior point can be used to fill the pie.
.desc end
.return begin
The &func &routine returns a non-zero value when successful.
If the previous arc or pie was not successfully drawn, zero is returned.
.return end
.see begin
.seelist &function. _arc _pie
.see end
.grexam begin eg_getai.&langsuff
.grexam output
.picture eg_getai
.grexam end
.class &grfun
.system
