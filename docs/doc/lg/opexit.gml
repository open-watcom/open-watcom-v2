.*
.*
.option EXIT
.*
.np
The format of the "EXIT" option (short form "EX") is as follows.
.mbigbox
    OPTION EXIT=symbol_name
.embigbox
.synote
.mnote symbol_name
specifies the name of the procedure that is executed when an NLM is
unloaded.
.esynote
.np
The default name of the exit procedure is "_Stop".
.np
Note that the exit procedure cannot prevent the NLM from being
unloaded.
Once the exit procedure has executed, the NLM will be unloaded.
The "CHECK" option can be used to specify a check procedure that can
prevent an NLM from being unloaded.
