.* test for .ix controlword
.* comment the next 2 lines for wgml4.0
:set symbol='willi' value='emil'
:gdoc
:body
:H0 id="H01".Header 0 title
:i1 id="h01".Index1
.ix "i1" "i2" "i3" "extra parameter"
.pa
.* comment the next line for wgml4.0
.ix "i1" "i2" "i3";.cm extra parameters
.ix "a1" "a2"
.ix "b1" "b2"
.ix "a10" "a21"
.ix "a10" "a22"
.ix "a1" "a22"
.ix "ccc"
.ix "a10" "a22"
.ix "a10" "a22"
.ix "a10" "a22"
.pa
:Iref refid=h01.
.ix "a10" "a19"
.br
:HDref refid=H01.
.pa
.ix . dump
:egdoc
