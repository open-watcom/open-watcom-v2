:H2.Banners
With &WGML., banners define the text content at the top and/or
bottom of the output page.
Banners are also called running titles and running footers.
Most of the default layout consists of banner definitions.
:P.
The most common use of a banner is to define the place on the output
page to display the current heading and page number.
The following illustrates what might appear at the bottom of a
document page.
:XMP.
WATCOM Script/GML      3
:eXMP.
:PC.
The current heading starts at the left hand side of the page, and
the current page number is set to the right hand side of the page.
The banner therefore consists of two distinct types of information.
Banners are subdivided into regions which contain different types
of information.
The regions for the above example would look as follows:
:XMP.
+------------------------+
| WATCOM Script/GML  | 3 |
+------------------------+
:eXMP.
:P.
The banner starts with the :HP2.&gml.banner:eHP2. tag, and defines the size
and placement of the banner.
Within the banner definition, each region is defined
with the :HP2.&gml.banregion:eHP2. tag.
Since each region contains only one type of information, the banner
region tag and its attributes must be specified for each area of the
banner.
:H3.Defining the Banner
:P.
The banner tag has a number of attributes which define the area
of the output page to place the banner.
The following shows how to define the banner area for our previous example.
:XMP.
&gml.BANNER
        left_adjust = 0
        right_adjust = 0
        depth = 3
        docsect = body
        place = botodd
:eXMP.
:P.
The :HP1.left_adjust:eHP1. and :HP1.right_adjust:eHP1.
attributes allow you to change the banner
margins relative to the page margins.
The value of the left adjust attribute is added to the left margin
of the page.
The value of the right adjust attribute is subtracted from the page
right margin.
:P.
The :HP1.depth:eHP1. attribute specifies the depth of the entire banner.
The banner regions can be placed through more than one output line, or placed
above one another.
:P.
The :HP1.docsect:eHP1. attribute determines the document section
in which the banner will be used.
In this example, the value BODY means that the banner will appear in the
body section of the document.
If the value HEAD0 was specified, the banner would only be
used when a heading
of level zero appears on the output page.
:P.
The :HP1.place:eHP1. specifies the page position of the banner.
The value BOTODD means the bottom of odd numbered pages.
The values that can be specified are:
:XMP.
  top       top of all pages
  topodd    top of odd pages
  topeven   top of even pages
  bottom    bottom of all pages
  botodd    bottom of odd pages
  boteven   bottom of even pages
:eXMP.
:H3.Defining the Banner Region
:P.
The banner region tag has a number of attributes which define the area
of the banner in which the region is placed.
The following shows how to define one of the
banner regions for our previous example.
:XMP.
&gml.BANREGION
        indent = 0
        hoffset = left
        width = extend
        voffset = 2
        depth = 1
        font = 0
        refnum = 1
        region_position = left
        pouring = last
        contents = headtext0
&gml.eBANREGION.
:eXMP.
:P.
The horizontal position of the banner region within the banner is
defined by the :HP1.hoffset:eHP1. and :HP1.indent:eHP1. attributes.
The horizontal offset attribute (hoffset) defines the offset from
the left edge of the banner.
In this case, the value LEFT specifies that the region should start
at the left side of the banner.
The indent attribute supplies an indentation to be applied to the
region position after the horizontal offset is determined.
:P.
The :HP1.width:eHP1. attribute defines the width of the banner.
The value EXTEND sets the region width to be from the start point
to the right edge of the banner or the next region, whichever comes
first.
Use this value for regions with a large width.
If the page margins change, the region widths will automatically
be adjusted for the new margins.
:P.
The :HP1.voffset:eHP1. attribute defines the vertical offset from
the top of the banner area to the top of the region.
If the banner depth is "3", a vertical offset of "2" will start
the region on the last line of the banner.
This would put the text on the last line of the page and ensure
that at least two blank lines are left between the main body
of text and the banner text.
:P.
The :HP1.depth:eHP1. attribute defines the depth of the region within
the banner.
If it specifies more than one line space, text which does not fit on
the first line of the region will be split to the following region lines.
:P.
Each region must be uniquely identified by the value of the
:HP1.refnum:eHP1. attribute.
These values are later used if one region of a banner is replaced.
The :HP1.font:eHP1. attribute defines the character set used in the
region.
:P.
Within the banner region, text can be placed with the
:HP1.region_position:eHP1. attribute.
The value LEFT specifies that the text should be placed starting
at the left side of the region.
:P.
The :HP1.pouring:eHP1. attribute is used to specify how headings
are placed in a banner.
If the requested heading does not appear on the page, the value LAST
obtains the last heading used in the document of the same level
(in the previous example, this would be level zero).
This attribute is ignored if the region contents does not contain
heading information.
:P.
The :HP1.contents:eHP1. attribute specifies the content of the banner
region.
The value HEADTEXT0 requests the text component of the last heading
zero produced in the document.
This value would not include the heading number.
A number of different values may be specified with this attribute,
including constant string data.
:H3.Sample Banner Definition
:P.
The following shows the banner definition which would create the
bottom banner for the example output shown earlier.
:XMP.
&gml.BANNER
        left_adjust = 0
        right_adjust = 0
        depth = 3
        docsect = body
        place = topodd
&gml.BANREGION
        indent = 0
        hoffset = left
        width = extend
        voffset = 2
        depth = 1
        font = 0
        refnum = 1
        region_position = left
        pouring = last
        contents = headtext0
&gml.eBANREGION.
&gml.BANREGION
        indent = 0
        hoffset = right
        width = 3
        voffset = 2
        depth = 1
        font = 0
        refnum = 2
        region_position = right
        pouring = none
        contents = pgnuma
&gml.eBANREGION.
&gml.eBANNER.
:eXMP.
:H3.Using Symbols in Banner Definitions
:P.
Symbol names may be specified in the string value of a banner definition
content attribute.
:I1.banner symbols
A number of symbol names are defined when a banner is created to contain
special values.
To create a bottom banner with the page number centered and surrounded
by dashes as shown in the following,
:XMP.
+------------------------+
|          - 1 -         |
+------------------------+
:eXMP.
:PC.
create the following banners:
:XMP.
&gml.BANNER
        left_adjust = 0
        right_adjust = 0
        depth = 3
        place = bottom
        docsect = body
&gml.BANREGION
        indent = 0
        hoffset = left
        width = extend
        voffset = 2
        depth = 1
        font = 0
        refnum = 1
        region_position = center
        pouring = last
        contents = '- &amp.amp.$pgnuma. -'
&gml.eBANREGION
&gml.eBANNER
:eXMP.
