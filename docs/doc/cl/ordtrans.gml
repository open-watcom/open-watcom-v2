.ix 'order of translation'
.ix 'translation order'
.pp
This chapter describes the sequence of steps that the C compiler
takes in order to translate a set of source files. Most programmers
do not need to thoroughly understand these steps, as they are
intuitive. However, occasionally it will be necessary to examine
the sequence to solve a problem in the translation process.
.pp
Even though the steps of translation are listed as separate phases,
the compiler may combine them together. However, this should be
transparent to the user.
.pp
The following are the phases of translation:
.autonote
.note
The characters of the source file(s) are mapped to the source
character set. Any end-of-line markers used in the file system
are translated, as necessary, to new-line characters. Any
.ix 'trigraphs'
trigraphs
are replaced by the appropriate single character.
.note
Physical source lines are
.ix 'continuation lines'
.ix 'line' 'continuation'
joined together
wherever a line is terminated by a backslash (
..ct .mono \
..ct )
character.
Effectively, the
.mono \
and the new-line character are deleted,
creating a longer line from that record and the one following.
.note
The source is broken down into preprocessing tokens and sequences of
"white-space" (space and tab) characters (including comments).
Each token is the longest sequence of characters that can be a token.
Each comment is replaced by one white-space character. The new-line
characters are retained at this point.
.note
Preprocessing directives are executed
and macro invocations are substituted.
A header named in a
.kwpp #include
directive is processed according to rules 1 to 4.
.note
.ix 'escape sequences'
Members of the source character set and
escape sequences in character constants and string literals are
converted to single characters in the execution character set.
.note
Adjacent character string literal tokens
and adjacent wide string literal tokens
are concatenated.
.note
White-space characters separating tokens are discarded.
Each preprocessing token is converted to a token.
The tokens are translated according to the syntactic and semantic rules.
.endnote
.pp
The final phase usually occurs outside of the compilation phase.
In this phase, often called the
.ix 'linking'
.us linking
phase, all external object definitions are resolved, and an executable
program image is created. The completed image contains all the
information necessary to run the program in the appropriate execution
environment.
