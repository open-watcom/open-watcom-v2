.*
.*
.dirctv DISABLE
.*
.np
The "DISABLE" directive is used to disable the display of linker messages.
.im wlerrfmt
.np
Note that the behaviour of the linker does not change when a message is
disabled.
For example, if a message that normally terminates the linker is disabled,
the linker will still terminate but the message describing the reason for
the termination will not be displayed.
For this reason, you should only disable messages that are warnings.
.np
The linker will ignore the severity of the message number.
For example, some messages can be displayed as errors or warnings.
It is not possible to disable the message when it is issued as a warning
and display the message when it is issued as an error.
In general, do not specify the severity of the message when specifying a
message number.
.np
The format of the "DISABLE" directive (short form "DISA") is as follows.
.mbigbox
    DISABLE msg_num{, msg_num}
.embigbox
.synote
.mnote msg_num
is a message number.
See the chapter entitled :HDREF refid='lerrmsg'. for a list of
messages and their corresponding numbers.
.esynote
.np
The following "DISABLE" directive will disable message 28 (an undefined
symbol has been referenced).
.millust begin
disable 28
.millust end
