:P.
The &fn. performs the following steps:
:OL.
:LI.If no &rsvarea. is present, a buffer is allocated with the
:MONO.streambuf::allocate
member function, which may call the
:MONO.doallocate
virtual member function. If, after calling
:MONO.allocate
:CONT., no &rsvarea. is present, the &obj. is unbuffered and a
one-character &rsvarea. (plus putback area) is set up to do unbuffered input.
This buffer is embedded in the &obj.:PERIOD.
The &getarea. is set up as empty.
:CMT.=======================================================================
:CMT. start of conditional section for filebuf inclusion
.if '&cl_name' = 'filebuf' .do begin
:CMT.=======================================================================
:LI.If the &putarea. is present, it is flushed using the
:MONO.sync
virtual member function.
:CMT.=======================================================================
.do end
:CMT. end of conditional section for filebuf inclusion
:CMT.=======================================================================
:LI.The unused part of the &getarea. is used to read characters from the file
connected to the &obj.:PERIOD.
The &getarea. pointers are then set up to reflect the new &getarea.:PERIOD.
:eOL.
:RSLTS.
The &fn. returns the first unread character of the &getarea., on success,
otherwise &eof. is returned. Note that the &getptr. is not advanced on
success.
