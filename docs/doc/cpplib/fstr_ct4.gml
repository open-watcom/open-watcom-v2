This form of the &fn. creates an &obj. that is connected to the file
specified by the
:ARG.hdl
parameter.
The buffer specified by the
:ARG.buf
and
:ARG.len
parameters is offered to the associated
:MONO.filebuf
object via the
:MONO.setbuf
member function. If the
:ARG.buf
parameter is &null. or the
:ARG.len
is less than or equal to zero, the
:MONO.filebuf
is unbuffered, so that each read or write
operation reads or writes a single character at a time.
:RSLTS.
The &fn. produces an &obj. that is attached to
:ARG.hdl
:PERIOD.
If the connection to
:ARG.hdl
fails, &failbit. and &badbit. are set in the &errstate.:PERIOD.
If the
:MONO.setbuf
fails, &failbit. is set in the &errstate.:PERIOD.
