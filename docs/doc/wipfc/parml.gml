.section *refid='parml' parml
.ix 'parml'
.ix 'Parameter list'
.ix 'Lists' 'parameter'
.ix 'List-block' 'parml'
.tag begin
.tdesc
A two-column list of parameters and their definitions, similar to a definition list (dl tag). A pt tag defines 
the term, and a pd tag defines a definition of the term. Each pt tag must have a pd tag. Multiple consecutive 
pt tags are allowed (synonyms), but only one pd tag is allowed for each set of pt tags. Other lists and blocks 
may be nested in a parameter table.
.tattrbs
.tattr tsize=number
The width of the term column. The default is 10.
.tattr break=all | fit | none
If break is 'all' then each description is on a line below the term. This is the default. If break is 'fit' 
then the description is on the same line as the term provided that the width of the term is less than tsize 
characters. If break is 'none' then the description is on the same line as the term.
.tattr compact
Do not add blank lines between each term/description pair.
.tclass List-block
.tcont :HDREF refid='pt'., :HDREF refid='pd'.
.tseealso
:HDREF refid='dl'.
.tag end
.*
.beglevel
.*
.section *refid='pt' pt
.ix 'pt'
.ix 'Lists' 'parameter term'
.ix 'Parameter list' 'term'
.ix 'Block' 'pt'
.tag begin
.tdesc
The parameter to be described. Multiple pt tags (synonyms) may described by a single pd tag.
.tclass List-block
.tcont Text, Formatting, In-line, :HDREF refid='pd'., pt
.tag end
.*
.section *refid='pd' pd
.ix 'pd'
.ix 'Lists' 'parameter description'
.ix 'Parameter list' 'description'
.ix 'Block' 'pd'
.tag begin
.tdesc
The description of the parameter.
.tclass List-block
.tcont Text, Formatting, In-line
.tseealso
:HDREF refid='pt'.
.tag end
.*
.endlevel

