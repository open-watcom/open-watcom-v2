.cp 13
.if &e'&nobox eq 0 .do begin
.boxdef
..if '&format' eq '7x9' ..th ..do begin
.  .boxcol 14
.  .boxcol 16
.  .boxcol 16
..do end
..el ..if '&format' eq '8.5x11' ..th ..do begin
.  .boxcol 17
.  .boxcol 19
.  .boxcol 19
..do end
..el ..do begin
.  .boxcol 18
.  .boxcol 20
.  .boxcol 20
..do end
.boxbeg
$Type          $Smallest Value$Largest Value
.boxline
.smonoon
$signed char       $-128                $127
$unsigned char     $0                   $255
$signed short      $-32768              $32767
$unsigned short    $0                   $65535
$signed long       $-2147483648         $2147483647
$unsigned long     $0                   $4294967295
$signed long long  $-9223372036854775808$9223372036854775807
$unsigned long long$0                   $18446744073709551615
.smonooff
.boxend
.do end
.el .do begin
.millust begin
                    Smallest               Largest
Type                 Value                  Value
------------------  ---------------------  ----------------------
signed char                         -128                     127
unsigned char                          0                     255
signed short                      -32768                   32767
unsigned short                         0                   65535
signed long                  -2147483648              2147483647
unsigned long                          0              4294967295
signed long long    -9223372036854775808     9223372036854775807
unsigned long long                     0    18446744073709551615

.millust end
.do end
