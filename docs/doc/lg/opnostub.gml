.*
.*
.option NOSTUB
.*
.np
The "NOSTUB" option specifies that no "stub" program is to be placed at the
beginning of the executable file being generated.
The format of the "NOSTUB" option is as
follows.
.mbigbox
    OPTION NOSTUB
.embigbox
.np
This option is helpful in cases when the executable file being generated
cannot be directly executed by the user, such as a device driver, and hence
the stub program would be redundant.
