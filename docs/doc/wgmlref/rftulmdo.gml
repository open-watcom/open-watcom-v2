:H2.Modifying Document Elements
:P.
Many of the tags in the &WGML. layout relate directly to
the tags used in specifying the document.
The following example illustrates a modification to the way in
which the example tag is formatted:
:XMP.
&gml.LAYOUT
&gml.XMP
        pre_skip = 3
        post_skip = 2
&gml.eLAYOUT.
:eXMP.
:PC.
The attributes :HP1.pre_skip:eHP1. and :HP1.post_skip:eHP1.
define the amount of space to leave before (pre) and after (post)
the example tag.
The attribute values are in line units, which means the amount of space
will depend on the height of a text line within an example.
For more information on the possible vertical space values, see
:HDREF refid='specver'..
:P.
The skip attributes imply certain actions.
Skip values which are specified as line units are multiplied by the
current spacing value.
For example,
if the example is double spaced, the skip value will be doubled.
Skips are also merged.
If an example was to follow another example, the amount
of skip between them would be three.
