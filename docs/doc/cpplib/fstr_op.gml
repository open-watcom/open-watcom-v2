The &fn. connects the &obj. to the file specified by the
:ARG.name
parameter, using the specified
:ARG.mode
and
:ARG.prot
parameters.
The
:MONO.mode
parameter is optional and usually is not specified unless
additional bits (such as
:MONO.ios::binary
or
:MONO.ios::text
:CONT.) are to be specified.
The connection is made via the C library
:MONO.open
function.
:RSLTS.
If the open fails, &failbit. is set in the &errstate.:PERIOD.
