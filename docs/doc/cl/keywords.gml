.ix 'keyword' 'list of'
.ix 'keyword'
.ix 'reserved identifier'
.pp
The following words are reserved as part of the C language and are
called
.us keywords.
They may not be used for any kind of identifier, including
object names, function names, labels, structure or union
tags (names).
.ix 'identifier'
.sk 1
.im kwlist
.sk 1
.*
.************************************************************************
.*
..if '&target' eq 'PC' or '&target' eq 'PC 370' ..th ..do begin
The &company. compilers also reserve the following extended keywords:
.sk 1
.im kwwatcom
.sk 1
..do end
.*
.************************************************************************
.*
.pp
Note that, since C is sensitive to the
case
.ix 'case sensitive'
of letters,
changing one or more letters in a keyword to upper case will prevent
the compiler from recognizing it as a keyword,
thereby allowing it to be used
as an identifier. However, this is not a recommended
programming practice.
