..sk 1 c
.if &e'&nobox eq 0 .do begin
.boxdef
..if '&format' eq '7x9' ..th ..do begin
.  .boxcol 10
.  .boxcol 8
.  .boxcol 8
.  .boxcol 7
.  .boxcol 6
..do end
..el ..do begin
.  .boxcol 13
.  .boxcol 9
.  .boxcol 9
.  .boxcol 8
.  .boxcol 8
..do end
.keep break
.boxbeg
$           $Smallest$Largest $Digits  $80x87
$           $Absolute$Absolute$Of      $Type
$Type       $Value   $Value   $Accuracy$Name
.boxline
.monoon
$float      $1.1E-38 $3.4E+38 $6$:HP0.short real:eHP0.
.monooff
.boxline
.monoon
$double     $2.2E-308$1.7E+308$15$:HP0.long real:eHP0.
.monooff
..if '&target' eq 'PC'  or '&target' eq 'PC 370' ..th ..do begin
.boxline
.monoon
$long double$2.2E-308$1.7E+308$15$:HP0.long real:eHP0.
.monooff
..do end
.boxend
.do end
.el .do begin
.millust begin
              Smallest   Largest    Digits     80x87
              Absolute   Absolute   Of         Type
Type          Value      Value      Accuracy   Name
-----------   --------   --------   --------   ----------
float         1.1E-38    3.4E+38    6          short real
double        2.2E-308   1.7E+308   15         long real
..if '&target' eq 'PC'  or '&target' eq 'PC 370' ..th ..do begin
long double   2.2E-308   1.7E+308   15         long real
..do end
.millust end
.* .us &wcboth. floating-point types and ranges
..sk 1 c
