.*
.*
.option NAMELEN
.*
.np
The "NAMELEN" option tells the &lnkname that all symbols must be
uniquely identified in the number of characters specified or less.
If any symbol fails to satisfy this condition, a warning message will
be issued.
The warning message will state that a symbol has been defined more
than once.
.np
The format of the "NAMELEN" option (short form "NAMEL") is as follows.
.mbigbox
    OPTION NAMELEN=n
.embigbox
.synote
.im lnkvalue
.esynote
.np
Some computer systems, for example, require that all global symbols be
uniquely identified in 8 characters.
By specifying an appropriate value for the "NAMELEN" option, you can
ease the task of porting your application to other computer systems.
