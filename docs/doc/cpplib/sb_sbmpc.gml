The &fn. extracts the next available character from the &getarea. and advances
the &getptr.
.dot
If no character is available, it calls the
:MONO.underflow
virtual member function to fetch more characters from the source into the
&getarea.
.dot
:P.
Due to the
:MONO.sbumpc
member functions's awkward name, the
:MONO.sgetchar
member function was added to take its place in the WATCOM implementation.
:RSLTS.
The &fn. returns the next available character in the &getarea.
.dot
If no character is available, &eof. is returned.
