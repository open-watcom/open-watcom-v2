.section FORMAT Statement
.*
.mbox begin
      label    FORMAT fs
.mbox end
.synote 7
.mnote fs
is a format specification and is described in the
chapter entitled :HDREF refid='fformat'..
.mnote label
is the statement label used by an I/O statement to identify the
.kw FORMAT
statement to be used.
The
.kw FORMAT
statement must be labelled.
.endnote
.exam begin
      REAL X
      X = 234.43
      PRINT 100, X
100   FORMAT(F10.2)
      END
.exam end
.pc
In the previous example, the
.kw PRINT
statement uses the format specification in the
.kw FORMAT
statement whose statement label is 100 to display the value of
.id X.
.np
For more information on the
.kw FORMAT
statement, see the chapter entitled :HDREF refid='fformat'..
