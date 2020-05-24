:P.
The
:ARG.str
:CONT.,
:ARG.len
and
:ARG.pstart
parameters are interpreted as follows:
:OL.
:LI.The buffer starts at
:ARG.str
:PERIOD.
:LI.If
:ARG.len
is positive, the buffer is
:ARG.len
characters long.
:LI.If
:ARG.len
is zero,
:ARG.str
is a pointer to a C string which is terminated by a null character, and the
length of the buffer is the length of the string.
:LI.If
:ARG.len
is negative, the buffer is unbounded.
This last form should be used with extreme caution, since no buffer is truly
unlimited in size and it would be easy to write beyond the available space.
:LI.If the
:ARG.pstart
parameter is &null., the &obj. is read-only.
:LI.
Otherwise,
:ARG.pstart
divides the buffer into two regions. The &getarea. starts at
:ARG.str
and ends at
:ARG.pstart
:CONT.-1.
The &putarea. starts at
:ARG.pstart
and goes to the end of the buffer.
:eOL.
