..sk 1 c
.keep begin
.if &e'&nobox eq 0 .do begin
.boxdef
.boxcol 13
.boxcol  9
.boxcol  9
.boxcol  9
.boxcol  9
.boxbeg
$           $Smallest$Largest $Digits  $370
$           $Absolute$Absolute$Of      $Type
$Type       $Value   $Value   $Accuracy$Name
.boxline
.monoon
$float      $5.4E-79 $7.2E75  $7       $:HP0.short:eHP0.
.monooff
.boxline
.monoon
$double     $5.4E-79 $7.2E75  $16      $:HP0.long:eHP0.
.monooff
.boxline
.monoon
$long double$5.4E-79 $7.2E75  $16      $:HP0.long:eHP0.
.monooff
.boxend
.do end
.el .do begin
.millust begin
              Smallest   Largest    Digits     370
              Absolute   Absolute   Of         Type
Type          Value      Value      Accuracy   Name
-----------   --------   --------   --------   -----
float         5.4E-79    7.2E75     7          short
double        5.4E-79    7.2E75     16         long
long double   5.4E-79    7.2E75     16         long
.millust end
.do end
.* .us &wlooc. floating-point types and ranges
.keep end
..sk 1 c
