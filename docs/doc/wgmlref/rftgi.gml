:H2.I1, I2, I3
:XMP.
Format&gml. &gml.I:HP1.n:eHP1. \(id='id-name'\)
            \(pg=start
                end
                major
                'character string'\)
            \(refid='id-name'\).<text line>
        (:HP1.n:eHP1.=1,2,3)
:eXMP.
:I1.indexing
:I2 refid='gti1' pg=major.gti1
:I2 refid='gti2' pg=major.gti2
:I2 refid='gti3' pg=major.gti3
:P.
These tags will cause an index entry to be created.
Index entry tags may be used at any point in the document after the
:HP2.&gml.gdoc:eHP2. tag.
The text line with the index entry tag is used to
create an index term for the index entry.
The index command line option must be specified for the
index entry tags to be processed.
The :hp2.&gml.I1:ehp2. tag is used to create a primary index entry.
The :hp2.&gml.I2:ehp2. tag is used to create an index subentry
for the previous primary index entry.
The :hp2.&gml.I3:ehp2. creates an index subentry
for the previously specified :hp2.&gml.I2:ehp2. tag.
:INCLUDE file='rftgii'.
:INCLUDE file='rftgipg'.
:p.
The :hp2.refid:ehp2.
:I2 refid='garefid'.garefid
attribute will cause the index entry
to be associated with a specific higher level index entry rather
than the index entry which directly precedes it in the document.
The refid attribute may be used
with the :hp2.&gml.I2:ehp2. and the :hp2.&gml.I3:ehp2. tags.
