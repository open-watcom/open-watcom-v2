:P.
The &rsvarea., &getarea., and &putarea. pointer functions return the following
values:
.sk
.fo off
.tb set ^
:SF font=4.base():eSF.^start of the :HP1.reserve area:eHP1..
:SF font=4.ebuf():eSF.^end of the :HP1.reserve area:eHP1..
:SF font=4.blen():eSF.^length of the :HP1.reserve area:eHP1..
.sk
:SF font=4.eback():eSF.^start of the :HP1.get area:eHP1..
:SF font=4.gptr():eSF.^the :HP1.get pointer:eHP1..
:SF font=4.egptr():eSF.^end of the :HP1.get area:eHP1..
.sk
:SF font=4.pbase():eSF.^start of the :HP1.put area:eHP1..
:SF font=4.pptr():eSF.^the :HP1.put pointer:eHP1..
:SF font=4.epptr():eSF.^end of the :HP1.put area:eHP1..
.co &cpp_co.
.ju &cpp_ju.
.tb set
.sk
From
:MONO.eback
to
:MONO.gptr
are characters buffered and read. From
:MONO.gptr
to
:MONO.egptr
are characters buffered but not yet read. From
:MONO.pbase
to
:MONO.pptr
are characters buffered and not yet written.
From
:MONO.pptr
to
:MONO.epptr
is unused buffer area.
