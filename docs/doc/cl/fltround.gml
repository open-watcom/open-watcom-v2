.if &e'&nobox eq 0 .do begin
.boxdef
..if '&format' eq '7x9' ..th ..do begin
.  .boxcol 10 c
.  .boxcol 15
..do end
..el ..do begin
.  .boxcol 12 c
.  .boxcol 17
..do end
.boxbeg
.monoon
$FLT_ROUNDS$:HP0.Technique:eHP0.
.monooff
.boxline
.monoon
$-1$:HP0.indeterminable:eHP0.
$ 0$:HP0.toward zero:eHP0.
$ 1$:HP0.to nearest number:eHP0.
$ 2$:HP0.toward positive infinity:eHP0.
$ 3$:HP0.toward negative infinity:eHP0.
.monooff
.boxend
.do end
.el .do begin
.millust begin
FLT_ROUNDS      Technique
----------  ------------------------
    -1      indeterminable
     0      toward zero
     1      to nearest number
     2      toward positive infinity
     3      toward negative infinity
.millust end
.do end
