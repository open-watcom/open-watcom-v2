:H1 id='rflays'.Layouts
:H2.Specifying and Using Layouts
:P.
The layout determines the way in which the document elements
specified by the GML tags are formatted on the output page.
Many of the formatting actions may be modified through the
supplied layout tags.
The layout tags are specified in much the same way as
the GML tags are specified in the document.
Some of the layout tags, such as :hp2.&gml.fig:ehp2., have the same name
as the GML tags which they modify.
:P.
A layout section starts with the :hp2.&gml.layout:ehp2.
:I2 refid='ltlayou'.ltlayou
tag, and must appear before the :hp2.&gml.gdoc:ehp2. tag.
The layout section is terminated with an :hp2.&gml.elayout:ehp2. tag.
Only the portions of the layout you wish to change need to be
specified, as the changes
modify the default layout which is built
into &WGML..
If more than one layout is specified, the changes are cumulative.
With the exception of the :HP2.&gml.banner:eHP2. and :HP2.&gml.banregion:eHP2.
tags, the attributes of the layout tags are all optional.
:P.
The layout section may be stored in a separate file.
This file may be included at the start of the document with
the :HP2.&gml.include:eHP2. tag, or specified when you run &WGML.
with the LAYOUT command line option.
Including the layout with either of these two methods makes it
easier to select a different layout.
:P.
The :hp2.&gml.convert:ehp2.
:I2 refid='ltconve'.ltconve
tag may be used to determine the
attribute values in the current layout.
:H2 id='numstyl'.Number Style
:P.
The term :hp2.number style:ehp2.
:I1.number styles
is used throughout
the layout section of this document.
The number style is a sequence of up to three codes which
defines the style of a generated number.
The first code indicates the form of the number digits.
:DL.
:DT.A
:DD.The number is formed with lower case alphabetic
    characters.  Example&gml. :hp2.28:ehp2. is represented by
    :hp2.ab:ehp2. while :hp2.29:ehp2. is represented by
    :hp2.ac:ehp2.. (a=1, b=2,..., z=26, aa=27, ab=28)
:DT.B
:DD.The number is formed with upper case alphabetic
    characters.  Example&gml. :hp2.28:ehp2. is represented by
    :hp2.AB:ehp2. while :hp2.29:ehp2. is represented by
    :hp2.AC:ehp2..
    (A=1, B=2,..., Z=26, AA=27, AB=28)
:DT.H
:DD.The number is formed with hindu-arabic characters.
    Example&gml. The number twenty eight is represented by
    :hp2.28:ehp2..
:DT.R
:DD.The number is formed with lower case roman numerals.
    Example&gml. The number :hp2.28:ehp2. is represented by
    :hp2.xxviii:ehp2..
:DT.C
:DD.The number is formed with upper case roman numerals.
    Example&gml. The number :hp2.28:ehp2. is represented by
    :hp2.XXVIII:ehp2..
:EDL.
:PC.
The second code, which does not have to be specified,
defines how the number is separated from other numbers
or text.
:DL.
:DT.D
:DD.The number is followed by a decimal point.
:DT.P
:DD.The number is surrounded by parentheses.
:EDL.
:PC.
The third code may be specified if parentheses were specified
in the second code.
:DL.
:DT.A
:DD.The number is preceded by a left parenthesis and is
    not followed by a right parenthesis.
:DT.B
:DD.The number is followed by a right parenthesis and is
    not preceded by a left parenthesis.
:EDL.
:H2.Layout Tags
:P.
This section contains a subsection on each of the
layout tags supported by &WGML..
The tags are presented in alphabetical order, each with an example.
Most of the example values are the values used with
the default layout.
The :HP2.&gml.convert:eHP2.
:I2 refid='ltconve'.ltconve
tag can be used to determine the exact values.
:INCLUDE file='rflay1'.
:INCLUDE file='rflay2'.
