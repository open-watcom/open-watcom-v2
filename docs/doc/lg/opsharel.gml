.*
.*
.option SHARELIB
.*
.np
The "SHARELIB" option specifies the file name of an NLM to be loaded
as a shared NLM.
Shared NLMs contain global code and global data that are mapped into
all memory protection domains.
This method of loading APIs can be used to avoid ring transitions to
call other APIs in other domains.
.np
The format of the "SHARELIB" option (short form "SHA") is as follows.
.mbigbox
    OPTION SHARELIB=shared_nlm
.embigbox
.synote
.mnote shared_nlm
is the file name of the shared NLM.
.esynote
