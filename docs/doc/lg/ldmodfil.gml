.*
.*
.dirctv MODFILE
.*
.np
The "MODFILE" directive instructs the linker that only the specified
object files have changed.
The format of the "MODFILE" directive (short form "MODF") is as follows.
.mbigbox
    MODFILE obj_file{,obj_file}
.embigbox
.synote
.im objfile
.esynote
.np
This directive is used only in concert with incremental linking.
This directive tells the linker that only the specified object files
have changed.
When this option is specified, the linker will not check the dates on
any of the object files or libraries when incrementally linking.
