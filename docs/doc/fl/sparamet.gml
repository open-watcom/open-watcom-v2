.section PARAMETER Statement
.*
.mbox begin
      PARAMETER (p=e [,p=e] ...)
.mbox end
.synote
.mnote p
is a symbolic name.
.mnote e
is a constant expression.
Refer to the chapter entitled :HDREF refid='fexpr'. for more information.
.endnote
.np
.id p
is known as a symbolic constant whose value is determined by the
value of the expression
.id e
according to the rules of assignment as described in the chapter
entitled :HDREF refid='fassmnt'..
Any symbolic constant appearing in expression
.id e
must have been previously defined in the same or a previous
.kw PARAMETER
statement in the same program unit.
A symbolic constant may not be defined more than once in a program unit.
.np
If the symbolic name
.id p
is of type integer, real, double precision or complex then the
corresponding expression
.id e
must be an arithmetic constant expression
(see the chapter entitled :HDREF refid='fexpr'.).
If the symbolic name
.id p
is of type character or logical then the expression
.id e
must be a character constant expression or a logical constant
expression respectively
(see the chapter entitled :HDREF refid='fexpr'.).
.exam begin
      PARAMETER (PI=3.14159,BUFFER=80,PIBY2=PI/2)
      PARAMETER (ERRMSG='AN ERROR HAS OCCURRED')
.exam end
.np
If a symbolic constant is not of default implied type, its type
must be specified in an
.kw IMPLICIT
statement or a type statement before its occurrence in a
.kw PARAMETER
statement.
Similarly, if the length of a character symbolic constant is not the
default length of 1, its length must be specified in an
.kw IMPLICIT
statement or a type statement before its occurrence in a
.kw PARAMETER
statement.
