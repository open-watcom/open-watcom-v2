.np
The restartable multibyte/wide string conversion functions differ from
the corresponding internal-state multibyte string functions (
.ct
.kw mbstowcs
and
.kw wcstombs
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
Also unlike their corresponding functions, the conversion source
argument,
.arg src
.ct , has a pointer-to-pointer type.
When the function is storing conversion results (that is, when
.arg dst
is not a null pointer), the pointer object pointed to by this argument
will be updated to reflect the amount of the source processed by that
invocation.
.np
If the encoding is state-dependent, on entry each function takes
the described conversion state (either internal or pointed to by
.arg ps
.ct )
as current and then, if the destination pointer,
.arg dst
.ct , is not a null pointer, the conversion state described by the pointed-to
object is altered as needed to track the shift state of the associated
multibyte character sequence.
For encodings without state dependency, the pointer to the
.kw mbstate_t
argument is ignored.
