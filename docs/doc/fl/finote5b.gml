CHAR returns the character in the i'th position of the processor
collating sequence.
The first character in the collating sequence is in position 0 and the
last character of the collating sequence is in position n&minus.1
where n is the number of characters in the collating sequence.
The value of CHAR(I) is of type CHARACTER of length one.
The argument I must be in the range 0 <= I <= n&minus.1.
.np
ICHAR is the inverse of CHAR.
