.*
.*
.option AUTOUNLOAD
.*
.np
The "AUTOUNLOAD" option specifies that a NetWare Loadable Module
(NLM) built with this option should automatically be unloaded when
all of its entry points are no longer in use. This only applies
if the NLM was automatically loaded by another modules loading.
.np
The format of the "AUTOUNLOAD" option (short form "AUTOUN") is as
follows.
.mbigbox
    OPTION AUTOUNLOAD
.embigbox
