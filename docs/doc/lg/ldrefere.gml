.*
.*
.dirctv REFERENCE
.*
.np
The "REFERENCE" directive is used to explicitly reference a symbol
that is not referenced by any object file processed by the linker.
If any symbol appearing in a "REFERENCE" directive is not resolved by
the linker, an error message will be issued for that symbol specifying
that the symbol is undefined.
.np
The "REFERENCE" directive can be used to force object files from
libraries to be linked with the application.
Also note that a symbol appearing in a "REFERENCE" directive will not
be eliminated by dead code elimination.
.ix 'dead code elimination'
For more information on dead code elimination,
see the section entitled :HDREF refid='xelimin'..
.np
The format of the "REFERENCE" directive (short form "REF") is as
follows.
.mbigbox
    REFERENCE symbol_name{, symbol_name}
.embigbox
.synote
.mnote symbol_name
is the symbol for which a reference is made.
.esynote
.np
Consider the following example.
.millust begin
reference domino
.millust end
.np
The symbol
.id domino
will be searched for.
The object module that defines this symbol will be linked with the
application.
Note that the linker will also attempt to resolve symbols referenced
by this module.
