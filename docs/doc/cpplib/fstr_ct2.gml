This form of the &fn. creates an &obj. that is connected to the file
specified by the
:ARG.name
parameter, using the specified
:ARG.mode
and
:ARG.prot
parameters.
The connection is made via the C library
:MONO.open
function.
:RSLTS.
The &fn. produces an &obj. that is connected to the file specified by
:ARG.name
:PERIOD.
If the
:MONO.open
fails, &failbit. and &badbit. are set in the &errstate.:PERIOD.
