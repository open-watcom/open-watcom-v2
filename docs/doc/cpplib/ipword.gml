:P.
Note that the
:MONO.iword
and
:MONO.pword
public member functions return references to the same storage with a
different type. Therefore, each
:ARG.index
obtained from the
:MONO.xalloc
static member function can be used only for an integer or a pointer, not both.
:P.
Since the &fn. returns a reference and the &cls. cannot predict how many such
items will be required by a program, it should be assumed that each call to
the
:MONO.xalloc
static member function invalidates all previous references returned by the
&fn.:PERIOD.
Therefore, the &fn. should be called each time the reference is needed.
