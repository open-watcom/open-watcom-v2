.section *refid='dl' dl
.ix 'dl'
.ix 'Definition list'
.ix 'Lists' 'definition'
.ix 'List-block' 'dl'
.tag begin
.tdesc
Begin a definition list (a sequence of terms and their definitions).
.tclose edl
.tattrbs
.tattr compact
Omit inserting a blank line between each term-description pair.
.tattr tsize=number
Sets the width of the "term " column (the default is 10).
.tattr break=none | fit | all
Controls how a description follows a term. If break is set to "none", descriptions are on the same line as
the term, even if the length of the term exceeds "tsize" characters. If break is set to "fit", descriptions 
are placed on the next line only if the length of the term exceeds "tsize" characters. If break is set to "all" 
descriptions are always placed on the next line.
.tno Conditions:
The dthd and ddhd tags must occur before any dt and dd tags. Multiple consecutive dt tags are allowed (synonyms), 
but only one dd tag is allowed for each set of dt tags.
.tclass List-block
.tcont :HDREF refid='dthd'., :HDREF refid='ddhd'., :HDREF refid='dt'., :HDREF refid='dd'., Block, List-block
.tseealso
:HDREF refid='ol'., :HDREF refid='parml'., :HDREF refid='ul'., :HDREF refid='sl'.
.tag end
.*
.beglevel
.*
.section *refid='dthd' dthd
.ix 'dthd'
.ix 'Lists' 'term header'
.ix 'Definition list' 'term header'
.ix 'Block' 'dthd'
.tag begin
.tdesc
The term header for a definition list. It acts as a column title for all term elements.
.tno Conditions:
Must appear before the first ddhd, dt, or dd tag.
.tclass Block
.tcont Text, Formatting, In-line
.tseealso
:HDREF refid='ddhd'.
.tag end
.*
.section *refid='ddhd' ddhd
.ix 'ddhd'
.ix 'Lists' 'definition header'
.ix 'Definition list' 'definition header'
.ix 'Block' 'ddhd'
.tag begin
.tdesc
The definition header for a definition list. It acts as a column title for all definition elements.
.tno Conditions:
Must appear after a dthd tag and before the first dt or dd tag.
.tclass Block
.tcont Text, Formatting, In-line
.tseealso
:HDREF refid='dthd'.
.tag end
.*
.section *refid='dt' dt
.ix 'dt'
.ix 'Lists' 'term'
.ix 'Definition list' 'term'
.ix 'Block' 'dt'
.tag begin
.tdesc
A term to be defined.
.tclass Block
.tcont Text, Formatting, In-line
.tseealso
:HDREF refid='dd'.
.tag end
.*
.section *refid='dd' dd
.ix 'dd'
.ix 'Lists' 'definition'
.ix 'Definition list' 'definition'
.ix 'Block' 'dd'
.tag begin
.tdesc
A definition of a term.
.tclass Block
.tcont Text, Formatting, In-line
.tseealso
:HDREF refid='dt'.
.tag end
.*
.endlevel

