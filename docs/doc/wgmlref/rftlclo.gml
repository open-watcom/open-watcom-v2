:H2.CLOSE
:XMP.
Format&gml. &gml.CLOSE \(depth='vert-space-unit'\).<text line>
         <author lines>
:eXMP.
:I2 refid='gtclose'.gtclose
:P.
This tag closes the letter, and must be specified after the
main body of the document.
The close text line specifies the closing salutation.
The closing salutation text is placed in the output document followed by
a layout-determined delimiter (such as a comma).
Each line following the close tag will be an output line of the author's
signature and position.
The :HP2.&gml.eclose:eHP2. tag will terminate the CLOSE.
:P.
The :HP2.depth:eHP2.
:I2 refid='gadepth'.gadepth
attribute accepts any valid vertical space unit.
The specified amount of space is placed between the closing salutation
and the author lines.
