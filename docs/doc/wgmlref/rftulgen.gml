:H2.General Modifications
:P.
Some of the layout items control general aspects of the document
instead of specific document elements.
One of these is the :HP2.&gml.page:eHP2. layout tag.
:XMP.
&gml.LAYOUT
&gml.PAGE
        left_margin = '.5i'
        right_margin = 80
&gml.eLAYOUT.
&gml.GDOC.
   .
   .
   .
:eXMP.
:PC.
The :HP2.&gml.page:eHP2. layout tag defines information about
the page you are printing on.
The attributes used in the example above define the
left and right margins.
The other attributes of this tag do not have to be specified
if you do not need to change their values.
:P.
The :HP1.right_margin:eHP1. attribute value is eighty(80) characters.
The actual amount of space that this value represents depends on
the size of the characters used to produce the document.
The :HP1.left_margin:eHP1. attribute is half of one inch, and does
not have any dependency on the character set used.
For more information on the possible width values, see
:HDREF refid='spechor'..
