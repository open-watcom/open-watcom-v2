.pp
The following is the list of trigraphs.
In a C source file, all occurrences (including inside quoted strings
and character constants)
of any of the trigraph sequences
below are replaced by the corresponding single character.
.im trigraph
.pc
No other trigraphs exist. Any question mark (
..ct .mono ?
..ct )
that does not belong
to one of the trigraphs is not changed.
.pp
To get a sequence of characters that would otherwise be a trigraph,
place a
.mono \
before the second question mark.
This will cause the trigraph to be broken up so that it is not recognized,
but later in the translation process, the
.monoon
\?&SYSRB.
.monooff
will be converted to
.mono ?.
For example,
.monoon
?\?=
.monooff
will be translated to
.mono ??=.
