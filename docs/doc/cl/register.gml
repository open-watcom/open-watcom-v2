.ix 'register'
.ix 'storage class' 'register'
.pp
An object that is declared within a function, and whose
declaration includes the keyword
.kw register
..ct ,
is considered to have automatic storage duration.
The
.kw register
keyword merely provides a hint to the compiler that this object
is going to be heavily used, allowing the compiler to try to put it into
a high-speed access part of the machine, such as a machine
register. The compiler may, however, ignore such a directive for
any number of reasons, such as,
.keep begin
.begbull $compact
.bull
the compiler does not support objects in registers,
.bull
there are no available registers, or,
.bull
the compiler makes its own decisions about register usage.
.endbull
.keep end
.pp
Only certain types of objects may be placed in registers, although
the set of such types is implementation-defined.
.*
.************************************************************************
.*
..if '&target' eq 'PC' or '&target' eq 'PC 370' ..th ..do begin
.shade begin
The &wcboth. compilers may place any object that is sufficiently small,
including a small structure,
in one or more registers.
.pp
The compiler will decide which objects will be placed in registers.
The
.kw register
keyword is ignored, except to prevent taking the address of such
an object.
.shade end
..do end
..if '&target' eq 'PC 370' ..th ..do begin
.shade begin
The &wlooc. compiler may place
.* .kw int
.* (both
.* .kw signed
.* and
.* .kw unsigned
.* ..ct ) values in registers.
any integer, floating-point or pointer values in registers.
.pp
The compiler will decide which objects will be placed in registers.
The
.kw register
keyword is ignored, except to prevent taking the address of such
an object.
.shade end
..do end
.*
.************************************************************************
.*
.pp
Objects declared with or without
.kw register
may generally be treated in the same way. An exception to this rule
is that the
.ix 'address-of operator'
.ix 'operator' 'address-of'
.us address-of
operator
.mono (&)
may not be applied to a
.kw register
object, since registers are generally not within the normal storage
of the computer.
