.ix 'keyword'
..sr firsttab=&SYSIN.+3
..tb set $
..tb &firsttab. +13 +13 +13 +13
.kwon
$auto     $double     $inline   $static   
$_Bool    $else       $int      $struct
$break    $enum       $long     $switch
$case     $extern     $register $typedef
$char     $float      $restrict $union
$_Complex $for        $return   $unsigned
$const    $goto       $short    $void
$continue $if         $signed   $volatile
$default  $_Imaginary $sizeof   $while
$do       
.kwoff
..tb set
..tb
