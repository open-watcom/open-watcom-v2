.section PAUSE Statement
.*
.mbox begin
      PAUSE [n]
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
.kw PAUSE
statement causes a cessation of execution of the program.
Execution of the program may be resumed by the program operator
by pressing the terminal line entering key (e.g., ENTER or
RETURN).
The
.kw PAUSE
statement may appear in any program unit.
.np
If the &product debugger was requested then execution of the
.kw PAUSE
statement will cause entry into the debugger.
Program execution may be resumed by issuing the debugger "go"
command.
.exam begin
      PAUSE 4341
.exam end
.pc
The four digit number
.mono 4341
is displayed on the terminal.
The program temporarily ceases execution.
Execution is resumed by pressing the terminal line entering key.
.exam begin
      PAUSE 'Ready the paper and then resume execution'
.exam end
.pc
The character string
.millust Ready the paper and then resume execution
is displayed on the terminal.
Execution of the program may be resumed.
