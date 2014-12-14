.gfuncw floodfill
.gsynop begin 
.if '&lang' eq 'C' .do begin
short _FAR _floodfill( short x, short y,
                       short stop_color );

short _FAR _floodfill_w( double x, double y,
                         short stop_color );
.do end
.el .do begin
integer*2 function _floodfill( x, y, stop_color )
integer*2 x, y
integer*2 stop_color

integer*2 function _floodfill_w( x, y, stop_color )
double precision x, y
integer*2 stop_color
.do end
.gsynop end
.desc begin
The
.id &func.
&routines fill an area of the screen.
The
.id &func.
&routine uses the view coordinate system.
The
.id &func2.
&routine uses the window coordinate system.
.np
The filling starts at the point
.coord x y
and continues in all directions: when a pixel is filled, the
neighbouring pixels (horizontally and vertically) are then considered
for filling.
Filling is done using the current color and fill mask.
No filling will occur if the point
.coord x y
lies outside the clipping region.
.pp
If the argument
.arg stop_color
is a valid pixel value, filling will occur in each direction until a
pixel is encountered with a pixel value of
.arg stop_color
.ct .li .
The filled area will be the area around
.coord x y ,
bordered by
.arg stop_color
.ct .li .
No filling will occur if the point
.coord x y
has the pixel value
.arg stop_color
.ct .li .
.pp
If
.arg stop_color
has the value (-1), filling occurs until a pixel is encountered
with a pixel value different from the pixel value of the starting point
.coord x y .
No filling will occur if the pixel value of the point
.coord x y
is the current color.
.* .pp
.* .im gr_pixvl
.* .pp
.* .im gr_fillm
.desc end
.return begin
The
.id &func.
&routines return zero
when no filling takes place; a non-zero
value is returned to indicate that filling has occurred.
.return end
.see begin
.seelist _setcliprgn _setcolor _setfillmask _setplotaction
.see end
.grexam begin eg_flood.&langsuff
.grexam end
.class &grfun
.system
