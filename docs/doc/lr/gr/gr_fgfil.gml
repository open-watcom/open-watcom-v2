.np
The argument
.arg &*1
determines whether the &*2 is filled in
or has only its outline drawn.
The argument can have one of two values:
.begterm 15
.uterm GFILLINTERIOR
fill the interior by writing pixels with the current plot action
using the current color and the current fill mask
.uterm GBORDER
leave the interior unchanged; draw the outline of the figure
with the current plot action using the current color and line style
.endterm
