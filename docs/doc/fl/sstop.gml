.section STOP Statement
.*
.mbox begin
      STOP [n]
.mbox end
.synote
.mnote n
is a character constant or an unsigned integer constant of
no more than five digits.
.np
.xt begin
&product allows
.id n
to be any unsigned integer constant.
.xt end
.endnote
.np
Execution of a
.kw STOP
statement causes termination of execution of the program.
A
.kw STOP
statement may appear in any program unit (although good programming
practice suggests that the main program is the proper place for this
statement).
.exam begin
      STOP 943
.exam end
.pc
The three digit number
.mono 943
is displayed on the console prior to program termination.
.exam begin
      STOP 'Finished at last'
.exam end
.pc
The character string
.millust Finished at last
is displayed on the console prior to program termination.
