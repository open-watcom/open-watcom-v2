.*
.*
.option NLMFLAGS
.*
.np
The "NLMFLAGS" option is used to set bits in the flags field of the
header of the Netware executable file.
The format of the "NLMFLAGS" option (short form "NLMF") is as follows.
.mbigbox
    OPTION NLMFLAGS=some_value
.embigbox
.synote
.mnote some_value
is an integer value that is OR'ed into the flags field of the header
of the Netware executable.
.esynote
