When the argument is outside the permissible range, the
.kw matherr
function is called.
Unless the default
.kw matherr
function is replaced, it will set the global variable
.kw errno
to
.kw EDOM
.ct ,
and print a "DOMAIN error" diagnostic message using the
.kw stderr
stream.
