.chap Programming Style

.ix 'style'
Programming style is, unfortunately, a religious matter. Many holy wars
have been fought over it with no clear result (because the losing side
usually survives). Still, with a project the size of Open Watcom (that
is, a very big project) there is a clear need for common programming
style.
.np
Conformance of all projects to a common style has several
benefits. Programmers who know this style will be easily able to find
their way around any of the multitude of projects. Various projects
will easily fit together. And last but not least, consistent style
looks good.
.np
Note: the fact that certain Open Watcom projects do not adhere to the
common programming style should not be construed as an endorsement of
non-conformance. It is simply a result of the long and varied history
of the project.
.np
The following sections examine various aspects of programming practice
and give specific guidelines where applicable. Note that these are
guidelines, not rules or laws. Violating them is not a crime and not
even a mortal sin. In fact, you might have a very good reason not to
stick to the guidelines, and we always prefer common sense to fixed
rules. However breaking the guidelines with no good reason is bad for
your karma. Don't do it!


.section Source file structure
.*
.np
.ix 'source files'
First a few words on source file structure. Every source file should
start with a copyright header. This only applies to source and include
files (regardless of programming language used). Other types of files
such as makefiles, scripts, etc. do not need a copyright header. The
header should also contain a short description of the source file, one
or two lines is usually enough. Longer comments explaining specifics of
the implementation should be placed after the header.
.np
The rest of the source file structure depends on the language used.
Here we will only examine the most common kind, which is a C source
file. The overall structure is as follows:
.begbull
.bull
copyright header
.bull
.kw #include
directives
.bull
function declarations and global variable definitions
.bull
function implementation
.endbull
As you can see, nothing fancy. Many programmers prefer to order functions
so as to minimize forward declarations, ie. a
.id main()
function would be located at the end and every function fully defined
before it is first used.
.np
You can use
.kw extern
declarations but you should be very careful. It is strongly encouraged
that all declarations of external functions and variable be located in
header files which are included in modules that use them as well as
modules that define them. The reason is simple &mdash this way the compiler
will tell you if you change the definition but not the header file. If you
use ad-hoc
.kw extern
declarations, you better make sure they're in sync with the actual definitions.


.section Help the compiler and it will help you
.*
.np
While the compiler is a rather sophisticated piece of software, it cannot
divine your intentions. Hence you have to help it a bit by giving hints
here and there. And if you help the compiler in this way, it will be better
able to help you.
.np
First of all, always compile with maximum warning level. Just because
a message is called a warning and not error doesn't mean you can ignore
it. In fact many projects treat warnings as errors and will not build if
any warnings are present. That is a Good Thing.
.np
Use the
.kw static
keyword often because it is good. This is a multi-faceted keyword and its
exact semantics depend on where it is applied:
.begnote
.note globals
here the
.kw static
modifier does not change the storage class but makes the variables local to the
module where they are defined, that is they won't be visible from other modules.
If a variable needs to be global but doesn't have to be accessed from other
modules, you should definitely make it static. That way you will not needlessly
pollute the global namespace (reducing the chance of name clashes) and if you
stop using the variable, the compiler will warn you. If you have a non-static
global variable that is unused, the compiler cannot warn you as it has to assume
that it is used from other modules.
.note locals
in this case the
.kw static
keyword changes the storage class and the variable will be stored in data segment
instead of on stack. If you need a variable that has global lifetime but only
needs to be accessible from one function, making it local and static is a good
choice.
.note functions
the effect of the
.kw static
keyword here is similar to the global variables. The function will not be visible
from other modules. Again, the compiler will warn you if you stop using such
a function. But declaring a function static also helps the optimizer. For instance
if the optimizer inlines a static function, it can completely remove the non-inlined
version. If the function weren't static, the non-inlined version always has to be
present for potential external callers.
.endnote
.np
Similar in vein to the
.kw static
keyword is the
.kw const
keyword. Again it can be applied in various ways and it helps the compiler and
optimizer by giving it hints about how you intend to use a particular variable
or argument. Saying that a variable is constant is essentially the same as saying
that it's read-only. The compiler/linker might place such variable in a read-only
segment. While it is possible to circumvent the
.kw const
modifier by taking the address of a constant variable and modifying it through
a pointer, this is a bad thing to do and it may not work at all because the
variable might be not be physically writable. It is perhaps worthwhile to remark
that there are three possible outomes of applying the
.kw const
modifier to a pointer:
.begbull
.bull
a constant pointer, that is the value of the pointer is constant but the data it
points to isn't
.bull
a pointer to a constant, that is the pointer itself is not constant but the value
it points to cannot be modified through it
.bull
a constant pointer to a constant, that is both the pointer itself and the value
it points to are constant.
.endbull
.np
The
.kw const
keyword is especially useful when used in function declarations. Consider the
following typical declaration:
.millust begin
char *strcpy( char *s1, const char *s2 );
.millust end
Here we have a function which takes two arguments that are both pointers to char
but one of them is a pointer to a constant. In the function body the compiler
will not let you modify the contents of
.id *s2
but the declaration is also important for the caller. In the calling funtion, the
optimizer now knows that the data the
.id s2
argument points to will not be modified by
.id strcpy()
and it can take advantage of this knowledge.


.* Get text in a neat frame
.*.remark
.*Kevin Goodman describes "unwinds" in his article.
.*.eremark
