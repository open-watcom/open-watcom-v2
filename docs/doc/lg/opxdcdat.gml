.*
.*
.option XDCDATA
.*
.np
The "XDCDATA" option specifies the name of a file that contains Remote
Procedure Call (RPC) descriptions for calls in this NLM.
RPC descriptions for APIs make it possible for APIs to be exported
across memory-protection domain boundaries.
.np
The format of the "XDCDATA" option (short form "XDC") is as follows.
.mbigbox
    OPTION XDCDATA=rpc_file
.embigbox
.synote
.mnote rpc_file
is the name of the file containing RPC descriptions.
.esynote
