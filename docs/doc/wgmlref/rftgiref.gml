:H2.IREF
:xmp
Format&gml. &gml.IREF refid='id-name'
              \(pg=start
                  end
                  major
                  'character string'\)
              \(see='character string'\)
              \(seeid='id-name'\).
:exmp
:I1.index reference
:I2 refid='gtiref' pg=major.gtiref
:p.This tag will cause an index entry to be created.
The entry will be similar to the one referenced by the
:HP2.refid:eHP2. attribute, which must be specified.
Index references may be placed anywhere in the document.
The index command line option must be specified for the
index reference tag to be processed.
:p.
The :hp2.refid:ehp2.
:I2 refid='garefid'.garefid
attribute is used to reference an
index entry identified by the specified identifier name.
:INCLUDE file='rftgipg'.
:INCLUDE file='rftgisee'.
:INCLUDE file='rftgisid'.
