:cmt. test for usertags and related control words
:GDOC.
:BODY.
.dm br /.ty br/
.*
:set symbol='trfrom1'   value=" -+*/\%<>[](){}=#~@$,.&|':?!".
 .*
.gt see add @set attr texte
.ga * symbol req len 10
.ga * value req any

.gt see print
.gt see cha bset
.gt see print

.dm @set begin
.if ›&*value› = ›delete› .do
.   .se &*symbol off
.do end
.el .do
.   .se &*symbol=›&*value›
.do end
.ty --->&&*symbol..<---
.dm @set end

.dm bset begin
.if ›&*value› = ›delete› .se &*symbol off
.el .se &*symbol=›&*value›
.ty +++>&&*symbol..<+++
.dm bset end

:see symbol='trfrom1'   value=" -+*/\%<>[](){}=#~@$,.&|':?!".
:see symbol="seesym"
     value="acd".
.br
&seesym.
.br
:see symbol="seesym" value=delete.
&seesym.
