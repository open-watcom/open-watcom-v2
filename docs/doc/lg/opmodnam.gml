.*
.*
.*
.option MODNAME
.*
.np
The "MODNAME" option specifies a name to be given to the module being
created.
The format of the "MODNAME" option (short form "MODN") is as follows.
.mbigbox
    OPTION MODNAME=module_name
.embigbox
.synote
.mnote module_name
is the name of a Dynamic Link Library.
.esynote
.np
Once a module has been loaded (whether it be a program module or a
Dynamic Link Library),
.sy mod_name
is the name of the module known to the operating system.
If the "MODNAME" option is not used to specify a module name, the
default module name is the name of the executable file without the
file extension.
