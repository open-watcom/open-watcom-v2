.ix 'modularity'
.pp
For many small programs, it is possible to write a single module
which contains all of the C source for the program. This module
can then be compiled, linked and run.
.pp
However, for larger applications it is not
possible to maintain one module with everything in it. Or, if it
is technically possible, compiling such a large module
every time a change is made to the source carries too great
a time penalty with it.
At this point, it becomes necessary to break the program into pieces,
or modules.
.pp
Dividing a program can be done quite easily. If the only issue is
to reduce the size of the modules that need to be compiled, then
arbitrary divisions of the code into modules will accomplish the goal.
.pp
There are other advantages, however, to planning program modularity.
Some of these
advantages are:
.begbull
.bull
recompilation time is reduced,
.bull
code can be grouped into logically-connected areas, making it
easier to find things,
.bull
data structures can be hidden in one module, avoiding the temptation
of letting an outside piece of code "peek" into a structure it really
should not access directly,
.bull
whole modules can be rewritten or redesigned without affecting
other modules,
.bull
areas of the code that depend on the hardware or operating system
can be isolated for easy replacement when the program is ported.
This may extend to replacing the module with an assembly language
equivalent for increased performance.
.endbull
.pp
The following sections discuss each of these points in more detail.
.*
.section Reducing Recompilation Time
.*
.ix 'reducing recompile time'
.pp
As discussed above, merely breaking a program into pieces will reduce
the amount of time spent recompiling the source. A bug is often
a simple coding error, requiring only a one or two line change.
Recompiling only a small percentage of the code and relinking
will be faster than recompiling everything.
.pp
Occasionally, recompiling all of the modules will be required.
This usually arises when a data structure, constant, macro or other
item that is used by several modules is changed. With good program
design, such a change would occur in a
.ix 'header'
header file, and all modules that include that header would be
recompiled.
.*
.section Grouping Code With Related Functionality
.*
.pp
The best way to break programs into modules is to designate each
module as having some overall purpose. For example, one module
may deal exclusively with interacting with the user. Another module
may manage a table of names, while yet another may process some
small subset of the set of actions that may be performed by the program.
.pp
Many of the modules then become
.ix 'resource manager'
.us resource managers,
and every part of the
code that needs to do something significant with that resource must
act through that resource manager.
.pp
Using the example of the names table manager,
it is likely that the manager will need
to do things like create and delete a name entry in the table.
These actions would
translate directly to two functions with external linkage.
.pp
By dividing up a program along lines of related functionality, it
is usually easy to know where to look when a problem is being
tracked.
.pp
.ix 'module name'
.ix 'naming modules'
Module names that clearly state the purpose of the module
also help to locate things.
.*
.section Data Hiding
.*
.beglevel
.*
.ix 'data hiding'
.ix 'hiding data'
.pp
Sometimes a module is written that has exclusive ownership of a data
structure, such as a linked list. All other modules that wish to
access the structure must call a function in the module that owns
it. This technique is known as
.us data hiding.
The actual data is hidden in the structure, and only the
.ix 'functional interface'
.us functional interface
(also called the
.ix 'procedural interface'
.us procedural interface)
may be used to access it.
The functional interface is just the set of functions provided for
accessing the structure.
.pp
The main advantage of data hiding is that the data structure may
be changed with little or no impact on other modules.
Also, access to the structure is controlled, leading to fewer errors
because of misuse of the structure.
.pp
It is possible to have different levels of data hiding. Complete data
hiding occurs when no outside module has access to the structure at
all.
Partial data hiding occurs when elements of the structure can be
accessed, but the overall structure may not be manipulated.
.pp
Note that these rules work only if the programmer respects them.
The rules are not enforced by the compiler. If a module includes a
header that describes the data structures being used by another module
that wants exclusive access to the structures,
a rule is being broken. Whether this is good or bad depends entirely on
the judgement of the programmer.
.*
.section Complete Data Hiding
.*
.ix 'data hiding' 'complete'
.ix 'complete data hiding'
.pp
With complete data hiding,
having a pointer to an element of the structure has no intrinsic
value except as a parameter to the functional interface.
Getting or setting a value in the structure requires a function call.
.pp
The advantage of this technique is that the complete data
structure may be
totally redesigned without affecting other modules. The definitions of
the individual structures
(
..ct .kw struct
..ct 's,
.kw union
..ct 's, arrays)
may be changed and no other module will have to be changed, or even
recompiled.
.pp
The main disadvantage of complete data hiding is that even simple
accesses require a function call, which is less efficient
than just referencing a storage location.
.pp
Function-like macros may also be used to implement complete data hiding,
avoiding the function call but hiding the true structure of the data.
Recompilation of all modules may be required if the data structures
change.
.*
.section Partial Data Hiding
.*
.ix 'data hiding' 'partial'
.ix 'partial data hiding'
.pp
Partial data
hiding occurs when the structure itself (for example, a linked list)
is not accessible in its entirety, but elements of the structure (an
element of the linked list) are accessible.
.pp
Using the names table manager as an example, it may be necessary to
call the names table manager to create a name entry, but once
the name is created, a pointer to the name is returned as the
return value of the create function. This pointer points to
a structure which is defined in a header that any module can include.
Therefore, the contents of an element of the data structure can be
manipulated directly.
.pp
This method is more efficient than the complete data hiding technique.
However, when the structure used for the names table is changed, all
modules that refer to that structure must be recompiled.
.endlevel
.*
.section Rewriting and Redesigning Modules
.*
.pp
With modular program design and data hiding, it is often possible
to completely replace a module without affecting others. This is
usually only possible when the functional interface does not change.
With partial data hiding, the actual types used to implement the
structure would have to remain unchanged, otherwise at least a
recompilation would be required. Changing a
.kw struct
..ct ,
for example, would probably require a recompilation if only the
types changed, or new members were added.
If, however, the names of the
members changed, or some other fundamental change occurred, then
source code changes in these other modules would be necessary.
.*
.section Isolating System Dependent Code in Modules
.*
.ix 'system dependencies'
.pp
System dependencies are only relevant if the program being developed
is to be run on different computers or operating systems. Isolating
system dependent code is discussed more thoroughly in the chapter
"&portable.".
.pp
It is quite difficult, sometimes, to identify what constitutes
system dependent code. The first time a program is ported to a new
system, a number of problem areas usually arise.
These areas should be
carefully examined, and the code that is dependent on the host
environment should be isolated. Isolation may be accomplished by
placing the code in a separate module marked as system dependent,
or by placing macros in the code to compile
differently for the different systems.
