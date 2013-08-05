:LAYOUT
   :PAGE
      top_margin = 0 left_margin = '1i' right_margin = '7i' depth = '9.66i'
   :DEFAULT
      spacing = 1 columns = 1 font = 0 justify = no input_esc = '~'
      gutter = '0.5i' binding = 0
   :WIDOW
      threshold = 1
   :P	
      line_indent = '0.5i' pre_skip = 1 post_skip = 0
:eLAYOUT.
:GDOC.
:FRONTM.
:PREFACE.
:cmt.:BODY.
.co on
.ju off

.im ttsyms.gml
.im ttmacs.gml

:CMT.This is from cg\ccall.gml and is in the User's Guide 
:CMT.Topics: "16-bit: Sizes of Predefined Types" and "32-bit: Sizes of Predefined Types"
.tb set ^
.tb &sysin.+5 &sysin.+15c &sysin.+23c &sysin.+31
.ul ^Basic Type^"sizeof"^Argument^Registers
.ul ^^^Size^Used
.tb &sysin.+5 &sysin.+15 &sysin.+23 &sysin.+31
^char^1^&intsize^[&axup]
^short int^2^&intsize^[&axup]
^int^&intsize.^&intsize^[&axup]
^long int^4^4^[&reg4]
^float^4^8^[&reg8]
^double^8^8^[&reg8]
^near pointer^&nptrsz.^&nargsz.^[&axup]
^far pointer^&fptrsz.^&fargsz.^[&dxup &axup]
^huge pointer^&fptrsz.^&fargsz.^[&dxup &axup]
.tb set
.tb

:CMT.This is from cg\ccall.gml and is in the User's Guide 
:CMT.Topics: "16-bit: Interfacing to Assembly Language Functions" and "32-bit: Interfacing to Assembly Language Functions"
.tb set $
.tb 20 28 38
.millust begin
.us Small Memory Model (small code, small data)
.monoon
DGROUP  $group  $_DATA, _BSS
_TEXT   $segment byte public 'CODE'
        $assume $CS:_TEXT
        $assume $DS:DGROUP
        $public $myrtn_
myrtn_  $proc   $near
        $push   $&bpup        $; save &bpup
        $mov    $&bpup,&spup  $; get access to arguments
;
; body of function
;
        $mov    $&spup,&bpup  $; restore &spup
        $pop    $&bpup        $; restore &bpup
        $ret    $&argsize     $; return and pop last arg
myrtn_  $endp
_TEXT   $ends
.monooff
.millust end
.millust begin
.us Large Memory Model (big code, big data)
.monoon
DGROUP  $group $_DATA, _BSS
MYRTN_TEXT segment byte public 'CODE'
        $assume $CS:MYRTN_TEXT
        $public $myrtn_
myrtn_  $proc   $far
        $push   $&bpup        $; save &bpup
        $mov    $&bpup,&spup  $; get access to arguments
;
; body of function
;
        $mov    $&spup,&bpup  $; restore &spup
        $pop    $&bpup        $; restore &bpup
        $ret    $&argsize     $; return and pop last arg
myrtn_  $endp
MYRTN_TEXT ends
.monooff
.millust end
.tb set
.tb

.sk 1
:CMT.This is from cl\basedptr.gml and is in C Language Reference
:CMT.Topic: "Based Pointers for Open Watcom C/16 and C/32"
To support based pointers, the following keywords are provided:
..sr firsttab=&SYSIN.+3
..tb set $
..tb &firsttab. +9 +9 +9 +9
.kwon
$&kwbased.
$&kwsegment.
$&kwsegname.
$&kwself.
.kwoff
..tb set
..tb

:CMT.This is from cl\grammar.gml and is in C Language Reference
:CMT.Topic: "Definition of a Token" (and following sections)
.*
.************************************************************************
.*
..sr firsttab=&SYSIN.+5
..tb set $
..tb &firsttab. +5 +5 +5 +5 +5 +5 +5 +5 +5 +5
.*
.beglevel
:CMT..keep begin
:CMT..section Definition of a Token
.pp
:ITAL.token:eITAL.
$$keyword
$or$identifier
$or$constant
$or$string-literal
$or$operator
$or$punctuator
:CMT..keep break
.*
.************************************************************************
.*
:CMT..section Definition of a Keyword
.pp
:ITAL.keyword:eITAL.
$$standard-keyword
$or$&company.-extended-keyword
:CMT..keep break
.*
..sk 1 c
:ITAL.standard-keyword:eITAL.
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
:CMT..keep break
.*
..sk 1 c
:ITAL.&company.-extended-keyword:eITAL.
.sk 1
Microsoft compilers compatible
..sr firsttab=&SYSIN.+3
..tb set $
..tb &firsttab. +20 +20
.kwon
$&kwasm.     $&kwfinally. $&kwpascal.  
$&kwbased.   $&kwfortran. $&kwsaveregs.
$&kwcdecl.   $&kwhuge.    $&kwsegment. 
$&kwdeclspec.$&kwinline.  $&kwsegname.    
$&kwexcept.  $&kwint64.   $&kwself.
$&kwexport.  $&kwintrpt.  $&kwstdcall. 
$&kwfar.     $&kwleave.   $&kwsyscall.
$&kwfar16.   $&kwloadds.  $&kwtry.     
$&kwfastcall.$&kwnear.    $&kwunalign.     
.kwoff
.sk 1
IBM compilers compatible
.kwon
$&kwicdecl.  $&kwifinally.$&kwiseg16.  
$&kwiexcept. $&kwileave.  $&kwisyscall.
$&kwiexport. $&kwipacked. $&kwisystem. 
$&kwifar16.  $&kwipascal. $&kwitry.    
$&kwifastcal.
.kwoff
.sk 1
&company specific
.kwon
$&kwbldiflt. $&kwimagunit. $&kwwatcall.
.kwoff
..tb set
..tb
:CMT..keep break
.*
..sr firsttab=&SYSIN.+5
..tb set $
..tb &firsttab. +5 +5 +5 +5 +5 +5 +5 +5 +5 +5
.*
.************************************************************************
.*
:CMT..section Definition of an Identifier
.pp
:ITAL.identifier:eITAL.
$$nondigit {nondigit | digit}&SUPER0.
:CMT..keep break
..sk 1 c
:ITAL.nondigit:eITAL.
$$:MONO.a | b | ... | z | A | B | ... | Z | _:eMONO.
:CMT..keep break
..sk 1 c
:ITAL.digit:eITAL.
$$:MONO.0 | 1 | ... | 9:eMONO.
:CMT..keep break
.*
.************************************************************************
.*
:CMT..section Definition of a Constant
.pp
:ITAL.constant:eITAL.
$$floating-constant
$or$integer-constant
$or$enumeration-constant
$or$character-constant
:CMT..keep break
..sk 1 c
:ITAL.floating-constant:eITAL.
$$fractional-constant &LANGLE.exponent-part&RANGLE. &LANGLE.floating-suffix&RANGLE.
$or$digit-sequence exponent-part &LANGLE.floating-suffix&RANGLE.
:CMT..keep break
..sk 1 c
:ITAL.exponent-part:eITAL.
$$:MONO.e|E:eMONO.
&LANGLE.:MONO.+|-:eMONO.
..ct &RANGLE. digit-sequence
:CMT..keep break
..sk 1 c
:ITAL.floating-suffix:eITAL.
$$:MONO.f | F | l | L:eMONO.
:CMT..keep break
..sk 1 c
:ITAL.fractional-constant:eITAL.
$$&LANGLE.digit-sequence&RANGLE. ~. digit-sequence
$or$digit-sequence ~.
:CMT..keep break
..sk 1 c
:ITAL.digit-sequence:eITAL.
$${digit}&SUPER1.
:CMT..keep break
..sk 1 c
:ITAL.integer-constant:eITAL.
$  $decimal-constant &LANGLE.integer-suffix&RANGLE.
$or$octal-constant &LANGLE.integer-suffix&RANGLE.
$or$hexadecimal-constant &LANGLE.integer-suffix&RANGLE.
:CMT..keep break
..sk 1 c
:ITAL.integer-suffix:eITAL.
$$:MONO.u|U:eMONO.
&LANGLE.:MONO.l|L:eMONO.
..ct &RANGLE.
$or$:MONO.l|L:eMONO.
..ct &LANGLE.:MONO.u|U:eMONO.
..ct &RANGLE.
:CMT..keep break
..sk 1 c
:ITAL.decimal-constant:eITAL.
$$nonzero-digit{digit}&SUPER0.
:CMT..keep break
..sk 1 c
:ITAL.nonzero-digit:eITAL.
$$:MONO.1 | 2 | ... | 9:eMONO.
:CMT..keep break
..sk 1 c
:ITAL.octal-constant:eITAL.
$$:MONO.0:eMONO.{octal-digit}&SUPER0.
:CMT..keep break
..sk 1 c
:ITAL.octal-digit:eITAL.
$$:MONO.0 | 1 | ... | 7:eMONO.
:CMT..keep break
..sk 1 c
:ITAL.hexadecimal-constant:eITAL.
$$:MONO.0x:eMONO.|:MONO.0X:eMONO.{hexadecimal-digit}&SUPER1.
:CMT..keep break
..sk 1 c
:ITAL.hexadecimal-digit:eITAL.
$$:MONO.0 | 1 | ... | 9 |:eMONO.
$$:MONO.a | b | ... | f | A | B | ... | F:eMONO.
:CMT..keep break
..sk 1 c
:ITAL.enumeration-constant:eITAL.
$$identifier
:CMT..keep break
..sk 1 c
:ITAL.character-constant:eITAL.
$  $:MONO.':eMONO.{c-char}&SUPER1.:MONO.':eMONO.
$or$:MONO.L':eMONO.{c-char}&SUPER1.:MONO.':eMONO.
:CMT..keep break
..sk
:ITAL.c-char:eITAL.
$$any character in the source character set except
$$the single-quote
.mono '
..ct , backslash
.mono \
..ct , or new-line character
$or$escape-sequence
:CMT..keep break
..sk
:ITAL.escape-sequence:eITAL.
is one of
$$:MONO.\' \" \\:eMONO.
$$:MONO.\:HP0.o:eHP0. \:HP0.oo:eHP0. \:HP0.ooo:eHP0.:eMONO.
$$:MONO.\x:HP0.{hexadecimal-digit}:eHP0.&SUPER1.:eMONO.
$$:MONO.\a   \b   \f   \n   \r   \t   \v:eMONO.
:CMT..keep break
.*
.************************************************************************
.*
:CMT..section Definition of a String Literal
.pp
:ITAL.string-literal:eITAL.
$  $:MONO.":eMONO.{s-char}&SUPER0.:MONO.":eMONO.
$or$:MONO.L":eMONO.{s-char}&SUPER0.:MONO.":eMONO.
:CMT..keep break
..sk 1 c
:ITAL.s-char:eITAL.
$$any character in the source character set except
$$the double-quote
.mono "
..ct , backslash
.mono \
..ct , or new-line character
$or$escape-sequence
:CMT..keep break
.*
.************************************************************************
.*
:CMT..section Definition of an Operator
.*
.pp
:ITAL.operator:eITAL.
is one of
..co off
..br
$:MONO.[  ]  (  )  .  ->:eMONO.
$:MONO.++  --  &  *  +  -  ~~  !  sizeof:eMONO.
$:MONO./  %  <<  >>  <  >  <=  >=  ==  !=  ^  |  &&  ||:eMONO.
$:MONO.?  : :eMONO.
$:MONO.=  *=  /=  %=  +=  -=  <<=  >>=  &=  ^=  |=:eMONO.
$:MONO.,  #  ##:eMONO.
$:MONO.:>:eMONO.
$:MONO.:> :HP0.(&wcboth.):eHP0.:eMONO.
..co on
:CMT..keep break
.*
.************************************************************************
.*
:CMT..section Definition of a Punctuator
.pp
:ITAL.punctuator:eITAL.
..co off
..br
$$:MONO.[  ]  (  )  {  }  *  ,  :  = ; ... #:eMONO.
..co on
:CMT..keep break
.endlevel
.*
.************************************************************************
.*
:CMT..section Phrase Structure Grammar
.pp
The following topics are discussed:
.begbull
.bull
Expressions
.bull
Declarations
.bull
Statements
.bull
External Definitions
.endbull
.*
.************************************************************************
.*
.beglevel
:CMT..section Definition of Expressions
.pp
:ITAL.constant-expression:eITAL.
$$conditional-expression
:CMT..keep break
..sk 1 c
:ITAL.expression:eITAL.
$$assignment-expression{
.monoon
:CMT...ct ,
.monooff
assignment-expression}&SUPER0.
:CMT..keep break
..sk 1 c
:ITAL.assignment-expression:eITAL.
$$conditional-expression
$or$unary-expression assignment-operator assignment-expression
:CMT..keep break
..sk 1 c
:ITAL.assignment-operator:eITAL.
is one of
..co off
..br
$$:MONO.=  *=  /=  %=  +=  -=  <<=  >>=  &=  ^=  |=:eMONO.
..co on
:CMT..keep break
..sk 1 c
:ITAL.conditional-expression:eITAL.
$$logical-OR-expression &LANGLE.:MONO.?:eMONO.
expression
.mono :
conditional-expression&RANGLE.
:CMT..keep break
..sk 1 c
:ITAL.logical-OR-expression:eITAL.
$$logical-AND-expression{:MONO.||:eMONO.
logical-AND-expression}&SUPER0.
:CMT..keep break
..sk 1 c
:ITAL.logical-AND-expression:eITAL.
$$inclusive-OR-expression {:MONO.&&:eMONO.
inclusive-OR-expression}&SUPER0.
:CMT..keep break
..sk 1 c
:ITAL.inclusive-OR-expression:eITAL.
$$exclusive-OR-expression {:MONO.|:eMONO.
exclusive-OR-expression}&SUPER0.
:CMT..keep break
..sk 1 c
:ITAL.exclusive-OR-expression:eITAL.
$$AND-expression {:MONO.^:eMONO.
AND-expression}&SUPER0.
:CMT..keep break
..sk 1 c
:ITAL.AND-expression:eITAL.
$$equality-expression {:MONO.&:eMONO.
equality-expression}&SUPER0.
:CMT..keep break
..sk 1 c
:ITAL.equality-expression:eITAL.
$$relational-expression
{:MONO.==:eMONO.|:MONO.!=:eMONO.
relational-expression}&SUPER0.
:CMT..keep break
..sk 1 c
:ITAL.relational-expression:eITAL.
$$shift-expression {:MONO.<:eMONO.|:MONO.>:eMONO.|:MONO.<=:eMONO.|:MONO.>=:eMONO.
shift-expression}&SUPER0.
:CMT..keep break
..sk 1 c
:ITAL.shift-expression:eITAL.
$$additive-expression {:MONO.<<:eMONO.|:MONO.>>:eMONO.
additive-expression}&SUPER0.
:CMT..keep break
..sk 1 c
:ITAL.additive-expression:eITAL.
$$multiplicative-expression {:MONO.+:eMONO.|:MONO.-:eMONO.
multiplicative-expression}&SUPER0.
:CMT..keep break
..sk 1 c
:ITAL.multiplicative-expression:eITAL.
$$cast-expression {:MONO.*:eMONO.|:MONO./:eMONO.|:MONO.%:eMONO.
cast-expression}&SUPER0.
:CMT..keep break
..sk 1 c
:ITAL.cast-expression:eITAL.
$$unary-expression
$or$:MONO.(:eMONO.
type-name
.mono )
cast-expression
:CMT..keep break
..sk 1 c
:ITAL.unary-expression:eITAL.
$$postfix-expression
$or$:MONO.++ | -- | sizeof:eMONO.
unary-expression
$or$:MONO.sizeof(:eMONO.
type-name
.mono )
$or$unary-operator cast-expression
:CMT..keep break
..sk 1 c
:ITAL.unary-operator:eITAL.
is one of
$$:MONO.&  *  +  -  ~~  !:eMONO.
:CMT..keep break
..sk 1 c
:ITAL.postfix-expression:eITAL.
$$primary-expression
$or$postfix-expression
.mono [
expression
.mono ]
$or$postfix-expression
.mono (
&LANGLE.argument-expression-list&RANGLE.
.mono )
$or$postfix-expression . identifier
$or$postfix-expression
.mono ->
identifier
$or$postfix-expression
.mono ++
$or$postfix-expression
.mono --
:CMT..keep break
..sk 1 c
:ITAL.argument-expression-list:eITAL.
$$assignment-expression {:MONO.,:eMONO.
assignment-expression}&SUPER0.
:CMT..keep break
..sk 1 c
:ITAL.primary-expression:eITAL.
$$identifier
$or$constant
$or$string-literal
$or$:MONO.(:eMONO. expression :MONO.):eMONO.
:CMT..keep break
.*
.************************************************************************
.*
:CMT..section Definition of a Declaration
.pp
:ITAL.declaration:eITAL.
$$declaration-specifiers &LANGLE.init-declarator-list&RANGLE.:MSEMI.
:CMT..keep break
..sk 1 c
:ITAL.declaration-specifiers:eITAL.
$$storage-class-specifier &LANGLE.declaration-specifiers&RANGLE.
$or$type-specifier &LANGLE.declaration-specifiers&RANGLE.
:CMT..keep break
..sk 1 c
:ITAL.init-declarator-list:eITAL.
$$init-declarator {:MONO.,:eMONO.
init-declarator}&SUPER0.
:CMT..keep break
..sk 1 c
:ITAL.init-declarator:eITAL.
$$declarator &LANGLE.:MONO.=:eMONO. initializer&RANGLE.
:CMT..keep break
..sk 1 c
:ITAL.storage-class-specifier:eITAL.
$$:MONO.typedef | extern | static | auto | register:eMONO.
:CMT..keep break
..sk 1 c
:ITAL.type-specifier:eITAL.
$$:MONO.void | char | short | int | long | float |:eMONO.
$$:MONO.double | signed | unsigned:eMONO.
$or$struct-or-union-specifier
$or$enum-specifier
$or$typedef-name
$or$type-qualifier
:CMT..keep break
.*
.************************************************************************
.*
..sk 1 c
:ITAL.type-qualifier:eITAL.
$$:MONO.const | volatile:eMONO.
$or$&company.-type-qualifier
:CMT..keep break
..sk 1 c
:ITAL.&company.-type-qualifier:eITAL.
.ix 'keyword'
..tb set $
..sr firsttab=&SYSIN.+3
..tb &firsttab. +18 +18
.kwon
$&kwbased.   $&kwfortran. $&kwiseg16.
$&kwicdecl.  $&kwhuge.    $&kwsegment. 
$&kwcdecl.   $&kwinline.  $&kwsegname. 
$&kwdeclspec.$&kwint64.   $&kwself.   
$&kwiexport. $&kwintrpt.  $&kwstdcall. 
$&kwexport.  $&kwloadds.  $&kwisyscall.
$&kwfar.     $&kwnear.    $&kwsyscall.
$&kwifar16.  $&kwipacked. $&kwisystem.
$&kwfar16.   $&kwipascal. $&kwunalign.
$&kwifastcal.$&kwpascal.  $&kwwatcall.
$&kwfastcall.$&kwsaveregs.
.kwoff
..tb set
..tb
.sk 1
.*
..sr firsttab=&SYSIN.+5
..tb set $
..tb &firsttab. +5 +5 +5 +5 +5 +5 +5 +5 +5 +5
.*
$$:MONO.const | volatile:eMONO.
:CMT..keep break
.*
.************************************************************************
.*
..sk 1 c
:ITAL.struct-or-union-specifier:eITAL.
$$struct-or-union &LANGLE.identifier&RANGLE.
.mono {
struct-declaration-list
.mono }
$or$struct-or-union identifier
:CMT..keep break
..sk 1 c
:ITAL.struct-or-union:eITAL.
$$:MONO.struct | union:eMONO.
:CMT..keep break
..sk 1 c
:ITAL.struct-declaration-list:eITAL.
$${struct-declaration}&SUPER1.
:CMT..keep break
..sk 1 c
:ITAL.struct-declaration:eITAL.
$$type-specifier-list struct-declarator-list:MSEMI.
:CMT..keep break
..sk 1 c
:ITAL.type-specifier-list:eITAL.
$${type-specifier}&SUPER1.
:CMT..keep break
..sk 1 c
:ITAL.struct-declarator-list:eITAL.
$$struct-declarator {:MONO.,:eMONO.
struct-declarator}&SUPER0.
:CMT..keep break
..sk 1 c
:ITAL.struct-declarator:eITAL.
$$declarator
$or$&LANGLE.declarator&RANGLE.
.mono :
constant-expression
:CMT..keep break
..sk 1 c
:ITAL.enum-specifier:eITAL.
$$:MONO.enum:eMONO.
&LANGLE.identifier&RANGLE.
.mono {
enumerator-list
.mono }
$or$:MONO.enum:eMONO.
identifier
:CMT..keep break
..sk 1 c
:ITAL.enumerator-list:eITAL.
$$enumerator {:MONO.,:eMONO.
enumerator}&SUPER0.
:CMT..keep break
..sk 1 c
:ITAL.enumerator:eITAL.
$$enumeration-constant &LANGLE.:MONO.=:eMONO.
constant-expression&RANGLE.
:CMT..keep break
..sk 1 c
:ITAL.declarator:eITAL.
$$&LANGLE.pointer&RANGLE. direct-declarator
:CMT..keep break
..sk 1 c
:ITAL.direct-declarator:eITAL.
$$identifier
$or$
..ct .mono (
declarator
.mono )
$or$direct-declarator
.mono [
&LANGLE.constant-expression&RANGLE.
.mono ]
$or$direct-declarator
.mono (
parameter-type-list
.mono )
$or$direct-declarator
.mono (
&LANGLE.identifier-list&RANGLE.
.mono )
:CMT..keep break
..sk 1 c
:ITAL.pointer:eITAL.
$${:MONO.*:eMONO.
&LANGLE.type-specifier-list&RANGLE.}&SUPER1.
:CMT..keep break
..sk 1 c
:ITAL.parameter-type-list:eITAL.
$$parameter-list &LANGLE.:MONO., ...:eMONO.
..ct &RANGLE.
:CMT..keep break
..sk 1 c
:ITAL.parameter-list:eITAL.
$$parameter-declaration {:MONO.,:eMONO.
parameter-declaration}&SUPER0.
..sk 1 c
:CMT..keep break
:ITAL.parameter-declaration:eITAL.
$$declaration-specifiers declarator
$or$declaration-specifiers &LANGLE.abstract-declarator&RANGLE.
:CMT..keep break
..sk 1 c
:ITAL.identifier-list:eITAL.
$$identifier {:MONO.,:eMONO.
identifier}&SUPER0.
:CMT..keep break
..sk 1 c
:ITAL.type-name:eITAL.
$$type-specifier-list &LANGLE.abstract-declarator&RANGLE.
:CMT..keep break
..sk 1 c
:ITAL.abstract-declarator:eITAL.
$$pointer
$or$&LANGLE.pointer&RANGLE. direct-abstract-declarator
:CMT..keep break
..sk 1 c
:ITAL.direct-abstract-declarator:eITAL.
$$:MONO.(:eMONO.
abstract-declarator
.mono )
$or$&LANGLE.direct-abstract-declarator&RANGLE.
.mono [
&LANGLE.constant-expression&RANGLE.
.mono ]
$or$&LANGLE.direct-abstract-declarator&RANGLE.
.mono (
&LANGLE.parameter-type-list&RANGLE.
.mono )
:CMT..keep break
..sk 1 c
:ITAL.typedef-name:eITAL.
$$identifier
:CMT..keep break
..sk 1 c
:ITAL.initializer:eITAL.
$$assignment-expression
$or$:MONO.{:eMONO.initializer-list &LANGLE.:MONO.,:eMONO.&RANGLE.:MONO.}:eMONO.
:CMT..keep break
..sk 1 c
:ITAL.initializer-list:eITAL.
$$initializer {:MONO.,:eMONO.
initializer}&SUPER0.
:CMT..keep break
.*
.************************************************************************
.*
:CMT..section Definition of a Statement
.pp
:ITAL.statement:eITAL.
$$labelled-statement
$or$compound-statement
$or$expression-statement
$or$selection-statement
$or$iteration-statement
$or$jump-statement
:CMT..keep break
..sk 1 c
:ITAL.labelled-statement:eITAL.
$$identifier
.mono :
statement
$or$:MONO.case:eMONO.
constant-expression
.mono :
statement
$or$:MONO.default ::eMONO.
statement
:CMT..keep break
..sk 1 c
:ITAL.compound-statement:eITAL.
$$:MONO.{:eMONO.
&LANGLE.declaration-list&RANGLE. &LANGLE.statement-list&RANGLE.:MONO.}:eMONO.
:CMT..keep break
..sk 1 c
:ITAL.declaration-list:eITAL.
$${declaration}&SUPER1.
:CMT..keep break
..sk 1 c
:ITAL.statement-list:eITAL.
$${statement}&SUPER1.
:CMT..keep break
..sk 1 c
:ITAL.expression-statement:eITAL.
$$&LANGLE.expression&RANGLE.:MSEMI.
:CMT..keep break
..sk 1 c
:ITAL.selection-statement:eITAL.
$$:MONO.if     (:eMONO.
expression
.mono )
statement
$or$:MONO.if     (:eMONO.
expression
.mono )
statement
.mono else
statement
$or$:MONO.switch (:eMONO.
expression
.mono )
statement
:CMT..keep break
..sk 1 c
:ITAL.iteration-statement:eITAL.
$$:MONO.while (:eMONO.
expression
.mono )
statement
$or$:MONO.do:eMONO.
statement
.mono while (
expression
.monoon
);
.monooff
$or$:MONO.for (:eMONO.
&LANGLE.expression&RANGLE.:MSEMI.
&LANGLE.expression&RANGLE.:MSEMI.
&LANGLE.expression&RANGLE.
.mono )
statement
:CMT..keep break
..sk 1 c
:ITAL.jump-statement:eITAL.
$$:MONO.goto:eMONO.
identifier:MSEMI.
$or$:MONO.continue:eMONO.:MSEMI.
$or$:MONO.break:eMONO.:MSEMI.
$or$:MONO.return:eMONO. &LANGLE.expression&RANGLE.:MSEMI.
:CMT..keep break
.*
.************************************************************************
.*
:CMT..section External Definitions
.*
.pp
:ITAL.file:eITAL.
$${external-definition}&SUPER1.
:CMT..keep break
..sk 1 c
:ITAL.external-definition:eITAL.
$$function-definition
$or$declaration
:CMT..keep break
..sk 1 c
:ITAL.function-definition:eITAL.
$$&LANGLE.declaration-specifiers&RANGLE. declarator &LANGLE.declaration-list&RANGLE.
$$$compound-statement
:CMT..keep break
.endlevel
.*
.************************************************************************
.*
:CMT..section Preprocessing Directives Grammar
.beglevel
.pp
:ITAL.preprocessing-file:eITAL.
$$group
:CMT..keep break
..sk 1 c
:ITAL.group:eITAL.
$${group-part}&SUPER1.
:CMT..keep break
..sk 1 c
:ITAL.group-part:eITAL.
$$&LANGLE.pp-token&RANGLE. new-line
$or$if-section
$or$control-line
:CMT..keep break
..sk 1 c
:ITAL.if-section:eITAL.
$$if-group {elif-group}&SUPER0. &LANGLE.else-group&RANGLE. endif-line
:CMT..keep break
..sk 1 c
:ITAL.if-group:eITAL.
$$:MONO.#if :eMONO.
const-expression new-line &LANGLE.group&RANGLE.
$$:MONO.#ifdef :eMONO.
identifier new-line &LANGLE.group&RANGLE.
$$:MONO.#ifndef :eMONO.
identifier new-line &LANGLE.group&RANGLE.
:CMT..keep break
..sk 1 c
:ITAL.elif-group:eITAL.
$$:MONO.#elif :eMONO.
constant-expression new-line &LANGLE.group&RANGLE.
:CMT..keep break
..sk 1 c
:ITAL.else-group:eITAL.
$$:MONO.#else :eMONO.
new-line &LANGLE.group&RANGLE.
:CMT..keep break
..sk 1 c
:ITAL.endif-line:eITAL.
$$:MONO.#endif :eMONO.
new-line
:CMT..keep break
..sk 1 c
:ITAL.control-line:eITAL.
$$:MONO.#include :eMONO.
pp-tokens new-line
$$:MONO.#define :eMONO.
identifier &LANGLE.pp-tokens&RANGLE. new-line
$$:MONO.#define :eMONO.
identifier
.mono (
&LANGLE.identifier-list&RANGLE.
.mono )
&LANGLE.pp-tokens&RANGLE. new-line
$$:MONO.#undef :eMONO.
identifier new-line
$$:MONO.#line :eMONO.
pp-tokens new-line
$$:MONO.#error :eMONO.
&LANGLE.pp-tokens&RANGLE. new-line
$$:MONO.#pragma :eMONO.
&LANGLE.pp-tokens&RANGLE. new-line
$$:MONO.#:eMONO.
new-line
:CMT..keep break
..sk 1 c
:ITAL.pp-tokens:eITAL.
$${preprocessing-token}&SUPER1.
:CMT..keep break
..sk 1 c
:ITAL.preprocessing-token:eITAL.
$$header-name (only within a
.mono #include
directive)
$or$identifier (no keyword distinction)
$or$constant
$or$string-literal
$or$operator
$or$punctuator
$or$each non-white-space character that cannot be one of the above
:CMT..keep break
..sk 1 c
:ITAL.header-name:eITAL.
$$:MONO.<:eMONO.
..ct {h-char}&SUPER0.
:cmt...ct .mono >
:CMT..keep break
..sk 1 c
:ITAL.h-char:eITAL.
$$any character in the source character set except :ITAL.new-line:eITAL. and
.mono >
:CMT..keep break
..sk 1 c
:ITAL.new-line:eITAL.
$$the new-line character
.endlevel
..tb set
..tb
:CMT..keep end

:CMT.This is from cl\orderop.gml and is in the C Language Reference
:CMT.Topic: "Operator Precedence"

:cmt...fk begin
..sr *start=&SYSIN.+1
:cmt...bx on  &*start. +16 +24
..sr *start=&*start.+1
..tb &*start. +16 +6 +6 +6 +6 +6
..tb set $
:SF font=0.
$Expression Type$Operators
:eSF.
:cmt...bx
:SF font=0.
$primary       $identifier  $$constant
$              $string      $$(expression)
:eSF.
:cmt...bx
:SF font=5.
$:SF font=0.postfix:eSF.$a[b] $f()
:eSF.:SF font=5.
$$a.b   $a->b$a++   $a--
:eSF.
:cmt...bx
:SF font=5.
$:SF font=0.unary:eSF.$sizeof u   $sizeof( a )
:eSF.:SF font=5.
$$++a  $--a  $&a    $*a
$$+a   $-a   $~~a   $!a
:eSF.
:cmt...bx
:SF font=5.
$:SF font=0.cast:eSF.$(type) a
:eSF.
:cmt...bx
:SF font=5.
$:SF font=0.multiplicative:eSF.$a * b   $a / b   $a % b
:eSF.
:cmt...bx
:SF font=5.
$:SF font=0.additive:eSF.$a + b   $a - b
:eSF.
:cmt...bx
:SF font=5.
$:SF font=0.shift:eSF.$a << b  $a >> b
:eSF.
:cmt...bx
:SF font=5.
$:SF font=0.relational:eSF.$a < b   $a > b   $a <= b   $a >= b
:eSF.
:cmt...bx
:SF font=5.
$:SF font=0.equality:eSF.$a == b  $a != b
:eSF.
:cmt...bx
:SF font=5.
$:SF font=0.bitwise AND:eSF.$a & b
:eSF.
:cmt...bx
:SF font=5.
$:SF font=0.bitwise exclusive OR:eSF.$a ^ b
:eSF.
:cmt...bx
:SF font=5.
$:SF font=0.bitwise inclusive OR:eSF.$a | b
:eSF.
:cmt...bx
:SF font=5.
$:SF font=0.logical AND:eSF.$a && b
:eSF.
:cmt...bx
:SF font=5.
$:SF font=0.logical OR:eSF.$a || b
:eSF.
:cmt...bx
:SF font=5.
$:SF font=0.conditional &dagger.:eSF.$a ? b : c
:eSF.
:cmt...bx
:SF font=5.
$:SF font=0.assignment &dagger.:eSF.$a = b $a += b $a -= b $a *= b
:eSF.:SF font=5.
$$a /= b  $a %= b  $a &= b  $a ^= b
$$a |= b  $a <<= b $a >>= b
:eSF.
:cmt...bx
:SF font=5.
$:SF font=0.comma:eSF.$a,b
:eSF.
:cmt...bx off
:SF font=0.
$&dagger. associates from right to left
..tb set
..tb
:eSF.
:cmt...fk end

:CMT.This is from cpplib\sb_rgp.gml and is in the C++ CLass Library Reference
:CMT.Topic: "eback() [streambuf]"

.np
The &rsvarea., &getarea., and &putarea. pointer functions return the following
values:
.tb 10 15 20
.sk
.fo off
.tb set ^
:SF font=4.base():eSF.^start of the :HP1.reserve area:eHP1..
:SF font=4.ebuf():eSF.^end of the :HP1.reserve area:eHP1..
:SF font=4.blen():eSF.^length of the :HP1.reserve area:eHP1..
.sk
:SF font=4.eback():eSF.^start of the :HP1.get area:eHP1..
:SF font=4.gptr():eSF.^the :HP1.get pointer:eHP1..
:SF font=4.egptr():eSF.^end of the :HP1.get area:eHP1..
.sk
:SF font=4.pbase():eSF.^start of the :HP1.put area:eHP1..
:SF font=4.pptr():eSF.^the :HP1.put pointer:eHP1..
:SF font=4.epptr():eSF.^end of the :HP1.put area:eHP1..
.co &cpp_co.
.ju &cpp_ju.
.tb set
.co on

:CMT.This is from cpplib\seekdir.gml and is in the C++ CLass Library Reference
:CMT.Topic: "seekg() [istream]"

.sk
.fo off
.tb set ^
:SF font=4.ios::beg:eSF.^the :HP1.offset:eHP1. is relative to the start and should be a positive value.
:SF font=4.ios::cur:eSF.^the :HP1.offset:eHP1. is relative to the current position and may be positive
^^(seek towards end) or negative (seek towards start).
:SF font=4.ios::end:eSF.^the :HP1.offset:eHP1. is relative to the end and should be a negative value.
.co &cpp_co.
.ju &cpp_ju.
.tb set
.sk
.co on

:CMT.This is from cpplib\seekmode.gml and is in the C++ CLass Library Reference
:CMT.Topic: "seekoff() [streambuf]"

.sk
.fo off
.tb set ^
:SF font=4.ios::in:eSF.^^^the :HP1.get pointer:eHP1. should be moved.
:SF font=4.ios::out:eSF.^^^the :HP1.put pointer:eHP1. should be moved.
:SF font=4.ios::in|ios::out:eSF.^both the :HP1.get pointer:eHP1. and the :HP1.put pointer:eHP1. should be moved.
.co &cpp_co.
.ju &cpp_ju.
.tb set
.sk
.co on

:CMT.This is from fg\fcall.gml and is in the FORTRAN 77 User's Guide
:CMT.Topics: "16-bit: Calling Conventions" and "32-bit: Calling Conventions"

.tb set $
.tb 5 25
:CMT..keep 20
:CMT..bx on &c0 &c1 &c2
$Type of Argument$Method Used to Pass Argument
:CMT..bx
$non-character constant$address of constant
$non-character expression$address of value of expression
$non-character variable$address of variable
$character constant$address of string descriptor
$character expression$address of string descriptor
$character variable$address of string descriptor
$non-character array$address of array
$non-character array element$address of array
$character array$address of string descriptor
$character array element$address of string descriptor
$character substring$address of string descriptor
$subprogram$address of subprogram
$alternate return specifier$no argument passed
$user-defined structure$address of structure
:CMT..bx off
:CMT..keep end
.tb set
.tb

:CMT.This is from fg\fdata.gml and is in the FORTRAN 77 User's Guide
:CMT.Topic: "Data Representation On x86-based Platforms"

.sk
Data Representation On x86-based Platforms
.sr c1=&sysin+1
.sr c2=&c1+17
.sr c3=&c2+10
.sr c4=&c3+13
.tb set $
.tb &c1+2 &c2+2 &c3+2
:CMT..keep begin
:CMT..bx &c1 &c2 &c3 &c4
$Data Type$Size$FORTRAN 77
$$(in bytes)$Standard
.tb &c1+2 &c2+(&c3-&c2)/2 &c3+2
:CMT..bx
$LOGICAL$4
$LOGICAL*1$1$(extension)
$LOGICAL*4$4$(extension)
$INTEGER$4
$INTEGER*1$1$(extension)
$INTEGER*2$2$(extension)
$INTEGER*4$4$(extension)
$REAL$4
$REAL*4$4$(extension)
$REAL*8$8$(extension)
$DOUBLE PRECISION$8
$COMPLEX$8
$COMPLEX*8$8$(extension)
$COMPLEX*16$16$(extension)
$DOUBLE COMPLEX$16$(extension)
$CHARACTER$1
$CHARACTER*n$n
:CMT..bx off
:CMT..keep end
.tb set
.tb
.*

:CMT.This is from fg\fdata.gml and is in the FORTRAN 77 User's Guide
:CMT.Topic: "REAL and REAL*4 Data Types"

.sk
REAL and REAL*4 Data Types
.sr c0=10+1
.sr c1=&c0+3
.sr c2=&c1+14
.sr c3=&c2+22
.tb set $
.tb &c0+1 &c1+3 &c2+6
:CMT..bx on &c0 &c1 &c2 &c3
$S$Biased$Significand
$$Exponent
:CMT..bx off
$31$30-23$22-0
.tb set
.tb

:CMT.This is from fg\fdata.gml and is in the FORTRAN 77 User's Guide
:CMT.Topic: "DOUBLE PRECISION and REAL*8 Data Types"

.sk
DOUBLE PRECISION and REAL*8 Data Types
.sr c0=10+1
.sr c1=&c0+3
.sr c2=&c1+15
.sr c3=&syscl-3
.tb set $
.tb &c0+1 &c1+3 &c2+12
:CMT..bx on &c0 &c1 &c2 &c3
$S$Biased$Significand
$$Exponent
:CMT..bx off
$63$62-52$51-0
.tb set
.tb

:CMT.This is from fg\fdata.gml and is in the FORTRAN 77 User's Guide
:CMT.Topic: "CHARACTER Data Type"

.sk
CHARACTER Data Type
:CMT..keep begin
.sr oc=10+4
.sr c0=&oc+6
.sr c1=&c0+15
.tb set $
.tb &oc &c0+2
$Offset
:CMT..bx &c0 &c1
$0$pointer to data
:CMT..bx
$4$length of data
:CMT..bx off
.tb set
.tb

:CMT.This is from fg\fdata.gml and is in the FORTRAN 77 User's Guide
:CMT.Topic: "Storage Organization of Data Types"

.pa
Storage Organization of Data Types
.sr c0=10+11
.sr bs=2
.sr c1=&c0+&bs
.sr c2=&c1+&bs
.sr c3=&c2+&bs
.sr c4=&c3+&bs
.sr c5=&c4+&bs
.sr c6=&c5+&bs
.sr c7=&c6+&bs
.sr c8=&c7+&bs
.sr c9=&c8+&bs
.sr c10=&c9+&bs
.sr c11=&c10+&bs
.sr c12=&c11+&bs
.sr c13=&c12+&bs
.sr c14=&c13+&bs
.sr c15=&c14+&bs
.sr c16=&c15+&bs
.tb set $
.tb &c0 &c1 &c2 &c3 &c4 &c5 &c6 &c7 &c8 &c9 &c10 &c11 &c12 &c13 &c14 &c15 &c16
:CMT..keep 24
Offset$0$1$2$3$4$5$6$7$8$9$10$11$12$13$14$15
:CMT..bx on &c0 &c1 &c2 &c3 &c4 &c5 &c6 &c7 &c8 &c9 &c10 &c11 &c12 &c13 &c14 &c15 &c16
.br;in bytes
:CMT..bx on &c0 &c1
.br;LOGICAL*1
:CMT..bx on &c0 &c4
.br;LOGICAL*4
:CMT..bx on &c0 &c1
.br;INTEGER*1
:CMT..bx on &c0 &c2
.br;INTEGER*2
:CMT..bx on &c0 &c4
.br;INTEGER*4
:CMT..bx on &c0 &c4
.br;REAL*4
:CMT..bx on &c0 &c8
.br;REAL*8
.tb &c2-1 &c6-2
:CMT..bx on &c0 &c4 &c8
.br;COMPLEX*8$real$imaginary
.tb &c4-2 &c12-3
:CMT..bx on &c0 &c8 &c16
.br;COMPLEX*16$real part$imaginary part
:CMT..bx off
:CMT..keep end
.tb set
.tb

:CMT.This is from fl\fsrcfmt.gml and is in the FORTRAN 77 Language Reference
:CMT.Topic: "Character Set"

.np
The special characters are:
.tb set \
.se INDlvl=0
.se c0=&INDlvl+2+1
.se c1=&INDlvl+2+10
.se c2=&INDlvl+2+27
.tb &c0 &c1 &c2
.se c0=&INDlvl+1
.se c1=&INDlvl+10
.se c2=&INDlvl+27
:CMT..bx on &c0 &c1 &c2
\Character \Name of Character\
:CMT..bx
.se c0=&INDlvl+2+4
.se c1=&INDlvl+2+13
.se c2=&INDlvl+2+27
.tb &c0 &c1 &c2
\          \Blank
\=         \Equals
\+         \Plus
\-         \Minus
\*         \Asterisk
\/         \Slash
\(         \Left Parenthesis\
\)         \Right Parenthesis\
\,         \Comma
\.         \Decimal Point\
\$         \Currency Symbol\
\'         \Apostrophe
\:         \Colon
:CMT..bx off
.tb
.tb set

:CMT.This is from fl\fsrcfmt.gml and is in the FORTRAN 77 Language Reference
:CMT.Topic: "Extended Character Set"

.np
Extended Character Set
.se c0=&INDlvl+1
.se c1=&INDlvl+10
.se c2=&INDlvl+27
.bxt on &c0 &c2 &c1
.tb set `
`Character `Name of Character`
.bxt
.se c0=&INDlvl+2+4
.se c1=&INDlvl+2+13
.se c2=&INDlvl+2+27
.tb &c0 &c1 &c2
`!         `Exclamation Mark
`%         `Percentage Symbol
`\         `Back slash
.bxt off
.tb set
.tb

:CMT.This is from fl\fsrcfmt.gml and is in the FORTRAN 77 Language Reference
:CMT.Topic: "Order of FORTRAN Statements and Lines"

.np
Order of FORTRAN Statements and Lines
.sr c0=&INDlvl+1
.sr c1=&INDlvl+12
.sr c2=&INDlvl+24
.sr c3=&INDlvl+36
.sr c4=&INDlvl+48
.sr t0=&c0+2
.sr t1=&c1+2
.sr t2=&c2+2
.sr t3=&c3+2
.sr t4=&c4+2
.tb set \
.tb &t0 &t1 &t2 &t3 &t4
.*
:CMT..keep 25
:CMT..bx &c0 &c1 &c4
\\PROGRAM, FUNCTION, SUBROUTINE, or
.br
\\    BLOCK DATA Statement
:CMT..bx new &c1 &c2 &c3 &c4
\\\\IMPLICIT
\\\PARAMETER\Statements
:CMT..bx new &c2 / &c3 &c4
\Comment\ENTRY\Statements\Other
\Lines\\\Specification
\\and\\Statements
:CMT..bx off &c2 &c3 &c4
\\FORMAT\\Statement
\INCLUDE\\\Function
\Statement\Statements\DATA\Statements
:CMT..bx new &c2 / &c3 &c4
\\\Statements\Executable
\\\\Statements
.* .bx &c0 &c4
:CMT..bx &c1 &c4
\\        END\Statement
:CMT..bx can
:CMT..bx can
:CMT..bx can
:CMT..bx off &c0 &c1 &c4
.tb
.tb set

:CMT.This is from fl\farrays.gml and is in the FORTRAN 77 Language Reference
:CMT.Topic: "Array Elements"

.np
The following table describes how to compute the subscript value.
.cp 23
.sr c0=&INDlvl+1
.sr c1=&INDlvl+5
.sr c2=&INDlvl+20
.sr c3=&INDlvl+30
.sr c4=&INDlvl+50
.box on &c0 &c1 &c2 &c3 &c4
\n \Dimension          \Subscript  \Subscript
\  \Declarator         \Value
.box
\1 \(J1:K1)            \(S1)       \1+(S1-J1)
.box
\2 \(J1:K1,J2:K2)      \(S1,S2)    \1+(S1-J1)
\  \                   \           \ +(S2-J2)*D1
.box
\3 \(J1:K1,J2:K2,J3:K3)\(S1,S2,S3) \1+(S1-J1)
\  \                   \           \ +(S2-J2)*D1
\  \                   \           \ +(S3-J3)*D2*D1
.box
\. \.                  \.          \.
\. \.                  \.          \.
\. \.                  \.          \.
.box
\n \(J1:K1,...,Jn:Kn)  \(S1,...,Sn)\1+(S1-J1)
\  \                   \           \ +(S2-J2)*D1
\  \                   \           \ +(S3-J3)*D2*D1
\  \                   \           \ +
\  \                   \           \ +(Sn-Jn)*Dn-1*Dn-2*...*D1
.box off

:CMT.This is from fl\fexpr.gml and is in the FORTRAN 77 Language Reference
:CMT.Topic: "Data Type of Arithmetic Expressions"

.np
This table is valid for all of the arithmetic operators.

.sr c0=&INDlvl+1
.sr c1=&INDlvl+5
.sr c2=&INDlvl+9
.sr c3=&INDlvl+13
.sr c4=&INDlvl+18
.sr c5=&INDlvl+22
.sr c6=&INDlvl+26
.sr c7=&INDlvl+30
.sr c8=&INDlvl+34
.sr c9=&INDlvl+38
.box on &c0 &c1 &c2 &c3 &c4 &c5 &c6 &c7 &c8 &c9
\ op  \ I*1 \ I*2 \ I*4 \  R  \  D  \  C  \  Z \
.box
\ I*1 \ I*1 \ I*2 \ I*4 \  R  \  D  \  C  \  Z \
\ I*2 \ I*2 \ I*2 \ I*4 \  R  \  D  \  C  \  Z \
\ I*4 \ I*4 \ I*4 \ I*4 \  R  \  D  \  C  \  Z \
\  R  \  R  \  R  \  R  \  R  \  D  \  C  \  Z \
\  D  \  D  \  D  \  D  \  D  \  D  \  Z  \  Z \
\  C  \  C  \  C  \  C  \  C  \  Z  \  C  \  Z \
\  Z  \  Z  \  Z  \  Z  \  Z  \  Z  \  Z  \  Z \
.box off

:CMT.This is from fl\fformats.gml and is in the FORTRAN 77 Language Reference
:CMT.Topic: "G Editing"

determines the editing as shown in the following table.
.cp 15
.sr c0=&INDlvl+1
.sr c1=&INDlvl+19
.sr c2=&INDlvl+45
.box on &c0 &c1 &c2
\Magnitude of Datum    \Equivalent Edit Descriptor\
.box
\0.1<=M<1              \F<w-n>.d followed by n blanks\
\1<=M<10               \F<w-n>.<d-1> followed by n blanks\
\    .                 \           .\
\    .                 \           .\
\    .                 \           .\
\10**(d-2)<=M<10**(d-1)\F<w-n>.1 followed by n blanks\
\10**(d-1)<=M<10**d    \F<w-n>.0 followed by n blanks\
.box off

:CMT.This is from fl\fitab.gml and is in the FORTRAN 77 Language Reference
:CMT.Topic: "Intrinsic Functions"

:CMT.Note: this is pretty much the entire file with a few text lines removed.
:CMT.Note: it is only part of the topic indicated above, but a very large part!

.np
Type Conversion
.*
.sr c0=&INDlvl+1
.sr c1=&INDlvl+17
.sr c2=&INDlvl+30
.sr c3=&INDlvl+54
.box on &c0 &c1 &c2 &c3

.np
Conversion to integer

\Definition\Name\Usage
.box
\Conversion to integer:\INT generic\I&arrow.INT(I)
\int(a)                \           \I&arrow.INT(R)
\                      \           \I&arrow.INT(D)
\                      \           \I&arrow.INT(C)
\                      \           \I&arrow.INT(Z) &dagger\
\                      \INT        \I&arrow.INT(R)
\                      \HFIX &dagger     \H&arrow.HFIX(R)
\                      \IFIX       \I&arrow.IFIX(R)
\                      \IDINT      \I&arrow.IDINT(D)
.box off

.np
Conversion to real
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\Conversion to real\REAL generic\R&arrow.REAL(I)
\                  \            \R&arrow.REAL(R)
\                  \            \R&arrow.REAL(D)
\                  \            \R&arrow.REAL(C)
\                  \            \R&arrow.REAL(Z) &dagger\
\                  \REAL        \R&arrow.REAL(I)
\                  \FLOAT       \R&arrow.FLOAT(I)
\                  \SNGL        \R&arrow.SNGL(D)
.box off

.np
Conversion to double
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\Conversion to double \DBLE generic\D&arrow.DBLE(I)
\precision            \            \D&arrow.DBLE(R)
\                     \            \D&arrow.DBLE(D)
\                     \            \D&arrow.DBLE(C)
\                     \            \D&arrow.DBLE(Z) &dagger\
\                     \DREAL       \D&arrow.DREAL(Z) &dagger\
\                     \DFLOAT      \D&arrow.DFLOAT(I) &dagger\
.box off

.np
Conversion to complex
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\Conversion to complex \CMPLX generic \C&arrow.CMPLX(I)|C&arrow.CMPLX(I,I)
\                      \              \C&arrow.CMPLX(R)|C&arrow.CMPLX(R,R)
\                      \              \C&arrow.CMPLX(D)|C&arrow.CMPLX(D,D)
\                      \              \C&arrow.CMPLX(C)
\                      \              \C&arrow.CMPLX(Z) &dagger\
.box off

.np
Conversion to double
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\Conversion to double \DCMPLX generic &dagger\Z&arrow.DCMPLX(I)|Z&arrow.DCMPLX(I,I)
\complex              \                \Z&arrow.DCMPLX(R)|Z&arrow.DCMPLX(R,R)
\                     \                \Z&arrow.DCMPLX(D)|Z&arrow.DCMPLX(D,D)
\                     \                \Z&arrow.DCMPLX(C)
\                     \                \Z&arrow.DCMPLX(Z)
.box off

.np
Conversion to integer
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\Conversion to integer \ICHAR \I&arrow.ICHAR(CH)
.box off

.np
Conversion to character
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\Conversion to character \CHAR \CH&arrow.CHAR(I)
.box off

.np
Truncation
:cmt..section Truncation
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\int(a)     \AINT generic \R&arrow.AINT(R)
\           \             \D&arrow.AINT(D)
\           \AINT         \R&arrow.AINT(R)
\           \DINT         \D&arrow.DINT(D)
.box off

.np
Nearest Whole Number
:cmt..section Nearest Whole Number
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\int(a+.5) if a>=0 \ANINT generic \R&arrow.ANINT(R)
\int(a-.5) if a<0  \              \D&arrow.ANINT(D)
\                  \ANINT         \R&arrow.ANINT(R)
\                  \DNINT         \D&arrow.DNINT(D)
.box off

.np
Nearest Integer
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\int(a+.5) if a>=0 \NINT generic \I&arrow.NINT(R)
\int(a-.5) if a<0  \             \I&arrow.NINT(D)
\                  \NINT         \I&arrow.NINT(R)
\                  \IDNINT       \I&arrow.IDNINT(D)
.box off

.np
Absolute Value
:cmt..section Absolute Value
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\(ar**2+ai**2)**1/2 \ABS generic \I&arrow.ABS(I)
\if a is complex;   \            \R&arrow.ABS(R)
\|a| otherwise      \            \D&arrow.ABS(D)
\                   \            \R&arrow.ABS(C)
\                   \            \D&arrow.ABS(Z) &dagger\
\                   \IABS        \I&arrow.IABS(I)
\                   \ABS         \R&arrow.ABS(R)
\                   \DABS        \D&arrow.DABS(D)
\                   \CABS        \R&arrow.CABS(C)
\                   \CDABS &dagger     \D&arrow.CDABS(Z)
.box off

.np
Remainder
:cmt..section Remainder
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\a1-int(a1/a2)*a2 \MOD generic \I&arrow.MOD(I,I)
\                 \            \R&arrow.MOD(R,R)
\                 \            \D&arrow.MOD(D,D)
\                 \MOD         \I&arrow.MOD(I,I)
\                 \AMOD        \R&arrow.AMOD(R,R)
\                 \DMOD        \D&arrow.DMOD(D,D)
.box off

.np
Transfer of Sign
:cmt..section Transfer of Sign
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\|a1| if a2>=0 \SIGN generic \I&arrow.SIGN(I,I)
\-|a1| if a2<0 \             \R&arrow.SIGN(R,R)
\              \             \D&arrow.SIGN(D,D)
\              \ISIGN        \I&arrow.ISIGN(I,I)
\              \SIGN         \R&arrow.SIGN(R,R)
\              \DSIGN        \D&arrow.DSIGN(D,D)
.box off

.np
Positive Difference
:cmt..section Positive Difference
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\a1-a2 if a1>a2 \DIM generic \I&arrow.DIM(I,I)
\0 if a1<=a2    \            \R&arrow.DIM(R,R)
\               \            \D&arrow.DIM(D,D)
\               \IDIM        \I&arrow.IDIM(I,I)
\               \DIM         \R&arrow.DIM(R,R)
\               \DDIM        \D&arrow.DDIM(D,D)
.box off

.np
Double Precision Product
:cmt..section Double Precision Product
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\a1*a2 \DPROD \D&arrow.DPROD(R,R)
.box off

.np
Choosing Largest Value
:cmt..section Choosing Largest Value
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\max(a1,a2,...) \MAX generic \I&arrow.MAX(I,...)
\               \            \R&arrow.MAX(R,...)
\               \            \D&arrow.MAX(D,...)
\               \MAX0        \I&arrow.MAX0(I,...)
\               \AMAX1       \R&arrow.AMAX1(R,...)
\               \DMAX1       \D&arrow.DMAX1(D,...)
\               \AMAX0       \R&arrow.AMAX0(I,...)
\               \MAX1        \I&arrow.MAX1(R,...)
.box off

.np
Choosing Smallest Value
:cmt..section Choosing Smallest Value
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\min(a1,a2,...) \MIN generic \I&arrow.MIN(I,...)
\               \            \R&arrow.MIN(R,...)
\               \            \D&arrow.MIN(D,...)
\               \MIN0        \I&arrow.MIN0(I,...)
\               \AMIN1       \R&arrow.AMIN1(R,...)
\               \DMIN1       \D&arrow.DMIN1(D,...)
\               \AMIN0       \R&arrow.AMIN0(I,...)
\               \MIN1        \I&arrow.MIN1(R,...)
.box off

.np
Length
:cmt..section Length
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\Length of character \LEN \I&arrow.LEN(CH)
\entity
.box off

.np
Length Without Trailing Blanks
:cmt..section Length Without Trailing Blanks
.bxt on &c0 &c3 &c1 &c2
\Definition\Name\Usage
.bxt
\Length of character \LENTRIM   \I&arrow.LENTRIM(CH)
\entity excluding    \          \
\trailing blanks     \
.bxt off

.np
Index of a Substring
:cmt..section Index of a Substring
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\Location of substring \INDEX \I&arrow.INDEX(CH,CH)
\a2 in string a1
.box off

.np
Imaginary Part of Complex Number
:cmt..section Imaginary Part of Complex Number
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\ai         \IMAG generic &dagger \R&arrow.IMAG(C)
\           \               \D&arrow.IMAG(Z)
\           \AIMAG          \R&arrow.AIMAG(C)
\           \DIMAG &dagger        \D&arrow.DIMAG(Z)
.box off

.np
Conjugate of a Complex Number
:cmt..section Conjugate of a Complex Number
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\(ar,-ai)   \CONJG generic &dagger \C&arrow.CONJG(C)
\           \                \Z&arrow.CONJG(Z)
\           \CONJG           \C&arrow.CONJG(C)
\           \DCONJG &dagger        \Z&arrow.DCONJG(Z)
.box off

.np
Square Root
:cmt..section Square Root
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\a**1/2 \SQRT generic \R&arrow.SQRT(R)
\       \             \D&arrow.SQRT(D)
\       \             \C&arrow.SQRT(C)
\       \             \Z&arrow.SQRT(Z) &dagger\
\       \SQRT         \R&arrow.SQRT(R)
\       \DSQRT        \D&arrow.DSQRT(D)
\       \CSQRT        \C&arrow.CSQRT(C)
\       \CDSQRT &dagger     \Z&arrow.CDSQRT(Z)
.box off

.np
Exponential
:cmt..section Exponential
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\e**a \EXP generic \R&arrow.EXP(R)
\     \            \D&arrow.EXP(D)
\     \            \C&arrow.EXP(C)
\     \            \Z&arrow.EXP(Z) &dagger\
\     \EXP         \R&arrow.EXP(R)
\     \DEXP        \D&arrow.DEXP(D)
\     \CEXP        \C&arrow.CEXP(C)
\     \CDEXP &dagger     \Z&arrow.CDEXP(Z)
.box off

.np
Natural Logarithm
:cmt..section Natural Logarithm
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\log (a) \LOG generic \R&arrow.LOG(R)
\    e   \            \D&arrow.LOG(D)
\        \            \C&arrow.LOG(C)
\        \            \Z&arrow.LOG(Z) &dagger\
\        \ALOG        \R&arrow.ALOG(R)
\        \DLOG        \D&arrow.DLOG(D)
\        \CLOG        \C&arrow.CLOG(C)
\        \CDLOG &dagger     \Z&arrow.CDLOG(Z)
.box off

.np
Common Logarithm
:cmt..section Common Logarithm
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\log  (a) \LOG10 generic \R&arrow.LOG10(R)
\   10    \              \D&arrow.LOG10(D)
\         \ALOG10        \R&arrow.ALOG10(R)
\         \DLOG10        \D&arrow.DLOG10(D)
.box off

.np
Sine
:cmt..section Sine
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\sin(a) \SIN generic \R&arrow.SIN(R)
\       \            \D&arrow.SIN(D)
\       \            \C&arrow.SIN(C)
\       \            \Z&arrow.SIN(Z) &dagger\
\       \SIN         \R&arrow.SIN(R)
\       \DSIN        \D&arrow.DSIN(D)
\       \CSIN        \C&arrow.CSIN(C)
\       \CDSIN &dagger     \Z&arrow.CDSIN(Z)
.box off

.np
Cosine
:cmt..section Cosine
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\cos(a) \COS generic \R&arrow.COS(R)
\       \            \D&arrow.COS(D)
\       \            \C&arrow.COS(C)
\       \            \Z&arrow.COS(Z) &dagger\
\       \COS         \R&arrow.COS(R)
\       \DCOS        \D&arrow.DCOS(D)
\       \CCOS        \C&arrow.CCOS(C)
\       \CDCOS &dagger     \Z&arrow.CDCOS(Z)
.box off

.np
Tangent
:cmt..section Tangent
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\tan(a)     \TAN generic     \R&arrow.TAN(R)
\           \                \D&arrow.TAN(D)
\           \TAN             \R&arrow.TAN(R)
\           \DTAN            \D&arrow.DTAN(D)
.box off

.np
Cotangent
:cmt..section Cotangent
.bxt on &c0 &c3 &c1 &c2
\Definition\Name\Usage
.bxt
\cotan(a)   \COTAN generic &dagger \R&arrow.COTAN(R)
\           \                \D&arrow.COTAN(D)
\           \COTAN &dagger         \R&arrow.COTAN(R)
\           \DCOTAN &dagger        \D&arrow.DCOTAN(D)
.bxt off

.np
Arcsine
:cmt..section Arcsine
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\arcsin(a)  \ASIN generic    \R&arrow.ASIN(R)
\           \                \D&arrow.ASIN(D)
\           \ASIN            \R&arrow.ASIN(R)
\           \DASIN           \D&arrow.DASIN(D)
.box off

.np
Arccosine
:cmt..section Arccosine
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\arccos(a)  \ACOS generic    \R&arrow.ACOS(R)
\           \                \D&arrow.ACOS(D)
\           \ACOS            \R&arrow.ACOS(R)
\           \DACOS           \D&arrow.DACOS(D)
.box off

.np
Arctangent
:cmt..section Arctangent
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\arctan(a)     \ATAN generic  \R&arrow.ATAN(R)
\              \              \D&arrow.ATAN(D)
\              \ATAN          \R&arrow.ATAN(R)
\              \DATAN         \D&arrow.DATAN(D)
\arctan(a1/a2) \ATAN2 generic \R&arrow.ATAN2(R,R)
\              \              \D&arrow.ATAN2(D,D)
\              \ATAN2         \R&arrow.ATAN2(R,R)
\              \DATAN2        \D&arrow.DATAN2(D,D)
.box off

.np
Hyperbolic Sine
:cmt..section Hyperbolic Sine
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\sinh(a)    \SINH generic    \R&arrow.SINH(R)
\           \                \D&arrow.SINH(D)
\           \SINH            \R&arrow.SINH(R)
\           \DSINH           \D&arrow.DSINH(D)
.box off

.np
Hyperbolic Cosine
:cmt..section Hyperbolic Cosine
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\cosh(a)    \COSH generic    \R&arrow.COSH(R)
\           \                \D&arrow.COSH(D)
\           \COSH            \R&arrow.COSH(R)
\           \DCOSH           \D&arrow.DCOSH(D)
.box off

.np
Hyperbolic Tangent
:cmt..section Hyperbolic Tangent
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\tanh(a)    \TANH generic    \R&arrow.TANH(R)
\           \                \D&arrow.TANH(D)
\           \TANH            \R&arrow.TANH(R)
\           \DTANH           \D&arrow.DTANH(D)
.box off

.np
Gamma Function
:cmt..section Gamma Function
.bxt on &c0 &c3 &c1 &c2
\Definition\Name\Usage
.bxt
\gamma(a)   \GAMMA generic   \R&arrow.GAMMA(R)
\           \                \D&arrow.GAMMA(D)
\           \GAMMA           \R&arrow.GAMMA(R)
\           \DGAMMA          \D&arrow.DGAMMA(D)
.bxt off

.np
Natural Log of Gamma Function
:cmt..section Natural Log of Gamma Function
.bxt on &c0 &c3 &c1 &c2
\Definition\Name\Usage
.bxt
\log (gamma(a))     \LGAMMA generic  \R&arrow.LGAMMA(R)
\    e              \                \D&arrow.LGAMMA(D)
\                   \ALGAMA          \R&arrow.ALGAMA(R)
\                   \DLGAMA          \D&arrow.DLGAMA(D)
.bxt off

.np
Error Function
:cmt..section Error Function
.bxt on &c0 &c3 &c1 &c2
\Definition\Name\Usage
.bxt
\erf(a)     \ERF generic     \R&arrow.ERF(R)
\           \                \D&arrow.ERF(D)
\           \ERF             \R&arrow.ERF(R)
\           \DERF            \D&arrow.DERF(D)
.bxt off

.np
Complement of Error Function
:cmt..section Complement of Error Function
.bxt on &c0 &c3 &c1 &c2
\Definition\Name\Usage
.bxt
\1-erf(a)   \ERFC generic    \R&arrow.ERFC(R)
\           \                \D&arrow.ERFC(D)
\           \ERFC            \R&arrow.ERFC(R)
\           \DERFC           \D&arrow.DERFC(D)
.bxt off

.np
Lexically Greater Than or Equal
:cmt..section Lexically Greater Than or Equal
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\a1>=a2     \LGE     \L&arrow.LGE(CH,CH)
.box off

.np
Lexically Greater Than
:cmt..section Lexically Greater Than
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\a1>a2      \LGT     \L&arrow.LGT(CH,CH)
.box off

.np
Lexically Less Than or Equal
:cmt..section Lexically Less Than or Equal
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\a1<=a2     \LLE     \L&arrow.LLE(CH,CH)
.box off

.np
Lexically Less Than
:cmt..section Lexically Less Than
.box on &c0 &c1 &c2 &c3
\Definition\Name\Usage
.box
\a1<a2      \LLT     \L&arrow.LLT(CH,CH)
.box off

.np
Binary Pattern Processing Functions: Boolean Operations
:cmt..section Binary Pattern Processing Functions: Boolean Operations
.bxt on &c0 &c3 &c1 &c2
\Definition\Name\Usage
.bxt
\Boolean AND        \IAND            \I&arrow.IAND(I,I)
.bxt
\Boolean inclusive OR       \IOR     \I&arrow.IOR(I,I)
.bxt
\Boolean exclusive OR       \IEOR    \I&arrow.IEOR(I,I)
.bxt
\Boolean complement \NOT             \I&arrow.NOT(I)
.bxt off

.np
Binary Pattern Processing Functions: Shift Operations
:cmt..section Binary Pattern Processing Functions: Shift Operations
.bxt on &c0 &c3 &c1 &c2
\Definition\Name\Usage
.bxt
\Logical shift      \ISHL            \I&arrow.ISHL(I,I)
\                   \ISHFT           \I&arrow.ISHFT(I,I)
.bxt
\Arithmetic shift   \ISHA            \I&arrow.ISHA(I,I)
.bxt
\Circular shift     \ISHC            \I&arrow.ISHC(I,I)
.bxt off

.np
Binary Pattern Processing Functions: Bit Testing
:cmt..section Binary Pattern Processing Functions: Bit Testing
.bxt on &c0 &c3 &c1 &c2
\Definition\Name\Usage
.bxt
\Test bit           \BTEST           \L&arrow.BTEST(I,I)
\a2'th bit of a1 is tested.\
\If it is 1, .TRUE. is  \
\returned.              \
\If it is 0, .FALSE. is \
\returned.
.bxt off

.np
Binary Pattern Processing Functions: Set Bit
:cmt..section Binary Pattern Processing Functions: Set Bit
.bxt on &c0 &c3 &c1 &c2
\Definition\Name\Usage
.bxt
\Set bit            \IBSET           \I&arrow.IBSET(I,I)
\Return a1 with a2'th  \
\bit set               \
.bxt off

.np
Binary Pattern Processing Functions: Clear Bit
:cmt..section Binary Pattern Processing Functions: Clear Bit
.bxt on &c0 &c3 &c1 &c2
\Definition\Name\Usage
.bxt
\Clear bit          \IBCLR           \I&arrow.IBCLR(I,I)
\Return a1 with a2'th \
\bit cleared          \
.bxt off

.np
Binary Pattern Processing Functions: Change Bit
:cmt..section Binary Pattern Processing Functions: Change Bit
.bxt on &c0 &c3 &c1 &c2
\Definition\Name\Usage
.bxt
\Change bit         \IBCHNG          \I&arrow.IBCHNG(I,I)
\Return a1 with a2'th \
\bit complemented     \
.bxt off

.np
Allocated Array
:cmt..section Allocated Array
.bxt on &c0 &c3 &c1 &c2
\Definition\Name\Usage
.bxt
\Is array A allocated?      \ALLOCATED       \L&arrow.ALLOCATED(A)
.bxt off

.np
Memory Location
:cmt..section Memory Location
.bxt on &c0 &c3 &c1 &c2
\Definition\Name\Usage
.bxt
\location of A where      \LOC  \I&arrow.LOC(A)
\A is any variable, array \
\or array element         \
.bxt off

.np
Size of Variable or Structure
:cmt..section Size of Variable or Structure
.bxt on &c0 &c3 &c1 &c2
\Definition\Name\Usage
.bxt
\size of A in bytes where \ISIZEOF  \I&arrow.ISIZEOF(A)
\A is any constant,       \
\variable, array, or      \
\structure                \
.bxt off

.np
Volatile Reference
:cmt..section Volatile Reference
.bxt on &c0 &c3 &c1 &c2
\Definition\Name\Usage
.bxt
\A is a volatile          \VOLATILE  \A&arrow.VOLATILE(A)
\reference                \
.bxt off

:CMT.This is from gui\wguispy.gml and is in Spy Help
:CMT.Topic: "Predefined Spy Message Classes" (multiple subsections)

.np
The sections that follow list the messages contained in each message class
that &spy defines for you.
.*
.tb set $
.tb &INDlvl.+2 &syscl./2+1
.*
.beglevel
.*
.np
Clipboard Messages
:cmt..section Clipboard Messages
.*
:cmt..np
.monoon
$WM_ASKCBFORMATNAME $WM_CHANGECBCHAIN
$WM_CLEAR           $WM_CLIPBOARDUPDATE (**)
$WM_COPY            $WM_CUT
$WM_DESTROYCLIPBOARD$WM_DRAWCLIPBOARD
$WM_HSCROLLCLIPBOARD$WM_PAINTCLIPBOARD
$WM_PASTE           $WM_RENDERALLFORMATS
$WM_RENDERFORMAT    $WM_SIZECLIPBOARD
$WM_UNDO            $WM_VSCROLLCLIPBOARD
.monooff
.*
.np
DDE Messages
:cmt..section DDE Messages
.*
:cmt..np
.monoon
$WM_DDE_ACK         $WM_DDE_ADVISE
$WM_DDE_DATA        $WM_DDE_EXECUTE
$WM_DDE_INITIATE    $WM_DDE_POKE
$WM_DDE_REQUEST     $WM_DDE_TERMINATE
$WM_DDE_UNADVISE
.monooff
.*
.np
Init Messages
:cmt..section Init Messages
.*
:cmt..np
.monoon
$WM_INITDIALOG      $WM_INITMENU
$WM_INITMENUPOPUP   $WM_UNINITMENUPOPUP (**)

.monooff
.*
.np
Input Messages
:cmt..section Input Messages
.*
:cmt..np
.monoon
$WM_APPCOMMAND (**)    $WM_BEGINDRAG (*)
$WM_CHAR               $WM_CHARTOITEM
$WM_COMMAND            $WM_CONTEXTMENU (**)
$WM_COPYDATA (**)      $WM_DEADCHAR
$WM_DRAGLOOP (*)       $WM_DRAGMOVE (*)
$WM_DRAGSELECT (*)     $WM_DROPOBJECT (*)
$WM_EXITSIZEMEOVE (*)  $WM_HSCROLL
$WM_INPUT (**)         $WM_INPUT_DEVICE_CHANGE (**)
$WM_INPUTLANGCHANGE (**)$WM_INPUTLANGCHANGEREQUEST (**)
$WM_KEYDOWN            $WM_KEYUP
$WM_LBTRACKPOINT (*)   $WM_MENUCHAR
$WM_MENUCOMMAND (**)   $WM_MENUDRAG (**)
$WM_MENUGETOBJECT (**) $WM_MENURBUTTONUP (**)
$WM_MENUSELECT         $WM_NEXTMENU (*)
$WM_NOTIFY (**)        $WM_NOTIFYFORMAT (**)
$WM_QUERYDROPOBJECT (*)$WM_TCARD (**)
$WM_UNICHAR (**)       $WM_VSCROLL
.monooff
.*
.np
MDI Messages
:cmt..section MDI Messages
.*
:cmt..np
.monoon
$WM_ISACTIVEICON (*)$WM_MDIACTIVATE
$WM_MDICASCADE      $WM_MDICREATE
$WM_MDIDESTROY      $WM_MDIGETACTIVE
$WM_MDIICONARRANGE  $WM_MDIMAXIMIZE
$WM_MDINEXT         $WM_MDIRESTORE
$WM_MDISETMENU      $WM_MDITILE
.monooff
.*
.np
Mouse Messages
:cmt..section Mouse Messages
.*
:cmt..np
.monoon
$WM_CAPTURECHANGED (**)$WM_LBUTTONDBLCLK
$WM_LBUTTONDOWN     $WM_LBUTTONUP
$WM_MBUTTONDBLCLK   $WM_MBUTTONDOWN
$WM_MBUTTONUP       $WM_MOUSEACTIVATE   
$WM_MOUSEHOVER (**) $WM_MOUSEHWHEEL (**)
$WM_MOUSELEAVE (**) $WM_MOUSEMOVE
$WM_MOUSEWHEEL (**) $WM_RBUTTONDBLCLK
$WM_RBUTTONDOWN     $WM_RBUTTONUP
$WM_SETCURSOR       $WM_XBUTTONDBLCLK (**)
$WM_XBUTTONDOWN (**)$WM_XBUTTONUP (**)
.monooff
.*
.np
NC Misc Messages
:cmt..section NC Misc Messages
.*
:cmt..np
.monoon
$WM_NCACTIVATE      $WM_NCCALCSIZE
$WM_NCCREATE        $WM_NCDESTROY
$WM_NCPAINT
.monooff
.*
.np
NC Mouse Messages
:cmt..section NC Mouse Messages
.*
:cmt..np
.monoon
$WM_NCHITTEST       $WM_NCLBUTTONDBLCLK
$WM_NCLBUTTONDOWN   $WM_NCLBUTTONUP
$WM_NCMBUTTONDBLCLK $WM_NCMBUTTONDOWN
$WM_NCMBUTTONUP     $WM_NCMOUSEHOVER (**)
$WM_NCMOUSELEAVE (**)$WM_NCMOUSEMOVE
$WM_NCRBUTTONDBLCLK $WM_NCRBUTTONDOWN
$WM_NCRBUTTONUP     $WM_NCXBUTTONDBLCLK (**)
$WM_NCXBUTTONDOWN (**)$WM_NCXBUTTONUP (**)
.monooff
.*
.np
Other Messages
:cmt..section Other Messages
.*
:cmt..np
.monoon
$WM_CHANGEUISTATE (**)$WM_COALESCE_FIRST
$WM_COALESCE_LAST     $WM_COMMNOTIFY
$WM_CONVERTREQUEST (*)$WM_CONVERTRESULT (*)
$WM_DROPFILES         $WM_INTERIM (*)
$WM_PENWINFIRST       $WM_PENWINLAST
$WM_PRINT (**)        $WM_PRINTCLIENT (**)
$WM_QUERYSAVESTATE (*)$WM_QUEUESYNC
$WM_QUERYUISTATE (**) $WM_SIZEWAIT (*)
$WM_SYNCTASK (*)      $WM_TESTING (*)
$WM_UPDATEUISTATE (**)$WM_YOMICHAR (*)
.monooff
.*
.np
System Messages
:cmt..section System Messages
.*
:cmt..np
.monoon
$WM_CANCELJOURNAL (**)$WM_COMPACTING
$WM_DEVMODECHANGE     $WM_DEVICECHANGE (**)
$WM_DISPLAYCHANGE (**)$WM_ENDSESSION
$WM_ENTERIDLE         $WM_ENTERMENULOOP
$WM_EXITMENULOOP      $WM_FILESYSCHANGE (*)
$WM_NULL              $WM_POWER
$WM_POWERBROADCAST (**)$WM_SPOOLERSTATUS
$WM_SYSCHAR           $WM_SYSCOLORCHANGE
$WM_SYSCOMMAND        $WM_SYSDEADCHAR
$WM_SYSKEYDOWN        $WM_SYSKEYUP
$WM_SYSTEMERROR (*)   $WM_SYSTIMER (*)
$WM_THEMECHANGED (**) $WM_TIMECHANGE
$WM_TIMER             $WM_USERCHANGED (**)
$WM_WININICHANGE      $WM_WTSSESSION_CHANGE (**)
.monooff
.*
.np
User Messages
:cmt..section User Messages
.*
:cmt..np
.monoon
$WM_USER
.monooff
.*
.np
Unknown Messages
:cmt..section Unknown Messages
.*
:cmt..np
.monoon
$None
.monooff
.*
.np
Window Messages
:cmt..section Window Messages
.*
:cmt..np
.monoon
$WM_ACTIVATE           $WM_ACTIVATEAPP
$WM_ACTIVATESHELLWINDOW (*)$WM_ALTTABACTIVE (*)
$WM_CANCELMODE         $WM_CREATE
$WM_CHILDACTIVATE      $WM_CLOSE
$WM_COMPAREITEM        $WM_CTLCOLOR
$WM_CTLCOLORBTN (**)   $WM_CTLCOLORDLG (**)
$WM_CTLCOLOREDIT (**)  $WM_CTLCOLORLISTBOX (**)
$WM_CTLCOLORMSGBOX (**)$WM_CTLCOLORSCROLLBAR (**)
$WM_CTLCOLORSTATIC (**)$WM_DELETEITEM
$WM_DESTROY            $WM_DRAWITEM
$WM_DWMCOLORIZATIONCHANGED (**)$WM_DWMCOMPOSITIONCHANGED (**)
$WM_DWMNCRENDERINGCHANGED (**)$WM_DWMWINDOWMAXIMIZEDCHANGE (**)
$WM_ENABLE             $WM_ENTERSIZEMOVE (*)
$WM_ERASEBKGND         $WM_FONTCHANGE
$WM_GETDLGCODE         $WM_GETFONT
$WM_GETHOTKEY          $WM_GETICON (**)
$WM_GETMINMAXINFO      $WM_GETTEXT
$WM_GETTEXTLENGTH      $WM_GETTITLEBARINFOEX (**)
$WM_HELP (**)          $WM_ICONERASEBKGND
$WM_KILLFOCUS          $WM_MEASUREITEM
$WM_MOVE               $WM_MOVING (**)
$WM_NEXTDLGCTL         $WM_OTHERWINDOWCREATED
$WM_OTHERWINDOWDESTROYED$WM_PAINT
$WM_PAINTICON          $WM_PALETTECHANGED
$WM_PALETTEISCHANGING  $WM_PARENTNOTIFY
$WM_QUERYDRAGICON      $WM_QUERYENDSESSION
$WM_QUERYNEWPALETTE    $WM_QUERYOPEN
$WM_QUERYPARKICON (*)  $WM_QUIT
$WM_SETFOCUS           $WM_SETFONT
$WM_SETHOTKEY          $WM_SETICON (**)
$WM_SETREDRAW          $WM_SETTEXT
$WM_SETVISIBLE (*)     $WM_SHOWWINDOW
$WM_SIZE               $WM_SIZING (**)
$WM_STYLECHANGED (**)  $WM_STYLECHANGING (**)
$WM_SYNCPAINT (*)      $WM_VKEYTOITEM
$WM_WINDOWPOSCHANGED   $WM_WINDOWPOSCHANGING
.monooff
.*
:cmt..endlevel
.*
.tb set
.tb
.*

:CMT.This is from lg\spdos.gml and is in the Linker Help Guide
:CMT.Topic: "How Overlay Files are Opened"

.np
.np
Legal values for the sharing mode are as follows.
:cmt..illust begin
.co off
.tb set `
c.tb 1 +15
Sharing Mode`Value
-----------------`-------
compatibility mode`0x00
deny read/write mode`0x01
deny write mode`0x02
deny read mode`0x03
deny none mode`0x04
.tb set
.tb
:cmt..illust end
.co on

:CMT.This is from rsi\config.gml and is in both of the Programmer's Guides
:CMT.Topic: "Changing the Switch Mode Setting"

.se c0=&INDlvl+2
.se c1=&c0.+5
.se c2=&c1.+12
.se c3=&c2.+8
.se c4=&c3.+8
.se c5=&c4.+22
.tb set $
.tb &c0 &c1 &c2 &c3 &c4 &c5
:cmt..se c0=&c0.-1
:cmt..se c1=&c1.-1
:cmt..se c2=&c2.-1
:cmt..se c3=&c3.-1
:cmt..se c4=&c4.-1
:cmt..se c5=&c5.-1
:cmt..bx on &c0 &c1 &c2 &c3 &c4 &c5
.np
Switch Mode Settings
$      $               $       $Alternate
$Status$Machine        $Setting$Name      $Comment
:cmt..bx
$auto  $386/486 w/ DPMI$0      $None      $Set automatically if DPMI is active
$req'd $NEC 98-series  $1      $9801      $Must be set for NEC 98-series
$auto  $PS/2           $2      $None      $Set automatically for PS/2
$auto  $386/486        $3      $386, 80386$Set automatically for 386 or 486
$auto  $386            $INBOARD$None      $386 with Intel Inboard
$req'd $Fujitsu FMR-70 $5      $None      $Must be set for Fujitsu FMR-70
$auto  $386/486 w/ VCPI$11     $None      $Set automatically if VCPI detected
$req'd $Hitachi B32    $14     $None      $Must be set for Hitachi B32
$req'd $OKI if800      $15     $None      $Must be set for OKI if800
$option$IBM PS/55      $16     $None      $May be needed for some PS/55s
:cmt..bx off
.tb set
.tb

:CMT.This is from rsi\config.gml and is in both of the Programmer's Guides
:CMT.Topic: "Changing the Switch Mode Setting"

.np
Specific Machine Switch Mode Setting
.se c0=&INDlvl+2+8
.se c1=&INDlvl+2+29
.se c2=&INDlvl+2+39
.tb set $
.tb &c0 &c1
.se c0=&c0.-2
.se c1=&c1.-2
.se c2=&c2.-2
:cmt..bx &c0 &c1 &c2
$Machine$Setting
:cmt..bx
$NEC 98-series       $1
$Fujitsu FMR-60,-70  $5
$Hitachi B32         $14
$OKI if800           $15
:cmt..bx off
.tb set
.tb

:CMT.This is from rsi\int31.gml and is in both of the Programmer's Guides
:CMT.Topic: "Translation Services"

.np
Register Translation
.se *p0=&INDlvl-1+11
.se *c1=8
.se *c2=41
.cp 40
.tb set $
.tb &*p0 +&*c1 +&*c2
:cmt..bx on &*p0-3 +&*c1 +&*c2
$Offset$Register
:cmt..bx
$00H$EDI
:cmt..bx
$04H$ESI
:cmt..bx
$08H$EBP
:cmt..bx
$0CH$Reserved by system
:cmt..bx
$10H$EBX
:cmt..bx
$14H$EDX
:cmt..bx
$18H$ECX
:cmt..bx
$1CH$EAX
:cmt..bx
$20H$Flags
:cmt..bx
$22H$ES
:cmt..bx
$24H$DS
:cmt..bx
$26H$FS
:cmt..bx
$28H$GS
:cmt..bx
$2AH$IP
:cmt..bx
$2CH$CS
:cmt..bx
$2EH$SP
:cmt..bx
$30H$SS
:cmt..bx off
.tb set
.tb

:CMT.This is from rsi\int31.gml and is in both of the Programmer's Guides
:CMT.Topic: "Memory Management Services"

.np
Return Buffer Contents
.se *p0=&INDlvl-1+11+3
.se *c1=8
.se *c2=40
.cp 28
.tb set $
.tb &*p0. +&*c1. +&*c2.
:cmt..bx on &*p0.-3 +&*c1. +&*c2.
$Offset$Description
:cmt..bx
$00H$Largest available block, in bytes
:cmt..bx
$04H$Maximum unlocked page allocation
:cmt..bx
$08H$Largest block of memory (in pages) that could$
$   $be allocated and then locked
:cmt..bx
$0CH$Total linear address space size, in pages, including$
$   $already allocated pages
:cmt..bx
$10H$Total number of free pages and pages currently$
$   $unlocked and available for paging out
:cmt..bx
$14H$Number of physical pages not in use
:cmt..bx
$18H$Total number of physical pages managed by host
:cmt..bx
$1CH$Free linear address space, in pages
:cmt..bx
$20H$Size of paging/file partition, in pages
:cmt..bx
$24H -$Reserved
$2FH$
:cmt..bx off
.tb set
.tb

:CMT.This is from rsi\nt2cls.gml and is in both of the Programmer's Guides
:CMT.Topic: "Interrupt 21H Functions"

.np
Int 21h Functions
.se c0=&INDlvl+2
.se c1=&INDlvl+5
.se c2=&INDlvl+9
.se c3=&INDlvl+53
.tb set $
.tb &c0 &c1 &c2 &c3.r
$Function$Purpose$Managed Registers
.sk 1 c
$00H$$Terminate Process                        $None
$01H$$Character Input with Echo                $None
$02H$$Character Output                         $None
$03H$$Auxiliary Input                          $None
$04H$$Auxiliary Output                         $None
$05H$$Print Character                          $None
$06H$$Direct Console I/O                       $None
$07H$$Unfiltered Character Input Without Echo  $None
$08H$$Character Input Without Echo             $None
$09H$$Display String                           $EDX
$0AH$$Buffered Keyboard Input                  $EDX
$0BH$$Check Keyboard Status                    $None
$0CH$$Flush Buffer, Read Keyboard              $EDX
$0DH$$Disk Reset                               $None
$0EH$$Select Disk                              $None
$0FH$$Open File with FCB                       $EDX
.sk 1 c
$10H$$Close File with FCB                      $EDX
$11H$$Find First File                          $EDX
$12H$$Find Next File                           $EDX
$13H$$Delete File                              $EDX
$14H$$Sequential Read                          $EDX
$15H$$Sequential Write                         $EDX
$16H$$Create File with FCB                     $EDX
$17H$$Rename File                              $EDX
$19H$$Get Current Disk                         $None
$1AH$$Set DTA Address                          $EDX
$1BH$$Get Default Drive Data                   $Returns in EBX, ECX, and EDX
$1CH$$Get Drive Data                           $Returns in EBX, ECX, and EDX
.sk 1 c
$21H$$Random Read                              $EDX
$22H$$Random Write                             $EDX
$23H$$Get File Size                            $EDX
$24H$$Set Relative Record                      $EDX
$25H$$Set Interrupt Vector                     $EDX
$26H$$Create New Program Segment Prefix        $None
$27H$$Random Block Read                        $EDX, returns in ECX
$28H$$Random Block Write                       $EDX, returns in ECX
$29H$$Parse Filename                           $ESI, EDI, returns in EAX, ESI and EDI (1.)
$2AH$$Get Date                                 $Returns in ECX
$2BH$$Set Date                                 $None
$2CH$$Get Time                                 $None
$2DH$$Set Time                                 $None
$2EH$$Set/Reset Verify Flag                    $None
$2FH$$Get DTA Address                          $Returns in EBX
.sk 1 c
$30H$$Get MS-DOS Version Number                $Returns in ECX
$31H$$Terminate and Stay Resident              $None
$33H$$Get/Set Control-C Check Flag             $None
$34H$$Return Address of InDOS Flag             $Returns in EBX
$35H$$Get Interrupt Vector                     $Returns in EBX
$36H$$Get Disk Free Space                      $Returns in EAX, EBX, ECX, and EDX
$38H$$Get/Set Current Country                  $EDX, returns in EBX
$39H$$Create Directory                         $EDX
$3AH$$Remove Directory                         $EDX
$3BH$$Change Current Directory                 $EDX
$3CH$$Create File with Handle                  $EDX, returns in EAX
$3DH$$Open File with Handle                    $EDX, returns in EAX
$3EH$$Close File                               $None
$3FH$$Read File or Device                      $EBX, ECX, EDX, returns in EAX (2.)
.sk 1 c
$40H$$Write File or Device                     $EBX, ECX, EDX, returns in EAX (2.)
$41H$$Delete File                              $EDX
$42H$$Move File Pointer                        $Returns in EDX, EAX
$43H$$Get/Set File Attribute                   $EDX, returns in ECX
$44H$$IOCTL                                    $(3.)
$$00H$Get Device Information                   $Returns in EDX
$$01H$SetDevice Information                    $None
$$02H$Read Control Data from CDD               $EDX, returns in EAX
$$03H$Write Control Data to CDD                $EDX, returns in EAX
$$04H$Read Control Data from BDD               $EDX, returns in EAX
$$05H$Write Control Data to BDD                $EDX, returns in EAX
$$06H$Check Input Status                       $None
$$07H$Check Output Status                      $None
$$08H$Check if Block Device is Removeable      $Returns in EAX
$$09H$Check if Block Device is Remote          $Returns in EDX
$$0AH$Check if Handle is Remote                $Returns in EDX
$$0BH$Change Sharing Retry Count               $None
$$0CH$Generic I/O Control for Character Devices$EDX
$$0DH$Generic I/O Control for Block Devices    $EDX
$$0EH$Get Logical Drive Map                    $None
$$0FH$Set Logical Drive Map                    $None
$45H$$Duplicate File Handle                    $Returns in EAX
$46H$$Force Duplicate File Handle              $None
$47H$$Get Current Directory                    $ESI
$48H$$Allocate Memory Block                    $Returns in EAX
$49H$$Free Memory Block                        $None
$4AH$$Resize Memory Block                      $None
$4BH$$Load and Execute Program (EXEC)          $EBX, EDX (4.)
$4CH$$Terminate Process with Return Code       $None
$4DH$$Get Return Code of Child Process         $None
$4EH$$Find First File                          $EDX
$4FH$$Find Next File                           $None
.sk 1 c
:cmt.$52H$$Get List of Lists                        $Returns in EBX
$52H$$Get List of Lists                        $(not supported)
$54H$$Get Verify Flag                          $None
$56H$$Rename File                              $EDX, EDI
$57H$$Get/Set Date/Time of File                $Returns in ECX, and EDX
$58H$$Get/Set Allocation Strategy              $Returns in EAX
$59H$$Get Extended Error Information           $Returns in EAX
$5AH$$Create Temporary File                    $EDX, returns in EAX and EDX
$5BH$$Create New File                          $EDX, returns in EAX
$5CH$$Lock/Unlock File Region                  $None
$5EH$$Network Machine Name/Printer Setup$
$$00H$Get Machine Name                         $EDX
$$02H$Set Printer Setup String                 $ESI
$$03H$Get Printer Setup String                 $EDI, returns in ECX
$5FH$$Get/Make Assign List Entry$
$$02H$Get Redirection List Entry               $ESI, EDI, returns in ECX
$$03H$Redirect Device                          $ESI, EDI
$$04H$Cancel Device Redirection                $ESI
.sk 1 c
$62H$$Get Program Segment Prefix Address       $Returns in EBX
$63H$$Get Lead Byte Table (version 2.25 only)  $Returns in ESI
$65H$$Get Extended Country Information         $EDI
$66H$$Get or Set Code Page                     $None
$67H$$Set Handle Count                         $None
.tb set
.tb

:CMT.This is from wgmlref\rfabst.gml and is in both of the Programmer's Guides
:CMT.Topic: "Abstract"

.np
From the Abstract
.tb set $
.tb 2 +18 +7
:cmt..bx on &bxindent +37
$WATCOM            $Telephone:$(519) 886-3700
$415 Phillip Street$FAX:      $(519) 747-4971
$Waterloo, Ontario $BBS:      $(519) 884-2103
$CANADA   N2L 3X2  $
.tb set
.tb
:cmt..bx off

:CMT.This is from cl\constant.gml and is in the C Language Reference
:CMT.Topic: "Integer Constants"

.np
Constants and Types
.boxdef
.boxcol 17
.boxcol 38
.boxbeg
$Constant              $Type
.boxline
.monoon
$:HP0.unsuffixed decimal:eHP0.    $int:HP0.,:eHP0. long:HP0.,:eHP0. unsigned long
.monooff
.boxline
.monoon
$:HP0.unsuffixed octal:eHP0.      $int:HP0.,:eHP0. unsigned int:HP0.,:eHP0. long:HP0.,:eHP0. unsigned long
.monooff
.boxline
.monoon
$:HP0.unsuffixed hexadecimal:eHP0.$int:HP0.,:eHP0. unsigned int:HP0.,:eHP0. long:HP0.,:eHP0. unsigned long
.monooff
.boxline
.monoon
$:HP0.suffix:eHP0. U :HP0.only:eHP0.         $unsigned int:HP0.,:eHP0. unsigned long
.monooff
.boxline
.monoon
$:HP0.suffix:eHP0. L :HP0.only:eHP0.         $long:HP0.,:eHP0. unsigned long
.monooff
.boxline
.monoon
$:HP0.suffixes:eHP0. U :HP0.and:eHP0. L$unsigned long
.monooff
.boxline
.monoon
$:HP0.suffix:eHP0. LL :HP0.only:eHP0.        $long long:HP0.,:eHP0. unsigned long long
.monooff
.boxline
.monoon
$:HP0.suffixes:eHP0. U :HP0.and:eHP0. LL$unsigned long long
.monooff
.boxend

:CMT.This is from cl\constant.gml and is in the C Language Reference
:CMT.Topic: "Integer Constants"

.np
The following table illustrates a number of constants and their
interpretation and type:
.*
.************************************************************************
.*
..sk 1 c
.boxdef
.boxcol 11
.boxcol 11
.boxcol 9
.boxcol 14
.boxcol 13
.boxbeg
$$$Hexa$
$$Decimal$  -decimal$&wc286.$&wc386.
$ Constant$ Value$ Value$ Type$ Type
.boxline
.smonoon
$33        $33        $21      $signed int   $signed int
$033       $27        $1B      $signed int   $signed int
$0x33      $51        $33      $signed int   $signed int
$33333     $33333     $8235    $signed long  $signed int
$033333    $14043     $36DB    $signed int   $signed int
$0xA000    $40960     $A000    $unsigned int $signed int
$0x33333   $209715    $33333   $signed long  $signed int
$0x80000000$2147483648$80000000$unsigned long$unsigned int
$2147483648$2147483648$80000000$unsigned long$unsigned int
$4294967295$4294967295$FFFFFFFF$unsigned long$unsigned int
.smonooff
.boxend

:CMT.This is from cl\constant.gml and is in the C Language Reference
:CMT.Topic: "Floating-Point Constants"

.np
The following table illustrates a number of floating-point constants
and their type:
.boxdef
.boxcol 11
.boxcol 13
.boxcol 12
.boxbeg
$ Constant $   Value    $ Type
.boxline
.monoon
$3.14159265$3.14159265E0$double
$11E24     $1.1E25      $double
$.5L       $5E-1        $long double
$7.234E-22F$7.234E-22   $float
$0.        $0E0         $double
.monooff
.boxend

:CMT.This is from cl\constant.gml and is in the C Language Reference
:CMT.Topic: "Character Constants"

These characters can be entered using the following escape
sequences:
.boxdef
.boxcol  8 c
.boxcol 14
.boxcol 16
.boxbeg
$Character$  Character Name $Escape Sequence
.boxline
.monoon
$'$:HP0.single quote:eHP0.     $\'
$"$:HP0.double quote:eHP0.     $" :HP0.or:eHP0. \"
$?$:HP0.question mark:eHP0.    $? :HP0.or:eHP0. \?
$\$:HP0.backslash:eHP0.        $\\
$ $:HP0.octal value:eHP0.      $\:ITAL.octal digits:eITAL.:HP0. (max 3):eHP0.
$ $:HP0.hexadecimal value:eHP0.$\x:ITAL.hexadecimal digits:eITAL.
.monooff
.boxend

:CMT.This is from cl\convert.gml and is in the C Language Reference
:CMT.Topic: "Signed and Unsigned Integer Conversion"

.np
Examples of 32-bit quantities being converted to 16-bit quantities.
.boxdef
.boxcol  7
.boxcol 13
.boxcol 11
.boxcol  7
.boxcol  9
.boxbeg
.monoon
$    $:HP0.32-bit:eHP0.        $:HP0.16-bit:eHP0.        $signed$unsigned
$long$:HP0.representation:eHP0.$:HP0.representation:eHP0.$short $short
.monooff
.boxline
.monoon
$65538 $0x00010002$0x0002$2     $2
$100000$0x000186A0$0x86A0$-31072$34464
.monooff
.boxend

:CMT.This is from cl\convert.gml and is in the C Language Reference
:CMT.Topic: "Signed and Unsigned Integer Conversion"

.np
Examples of 16-bit signed quantities being
converted to 32-bit quantities.
.boxdef
.boxcol  7
.boxcol 10
.boxcol 12
.boxcol  8
.boxcol 12
.boxbeg
.monoon
$signed$:HP0.16-bit$32-bit:eHP0.$signed$unsigned
$short $:HP0.represention$representation:eHP0.$long  $long
.monooff
.boxline
.monoon
$-2    $0xFFFE $0xFFFFFFFE$-2   $4294967294
$32766 $0x7FFE $0x00007FFE$32766$32766
.monooff
.boxend

:CMT.This is from cl\convert.gml and is in the C Language Reference
:CMT.Topic: "Arithmetic Conversion"

.np
The following table illustrates the result type of performing
an addition on combinations of various types:

.boxdef
.boxcol 28
.boxcol 14
.boxbeg
$ Operation                     $ Result Type
.boxline
.monoon
$signed char + signed char  $signed int
$unsigned char + signed int $signed int
$signed int + signed int    $signed int
$signed int + unsigned int  $unsigned int
$unsigned int + signed long $signed long
$signed int + unsigned long $unsigned long
$signed char + float        $float
$signed long + double       $double
$float + double             $double
$float + long double        $long double
.monooff
.boxend

:CMT.This is from cl\enumtab.gml and is in the C Language Reference
:CMT.Topic: "Structures, Unions, Enumerations and Bit-Fields"

.np
Enumeration sizes.
.boxdef
.boxcol 18
.boxcol 20
.boxcol 20
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

:CMT.This is from cl\escape.gml and is in the C Language Reference
:CMT.Topic: "Character Constants"

.np
In addition to the above escape sequences, the following escape sequences may be used to represent
non-graphic characters:
.boxdef
.boxcol  8 c
.boxcol 25
.boxbeg
$Escape$
$Sequence$ Meaning
.boxline
.monoon
$\a$:HP0.Causes an audible or visual alert:eHP0.
$\b$:HP0.Back up one character:eHP0.
$\f$:HP0.Move to the start of the next page:eHP0.
$\n$:HP0.Move to the start of the next line:eHP0.
$\r$:HP0.Move to the start of the current line:eHP0.
$\t$:HP0.Move to the next horizontal tab:eHP0.
$\v$:HP0.Move to the next vertical tab:eHP0.
.monooff
.boxend

:CMT.This is from cl\express.gml and is in the C Language Reference
:CMT.Topic: "Bitwise AND Operator"

.pa
.np
The following table illustrates some bitwise AND operations:

.boxdef
.boxcol 16
.boxcol  7
.boxbeg
$Operation      $Result
.boxline
.monoon
$0x0000 & 0x7A4C$0x0000
$0xFFFF & 0x7A4C$0x7A4C
$0x1001 & 0x0001$0x0001
$0x29F4 & 0xE372$0x2170
.monooff
.boxend

:CMT.This is from cl\express.gml and is in the C Language Reference
:CMT.Topic: "Bitwise Exclusive OR Operator"

.np
The following table illustrates some exclusive OR operations:

.boxdef
.boxcol 16
.boxcol  7
.boxbeg
$Operation       $Result
.boxline
.monoon
$0x0000 ^ 0x7A4C$0x7A4C
$0xFFFF ^ 0x7A4C$0x85B3
$0xFFFF ^ 0x85B3$0x7A4C
$0x1001 ^ 0x0001$0x1000
$0x29F4 ^ 0xE372$0xCA86
.monooff
.boxend

:CMT.This is from cl\express.gml and is in the C Language Reference
:CMT.Topic: "Bitwise Inclusive OR Operator"

.np
The following table illustrates some inclusive OR operations:

.boxdef
.boxcol 16
.boxcol  7
.boxbeg
$Operation      $Result
.boxline
.monoon
$0x0000 | 0x7A4C$0x7A4C
$0xFFFF | 0x7A4C$0xFFFF
$0x1100 | 0x0022$0x1122
$0x29F4 | 0xE372$0xEBF6
.monooff
.boxend

:CMT.This is from cl\float87.gml and is in the C Language Reference
:CMT.Topic: "Floating-Point Types"

.np
The floating-point format is the IEEE Standard for Binary Floating-Point
Arithmetic (ANSI/IEEE Std 754-1985).

.boxdef
.boxcol 13
.boxcol 9
.boxcol 9
.boxcol 8
.boxcol 8
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
.boxline
.monoon
$long double$2.2E-308$1.7E+308$15$:HP0.long real:eHP0.
.monooff
.boxend

:CMT.This is from cl\fltround.gml and is in the C Language Reference
:CMT.Topic: "Integer to Floating-Point Conversion"

.np
The following table describes the meaning of the various values:

.boxdef
.boxcol 12 c
.boxcol 17
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

:CMT.This is from cl\impldef1.gml and is in the C Language Reference
:CMT.Topic: "Characters"

:CMT.This prevents a pagination discrepancy from appearing.
:CMT.Remove the PA to see the problem.
:CMT.If that doesn't work, set WDWlvl to 1 or disable the invocations of macro
:CMT.KEEP in the BOXDEF and BOXEND macros, which will have the same effect, since
:CMT.KEEP basically does ".cp &WDWlvl." plus some bookkeeping.

.pa

.np
The following table shows escape sequences available in the source
character set, and what they translate to in the execution character set.

.boxdef
.boxcol 8
.boxcol 6
.boxcol 20
.boxbeg
$Escape  $Hex
$Sequence$Value$Meaning
.boxline
.monoon
$\a$07
.monooff
..ct $Bell or alert
.monoon
$\b$08
.monooff
..ct $Backspace
.monoon
$\f$0C
.monooff
..ct $Form feed
.monoon
$\n$0A
.monooff
..ct $New-line
.monoon
$\r$0D
.monooff
..ct $Carriage return
.monoon
$\t$09
.monooff
..ct $Horizontal tab
.monoon
$\v$0B
.monooff
..ct $Vertical tab
.monoon
$\'$27
.monooff
..ct $Apostrophe or single quote
.monoon
$\"$22
.monooff
..ct $Double quote
.monoon
$\?$3F
.monooff
..ct $Question mark
.monoon
$\\$5C
.monooff
..ct $Backslash
.monoon
$\:HP1.ddd:eHP1.$
.monooff
..ct $Octal value
.monoon
$\x:HP1.ddd:eHP1.$
.monooff
..ct $Hexadecimal value
.boxend

:CMT.This is from cl\impldef1.gml and is in the C Language Reference
:CMT.Topic: "Arrays and Pointers"

.np
The result of casting an integer to a pointer or vice versa (6.3.4).

.boxdef
.boxcol 10
.boxcol 20
.boxcol 20
.boxbeg
$Pointer$:MONO.short int:eMONO.
$Type   $:MONO.int                $long int:eMONO.
.boxline
$near$result is pointer value     $result is DS register in
$    $                            $high-order 2 bytes, pointer
$    $                            $value in low-order 2 bytes
.boxline
$far $segment is discarded, result$result is segment in high-
$huge$is pointer offset (low-order$order 2 bytes, offset in
$    $2 bytes of pointer)         $low-order 2 bytes
.boxend

:CMT.This is from cl\impldef1.gml and is in the C Language Reference
:CMT.Topic: "Arrays and Pointers"

.np
&wc286. conversion of integer to pointer:

.boxdef
.boxcol 10
.boxcol 20
.boxcol 20
.boxbeg
$Integer  $             $far pointer
$Type     $near pointer $huge pointer
.boxline
.monoon
$short int$:HP0.result is integer value     $result segment is DS:eHP0.
$int      $:HP0.                            $register, offset is:eHP0.
$         $:HP0.                            $integer value:eHP0.
.monooff
.boxline
.monoon
$long int $:HP0.result is low-order 2 bytes $result segment is high-:eHP0.
$         $:HP0.of integer value            $order 2 bytes, offset is:eHP0.
$         $:HP0.                            $low-order 2 bytes:eHP0.
.monooff
.boxend

:CMT.This is from cl\impldef1.gml and is in the C Language Reference
:CMT.Topic: "Arrays and Pointers"

.np
&wc386. conversion of pointer to integer:

.boxdef
.boxcol 10
.boxcol 20
.boxcol 20
.boxbeg
$Pointer$            $:MONO.int:eMONO.
$Type   $:MONO.short $long int:eMONO.
.boxline
$near$result is low-order 2 bytes $result is pointer value
$    $of pointer value            $
.boxline
$far $segment is discarded, result$segment is discarded, result
$huge$is low-order 2 bytes of     $is pointer offset
$    $pointer value               $
.boxend

:CMT.This is from cl\impldef1.gml and is in the C Language Reference
:CMT.Topic: "Arrays and Pointers"

.np
&wc386. conversion of integer to pointer:

.boxdef
.boxcol 10
.boxcol 20
.boxcol 20
.boxbeg
$Integer  $                             $far pointer
$Type     $near pointer                 $huge pointer
.boxline
.monoon
$short int$:HP0.result is integer value,  $result segment is DS:eHP0.
$         $:HP0.with zeroes for high-order$register, offset is integer:eHP0.
$         $:HP0.2 bytes                   $value, with zeroes for:eHP0.
$         $:HP0.                          $high-order 2 bytes:eHP0.
.monooff
.boxline
.monoon
$int      $:HP0.result is integer value   $result segment is DS:eHP0.
$long int $:HP0.                          $register, offset is:eHP0.
$         $:HP0.                          $integer value:eHP0.
.monooff
.boxend

:CMT.This is from cl\integer.gml and is in the C Language Reference
:CMT.Topic: "Integer Types"

.np
The following table describes all of the various integer types and
their ranges as implemented by the &wcboth. compilers.

.boxdef
.boxcol 18
.boxcol 21
.boxcol 20
.boxbeg
$                  $Minimum    $Maximum
$Type              $Value      $Value
.boxline
.smonoon
$signed char       $-128       $127
.smonooff
.boxline
.smonoon
$unsigned char     $0          $255
.smonooff
.boxline
.smonoon
$char              $0          $255
.smonooff
.boxline
.smonoon
$short int         $-32768     $32767
.smonooff
.boxline
.smonoon
$unsigned short int$0          $65535
.smonooff
.boxline
.smonoon
$int :HP0.(&c286.):eHP0.$-32768     $32767
.smonooff
.smonoon
$int :HP0.(&c386.):eHP0.$-2147483648$2147483647
.smonooff
.boxline
.smonoon
$unsigned int :HP0.(&c286.):eHP0.$0          $65535
.smonooff
.smonoon
$unsigned int :HP0.(&c386.):eHP0.$0          $4294967295
.smonooff
.boxline
.smonoon
$long int          $-2147483648$2147483647
.smonooff
.boxline
.smonoon
$unsigned long int $0          $18446744073709551615
.smonooff
.boxline
.smonoon
$long long int     $-92233720368547758078$9223372036854775807
.smonooff
.boxline
.smonoon
$unsigned long long$0     $18446744073709551615
.smonooff
.boxend

:CMT.This is from cl\macro.gml and is in the C Language Reference
:CMT.Topic: "Converting An Argument to a String"

.np
The following table gives a number of examples of the result of the
application of the macro,
.millust #define string( parm ) # parm
as shown in the first column:
.boxdef
.boxcol 23
.boxcol 19
.boxbeg
$Argument             $After Substitution
.boxline
.monoon
$string( abc )        $"abc"
$string( "abc" )      $"\"abc\""
$string( "abc" "def" )$"\"abc\" \"def\""
$string( \'/ )        $"\\'/"
$string( f(x) )       $"f(x)"
.monooff
.boxend

:CMT.This is from cl\macro.gml and is in the C Language Reference
:CMT.Topic: "Concatenating Tokens"

.np
The following table gives a number of examples of the result of the
application of the macro,
.millust #define glue( x, y ) x ## y
as shown in the first column.

.boxdef
.boxcol 21
.boxcol 17
.boxbeg
$Argument           $After Substitution
.boxline
.mono $glue( 12, 34 )     $1234
.boxline
.mono $glue( first, 1 )   $first1
.mono $                   $"Peas"
.boxline
.mono $glue( first, 2 )   $first2
.boxline
.mono $glue( first, last )$firstlast
.mono $                   $"Peace on Earth"
.boxend

:CMT.This is from cl\macro.gml and is in the C Language Reference
:CMT.Topic: "Simple Argument Substitution"

.np
Consider the following examples, with these macro definitions in place:
.millust begin
#define f(a)   a
#define g(x)   (1+x)
#define h(s,t) s t
#define i(y)   2-y
#define xyz    printf
#define rcrs   rcrs+2
.millust end
.boxdef
.boxcol 22
.boxcol 21
.boxbeg
$Invocation           $After Substitution
.boxline
.mono $f(c)                 $c
.boxline
.mono $f(f(c))              $f(c)
.mono $                     $c
.boxline
.mono $f(g(c))              $f((1+c))
.mono $                     $(1+c)
.boxline
.mono $h("hello",f("there"))$h("hello","there")
.mono $                     $"hello" "there"
.boxline
.mono $f(xyz)("Hello\n")    $f(printf)("Hello\n")
.mono $                     $printf("Hello\n")
.boxend

:CMT.This is from cl\macro.gml and is in the C Language Reference
:CMT.Topic: "Variable Argument Macros"

.np
Several example usages of the above macros follow:

.boxdef
.boxcol 23
.boxcol 22
.boxbeg
$Invocation           $After Substitution
.boxline
.mono $shuffle(x,y,z)           $y,z,x
.boxline
.mono $shuffle(x,y)             $y,x
.boxline
.mono $shuffle(a,b,c,d,e)       $b,c,d,e,a
.boxline
.mono $showlist(x,y,z)          $"x,y,z"
.boxline
.mono $args("%d+%d=%d",a,b,c)   $a,b,c
.boxline
.mono $args("none")             $
.boxend

:CMT.This is from cl\macro.gml and is in the C Language Reference
:CMT.Topic: "Rescanning for Further Replacement"

.np
Consider these examples, using the above macro definitions:

.boxdef
.boxcol 13
.boxcol 13
.boxbeg
$Invocation $After Rescanning
.boxline
.mono $f(g)(r)    $g(r)
.mono $           $(1+r)
.boxline
.mono $f(f)(r)    $f(r)
.boxline
.mono $h(f,(b))   $f (b)
.mono $           $b
.boxline
.mono $i(h(i,(b)))$i(i (b))
.mono $           $2-i (b)
.boxline
.mono $i(i (b))   $i(2-b)
.mono $           $2-2-b
.boxline
.mono $rcrs       $rcrs+2
.boxend

:CMT.This is from cl\numlimit.gml and is in the C Language Reference
:CMT.Topic: "Macros for Numerical Limits"
:CMT.Plus all subsections, of which there are quite a few.

.sr numboxcol1=19
.sr numboxcol2=27
.np
Numerical Limits for Integer Types
:cmt..section Numerical Limits for Integer Types
.*
.begbull
.keep begin
.bull
the number of bits in the smallest object that is not a bit-field
(byte)
.numboxbeg CHAR_BIT
.ansi >= 8
.bothc 8
.wlooc 8
.numboxend
.keep break
.bull
the minimum value for an object of type
.kw signed char
.numboxbeg SCHAR_MIN
.ansi  <= -127
.bothc -128
.wlooc -128
.numboxend
.keep break
.bull
the maximum value for an object of type
.kw signed char
.numboxbeg SCHAR_MAX
.ansi  >= 127
.bothc 127
.wlooc 127
.numboxend
.keep break
.bull
the maximum value for an object of type
.kw unsigned char
.numboxbeg UCHAR_MAX
.ansi  >= 255
.bothc 255
.wlooc 255
.numboxend
.keep break
.bull
the minimum value for an object of type
.kw char
.pp
If
.kw char
is
.kw unsigned
(the default case)
.numboxbeg CHAR_MIN
.ansi  0
.bothc 0
.wlooc 0
.numboxend
.keep break
.pc
If
.kw char
is
.kw signed
(by using the command-line switch to force it to be signed),
then
.mono CHAR_MIN
is equivalent to
.mono SCHAR_MIN
.numboxbeg CHAR_MIN
.ansi  <= -127
.bothc -128
.wlooc -128
.numboxend
.keep break
.bull
the maximum value for an object of type
.kw char
.pp
If
.kw char
is
.kw unsigned
(the default case),
then
.mono CHAR_MAX
is equivalent to
.mono UCHAR_MAX
.numboxbeg CHAR_MAX
.ansi  >= 255
.bothc 255
.wlooc 255
.numboxend
.keep break
.pc
If
.kw char
is
.kw signed
(by using the command-line switch to force it to be signed),
then
.mono CHAR_MAX
is equivalent to
.mono SCHAR_MAX
.numboxbeg CHAR_MAX
.ansi  >= 127
.bothc 127
.wlooc 127
.numboxend
.keep break
.bull
the maximum number of bytes in a multibyte character, for any supported locale
.numboxbeg MB_LEN_MAX
.ansi  >= 1
.bothc 2
.wlooc 1
.numboxend
.keep break
.bull
the minimum value for an object of type
.kw short int
.numboxbeg SHRT_MIN
.ansi  <= -32767
.bothc -32768
.wlooc -32768
.numboxend
.keep break
.bull
the maximum value for an object of type
.kw short int
.numboxbeg SHRT_MAX
.ansi  >= 32767
.bothc 32767
.wlooc 32767
.numboxend
.keep break
.bull
the maximum value for an object of type
.kw unsigned short int
.numboxbeg USHRT_MAX
.ansi  >= 65535
.bothc 65535
.wlooc 65535
.numboxend
.keep break
.bull
the minimum value for an object of type
.kw int
.numboxbeg INT_MIN
.ansi    <= -32767
.watcomc -32768
.c386    -2147483648
.wlooc   -2147483648
.numboxend
.keep break
.bull
the maximum value for an object of type
.kw int
.numboxbeg INT_MAX
.ansi    >= 32767
.watcomc 32767
.c386    2147483647
.wlooc   2147483647
.numboxend
.keep break
.bull
the maximum value for an object of type
.kw unsigned int
.numboxbeg UINT_MAX
.ansi    >= 65535
.watcomc 65535
.c386    4294967295
.wlooc   4294967295
.numboxend
.keep break
.bull
the minimum value for an object of type
.kw long int
.numboxbeg LONG_MIN
.ansi  <= -2147483647
.bothc -2147483648
.wlooc -2147483648
.numboxend
.keep break
.bull
the maximum value for an object of type
.kw long int
.numboxbeg LONG_MAX
.ansi  >= 2147483647
.bothc 2147483647
.wlooc 2147483647
.numboxend
.keep break
.bull
the maximum value for an object of type
.kw unsigned long int
.numboxbeg ULONG_MAX
.ansi  >= 4294967295
 -
.bothc 4294967295
.wlooc 4294967295
.numboxend
.keep break
.bull
the minimum value for an object of type
.kw long long int
.numboxbeg LLONG_MIN
.ansi  <= -9223372036854775807
.bothc -9223372036854775808
.wlooc -9223372036854775808
.numboxend
.keep break
.bull
the maximum value for an object of type
.kw long long int
.numboxbeg LLONG_MAX
.ansi  >= 9223372036854775807
.bothc 9223372036854775807
.wlooc 9223372036854775807
.numboxend
.keep break
.bull
the maximum value for an object of type
.kw unsigned long long int
.numboxbeg ULLONG_MAX
.ansi  >= 18446744073709551615
.bothc 18446744073709551615
.wlooc 18446744073709551615
.numboxend
.keep end
.endbull
.*
.np
Numerical Limits for Floating-Point Types
:cmt..section Numerical Limits for Floating-Point Types
.*
.sr numboxcol1=23
.sr numboxcol2=26
.np
For those characteristics that have three different macros, the
macros that start with
.mono FLT_&SYSRB.
refer to type
.kw float
..ct ,
.mono DBL_&SYSRB.
refer to type
.kw double
and
.mono LDBL_&SYSRB.
refer to type
.kw long double
..ct ..li .
.begbull
.keep begin
.bull
the radix (base) of representation for the exponent
.numboxbeg FLT_RADIX
.ansi  >= 2
.bothc 2
.wlooc 16
.numboxend
.keep break
.bull
the precision, or number of digits in the floating-point mantissa, expressed
in terms of the
.mono FLT_RADIX
.numboxbeg FLT_MANT_DIG
.noansi
.bothc 23
.wlooc 6
.numboxend
.keep break
.numboxbeg DBL_MANT_DIG
.noansi
.bothc 52
.wlooc 14
.numboxend
.keep break
.numboxbeg LDBL_MANT_DIG
.noansi
.bothc 52
.wlooc 14
.numboxend
.keep break
.bull
the number of decimal digits of precision
.numboxbeg FLT_DIG
.ansi  >= 6
.bothc 6
.wlooc 8
.numboxend
.keep break
.numboxbeg DBL_DIG
.ansi  >= 10
.bothc 15
.wlooc 17
.numboxend
.keep break
.numboxbeg LDBL_DIG
.ansi  >= 10
.bothc 15
.wlooc 17
.numboxend
.keep break
.bull
the minimum negative integer
.us n
such that
.mono FLT_RADIX
raised to the power
.us n
..ct ,
minus 1,
is a normalized floating-point number, or,
.begbull $compact
.bull
the minimum exponent value in terms of
.mono FLT_RADIX
..ct , or,
.bull
the base
.mono FLT_RADIX
exponent for the floating-point value that is closest, but
not equal, to zero
.endbull
.numboxbeg FLT_MIN_EXP
.noansi
.bothc -127
.wlooc -65
.numboxend
.keep break
.numboxbeg DBL_MIN_EXP
.noansi
.bothc -1023
.wlooc -65
.numboxend
.keep break
.numboxbeg LDBL_MIN_EXP
.noansi
.bothc -1023
.wlooc -65
.numboxend
.keep break
.bull
the minimum negative integer
.us n
such that
10
raised to the power
.us n
is in the range of normalized floating-point numbers, or,
.begbull $compact
.bull
the base 10 exponent for the floating-point value that is
closest, but not equal, to zero
.endbull
.numboxbeg FLT_MIN_10_EXP
.ansi  <= -37
.bothc -38
.wlooc -78
.numboxend
.keep break
.numboxbeg DBL_MIN_10_EXP
.ansi  <= -37
.bothc -307
.wlooc -78
.numboxend
.keep break
.numboxbeg LDBL_MIN_10_EXP
.ansi  <= -37
.bothc -307
.wlooc -78
.numboxend
.keep break
.bull
the maximum integer
.us n
such that
.mono FLT_RADIX
raised to the power
.us n
..ct ,
minus 1,
is a representable finite floating-point number, or,
.begbull $compact
.bull
the maximum exponent value in terms of
.mono FLT_RADIX
..ct , or,
.bull
the base
.mono FLT_RADIX
exponent for the largest valid floating-point value
.endbull
.numboxbeg FLT_MAX_EXP
.noansi
.bothc 127
.wlooc 62                      62
.numboxend
.keep break
.numboxbeg DBL_MAX_EXP
.noansi
 --
.bothc 1023
.wlooc 62
.numboxend
.keep break
.numboxbeg LDBL_MAX_EXP
.noansi
.bothc 1023
.wlooc 62
.numboxend
.keep break
.bull
the maximum integer
.us n
such that
10
raised to the power
.us n
is a representable finite floating-point number, or,
.begbull $compact
.bull
the base 10 exponent for the largest valid floating-point value
.endbull
.numboxbeg FLT_MAX_10_EXP
.ansi  >= 37
.bothc 38
.wlooc 75
.numboxend
.keep break
.numboxbeg DBL_MAX_10_EXP
.ansi  >= 37
.bothc 308
.wlooc 75
.numboxend
.keep break
.numboxbeg LDBL_MAX_10_EXP
.ansi  >= 37
.bothc 308
.wlooc 75
.numboxend
.keep break
.bull
the maximum representable finite floating-point number
.numboxbeg FLT_MAX
.ansi  >= 1E+37
.bothc 3.402823466E+38
.wlooc 7.2370051E+75
.numboxend
.keep break
.numboxbeg DBL_MAX
.ansi  >= 1E+37
.bothc 1.79769313486231560E+308
.wlooc 7.2370055773322608E+75
.numboxend
.keep break
.numboxbeg LDBL_MAX
.ansi  >= 1E+37
.bothc  1.79769313486231560E+308
.wlooc 7.2370055773322608E+75
.numboxend
.keep break
.bull
the difference between
.mono 1.0
and the least value greater than
.mono 1.0
that is representable in the given floating-point type, or,
.begbull $compact
.bull
the smallest number
.mono eps
such that
.mono (1.0 + eps) != 1.0
.endbull
.numboxbeg FLT_EPSILON
.ansi  <= 1E-5
.bothc 1.192092896E-15
.wlooc 9.5367432E-7
.numboxend
.keep break
.numboxbeg DBL_EPSILON
.ansi  <= 1E-9
.bothc 2.2204460492503131E-16
.wlooc 2.2204460492503129E-16
.numboxend
.keep break
.numboxbeg LDBL_EPSILON
.ansi  <= 1E-9
.bothc 2.2204460492503131E-16
.wlooc 2.2204460492503129E-16
.numboxend
.keep break
.bull
the minimum positive normalized floating-point number
.numboxbeg FLT_MIN
.ansi  <= 1E-37
.bothc 1.175494351E-38
.wlooc 5.3976053E-79
.numboxend
.keep break
.numboxbeg DBL_MIN
.ansi  <= 1E-37
.bothc 2.22507385850720160E-308
.wlooc 5.3976053469340275E-79
.numboxend
.keep break
.numboxbeg LDBL_MIN
.ansi  <= 1E-37
.bothc 2.22507385850720160E-308
.wlooc 5.3976053469340275E-79
.numboxend
.keep end
.endbull

:CMT.This is from cl\trigraph.gml and is in the C Language Reference
:CMT.Topic: "Character Constants"

.np
The following trigraph sequences may be used to represent characters not available on all terminals or
systems:

.boxdef
.boxcol 8 c
.boxcol 15 c
.boxbeg
$Character$Trigraph Sequence
.boxline
.monoon
$[  $??(
$]  $??)
${  $??<
$}  $??>
$|  $??!
$#  $??=
$\  $??/
$^  $??'
$~~ $??-
.monooff
.boxend

:CMT.This is from fl\ftypes.gml and is in the FORTRAN 77 Language Reference
:CMT.Topic: "Data Types"

:p.The following table summarizes all data types supported by &product..

.sr c0=&INDlvl+1
.sr c1=&INDlvl+18
.sr c2=&INDlvl+26
.sr c3=&INDlvl+36
.box on &c0 &c1 &c2 &c3
\ Data Type   \ Size      \ Standard\
\             \(in bytes) \ FORTRAN\
.box
\ LOGICAL            \ 4  \ yes
\ LOGICAL*1          \ 1  \ extension
\ LOGICAL*4          \ 4  \ extension
\ INTEGER            \ 4  \ yes
\ INTEGER*1          \ 1  \ extension
\ INTEGER*2          \ 2  \ extension
\ INTEGER*4          \ 4  \ extension
\ REAL               \ 4  \ yes
\ REAL*4             \ 4  \ extension
\ REAL*8             \ 8  \ extension
\ DOUBLE PRECISION   \ 8  \ yes
\ COMPLEX            \ 8  \ yes
\ COMPLEX*8          \ 8  \ extension
\ DOUBLE COMPLEX     \ 16 \ extension
\ COMPLEX*16         \ 16 \ extension
\ CHARACTER          \ 1  \ yes
\ CHARACTER*n        \ n  \ yes
.box off

:CMT.This is from fl\stmtclas.gml and is in the FORTRAN 77 Language Reference
:CMT.Topic: "Classifying Statements"

.np
The following table is a summary of &product statement classification.

.sr c0=&INDlvl+1
.sr c1=&INDlvl+20
.sr c2=&INDlvl+25
.sr c3=&INDlvl+30
.sr c4=&INDlvl+35
.sr c5=&INDlvl+40
.sr c6=&INDlvl+45
.sr c7=&INDlvl+50
.box on &c0 &c1 &c2 &c3 &c4 &c5 &c6 &c7
\Statement            \  1   \  2   \  3   \  4   \  5   \  6
.box
\ADMIT                \      \  *   \      \  *   \  *   \      \
\ALLOCATE             \      \      \      \      \      \      \
\ASSIGN               \      \      \      \      \      \      \
\AT END               \      \  *   \      \  *   \  *   \      \
\BACKSPACE            \      \      \      \      \      \      \
\BLOCK DATA           \      \  *   \  *   \  *   \  *   \      \
\CALL                 \      \      \      \      \      \      \
\CASE                 \      \  *   \      \  *   \  *   \      \
\CHARACTER            \  *   \  *   \  *   \  *   \  *   \  *   \
\CLOSE                \      \      \      \      \      \      \
\COMMON               \  *   \  *   \  *   \  *   \  *   \  *   \
.box off
:cmt..pa
.box on &c0 &c1 &c2 &c3 &c4 &c5 &c6 &c7
\Statement            \  1   \  2   \  3   \  4   \  5   \  6
.box
\COMPLEX              \  *   \  *   \  *   \  *   \  *   \  *   \
\CONTINUE             \      \      \      \      \      \      \
\CYCLE                \      \      \      \      \      \      \
\DATA                 \      \  *   \  *   \  *   \  *   \  *   \
\DEALLOCATE           \      \      \      \      \      \      \
\DIMENSION            \  *   \  *   \  *   \  *   \  *   \  *   \
\DO                   \      \  *   \      \  *   \      \      \
\DOUBLE COMPLEX       \  *   \  *   \  *   \  *   \  *   \  *   \
\DOUBLE PRECISION     \  *   \  *   \  *   \  *   \  *   \  *   \
\DO WHILE             \      \  *   \      \  *   \      \      \
\ELSE                 \      \  *   \      \  *   \  *   \      \
\ELSE IF              \      \  *   \      \  *   \  *   \      \
\END                  \      \  *   \      \  *   \      \  *   \
\END AT END           \      \  *   \      \  *   \  *   \      \
\END BLOCK            \      \  *   \      \  *   \  *   \      \
\END DO               \      \      \      \  *   \  *   \      \
\ENDFILE              \      \      \      \      \      \      \
\END GUESS            \      \  *   \      \  *   \      \      \
\END IF               \      \  *   \      \  *   \      \      \
\END LOOP             \      \  *   \      \  *   \  *   \      \
\END MAP              \  *   \  *   \  *   \  *   \  *   \  *   \
\END SELECT           \  *   \  *   \  *   \  *   \  *   \      \
\END STRUCTURE        \  *   \  *   \  *   \  *   \  *   \  *   \
\END UNION            \      \  *   \      \  *   \  *   \  *   \
\END WHILE            \      \  *   \      \  *   \  *   \      \
\ENTRY                \      \  *   \      \  *   \  *   \      \
\EQUIVALENCE          \  *   \  *   \  *   \  *   \  *   \  *   \
\EXECUTE              \      \      \      \      \      \      \
\EXIT                 \      \      \      \      \      \      \
\EXTERNAL             \  *   \  *   \  *   \  *   \  *   \      \
\FORMAT               \      \  *   \  *   \  *   \  *   \      \
\FUNCTION             \      \  *   \  *   \  *   \  *   \      \
\assigned GO TO       \      \  *   \      \      \      \      \
\computed GO TO       \      \      \      \      \      \      \
\unconditional GO TO  \      \  *   \      \      \      \      \
\GUESS                \      \  *   \      \  *   \      \      \
\arithmetic IF        \      \  *   \      \      \      \      \
\logical IF           \      \      \      \  *   \      \      \
\block IF             \      \  *   \      \  *   \      \      \
.box off
:cmt..pa
.box on &c0 &c1 &c2 &c3 &c4 &c5 &c6 &c7
\Statement            \  1   \  2   \  3   \  4   \  5   \  6
.box
\IMPLICIT             \  *   \  *   \  *   \  *   \  *   \  *   \
\INCLUDE              \      \  *   \  *   \  *   \  *   \  *   \
\INQUIRE              \      \      \      \      \      \      \
\INTEGER              \  *   \  *   \  *   \  *   \  *   \  *   \
\INTRINSIC            \  *   \  *   \  *   \  *   \  *   \      \
\LOGICAL              \  *   \  *   \  *   \  *   \  *   \  *   \
\LOOP                 \      \  *   \      \  *   \      \      \
\MAP                  \  *   \  *   \  *   \  *   \  *   \  *   \
\NAMELIST             \  *   \  *   \  *   \  *   \  *   \      \
\OPEN                 \      \      \      \      \      \      \
\OTHERWISE            \      \  *   \      \  *   \  *   \      \
\PARAMETER            \  *   \  *   \  *   \  *   \  *   \  *   \
\PAUSE                \      \      \      \      \      \      \
\PRINT                \      \      \      \      \      \      \
\PROGRAM              \      \  *   \  *   \  *   \  *   \      \
\QUIT                 \      \      \      \      \      \      \
\READ                 \      \      \      \      \      \      \
\REAL                 \  *   \  *   \  *   \  *   \  *   \  *   \
\RECORD               \  *   \  *   \  *   \  *   \  *   \  *   \
\REMOTE BLOCK         \      \  *   \  *   \  *   \  *   \      \
\RETURN               \      \  *   \      \      \      \      \
\REWIND               \      \      \      \      \      \      \
\SAVE                 \  *   \  *   \  *   \  *   \  *   \  *   \
\SELECT               \      \  *   \      \  *   \      \      \
\STOP                 \      \  *   \      \      \      \      \
\STRUCTURE            \  *   \  *   \  *   \  *   \  *   \  *   \
\SUBROUTINE           \      \  *   \  *   \  *   \  *   \      \
\UNION                \  *   \  *   \  *   \  *   \  *   \  *   \
\UNTIL                \      \  *   \      \  *   \  *   \      \
\VOLATILE             \  *   \  *   \  *   \  *   \  *   \  *   \
\WHILE                \      \  *   \      \  *   \      \      \
\WRITE                \      \      \      \      \      \      \
.box off

:eGDOC.


