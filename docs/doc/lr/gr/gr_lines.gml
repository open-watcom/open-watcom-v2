The line-style mask determines the style by which lines and arcs are
drawn.
The mask is treated as an array of 16 bits.
As a line is drawn, a pixel at a time, the bits in this array are
cyclically tested.
When a bit in the array is 1, the pixel value for the current point
is set using the current color according to the current plotting
action;
otherwise, the pixel
value for the point is left unchanged.
A solid line would result from a value of
.hex FFFF
and a dashed line would result from a value of
.hex F0F0 .
