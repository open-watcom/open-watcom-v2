.section CONTINUE Statement
.*
.mbox begin
      CONTINUE
.mbox end
.np
Execution of a
.kw CONTINUE
statement has no effect.
This statement is often used in conjunction with
.kw DO
statements.
It is usually identified with a label.
It often provides a convenient reference for statements which have the
ability to transfer control of execution.
.exam begin
      DO 10 X = -5.1, 12.8, 0.125
          .
          .
          .
10    CONTINUE

      IF( A .LT. B ) GO TO 20
      IF( A .GT. C ) GO TO 20
          .
          .
          .
20    CONTINUE
.exam end
