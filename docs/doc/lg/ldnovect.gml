.*
.*
.dirctv NOVECTOR
.*
.np
The "NOVECTOR" directive forces the &lnkname to not generate an
overlay vector for the specified symbols.
The format of the "NOVECTOR" directive (short form "NOV") is as
follows.
.mbigbox
    NOVECTOR symbol_name{,symbol_name}
.embigbox
.synote
.mnote symbol_name
is a symbol name.
.esynote
.np
The linker will create an overlay vector in the following cases.
.autonote
.note
If a function in section A calls a function in section B and section B
is not an ancestor of section A, an overlay vector will be generated
for the function in section B.
See the section entitled :HDREF refid='useover'. for a description of
ancestor.
.note
If a global symbol's address is referenced (except by a direct call)
and that symbol is defined in an overlay section, an overlay vector
for that symbol will be generated.
.endnote
.np
Note that in the latter case, more overlay vectors may be generated
that necessary.
Suppose section A contains three global functions,
.sy f,
.sy g
and
.sy h.
Function
.sy f
passes the address of function
.sy g
to function
.sy h
who can then calls function
.sy g
indirectly.
Also, suppose function
.sy g
is only called from sections that are ancestors of section A.
The linker will generate an overlay vector for function
.sy g
even though none is required.
In such a case, the "NOVECTOR" directive can be used to remove the
overhead associated with calling a function through an overlay vector.
