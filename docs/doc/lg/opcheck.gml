.*
.*
.option CHECK
.*
.np
The "CHECK" option specifies the name of a procedure to execute before
an NLM is unloaded.
This procedure can, for example, inform the operator that the NLM is
in use and prevent it from being unloaded.
.np
The format of the "CHECK" option (short form "CH") is as follows.
.mbigbox
    OPTION CHECK=symbol_name
.embigbox
.synote
.mnote symbol_name
specifies the name of a procedure to execute before the NLM is
unloaded.
.esynote
.np
If the "CHECK" option is not specified, no check procedure will be
called.
