:P.
The
:ARG.dir
parameter may be
:MONO.ios::beg
:CONT.,
:MONO.ios::cur
:CONT., or
:MONO.ios::end
and is interpreted in conjunction with the
:ARG.offset
parameter as follows:
.sk
.fo off
.tb set ^
:SF font=4.ios::beg:eSF.^the :HP1.offset:eHP1. is relative to the start and should be a positive value.
:SF font=4.ios::cur:eSF.^the :HP1.offset:eHP1. is relative to the current position and may be positive
^^(seek towards end) or negative (seek towards start).
:SF font=4.ios::end:eSF.^the :HP1.offset:eHP1. is relative to the end and should be a negative value.
.co &cpp_co.
.ju &cpp_ju.
.tb set
.sk
If the
:ARG.dir
parameter has any other value, or the
:ARG.offset
parameter does not have an appropriate sign, the &fn. fails.
