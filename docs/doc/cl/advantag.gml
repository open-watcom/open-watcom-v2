.pp
C has a number of major advantages over other programming languages.
.begbull
.bull
Most systems provide a C compiler.
.endbull
.pp
Vendors of computer systems realize that the success of a system is
dependent upon the availability of software for that system.
With the large body of C-based programs in existence,
most vendors
provide a C compiler in order to encourage the transporting
of some of these programs to their system.
For systems that don't provide a C compiler, independent companies may
develop a compiler.
.pp
With the development of the ISO/ANSI C standard, the trend towards
universal availability of C compilers will probably accelerate.
.begbull
.bull
C programs can be transported easily to other computers and
operating systems.
.endbull
.pp
Many programming languages claim transportability. FORTRAN, COBOL
and Pascal programs all have standards describing them, so a program
written entirely within the standard definition of the language will
likely be portable. The same is true of C. However, few languages can
match portability with the other advantages of C, including efficiency
of generated code and the ability to work close to the machine level.
.begbull
.bull
Programs written in C are very efficient in both execution
speed and code size.
.endbull
.pp
Few languages can match C in efficiency.
A good assembly language programmer
may be able to produce code better than a C compiler,
but he/she will
have to spend much more time in the development of the application,
because assembly language programming lends itself more easily to errors.
Compilers for other languages may produce efficient code
for applications within their scope, but few produce efficient
code for
.us all
applications.
.begbull
.bull
C programs can get close to the hardware, controlling devices
directly if necessary.
.endbull
.pp
Most programs do not need this ability, but if necessary,
the program can access particular features of the computer.
For example, a fixed memory location may exist
that contains a certain value of use to the program.
It is easy to access it from C, but not from many other
languages.
(Of course, if the program is designed to be portable, this section
of code will be isolated and clearly marked as depending on the
operating system.)
.begbull
.bull
C programs are easy to maintain.
.endbull
.pp
Assembly language
code is difficult to maintain owing to the very low level
of programming (registers, addressing modes, branching).
C programs provide comparable functionality, but at a higher
level. The programmer still thinks in terms of machine capabilities,
but without having to know the exact operation of the hardware,
leaving the programmer free to concentrate on program design rather
than the intimate details of coding on that particular machine.
.begbull
.bull
C programs are easy to understand.
.endbull
.pp
"Easy" is, of course, a relative term.
C programs are definitely easier to understand than the equivalent
assembly language program.
Another programming language may be easier to
understand for a particular kind of application, but in general
C is a good choice.
.begbull
.bull
All of the above advantages apply regardless of the application
or the hardware or operating system on which it is running.
.endbull
.pp
This is the biggest advantage. Because C programs are portable,
and C is not suited only to a certain class of applications,
it is often the best choice for developing an application.
