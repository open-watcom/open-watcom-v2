.ix 'precedence'
.ix 'order of operation'
.ix 'expression' 'precedence'
.ix 'expression' 'priority'
.ix 'operator' 'precedence'
.ix 'operator' 'priority'
..sk 1 c
.if &e'&nobox eq 0 .do begin
..fk begin
..sr *start=&SYSIN.+1
..bx on  &*start. +16 +24
..sr *start=&*start.+1
..tb &*start. +16 +6 +6 +6 +6 +6
..tb set $
:SF font=0.
$Expression Type$Operators
:eSF.
..bx
:SF font=0.
$primary       $identifier  $$constant
$              $string      $$(expression)
:eSF.
..bx
:SF font=5.
$:SF font=0.postfix:eSF.$a[b] $f()
:eSF.:SF font=5.
$$a.b   $a->b$a++   $a--
:eSF.
..bx
:SF font=5.
$:SF font=0.unary:eSF.$sizeof u   $sizeof( a )
:eSF.:SF font=5.
$$++a  $--a  $&a    $*a
$$+a   $-a   $~~a   $!a
:eSF.
..bx
:SF font=5.
$:SF font=0.cast:eSF.$(type) a
:eSF.
..bx
:SF font=5.
$:SF font=0.multiplicative:eSF.$a * b   $a / b   $a % b
:eSF.
..bx
:SF font=5.
$:SF font=0.additive:eSF.$a + b   $a - b
:eSF.
..bx
:SF font=5.
$:SF font=0.shift:eSF.$a << b  $a >> b
:eSF.
..bx
:SF font=5.
$:SF font=0.relational:eSF.$a < b   $a > b   $a <= b   $a >= b
:eSF.
..bx
:SF font=5.
$:SF font=0.equality:eSF.$a == b  $a != b
:eSF.
..bx
:SF font=5.
$:SF font=0.bitwise AND:eSF.$a & b
:eSF.
..bx
:SF font=5.
$:SF font=0.bitwise exclusive OR:eSF.$a ^ b
:eSF.
..bx
:SF font=5.
$:SF font=0.bitwise inclusive OR:eSF.$a | b
:eSF.
..bx
:SF font=5.
$:SF font=0.logical AND:eSF.$a && b
:eSF.
..bx
:SF font=5.
$:SF font=0.logical OR:eSF.$a || b
:eSF.
..bx
:SF font=5.
$:SF font=0.conditional &dagger.:eSF.$a ? b : c
:eSF.
..bx
:SF font=5.
$:SF font=0.assignment &dagger.:eSF.$a = b $a += b $a -= b $a *= b
:eSF.:SF font=5.
$$a /= b  $a %= b  $a &= b  $a ^= b
$$a |= b  $a <<= b $a >>= b
:eSF.
..bx
:SF font=5.
$:SF font=0.comma:eSF.$a,b
:eSF.
..bx off
:SF font=0.
$&dagger. associates from right to left
..tb set
..tb
:eSF.
..fk end
.do end
.el .do begin
.millust begin
Expression Type         Operators
----------------------  ------------------------------
primary                 identifier
                        constant
                        string
                        (expression)
----------------------  ------------------------------
postfix                 a[b]
                        f()
                        a.b     a->b
                        a++     a--
----------------------  ------------------------------
unary                   sizeof u   sizeof( a )
                        ++a     --a
                        &a      *a
                        +a      -a      ~~a      !a
----------------------  ------------------------------
cast                    (type) a
----------------------  ------------------------------
multiplicative          a * b   a / b   a % b
----------------------  ------------------------------
additive                a + b   a - b
----------------------  ------------------------------
shift                   a << b  a >> b
----------------------  ------------------------------
relational              a < b   a > b   a <= b  a >= b
----------------------  ------------------------------
equality                a == b  a != b
----------------------  ------------------------------
bitwise AND             a & b
----------------------  ------------------------------
bitwise exclusive OR    a ^ b
----------------------  ------------------------------
bitwise inclusive OR    a | b
----------------------  ------------------------------
logical AND             a && b
----------------------  ------------------------------
logical OR              a || b
----------------------  ------------------------------
conditional (*)         a ? b : c
----------------------  ------------------------------
assignment (*)          a = b
                        a += b     a -= b
                        a *= b     a /= b     a %= b
                        a &= b     a ^= b     a |= b
                        a <<= b    a >>= b
----------------------  ------------------------------
comma                   a,b
----------------------  ------------------------------
.millust end
.pp
(*) associates from right to left
.do end
..br
