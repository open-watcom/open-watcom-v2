.np
With these shift functions,
.id n
must be a non-negative integer.
In an arithmetic shift,
.id j
is considered a signed integer.
In the case of a left shift, zeros are shifted into the right
and bits shifted out of the left are lost.
In the case of a right shift, zeros are shifted into the left if
.id j
is positive and ones if
.id j
is negative.
Bits shifted out of the right are lost.
.np
If
.id n
is equal to 0, no shift is performed.
Note that the arguments are not modified.
.np
These functions are compiled as in-line code unless they are passed as
arguments.
