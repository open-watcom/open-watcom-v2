.section *refid='i2' i2
.ix 'i2'
.ix 'Indexing' 'secondary entry'
.ix 'Block' 'i2'
.tag begin
.tdesc
Add a secondary entry to the index. The text for the entry must be on the same line as the tag and cannot contain
other tags.
.tattrbs
.tattr refid=text
The id of the primary index entry to which this belongs.
.tattr global
For HLP files, allows this entry to also appear in the global index.
.tattr sortkey='key-text'.index-text
Specify a string used to sort this entry in the index, and a string to use in its place. In other words, place 
index-text where key-text would sort in index.
.tno Conditions
Cannot appear in a fn/efn footnote. If the global flag is set for the i1 tag, it must be set for the i2 tag.
.tclass Block
.tcont Text
.tseealso
:HDREF refid='i1'.
.tag end

