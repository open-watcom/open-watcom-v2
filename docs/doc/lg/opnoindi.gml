.*
.*
.option NOINDIRECT
.*
.np
The "NOINDIRECT" option suppresses the generation of overlay vectors
for symbols that are referenced indirectly (their address is taken)
when the module containing the symbol is not an ancestor of at least
one module that indirectly references the symbol.
This can greatly reduce the number of overlay vectors and is a safe
optimization provided there are no indirect calls to these symbols.
If, for example, the set of symbols that are called indirectly is
known, you can use the "VECTOR" option to force overlay vectors for
these symbols.
.np
The format of the "NOINDIRECT" option (short form "NOI") is as
follows.
.mbigbox
    OPTION NOINDIRECT
.embigbox
.np
For more information on overlays,
see the section entitled :HDREF refid='useover'..
