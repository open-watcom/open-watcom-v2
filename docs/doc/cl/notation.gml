.ix 'notation'
.pp
The C programming language contains many useful features, each of which
has a number of optional parts.
The ISO C standard describes
the language in very precise terms, often giving
syntax diagrams to describe the features.
.pp
This book attempts to describe the C language in more
friendly terms.
Where possible, features are described using ordinary English.
Jargon is avoided, although by necessity, new terminology is
introduced throughout the book.
A glossary is provided at the end of the book to describe any terms
that are used.
.pp
Where the variety of features would create excessive amounts
of text, simple syntax diagrams are used. It is hoped that these
are mostly self-explanatory.
However, a brief explanation of the notation used is offered here:
.autonote
.note
Required keywords are in normal lettering style
(for example,&SYSRB.
..ct .mono enum
..ct ).
.note
Terms that describe a class of object that replace the term are in
italics (for example,&SYSRB.
..ct .us identifier
..ct ).
.note
When two or more optional forms are available, they are shown
as follows:
.cillust begin
form 1
.cor
form 2
.cillust end
:CMT. .pp
:CMT. For example,
:CMT. .cillust begin
:CMT. .mono enum
:CMT. identifier
:CMT. .cor
:CMT. .mono enum
:CMT. :HP0.{:eHP0.
:CMT. enumeration-constant-list
:CMT. :HP0.}:eHP0.
:CMT. .cor
:CMT. .mono enum
:CMT. identifier
:CMT. :HP0.{:eHP0.
:CMT. enumeration-constant-list
:CMT. :HP0.}:eHP0.
:CMT. .cillust end
.note
Any other symbol that appears is required, unless otherwise noted.
.endnote
.keep begin
.pp
The following example is for an enumerated type:
.cillust begin
.mono enum
identifier
.cor
.mono enum
:HP0.{:eHP0.
enumeration-constant-list
:HP0.}:eHP0.
.cor
.mono enum
identifier
:HP0.{:eHP0.
enumeration-constant-list
:HP0.}:eHP0.
.cillust end
.keep end
.keep begin
.pc
An enumerated type has three forms:
.autonote
.note
The required keyword
.kw enum
followed by an identifier that names the type.
The identifier is chosen by the programmer.
.note
The required keyword
.kw enum
followed by a brace-enclosed list of enumeration constants.
The braces are required, and
.us enumeration-constant-list
is described elsewhere.
.note
The required keyword
.kw enum
followed by an identifier and a brace-enclosed
list of enumeration constants.
As with the previous two forms, the identifier may be chosen by the
programmer, the braces are required and
.us enumeration-constant-list
is described elsewhere.
.endnote
.keep end
