:P.
The
:MONO.filebuf
class allows only the &getarea. or the &putarea., but not both,
to be active at a time. This follows from the
capability of files opened for both reading and writing to have operations of
each type performed at arbitrary locations in the file. When writing is
occurring, the characters are buffered in the &putarea.:PERIOD.
If a seek or read
operation is done, the &putarea. must be flushed before the next operation in
order to ensure that the characters are written to the proper location in the
file. Similarly, if reading is occurring, characters are buffered in the
&getarea.:PERIOD.
If a write operation is done, the &getarea. must be flushed and synchronized
before the write operation in order to ensure the write occurs at the proper
location in the file. If a seek operation is done, the &getarea. does not have
to be synchronized, but is discarded. When the &getarea. is empty and a read
is done, the
:MONO.underflow
virtual member function reads more characters and fills the &getarea. again.
When the &putarea. is full and a write is done, the
:MONO.overflow
virtual member function writes the characters and makes the &putarea. empty
again.
