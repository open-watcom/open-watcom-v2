.*
.*
.dirctv MODTRACE
.*
.np
The "MODTRACE" directive instructs the &lnkname to print a list of all
modules that reference the symbols defined in the specified modules.
The format of the "MODTRACE" directive (short form "MODT") is as
follows.
.mbigbox
    MODTRACE  module_name{,module_name}
.embigbox
.synote
.mnote module_name
is the name of an object module defined in an object or library file.
.esynote
.np
The information is displayed in the map file.
Consider the following example.
.exam begin
&sysprompt.&lnkcmd &syst_drctv op map file test lib math modt trig
.exam end
.pc
If the module "trig" defines the symbols "sin" and "cos", the &lnkname
will list, in the map file, all modules that reference the symbols
"sin" and "cos".
