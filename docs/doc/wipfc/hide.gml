.section *refid='hide' hide
.ix 'hide'
.ix 'Formatting' 'hide'
.tag begin
.tdesc
Hide a section of information unless the IPF_KEYS environment variable contains the correct key.
.tclose ehide
.tattrbs
.tattr key='text'
The key that will unhide the enclosed information. Multiple keys can be specified by enclosing each key in 
single quotes then concatenating them with the plus sign. So, 'key1'+'key2'+... &ipfcmd will also accept 
'key1+key2+...'
.tno Conditions:
You cannot nest hide tags within each other. You cannot include a header tag with the 'res' attribute set (in 
other words, you cannot jump to hidden material). Note that the &ipfname does not check for this error.
.tclass Formatting
.tcont Text, Formatting, In-line, Block, List-block
.tag end

