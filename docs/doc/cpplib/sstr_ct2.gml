These forms of the &fn. create an initialized &obj.:PERIOD.
Dynamic allocation is not used. The buffer is specified by the
:ARG.str
and
:ARG.len
parameters. If the
:MONO.ios::append
or
:MONO.ios::atend
bits are set in the
:ARG.mode
parameter, the
:ARG.str
parameter is assumed to contain a C string terminated by a null character,
and writing commences at the null character. Otherwise, writing commences
at
:ARG.str
:PERIOD.
.if '&cl_name' = 'strstream' .do begin
Reading commences at
:ARG.str
:PERIOD.
.do end
:RSLTS.
This form of the &fn. creates an initialized &obj.:PERIOD.
