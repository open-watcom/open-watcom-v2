.*
.*
.option REENTRANT
.*
.np
The "REENTRANT" option specifies that the module is reentrant.
That is, if an NLM is LOADed twice, the actual code in the server's
memory is reused.
The NLM's start procedure is called once for each LOAD.
The format of the "REENTRANT" option (short form "RE") is as follows.
.mbigbox
    OPTION REENTRANT
.embigbox
