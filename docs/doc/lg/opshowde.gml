.*
.*
.option SHOWDEAD
.*
.np
.ix 'dead code elimination'
.ix 'virtual functions'
The "SHOWDEAD" option instructs the linker to list, in the map file,
the symbols associated with dead code and unused C++ virtual functions
that it has eliminated from the link.
The format of the "SHOWDEAD" option (short form "SHO") is as follows.
.mbigbox
    OPTION SHOWDEAD
.embigbox
.np
The "SHOWDEAD" option works best in concert with the "ELIMINATE"
and "VFREMOVAL" options.
