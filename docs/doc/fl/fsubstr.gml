.chap Character Substrings
.*
.if &e'&dohelp eq 0 .do begin
.section Introduction
.do end
.*
.np
A
.us substring
.ix 'substring'
is a contiguous portion of a character entity.
The substring operation selects a substring from a character entity.
The resulting substring can then be treated as a character entity in
itself.
Substringing also allows the replacement of substrings from character
entities with other character entities.
.*
.section Substring Names
.*
.np
Substrings are formed by specifying a substring name.
The forms of a
.us substring name
.ix 'substring name'
are:
.mbox begin
      v( [ e1] : [ e2] )
      a(s [,s] ...)( [ e1] : [ e2] )
.mbox end
.synote 15
.mnote v
is a character variable name.
.mnote a(s[,s]...)
is a character array element name.
.mnote e1
is an integer expression identifying the leftmost character of the
substring.
.mnote e2
is an integer expression identifying the rightmost character of the
substring.
.endnote
.np
.id e1
and
.id e2
are called
.us substring expressions.
.ix 'substring expression'
They must be such that
.millust begin
1 <= e1 <= e2 <= len
.millust end
.pc
where
.id len
is the length of the character entity.
If
.id e1
is omitted, a value of 1 is assumed.
If
.id e2
is omitted, a value of
.id len
is assumed.
Both
.id e1
and
.id e2
may be omitted.
The length of the substring is
.id e2
-
.id e1
+ 1.
.cp 25
.exam begin
      CHARACTER A*8, B(4)*8, C*14
* A gets the string 'EVERYDAY'
      A = 'EVERYDAY'
* Replace 'DAY' with 'ONE' in A
      A(6:8) = 'ONE'
* B(1) gets the string 'OTHELLO'
      B(1) = 'OTHELLO'
* B(2) gets same value as B(1)
      B(2)(:) = 'OTHELLO'
* B(3) gets last 6 characters of B(1)
      B(3) = B(1)(3:8)
* B(4) gets first 4 characters of B(1)
*     concatenated with the letter 'R'
      B(4) = B(1)(1:4) // 'R'
* C gets last 6 characters of B(1)
*     concatenated with the variable A
      C = B(1)(3:) // A
* Print out the results
      PRINT *, A
      PRINT '(A8)', B
      PRINT *, C
      END
.exam end
.*
.section Extensions
.*
.np
.xt begin
&product allows an external character function reference or a
character statement function reference as part of the substring name
(see the chapter entitled :HDREF refid='fsubp'.. for more
information).
.xt end
.cp 12
.exam begin
      CHARACTER*10 F,G
      CHARACTER*10 X
*
* DEFINE CHARACTER STATEMENT FUNCTION
*
      G(X) = X
*
      PRINT *, F('0123456789')(1:5)
      PRINT *, G('0123456789')(6:10)
      END
.exam break
*
* DEFINE CHARACTER EXTERNAL FUNCTION
*
      CHARACTER*(*) FUNCTION F( X )
      CHARACTER*10 X
      F = X
      END
.exam end
