:cmt. test for .pe
:GDOC.
:BODY.
.pe
.ty pe only;.pe delete

.se c=0
.pe on
.ty c;.se c=&c+1;.if &c > 3 .pe delete

.sr i=0;.pe 10;.ty .pp;.pe 100;.sr i=&i+1;&i
:egdoc
