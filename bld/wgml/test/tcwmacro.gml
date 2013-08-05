.* test for macro call with parms &amp.*0 was wrong
:gdoc
:body
.*
.dm mac begin
.ty parmcount = &*0
.ty mac called with = &*
.dm
.*
.ty .mac a b c *var1="1 " d e
.mac a b c *var1="1 " d e
.*
.ty .mac *var1="1 " a b c d e
.mac *var1="1 " a b c d e
.*
.gt gmac add mac
.ga * vall any
:gmac.'*var1="1 " a b c d e'
:gmac.'a b c *var1="1 " d e'
:egdoc
