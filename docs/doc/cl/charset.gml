.ix 'character set'
.ix 'source character set'
.ix 'character set' 'source'
.pp
The
.us source character set
contains the characters used during the
translation of the C source file into object code. The
.ix 'execution character set'
.ix 'character set' 'execution'
.us execution character set
contains the characters used during the execution of the C program.
In most cases, these two character sets are the same, since the program
is compiled and executed on the same machine.
However, C is sometimes used to
.ix 'cross-compile'
.us cross-compile,
whereby the compilation of the program occurs on one machine, but the
compiler generates code for some other machine. If the two machines
have different character sets (say EBCDIC and ASCII), then the
compiler will, where appropriate, map characters from the source
character set to the execution character set. This mapping is
implementation-defined, but generally maps the visual representation
of the character.
.*
.************************************************************************
.*
..if '&target' eq 'generic' ..th ..do begin
.pp
For example, suppose a C compiler running on a machine with the EBCDIC
character set is cross-compiling to a machine with the ASCII character
set.
In EBCDIC, the
letter 'A' is represented by (hexadecimal) C1,
while in ASCII it is 41. Any character
constant or string will automatically have the letter 'A' (and all other
graphic characters) mapped from EBCDIC to ASCII equivalents.
Characters with no graphic representation
may also be mapped, but the
mapping is implementation-defined.
(An exception to this rule is the set of characters that produce actions
on display devices, such as
.us backspace
and
.us new line.
These characters may be entered using an escape sequence described in
the section "Character Constants", and are mapped to the appropriate
characters in the execution character set.)
..do end
.*
.************************************************************************
.*
.pp
Regardless of which C compiler is used, the
source and execution character sets
contain (at least) the following characters:
.millust begin
a b c d e f g h i j k l m n o p q r s t u v w x y z
A B C D E F G H I J K L M N O P Q R S T U V W X Y Z
0 1 2 3 4 5 6 7 8 9
! " # % & ' ( ) * + , - . /
: ; < = > ? [ \ ] ^ _ { | } ~~
.millust end
.pc
as well as the
.us space
(blank),
.ix 'horizontal tab'
.us horizontal tab,
.ix 'vertical tab'
.us vertical tab
and
.ix 'form feed'
.us form feed.
Also, a
.ix 'new line'
.us new line
character will exist for both the source and
execution character sets.
.pp
Any character other than those previously listed should appear in a
source file in a character constant,
a string or a comment, otherwise the behavior is undefined.
.pp
If the character set of the computer being used to compile the program
does not contain a certain character, a
.ix 'trigraphs'
.us trigraph
sequence may be used to represent it.
Refer to the section
"Character Constants".
.*
.************************************************************************
.*
..if '&target' eq 'PC' or '&target' eq 'PC 370' ..th ..do begin
.shade begin
The &wcboth. compilers use the full IBM PC character set as both the
source and execution character sets.
The set of values from
hexadecimal 00 to 7F constitute the ASCII character set.
.shade end
..do end
..if '&target' eq 'PC 370' ..th ..do begin
.shade begin
The &wlooc. compiler normally
uses the full EBCDIC character set as both the
source and execution character sets.
In addition, a compiler option permits the execution character set
to be the set of EBCDIC characters with ASCII equivalents.
.pp
The &wlooc. compiler also provides the following characters or
character sequences as substitutes for certain standard characters:
..sk 1 c
.* NOTE: I don't really know the proper way to do this box inside the
.*       "shade" box. This is a kludge, done by trial and error.
.*               Steve
..bx new &SYSIN.+5 +10 +8
..sr firsttab=&SYSIN.+10
..tb set $
..tb &firsttab.c +9c
$Character
$Sequence$Replaces
..bx
$(:$[
$ :)$]
$|$|
$&turnstile.$^
..tb set
..tb
..bx off
.shade end
..do end
.*
.************************************************************************
.*
.beglevel
.section Multibyte Characters
.*
.pp
.ix 'multibyte character'
A multibyte character, as its name implies, is a character whose
representation consists of more than one byte.
Multibyte characters allow compilers to provide extended character
sets, often for human languages that contain more characters than those
found in the one-byte character set.
.pp
Multibyte characters are generally restricted to:
.begbull $compact
.bull
.ix 'comment'
comments,
.bull
.ix 'string literal'
string literals,
.bull
.ix 'character constant'
character constants,
.bull
.ix 'header'
header names.
.endbull
..sk 1 c
The method for specifying multibyte characters generally varies
depending upon the extended character set.
.endlevel
