.*
.*
.dirctv COMMIT
.*
.np
When the operating system allocates the stack and heap for an
application, it does not actually allocate the whole stack and heap to
the application when it is initially loaded.
Instead, only a portion of the stack and heap are allocated or
committed to the application.
Any part of the stack and heap that is not committed will be committed
on demand.
.np
The format of the "COMMIT" directive (short form "COM") is as follows.
.mbigbox
     COMMIT mem_type

     mem_type ::= STACK=n | HEAP=n
.embigbox
.synote
.im lnkvalue
.np
.id n
represents the amout of stack or heap that is initially committed to the
application.
The short form for "STACK" is "ST" and the short form for "HEAP" is "H".
.esynote
.np
If you do not specify the "COMMIT HEAP" directive then a 4k heap is
committed to the application.
.np
If you do not specify the "COMMIT STACK" directive then the default
size is the smaller of 64K or the size specified by the "STACK"
option.
See the section entitled :HDREF refid='xstack'. for more information
on specifying a stack size.
