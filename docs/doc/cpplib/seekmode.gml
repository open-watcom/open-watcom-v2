:P.
The
:ARG.mode
parameter may be
:MONO.ios::in
:CONT.,
:MONO.ios::out
:CONT., or
:MONO.ios::in|ios::out
and should be interpreted as follows, provided the interpretation is
meaningful:
.sk
.fo off
.tb set ^
:SF font=4.ios::in:eSF.^^^the :HP1.get pointer:eHP1. should be moved.
:SF font=4.ios::out:eSF.^^^the :HP1.put pointer:eHP1. should be moved.
:SF font=4.ios::in|ios::out:eSF.^both the :HP1.get pointer:eHP1. and the :HP1.put pointer:eHP1. should be moved.
.co &cpp_co.
.ju &cpp_ju.
.tb set
.sk
If
:ARG.mode
has any other value, the &fn. fails.
