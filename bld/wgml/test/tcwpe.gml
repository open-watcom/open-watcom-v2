:cmt. test for .pe   -- nested .pe not supported
.se c=0
.pe
.ty pe only &c;.pe delete

.pe on
.ty c=&c;.se c=&c+1;.if &c > 3 .pe delete

.pe 3
.sr c=&c+&c;.ty &c.

.sr i=0;.pe 10;.ty .pp;.pe 100;.sr i=&i+1;&i
