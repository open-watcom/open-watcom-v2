.*
.*
.option DISTRIBUTE
.*
.np
The "DISTRIBUTE" option specifies that object modules extracted from
library files are to be distributed throughout the overlay structure.
The format of the "DISTRIBUTE" option (short form "DIS") is as
follows.
.mbigbox
    OPTION DISTRIBUTE
.embigbox
.np
An object module extracted from a library file will be placed in the
overlay section that satisfies the following conditions.
.autonote
.note
The symbols defined in the object module are not referenced by an
ancestor of the overlay section selected to contain the object module.
.note
At least one symbol in the object module is referenced by an immediate
descendant of the overlay section selected to contain the module.
.endnote
.pc
Note that libraries specified in the "FIXEDLIB" directive will not be
distributed.
Also, if a symbol defined in a library module is referenced indirectly
(its address is taken), the module extracted from the library will be
placed in the root unless the "NOINDIRECT" option is specified.
For more information on the "NOINDIRECT" option,
see the section entitled :HDREF refid='xnoindi'..
.np
For more information on overlays,
see the section entitled :HDREF refid='useover'..
