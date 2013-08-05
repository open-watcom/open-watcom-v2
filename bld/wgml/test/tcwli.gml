.* test for .li controlword and bug chase for ".ct .li ." line
:body.
.ty         default settings: concat &$co, justify &$ju
.ju off
.ty                 settings: concat &$co, justify &$ju
In the following line is "..ct ..li ."
..ct ..li .
.* co off for one line in = one line out
.co off
.se var=outside li
.li .se var=inside single 1 li
.li .se var=inside single 2 li :cmt comment
.li .se var=inside single 3 li :p.new paragraph
.li 3
.se var=inside li 1 :cmt these should be text
.se var=inside li 2
.se var=inside li 3
.li .ty &var.
&var. not within li
.li on
.li xxxx in li
&var line1 in .li  :cmt xxxyyy
&var line2 in .li
.li off
.li off:cmt duplicate .li off accepted without err msg
.li on
.li on;.* duplicate .li on is treated as text
.li off
.li off
.se var=varvalueoff
.li
.ty                        &var
.ty vars at end
.im ts.inc
:cmt. egdoc.
