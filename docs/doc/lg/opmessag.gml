.*
.*
.option MESSAGES
.*
.np
The "MESSAGES" option specifies the file name of an internationalized
message file that contains the default messages for the NLM.
This is the name of the default message file to load for NLMs that are
enabled.
Enabling allows the same NLM to display messages in different
languages by switching message files.
.np
The format of the "MESSAGES" option (short form "MES") is as follows.
.mbigbox
    OPTION MESSAGES=msg_file
.embigbox
.synote
.mnote msg_file
is the name of the message file.
.esynote
