.np
The restartable multibyte/wide character conversion functions differ
from the corresponding internal-state multibyte character functions (
.ct
.kw mblen
.ct ,
.kw mbtowc
.ct , and
.kw wctomb
.ct )
in that they have an extra argument,
.arg ps
.ct , of type pointer to
.kw mbstate_t
that points to an object that can completely describe the current
conversion state of the associated multibyte character sequence.
If
.arg ps
is a null pointer, each function uses its own internal
.kw mbstate_t
object instead.
You are guaranteed that no other function in the library calls these
functions with a null pointer for
.arg ps
.ct , thereby ensuring the stability of the state.
.np
Also unlike their corresponding functions, the return value does not
represent whether the encoding is state-dependent.
.np
If the encoding is state-dependent, on entry each function takes
the described conversion state (either internal or pointed to by
.arg ps
.ct )
as current.
The conversion state described by the pointed-to object is altered as
needed to track the shift state of the associated multibyte character
sequence.
For encodings without state dependency, the pointer to the
.kw mbstate_t
argument is ignored.
