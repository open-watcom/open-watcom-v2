.pp
This chapter presents a series of examples of declarations of objects
and functions. Along with each example is a description that indicates
how to read the declaration.
.pp
This chapter may be used as a "cookbook" for declarations.
Some complicated but commonly required
declarations are given here.
.pp
The first examples are very simple, and build in complexity.
Some of the examples given near the end of each section are unlikely
to ever be required in a real program, but hopefully they will provide
an understanding of how to read and write C declarations.
.pp
To reduce the complexity and to better illustrate how a small difference
in the declaration can mean a big difference in the meaning, the
following rules are followed:
.autonote
.note
if an object is being declared, it is called
.mono x
or
.mono X
..ct ,
.note
if a function is being declared, it is called
.mono F
..ct ,
.note
if an object is being declared, it usually has type
.kw int
..ct ,
although any other type may be substituted,
.note
if a function is being declared, it usually returns type
.kw int
..ct ,
although any other type may be substituted.
.endnote
.pp
Storage class specifiers (
..ct .kw extern
..ct ,
.kw static
..ct ,
.kw auto
or
.kw register
..ct ) have purposely been omitted.
.*
.section Object Declarations
.*
.pp
Here are some examples of object (variable) declarations:
.keep begin
.pp
.millust begin
int x;
 &MSUPER2.  &MSUPER1.
.millust end
.discuss begin
&SUPER1.
..ct .mono x
is an
&SUPER2.integer.
.discuss end
.keep break
.millust begin
int * x;
 &MSUPER3.  &MSUPER2. &MSUPER1.
.millust end
.discuss begin
&SUPER1.
..ct .mono x
is a
&SUPER2.pointer to an &SUPER3.integer.
.discuss end
.keep break
.millust begin
int ** x;
 &MSUPER4.  &MSUPER3.&MSUPER2. &MSUPER1.
.millust end
.discuss begin
&SUPER1.
..ct .mono x
is a
&SUPER2.pointer to a &SUPER3.pointer to an &SUPER4.integer.
.discuss end
.keep break
.millust begin
const int x;
  &MSUPER2.    &MSUPER3.  &MSUPER1.
.millust end
.discuss begin
&SUPER1.
..ct .mono x
is a &SUPER2.constant
&SUPER3.integer.
.discuss end
.keep break
.millust begin
int const x;
 &MSUPER3.    &MSUPER2.   &MSUPER1.
.millust end
.discuss begin
&SUPER1.
..ct .mono x
is a &SUPER2.constant
&SUPER3.integer (same as above).
.discuss end
.keep break
.millust begin
const int * x;
  &MSUPER3.    &MSUPER4.  &MSUPER2. &MSUPER1.
.millust end
.discuss begin
&SUPER1.
..ct .mono x
is a &SUPER2.pointer to a &SUPER3.constant
&SUPER4.integer.
The value of
.mono x
may change, but the integer that it points to may not be changed.
In other words,
.mono *x
cannot be modified.
.discuss end
.keep break
.millust begin
int * const x;
 &MSUPER4.  &MSUPER3.   &MSUPER2.   &MSUPER1.
.millust end
.discuss begin
&SUPER1.
..ct .mono x
is a &SUPER2.constant &SUPER3.pointer to an &SUPER4.integer.
The value of
.mono x
may not change, but the integer that it points to may change.
In other words,
.mono x
will always point at the same location, but the contents of that location
may vary.
.discuss end
.keep break
.millust begin
const int * const x;
  &MSUPER4.    &MSUPER5.  &MSUPER3.   &MSUPER2.   &MSUPER1.
.millust end
.discuss begin
&SUPER1.
..ct .mono x
is a &SUPER2.constant &SUPER3.pointer to a &SUPER4.constant &SUPER5.integer.
The value of
.mono x
may not change, and the integer that it points to may not change.
In other words,
.mono x
will always point at the same location, which cannot be modified via
.mono x.
.discuss end
.keep break
.millust begin
int x[];
 &MSUPER3.  &MSUPER1.&MSUPER2.
.millust end
.discuss begin
&SUPER1.
..ct .mono x
is an &SUPER2.array of &SUPER3.integers.
.discuss end
.keep break
.millust begin
int x[53];
 &MSUPER4.  &MSUPER1.&MSUPER2.&MSUPER3.
.millust end
.discuss begin
&SUPER1.
..ct .mono x
is an &SUPER2.array of &SUPER3.53 &SUPER4.integers.
.discuss end
.keep break
.millust begin
int * x[];
 &MSUPER4.  &MSUPER3. &MSUPER1.&MSUPER2.
.millust end
.discuss begin
&SUPER1.
..ct .mono x
is an &SUPER2.array of &SUPER3.pointers to &SUPER4.integer.
.discuss end
.keep break
.millust begin
int (*x)[];
 &MSUPER4.   &MSUPER2.&MSUPER1. &MSUPER3.
.millust end
.discuss begin
&SUPER1.
..ct .mono x
is a &SUPER2.pointer to an &SUPER3.array of &SUPER4.integers.
.discuss end
.keep break
.millust begin
int * (*x)[];
 &MSUPER5.  &MSUPER4.  &MSUPER2.&MSUPER1. &MSUPER3.
.millust end
.discuss begin
&SUPER1.
..ct .mono x
is a &SUPER2.pointer to an &SUPER3.array of &SUPER4.pointers to
&SUPER5.integer.
.discuss end
.keep break
.millust begin
int (*x)();
 &MSUPER4.   &MSUPER2.&MSUPER1. &MSUPER3.
.millust end
.discuss begin
&SUPER1.
..ct .mono x
is a &SUPER2.pointer to a &SUPER3.function returning an &SUPER4.integer.
.discuss end
.keep break
.millust begin
int (*x[25])();
 &MSUPER6.   &MSUPER4.&MSUPER1.&MSUPER2.&MSUPER3.   &MSUPER5.
.millust end
.discuss begin
&SUPER1.
..ct .mono x
is an &SUPER2.array of &SUPER3.25 &SUPER4.pointers to
&SUPER5.functions returning an &SUPER6.integer.
.discuss end
.keep end
.*
.section Function Declarations
.*
.pp
Here are some examples of function declarations:
.keep begin
.millust begin
int F();
 &MSUPER3.  &MSUPER1.&MSUPER2.
.millust end
.discuss begin
&SUPER1.
..ct .mono F
is a &SUPER2.function returning an &SUPER3.integer.
.discuss end
.keep break
.millust begin
int * F();
 &MSUPER4.  &MSUPER3. &MSUPER1.&MSUPER2.
.millust end
.discuss begin
&SUPER1.
..ct .mono F
is a &SUPER2.function returning a &SUPER3.pointer to an &SUPER4.integer.
.discuss end
.* .keep break
.* .millust begin
.* int F()();
.*  &SUPER4.  &SUPER1.&SUPER2. &SUPER3.
.* .millust end
.* .discuss begin
.* &SUPER1.
.* ..ct .mono F
.* is a &SUPER2.function returning a (pointer to a) &SUPER3.function returning &SUPER4.
.* ..ct .kw int
.* ..ct ..li .
.* .discuss end
.keep break
.millust begin
int (*F())();
 &MSUPER5.   &MSUPER3.&MSUPER1.&MSUPER2.  &MSUPER4.
.millust end
.discuss begin
.* The same as above,
&SUPER1.
..ct .mono F
is a &SUPER2.function returning a &SUPER3.pointer to a
&SUPER4.function returning an &SUPER5.integer.
.discuss end
.* .keep break
.* .millust begin
.* int *(F())();
.*  &SUPER5.  &SUPER4. &SUPER1.&SUPER2.  &SUPER3.
.* .millust end
.* .discuss begin
.* &SUPER1.
.* ..ct .mono F
.* is a &SUPER2.function returning a (pointer to a) &SUPER3.function returning
.* a &SUPER4.pointer to an &SUPER5.
.* ..ct .kw int
.* ..ct ..li .
.* .discuss end
.keep break
.millust begin
int * (*F())();
 &MSUPER6.  &MSUPER5.  &MSUPER3.&MSUPER1.&MSUPER2.  &MSUPER4.
.millust end
.discuss begin
.* The same as above,
&SUPER1.
..ct .mono F
is a &SUPER2.function returning a &SUPER3.pointer to a
&SUPER4.function returning
a &SUPER5.pointer to an &SUPER6.integer.
.discuss end
.keep break
.millust begin
int (*F())[];
 &MSUPER5.   &MSUPER3.&MSUPER1.&MSUPER2.  &MSUPER4.
.millust end
.discuss begin
&SUPER1.
..ct .mono F
is a &SUPER2.function returning a &SUPER3.pointer to an
&SUPER4.array of &SUPER5.integers.
.discuss end
.* .keep break
.* .millust begin
.* int (*F())[]();
.*  &SUPER6.   &SUPER3.&SUPER1.&SUPER2.  &SUPER4. &SUPER5.
.* .millust end
.* .discuss begin
.* &SUPER1.
.* ..ct .mono F
.* is a &SUPER2.function returning a &SUPER3.pointer to an &SUPER4.array of
.* (pointers to) &SUPER5.functions returning &SUPER6.
.* ..ct .kw int
.* ..ct ..li .
.* .discuss end
.keep break
.millust begin
int (*(*F())[])();
 &MSUPER7.   &MSUPER5. &MSUPER3.&MSUPER1.&MSUPER2.  &MSUPER4.  &MSUPER6.
.millust end
.discuss begin
.* The same as above,
&SUPER1.
..ct .mono F
is a &SUPER2.function returning a &SUPER3.pointer to an &SUPER4.array of
&SUPER5.pointers to &SUPER6.functions returning an &SUPER7.integer.
.discuss end
.keep break
.millust begin
int * (*(*F())[])();
 &MSUPER8.  &MSUPER7.  &MSUPER5. &MSUPER3.&MSUPER1.&MSUPER2.  &MSUPER4.  &MSUPER6.
.millust end
.discuss begin
&SUPER1.
..ct .mono F
is a &SUPER2.function returning a &SUPER3.pointer to an &SUPER4.array of
&SUPER5.pointers to &SUPER6.functions returning a &SUPER7.pointer to an
&SUPER8.integer.
.discuss end
.keep end
.*
.************************************************************************
.*
..if '&target' eq 'PC' or '&target' eq 'PC 370' ..th ..do begin
.*
.section &kwfar_sp., &kwnear_sp. and &kwhuge_sp. Declarations
.*
..if '&target' eq 'PC 370' ..th ..do begin
.shade begin
These examples apply only to the &wcboth. compilers.
.shade end
..do end
.pp
The following examples illustrate the use of the
.kwix &kwfar_sp.
.kwfont &kwfar.
and
.kwix &kwhuge_sp.
.kwfont &kwhuge.
keywords.
.* The examples assume that the
.* .ix 'small code'
.* small code,
.* .ix 'small data'
.* small data
.* .ix 'small memory model'
.* .ix 'memory model' 'small'
.* (small memory model)
.* is being used.
.* .pp
.* No examples of the
.* .kwix &kwnear_sp.
.* .kwfont &kwnear.
.* keyword have been given since its use is
.* symmetrical with the use of the
.* .kwix &kwfar_sp.
.* .kwfont &kwfar.
.* keyword (when the
.* .ix 'big code'
.* big code and/or
.* .ix 'big data'
.* big data model
.* is being used).
.pp
The use of the
.kwix &kwnear_sp.
.kwfont &kwnear.
keyword is symmetrical with the use of the
.kwix &kwfar_sp.
.kwfont &kwfar.
keyword, so no examples of
.kwix &kwnear_sp.
.kwfont &kwnear.
are shown.
.keep begin
.millust begin
int __far X;
 &MSUPER3.    &MSUPER2.   &MSUPER1.
.millust end
.discuss begin
&SUPER1.
..ct .mono X
is a
&SUPER2.far
&SUPER3.integer.
.discuss end
.keep break
.millust begin
int * __far x;
 &MSUPER4.  &MSUPER3.   &MSUPER2.   &MSUPER1.
.millust end
.discuss begin
&SUPER1.
..ct .mono x
is
&SUPER2.far,
and is a
&SUPER3.pointer to an &SUPER4.integer.
.discuss end
.keep break
.millust begin
int __far * x;
 &MSUPER4.    &MSUPER2.   &MSUPER3. &MSUPER1.
.millust end
.discuss begin
&SUPER1.
..ct .mono x
is a &SUPER2.far
&SUPER3.pointer to an &SUPER4.integer.
.discuss end
.keep break
.millust begin
int __far * __far x;
 &MSUPER5.    &MSUPER3.   &MSUPER4.   &MSUPER2.   &MSUPER1.
.millust end
.discuss begin
&SUPER1.
..ct .mono x
is &SUPER2.far,
and is a &SUPER3.far
&SUPER4.pointer to an
&SUPER5.integer.
.discuss end
.keep break
.millust begin
int __far X[];
 &MSUPER4.    &MSUPER2.   &MSUPER1.&MSUPER3.
.millust end
.discuss begin
&SUPER1.
..ct .mono X
is a &SUPER2.far
&SUPER3.array of &SUPER4.integers.
.discuss end
.keep break
.millust begin
int __huge X[];
 &MSUPER4.    &MSUPER2.    &MSUPER1.&MSUPER3.
.millust end
.discuss begin
&SUPER1.
..ct .mono x
is a &SUPER2.huge
&SUPER3.array of &SUPER4.integers
(
..ct .mono X
is an array that can exceed 64K in size.)
.discuss end
.keep break
.millust begin
int * __far X[];
 &MSUPER5.  &MSUPER4.   &MSUPER2.   &MSUPER1.&MSUPER3.
.millust end
.discuss begin
&SUPER1.
..ct .mono X
is a &SUPER2.far
&SUPER3.array of &SUPER4.pointers to &SUPER5.integers.
.discuss end
.keep break
.millust begin
int __far * X[];
 &MSUPER5.    &MSUPER3.   &MSUPER4. &MSUPER1.&MSUPER2.
.millust end
.discuss begin
&SUPER1.
..ct .mono X
is an &SUPER2.array of &SUPER3.far
&SUPER4.pointers to &SUPER5.integers.
.discuss end
.keep break
.millust begin
int __far * __far X[];
 &MSUPER6.    &MSUPER4.   &MSUPER5.   &MSUPER2.   &MSUPER1.&MSUPER3.
.millust end
.discuss begin
&SUPER1.
..ct .mono X
is a &SUPER2.far
&SUPER3.array of &SUPER4.far
&SUPER5.pointers to
&SUPER6.integers.
.discuss end
.keep break
.millust begin
int __far F();
 &MSUPER4.    &MSUPER2.   &MSUPER1.&MSUPER3.
.millust end
.discuss begin
&SUPER1.
..ct .mono F
is a &SUPER2.far
&SUPER3.function returning an &SUPER4.integer.
.discuss end
.keep break
.millust begin
int * __far F();
 &MSUPER5.  &MSUPER4.   &MSUPER2.   &MSUPER1.&MSUPER3.
.millust end
.discuss begin
&SUPER1.
..ct .mono F
is a &SUPER2.far
&SUPER3.function returning a &SUPER4.pointer to an &SUPER5.integer.
.discuss end
.keep break
.millust begin
int __far * F();
 &MSUPER5.    &MSUPER3.   &MSUPER4. &MSUPER1.&MSUPER2.
.millust end
.discuss begin
&SUPER1.
..ct .mono F
is a
&SUPER2.function returning a &SUPER3.far
&SUPER4.pointer to an
&SUPER5.integer.
.discuss end
.keep break
.millust begin
int __far * __far F();
 &MSUPER6.    &MSUPER4.   &MSUPER5.   &MSUPER2.   &MSUPER1.&MSUPER3.
.millust end
.discuss begin
&SUPER1.
..ct .mono F
is a &SUPER2.far
&SUPER3.function returning a &SUPER4.far
&SUPER5.pointer to an
&SUPER6.integer.
.discuss end
.keep break
.millust begin
int (__far * x)();
 &MSUPER5.     &MSUPER2.   &MSUPER3. &MSUPER1. &MSUPER4.
.millust end
.discuss begin
&SUPER1.
..ct .mono x
is a &SUPER2.far
&SUPER3.pointer to a
&SUPER4.function returning an &SUPER5.integer.
.discuss end
.keep break
.millust begin
int __far * (* x)();
 &MSUPER6.    &MSUPER4.   &MSUPER5.  &MSUPER2. &MSUPER1. &MSUPER3.
.millust end
.discuss begin
&SUPER1.
..ct .mono x
is a &SUPER2.pointer to a
&SUPER3.function returning a &SUPER4.far
&SUPER5.pointer to an
&SUPER6.integer.
.discuss end
.keep break
.millust begin
int __far * (__far * x)();
 &MSUPER7.    &MSUPER5.   &MSUPER6.    &MSUPER2.   &MSUPER3. &MSUPER1. &MSUPER4.
.millust end
.discuss begin
&SUPER1.
..ct .mono x
is a &SUPER2.far
&SUPER3.pointer to a
&SUPER4.function returning a &SUPER5.far
&SUPER6.pointer to an
&SUPER7.integer.
.discuss end
.keep end
.*
.section &kwintr_sp. Declarations
.*
..if '&target' eq 'PC 370' ..th ..do begin
.shade begin
This example applies only to the &wcboth. compilers.
.shade end
..do end
.pp
The following example illustrates the use of the
.kwix &kwintr_sp.
.kwfont &kwintrpt.
keyword.
.keep begin
.millust begin
void __interrupt __far F();
 &MSUPER5.         &MSUPER3.       &MSUPER2.   &MSUPER1.&MSUPER4.
.millust end
.discuss begin
&SUPER1.
..ct .mono F
is a &SUPER2.far
&SUPER3.interrupt
&SUPER4.function returning &SUPER5.nothing.
.discuss end
.keep end
..do end
.*
.************************************************************************
.*
