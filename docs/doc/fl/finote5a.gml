ICHAR returns an integer which describes the position of the character
in the processor collating sequence.
The first character in the collating sequence is in position 0 and the
last character of the collating sequence is in position n&minus.1
where n is the number of characters in the collating sequence.
The value of ICHAR(X) for X a character of length one is such that 0
<= ICHAR(X) <= n&minus.1.
For any characters X and Y, the following holds true.
.autopoint
.point
X .LT. Y if and only if ICHAR(X) .LT. ICHAR(Y)
.point
X .EQ. Y if and only if ICHAR(X) .EQ. ICHAR(Y)
.endpoint
.np
CHAR is the inverse of ICHAR.
