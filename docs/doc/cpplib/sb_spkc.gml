The &fn. returns the next available character in the &getarea.:PERIOD.
The &getptr. is not advanced. If the &getarea. is empty, the
:MONO.underflow
virtual member function is called to fetch more characters from the
source into the &getarea.:PERIOD.
:P.
Due to the
:MONO.sgetc
member function's confusing name (the C library
:MONO.getc
function does advance the pointer), the
:MONO.speekc
member function was added to take its place in the WATCOM implementation.
:RSLTS.
The &fn. returns the next available character in the &getarea.:PERIOD.
If no character is available, &eof. is returned.
