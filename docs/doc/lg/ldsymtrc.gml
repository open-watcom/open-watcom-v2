.*
.*
.dirctv SYMTRACE
.*
.np
The "SYMTRACE" directive instructs the &lnkname to print a list of all
modules that reference the specified symbols.
The format of the "SYMTRACE" directive (short form "SYMT") is as
follows.
.mbigbox
    SYMTRACE  symbol_name{,symbol_name}
.embigbox
.synote
.mnote symbol_name
is the name of a symbol.
.esynote
.np
The information is displayed in the map file.
Consider the following example.
.exam begin
&sysprompt.&lnkcmd &syst_drctv op map file test lib math symt sin, cos
.exam end
.pc
The &lnkname will list, in the map file, all modules that reference
the symbols "sin" and "cos".
