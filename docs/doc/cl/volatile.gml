.ix 'volatile'
.ix 'type' 'volatile'
.pp
An object may be declared with the keyword
.kw volatile
..ct ..li .
Such an object may be freely modified by the program, and its value
also may be modified through actions outside the program. For example, a
flag may be set when a given interrupt occurs. The keyword
.kw volatile
indicates to the compiler that care must be taken when optimizing
code referring to the object, so that the meaning of the
program is not altered.
An object that the compiler might otherwise have been able
to keep in a register for an extended period of time will be forced
to reside in normal storage so that an external change to it will be
reflected in the program's behavior.
.* .pp
.* The
.* .kw volatile
.* keyword
.* is a type specifier that modifies the token that follows it.
.* If
.* .kw volatile
.* precedes
.* .mono *
.* (as in
.* .mono volatile *)
.* ..ct ,
.* then the pointer points to something volatile. Otherwise, if
.* .kw volatile
.* precedes the identifier of the object or function being declared
.* (as in
.* .mono volatile x)
.* ..ct ,
.* then the object itself is volatile.
.pp
If the declaration of an object does not include
.mono *
..ct ,
that is to say it is not a pointer of any kind, then the keyword
.kw volatile
appearing anywhere in the type specifier (including any
.kw typedef
..ct 's)
indicates that the object is volatile and may be changed at any time
without the program knowing.
If the object is a pointer and
.kw volatile
appears to the left of the
.mono *
..ct ,
the object is a pointer to a volatile value, meaning that
the value to which the pointer points may be changed at any time.
If
.kw volatile
appears to the right of the
.mono *
..ct ,
the object is a volatile pointer to a value, meaning that the pointer
to the value may be changed at any time.
If
.kw volatile
appears on both the left and the right of the
.mono *
..ct ,
the object is a volatile pointer to a volatile value, meaning that the
pointer or the value to which it points may
be changed at any time.
.pp
If the declaration of a structure, union or array includes
.kw volatile
..ct ,
then each member of the type, when referred to, is treated as if
.kw volatile
had been specified.
.* .pp
.* If a type is declared only with the keyword
.* .kw volatile
.* ..ct , then
.* .kw int
.* is implied.
.pp
The declarations,
.millust begin
volatile int   attncount;
volatile int * acptr;
.millust end
.pc
declare the object
.mono attncount
to be an integer whose value may be altered at any time
(say by an asynchronous attention handler), and
the object
.mono acptr
to be a pointer to a volatile object of integer type.
.pp
If both
.kw const
and
.kw volatile
are included in the declaration of an object, then that object may
not be modified by the program, but it may be modified through some
external action.
An example of such an object is the clock in a computer, which is
modified periodically (every clock "tick"), but programs
are not allowed to change it.
