The fill mask is an eight-byte array which
is interpreted as a square pattern (8 by 8) of 64 bits.
Each bit in the mask corresponds to a pixel.
When a region is filled, each point in the region
is mapped onto the fill mask.
When a bit from the mask is one, the pixel value of the corresponding
point is set using the current plotting action with the current color;
when the bit is zero, the
pixel value of that point is not affected.
.pp
When the fill mask is not set,
a fill operation will set all points in the fill region
to have a pixel value of the current color.
