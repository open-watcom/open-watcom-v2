.np
.ix 'fatal errors'
.ix 'errors'
.ix 'warnings'
The &lnkname issues three classes of messages; fatal errors, errors
and warnings.
Each message has a 4-digit number associated with it.
Fatal messages start with the digit 3, error messages start with the
digit 2, and warning messages start with the digit 1.
It is possible for a message to be issued as a warning or an error.
.np
If a fatal error occurs, the linker will terminate immediately and no
executable file will be generated.
.np
If an error occurs, the linker will continue to execute so that all
possible errors are issued.
However, no executable file will be generated since these errors do
not permit a proper executable file to be generated.
.np
If a warning occurs, the linker will continue to execute.
A warning message is usually informational and does not prevent the
creation of a proper executable file.
However, all warnings should eventually be corrected.
