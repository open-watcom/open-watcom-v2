.*
.*
.option VFREMOVAL
.*
.np
.ix 'virtual functions'
The "VFREMOVAL" option instructs the linker to remove unused C++
virtual functions.
The format of the "VFREMOVAL" option (short form "VFR") is as follows.
.mbigbox
    OPTION VFREMOVAL
.embigbox
.np
If the "VFREMOVAL" option is specified, the linker will attempt to
eliminate unused virtual functions.
In order for the linker to do this, the &cmppname "zv" compiler option
must be used for
.us all
object files in the executable.
The "VFREMOVAL" option works best in concert with the "ELIMINATE"
option.
