:P.
The
:MONO.stdiobuf
class behaves in a similar way to the
:MONO.filebuf
class, but does not need to switch between the &getarea. and &putarea.,
since no
:MONO.stdiobuf
object can be created for both reading and writing. When the &getarea. is
empty and a read is done, the
:MONO.underflow
virtual member function reads more characters and fills the &getarea. again.
When the &putarea. is full and a write is done, the
:MONO.overflow
virtual member function writes the characters and makes the &putarea. empty
again.
