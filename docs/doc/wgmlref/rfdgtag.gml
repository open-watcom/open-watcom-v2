:H2.General Device Tags
:I1.device tags
:H3.CMT
:xmp
Format&gml. &gml.CMT.
:exmp
:I1.comments
:I2 refid='etcmt'.dtcmt
:P.
The information following the comment tag on the input line
is treated as a comment.
Text data and device tags in the input line
following the comment tag are not processed.
The comment tag must be placed at the beginning of each input record
that is to be ignored.
This tag may appear at any point in the device definition source,
although it may not
be placed between device tag attributes.
:H3.INCLUDE
:XMP.
Format&gml. &gml.INCLUDE file='file name'.
:eXMP.
:IH1.include
:I2 refid='etinclu'.dtinclu
:P.
The value of the required attribute :hp2.file:ehp2.
:I2 refid='gafile'.gafile
is used as the name of the file to include.
The content of the included file is processed by WATCOM GENDEV as if
the data was in the original file.
This tag provides the means whereby a definition may be specified using
a collection of separate files.
More than one definition may be included into one file for processing
by WATCOM GENDEV.
:INCLUDE file='rfpcincl'.
