.ix 'storage class' 'static'
.ix 'static storage class'
.*
.pp
Any declaration of an object
may be preceded by the keyword
.kw static
..ct ..li .
A declaration inside a function
indicates to the compiler that the object
has
.ix 'no linkage'
.ix 'linkage' 'no'
.us no linkage,
meaning that it is available only within the function.
A declaration not inside any function
indicates to the compiler that this object has
.ix 'internal linkage'
.ix 'linkage' 'internal'
.us internal linkage,
meaning that it is available in all functions within the module
in which it is defined. Other modules may not refer to the
specific object.
They may have their own object defined
with the same name, but
this is a questionable programming practice and should be avoided.
.pp
The value of the object will be preserved between function calls.
Any value placed in an
object with static storage duration
will remain unchanged until changed by a function
within the same module.
It is also possible for a pointer to the object to be passed to a
function outside the module in which the object is defined. This
pointer could be used to modify the value of the object.
