The &fn. creates a pointer to the buffer being used by the &obj.:PERIOD.
If the &obj. was created without dynamic allocation (static mode),
the pointer is the same as the buffer pointer passed in the
constructor.
:P.
For &obj.s using dynamic allocation,
the &fn. makes an implicit call to the
:MONO.strstreambuf::freeze
member function.
If nothing has been written to the &obj.
:CONT., the returned pointer will be &null.:PERIOD.
:P.
Note that the buffer does not necessarily end with a null character.
If the pointer returned by the &fn. is to be interpreted as a C string,
it is the program's responsibility to ensure that the null character is
present.
:RSLTS.
The &fn. returns a pointer to the buffer being used by the &obj.:PERIOD.
