.*
.*
.option STACK
.*
.np
The "STACK" option can be used to increase the size of the stack.
The format of the "STACK" option (short form "ST") is as follows.
.mbigbox
    OPTION STACK=n
.embigbox
.synote
.im lnkvalue
.esynote
.np
The default stack size varies for both 16-bit and protected-mode
32-bit applications depending on the executable format.
You can determine the default stack size by looking at the map file
that can be generated when an application is linked ("OPTION MAP").
During execution of your program, you may get an error message
indicating your stack has overflowed.
If you encounter such an error, you must link your application again,
this time specifying a larger stack size using the "STACK" option.
.exam begin
option stack=8192
.exam end
