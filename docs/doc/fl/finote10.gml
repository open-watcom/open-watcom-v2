There are three shift operations: logical, arithmetic and circular.
These shift operations are implemented as integer functions having
two arguments.
The first argument,
.id j,
is the value to be shifted and the second argument,
.id n,
is the number of bits to shift.
If
.id n
is less than 0, a right shift is performed.
If
.id n
is greater than 0, a left shift is performed.
If
.id n
is equal to 0, no shift is performed.
Note that the arguments are not modified.
.np
In a logical shift, bits shifted out from the left or right are lost.
Zeros are shifted in from the opposite end.
.np
In an arithmetic shift,
.id j
is considered a signed integer.
In the case of a right shift, zeros are shifted into the left if
.id j
is positive and ones if
.id j
is negative.
Bits shifted out of the right are lost.
In the case of a left shift, zeros are shifted into the right
and bits shifted out of the left are lost.
.np
In a circular shift, bits shifted out one end are shifted into the
opposite end.
No bits are lost.
