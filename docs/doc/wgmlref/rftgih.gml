:H2.IH1, IH2, IH3
:XMP.
Format&gml. &gml.IH:HP1.n:eHP1. \(id='id-name'\)
             \(ix=:HP1.x:eHP1.\)
             \(print='character string'\)
             \(see='character string'\)
             \(seeid='id-name'\).<text line>
        (:HP1.n:eHP1.=1,2,3)
        (:HP1.x:eHP1.=0 -> 8)
:eXMP.
:I1.index headings
:I2 refid='gtih1' pg=major.gtih1
:I2 refid='gtih2' pg=major.gtih2
:I2 refid='gtih3' pg=major.gtih3
:p.The index heading tags will cause an index entry to be created.
Index headings may be used at any point in the document.
The text line with the index entry tag is used to
create an index term for the index entry.
The index heading tag does not generate a page number reference
with the index term in the index.
The index command line option must be specified for the
index entry tags to be processed.
The :hp2.&gml.IH1:ehp2. tag is used to create a primary index entry.
The :hp2.&gml.IH2:ehp2. tag is used to create an index subentry
for the previous primary index entry.
The :hp2.&gml.IH3:ehp2. creates an index subentry
for the previously specified :hp2.&gml.IH2:ehp2. tag.
:INCLUDE file='rftgii'.
:p.
The :hp2.ix:ehp2.
:I2 refid='gaix'.gaix
attribute selects one of the index groups (from zero through eight),
with zero being the default.
:p.
The :hp2.print:ehp2.
:I2 refid='gaprint'.gaprint
attribute causes the specified character
string to be displayed in the index instead of the index term.
The index term is still used to determine where in the index the entry
should be placed.
:INCLUDE file='rftgisee'.
:INCLUDE file='rftgisid'.
