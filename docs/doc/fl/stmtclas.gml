.np
The following table is a summary of &product statement classification.
.begnote $setptnt 11
.note Column 1
indicates that the statement is a specification statement.
.note Column 2
indicates that the statement is not allowed as the terminal statement
of a DO-loop.
.note Column 3
indicates that the statement is not executable.
.note Column 4
indicates that the statement is not allowed as the object of a logical
.kw IF
.br
.xt on
or
.kw WHILE
.xt off
statement.
.note Column 5
indicates that the statement cannot have control of execution transferred
to it by using a statement label.
.note Column 6
indicates that the statement is allowed in a block data subprogram.
.endnote
.if &e'&dohelp eq 0 .do begin
.* .box on 1 20 25 30 35 40 45 50
.sr c0=&INDlvl+1
.sr c1=&INDlvl+20
.sr c2=&INDlvl+25
.sr c3=&INDlvl+30
.sr c4=&INDlvl+35
.sr c5=&INDlvl+40
.sr c6=&INDlvl+45
.sr c7=&INDlvl+50
.box on &c0 &c1 &c2 &c3 &c4 &c5 &c6 &c7
\Statement            \  1   \  2   \  3   \  4   \  5   \  6
.box
\ADMIT                \      \  *   \      \  *   \  *   \      \
\ALLOCATE             \      \      \      \      \      \      \
\ASSIGN               \      \      \      \      \      \      \
\AT END               \      \  *   \      \  *   \  *   \      \
\BACKSPACE            \      \      \      \      \      \      \
\BLOCK DATA           \      \  *   \  *   \  *   \  *   \      \
\CALL                 \      \      \      \      \      \      \
\CASE                 \      \  *   \      \  *   \  *   \      \
\CHARACTER            \  *   \  *   \  *   \  *   \  *   \  *   \
\CLOSE                \      \      \      \      \      \      \
\COMMON               \  *   \  *   \  *   \  *   \  *   \  *   \
.box off
.pa
.box on &c0 &c1 &c2 &c3 &c4 &c5 &c6 &c7
\Statement            \  1   \  2   \  3   \  4   \  5   \  6
.box
\COMPLEX              \  *   \  *   \  *   \  *   \  *   \  *   \
\CONTINUE             \      \      \      \      \      \      \
\CYCLE                \      \      \      \      \      \      \
\DATA                 \      \  *   \  *   \  *   \  *   \  *   \
\DEALLOCATE           \      \      \      \      \      \      \
\DIMENSION            \  *   \  *   \  *   \  *   \  *   \  *   \
\DO                   \      \  *   \      \  *   \      \      \
\DOUBLE COMPLEX       \  *   \  *   \  *   \  *   \  *   \  *   \
\DOUBLE PRECISION     \  *   \  *   \  *   \  *   \  *   \  *   \
\DO WHILE             \      \  *   \      \  *   \      \      \
\ELSE                 \      \  *   \      \  *   \  *   \      \
\ELSE IF              \      \  *   \      \  *   \  *   \      \
\END                  \      \  *   \      \  *   \      \  *   \
\END AT END           \      \  *   \      \  *   \  *   \      \
\END BLOCK            \      \  *   \      \  *   \  *   \      \
\END DO               \      \      \      \  *   \  *   \      \
\ENDFILE              \      \      \      \      \      \      \
\END GUESS            \      \  *   \      \  *   \      \      \
\END IF               \      \  *   \      \  *   \      \      \
\END LOOP             \      \  *   \      \  *   \  *   \      \
\END MAP              \  *   \  *   \  *   \  *   \  *   \  *   \
\END SELECT           \  *   \  *   \  *   \  *   \  *   \      \
\END STRUCTURE        \  *   \  *   \  *   \  *   \  *   \  *   \
\END UNION            \      \  *   \      \  *   \  *   \  *   \
\END WHILE            \      \  *   \      \  *   \  *   \      \
\ENTRY                \      \  *   \      \  *   \  *   \      \
\EQUIVALENCE          \  *   \  *   \  *   \  *   \  *   \  *   \
\EXECUTE              \      \      \      \      \      \      \
\EXIT                 \      \      \      \      \      \      \
\EXTERNAL             \  *   \  *   \  *   \  *   \  *   \      \
\FORMAT               \      \  *   \  *   \  *   \  *   \      \
\FUNCTION             \      \  *   \  *   \  *   \  *   \      \
\assigned GO TO       \      \  *   \      \      \      \      \
\computed GO TO       \      \      \      \      \      \      \
\unconditional GO TO  \      \  *   \      \      \      \      \
\GUESS                \      \  *   \      \  *   \      \      \
\arithmetic IF        \      \  *   \      \      \      \      \
\logical IF           \      \      \      \  *   \      \      \
\block IF             \      \  *   \      \  *   \      \      \
.box off
.pa
.box on &c0 &c1 &c2 &c3 &c4 &c5 &c6 &c7
\Statement            \  1   \  2   \  3   \  4   \  5   \  6
.box
\IMPLICIT             \  *   \  *   \  *   \  *   \  *   \  *   \
\INCLUDE              \      \  *   \  *   \  *   \  *   \  *   \
\INQUIRE              \      \      \      \      \      \      \
\INTEGER              \  *   \  *   \  *   \  *   \  *   \  *   \
\INTRINSIC            \  *   \  *   \  *   \  *   \  *   \      \
\LOGICAL              \  *   \  *   \  *   \  *   \  *   \  *   \
\LOOP                 \      \  *   \      \  *   \      \      \
\MAP                  \  *   \  *   \  *   \  *   \  *   \  *   \
\NAMELIST             \  *   \  *   \  *   \  *   \  *   \      \
\OPEN                 \      \      \      \      \      \      \
\OTHERWISE            \      \  *   \      \  *   \  *   \      \
\PARAMETER            \  *   \  *   \  *   \  *   \  *   \  *   \
\PAUSE                \      \      \      \      \      \      \
\PRINT                \      \      \      \      \      \      \
\PROGRAM              \      \  *   \  *   \  *   \  *   \      \
\QUIT                 \      \      \      \      \      \      \
\READ                 \      \      \      \      \      \      \
\REAL                 \  *   \  *   \  *   \  *   \  *   \  *   \
\RECORD               \  *   \  *   \  *   \  *   \  *   \  *   \
\REMOTE BLOCK         \      \  *   \  *   \  *   \  *   \      \
\RETURN               \      \  *   \      \      \      \      \
\REWIND               \      \      \      \      \      \      \
\SAVE                 \  *   \  *   \  *   \  *   \  *   \  *   \
\SELECT               \      \  *   \      \  *   \      \      \
\STOP                 \      \  *   \      \      \      \      \
\STRUCTURE            \  *   \  *   \  *   \  *   \  *   \  *   \
\SUBROUTINE           \      \  *   \  *   \  *   \  *   \      \
\UNION                \  *   \  *   \  *   \  *   \  *   \  *   \
\UNTIL                \      \  *   \      \  *   \  *   \      \
\VOLATILE             \  *   \  *   \  *   \  *   \  *   \  *   \
\WHILE                \      \  *   \      \  *   \      \      \
\WRITE                \      \      \      \      \      \      \
.box off
.do end
.el .do begin
.code begin
+---------------------+------+------+------+------+------+------+
|Statement            |  1   |  2   |  3   |  4   |  5   |  6   |
+---------------------+------+------+------+------+------+------+
|ADMIT                |      |  *   |      |  *   |  *   |      |
|ALLOCATE             |      |      |      |      |      |      |
|ASSIGN               |      |      |      |      |      |      |
|AT END               |      |  *   |      |  *   |  *   |      |
|BACKSPACE            |      |      |      |      |      |      |
|BLOCK DATA           |      |  *   |  *   |  *   |  *   |      |
|CALL                 |      |      |      |      |      |      |
|CASE                 |      |  *   |      |  *   |  *   |      |
|CHARACTER            |  *   |  *   |  *   |  *   |  *   |  *   |
|CLOSE                |      |      |      |      |      |      |
|COMMON               |  *   |  *   |  *   |  *   |  *   |  *   |
+---------------------+------+------+------+------+------+------+
.code end
.code begin
+---------------------+------+------+------+------+------+------+
|Statement            |  1   |  2   |  3   |  4   |  5   |  6
+---------------------+------+------+------+------+------+------+
|COMPLEX              |  *   |  *   |  *   |  *   |  *   |  *   |
|CONTINUE             |      |      |      |      |      |      |
|CYCLE                |      |      |      |      |      |      |
|DATA                 |      |  *   |  *   |  *   |  *   |  *   |
|DEALLOCATE           |      |      |      |      |      |      |
|DIMENSION            |  *   |  *   |  *   |  *   |  *   |  *   |
|DO                   |      |  *   |      |  *   |      |      |
|DOUBLE COMPLEX       |  *   |  *   |  *   |  *   |  *   |  *   |
|DOUBLE PRECISION     |  *   |  *   |  *   |  *   |  *   |  *   |
|DO WHILE             |      |  *   |      |  *   |      |      |
|ELSE                 |      |  *   |      |  *   |  *   |      |
|ELSE IF              |      |  *   |      |  *   |  *   |      |
|END                  |      |  *   |      |  *   |      |  *   |
|END AT END           |      |  *   |      |  *   |  *   |      |
|END BLOCK            |      |  *   |      |  *   |  *   |      |
|END DO               |      |      |      |  *   |  *   |      |
|ENDFILE              |      |      |      |      |      |      |
|END GUESS            |      |  *   |      |  *   |      |      |
|END IF               |      |  *   |      |  *   |      |      |
|END LOOP             |      |  *   |      |  *   |  *   |      |
|END MAP              |  *   |  *   |  *   |  *   |  *   |  *   |
|END SELECT           |  *   |  *   |  *   |  *   |  *   |      |
|END STRUCTURE        |  *   |  *   |  *   |  *   |  *   |  *   |
|END UNION            |      |  *   |      |  *   |  *   |  *   |
|END WHILE            |      |  *   |      |  *   |  *   |      |
|ENTRY                |      |  *   |      |  *   |  *   |      |
|EQUIVALENCE          |  *   |  *   |  *   |  *   |  *   |  *   |
|EXECUTE              |      |      |      |      |      |      |
|EXIT                 |      |      |      |      |      |      |
|EXTERNAL             |  *   |  *   |  *   |  *   |  *   |      |
|FORMAT               |      |  *   |  *   |  *   |  *   |      |
|FUNCTION             |      |  *   |  *   |  *   |  *   |      |
|assigned GO TO       |      |  *   |      |      |      |      |
|computed GO TO       |      |      |      |      |      |      |
|unconditional GO TO  |      |  *   |      |      |      |      |
|GUESS                |      |  *   |      |  *   |      |      |
|arithmetic IF        |      |  *   |      |      |      |      |
|logical IF           |      |      |      |  *   |      |      |
|block IF             |      |  *   |      |  *   |      |      |
+---------------------+------+------+------+------+------+------+
.code end
.code begin
+---------------------+------+------+------+------+------+------+
|Statement            |  1   |  2   |  3   |  4   |  5   |  6
+---------------------+------+------+------+------+------+------+
|IMPLICIT             |  *   |  *   |  *   |  *   |  *   |  *   |
|INCLUDE              |      |  *   |  *   |  *   |  *   |  *   |
|INQUIRE              |      |      |      |      |      |      |
|INTEGER              |  *   |  *   |  *   |  *   |  *   |  *   |
|INTRINSIC            |  *   |  *   |  *   |  *   |  *   |      |
|LOGICAL              |  *   |  *   |  *   |  *   |  *   |  *   |
|LOOP                 |      |  *   |      |  *   |      |      |
|MAP                  |  *   |  *   |  *   |  *   |  *   |  *   |
|NAMELIST             |  *   |  *   |  *   |  *   |  *   |      |
|OPEN                 |      |      |      |      |      |      |
|OTHERWISE            |      |  *   |      |  *   |  *   |      |
|PARAMETER            |  *   |  *   |  *   |  *   |  *   |  *   |
|PAUSE                |      |      |      |      |      |      |
|PRINT                |      |      |      |      |      |      |
|PROGRAM              |      |  *   |  *   |  *   |  *   |      |
|QUIT                 |      |      |      |      |      |      |
|READ                 |      |      |      |      |      |      |
|REAL                 |  *   |  *   |  *   |  *   |  *   |  *   |
|RECORD               |  *   |  *   |  *   |  *   |  *   |  *   |
|REMOTE BLOCK         |      |  *   |  *   |  *   |  *   |      |
|RETURN               |      |  *   |      |      |      |      |
|REWIND               |      |      |      |      |      |      |
|SAVE                 |  *   |  *   |  *   |  *   |  *   |  *   |
|SELECT               |      |  *   |      |  *   |      |      |
|STOP                 |      |  *   |      |      |      |      |
|STRUCTURE            |  *   |  *   |  *   |  *   |  *   |  *   |
|SUBROUTINE           |      |  *   |  *   |  *   |  *   |      |
|UNION                |  *   |  *   |  *   |  *   |  *   |  *   |
|UNTIL                |      |  *   |      |  *   |  *   |      |
|VOLATILE             |  *   |  *   |  *   |  *   |  *   |  *   |
|WHILE                |      |  *   |      |  *   |      |      |
|WRITE                |      |      |      |      |      |      |
+---------------------+------+------+------+------+------+------+
.code end
.do end
