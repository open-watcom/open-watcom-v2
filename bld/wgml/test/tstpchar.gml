.* test for gml stop characters
.se fil=&'left(&sysfnam,&'lastpos('.',&sysfnam)-1)
:layout
:convert file="&fil..lay".
:elayout
:gdoc sec='top secret'
:body
.dm br /.ty br is noop/
:p.
fullstop. fullstop
.br
comma, comma
.br
question? question
.br
exclamation! exclamation
.br
colon: colon
.br
colonspace : colonspace
.br
semicolon; semicolon
:egdoc
