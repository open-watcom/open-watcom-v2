:P.
The &fn. performs the following steps:
:OL.
:LI.If no buffer is present, a buffer is allocated with the
:MONO.streambuf::allocate
member function, which may call the
:MONO.doallocate
virtual member function.
The &putarea. is then set up. If, after calling
:MONO.streambuf::allocate
:CONT., no buffer is present, the &obj. is unbuffered and
:ARG.ch
(if not &eof.) is written directly to the file without buffering,
and no further action is taken.
:LI.If the &getarea. is present, it is flushed with a call to the
:MONO.sync
virtual member function. Note that the &getarea. won't be present if a
buffer was set up in step 1.
:LI.If
:ARG.ch
is not &eof., it is added to the &putarea., if possible.
:LI.Any characters in the &putarea. are written to the file.
:LI.The &putarea. pointers are updated to reflect the new state of the
&putarea.:PERIOD.
If the write did not complete, the unwritten portion of the
&putarea. is still present. If the &putarea. was full before the write,
:ARG.ch
(if not &eof.) is placed at the start of the &putarea.:PERIOD.
Otherwise, the
&putarea. is empty.
:eOL.
:RSLTS.
The &fn. returns &noteof. on success, otherwise &eof. is returned.
