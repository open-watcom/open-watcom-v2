:H1.Introduction
:P.
The class template :CLASS.std::~vector
provides a dynamic array of objects with a type given by the type
parameter. Unlike :CLASS.std::~string
vectors can be instantiated with non-POD types. This complicates
the implementation of :CLASS.std::~vector
considerably, as discussed below.

:H2. Status
:P.
:AUTHOR date='1 Sep 2006'.Peter C. Chapin
:P.
Reviewer: Not reviewed
:P.
Most of the required functionality has been implemented. Some of the
methods are not yet exception safe.

.H1. Design Details
:P.
The internal structure of :CLASS.vector
is very similar to that of :CLASS.string.
Any enhancement or bug fix applied to either of these templates should
be reviewed for possible application to the other. Like a string
a vector allocates more raw memory than it needs. This allows the logical
size of the vector
to increase without necessarily requiring a reallocation of memory.
However, unlike a string, a vector can contain objects with user defined
copy constructors and user defined :CODE.operator=.
In addition, copying and assigning objects in a vector might cause an
exception to be thrown. These details make implementing :CLASS.vector
more difficult than implementing :CLASS.string.

:P.
For example, consider a vector of size 100 with 200 units of memory
allocated. Now suppose that 10 new objects are inserted in the middle of
this vector. The 10 objects at the end of the vector need to be copying
onto the raw memory just past the end using a copy constructor. However,
the other objects that are moved will be placed on top of existing objects
and thus must be copied with an :CODE.operator=.

:P.
If an exception occurs while the new objects are being constructed, the
objects constructed so far can be destroyed and the vector can be left in
an unmodifed state. However, if an exception occurs after the new objects
have been created but during the assignment of the remaining objects it is
somewhat unclear how to best proceed. If the new objects are destroyed
data may be lost since the original copies of those objects may have
already been overwritten. Yet trying to restore the vector to its initial
state is probably unwise; if an exception has occured while copying objects
around, further copying is unlikely to be successful. There is little
choice but to leave the vector in a corrupted, partially modified state.
