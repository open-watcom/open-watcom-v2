.*
.*
.dirctv ALIAS
.*
.np
The "ALIAS" directive is used to specify an equivalent name for a symbol name.
The format of the "ALIAS" directive (short form "A") is as follows.
.mbigbox
    ALIAS alias_name=symbol_name{, alias_name=symbol_name}
.embigbox
.synote
.mnote alias_name
is the alias name.
.mnote symbol_name
is the symbol name to which the alias name is mapped.
.esynote
.np
Consider the following example.
.millust begin
alias sine=mysine
.millust end
.np
When the linker tries to resolve the reference to
.id sine
.ct ,
it will immediately substitute the name
.id mysine
for
.id sine
and begin searching for the symbol
.id mysine
.ct .li .
