.section *refid='i1' i1
.ix 'i1'
.ix 'Indexing' 'primary entry'
.ix 'Block' 'i1'
.tag begin
.tdesc
Add a primary entry to the index. The text for the entry must be on the same line as the tag and cannot contain
other tags.
.tattrbs
.tattr id=text
A cross-reference for use by a secondary index (i2) tag.
.tattr global
For HLP files, allows this entry to also appear in the global index.
.tattr roots='text'
Specifies a space separated list of root words (as defined by isyn) that act as index entries to specific topics. 
.tattr sortkey='key-text'.index-text
Specify a string used to sort this entry in the index, and a string to use in its place. In other words, place 
index-text where key-text would sort in index.
.tno Conditions
Cannot appear in a footnote (fn/efn block).
.tclass Block
.tcont Text
.tseealso
:HDREF refid='i2'., :HDREF refid='icmd'., :HDREF refid='isyn'.
.tag end

