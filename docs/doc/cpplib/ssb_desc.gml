:P.
The
:MONO.strstreambuf
class differs quite markedly from the
:MONO.filebuf
and
:MONO.stdiobuf
classes. Since there is no actual source or destination for the characters in
:MONO.strstream
objects, the buffer itself takes on that role. When writing is occurring and
the &putarea. is full, the
:MONO.overflow
virtual member function reallocates the buffer to a larger size (if possible),
the &putarea. is extended and the writing continues. If reading is occurring
and the &getarea. is empty, the
:MONO.underflow
virtual member function checks to see if the &putarea. is present and not
empty. If so, the &getarea. is extended to overlap the &putarea.:PERIOD.
