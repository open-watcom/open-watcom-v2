:H3.INDEX
:P.
Define the characteristics of the index section.
:INCLUDE file='rfltindx'.
:DL.
:INCLUDE file='rflapos'.
If the index heading is not displayed (the header attribute
has a value of NO), the post-skip value has no effect.
:INCLUDE file='rflapts'.
:INCLUDE file='rflalad'.
:INCLUDE file='rflarad'.
:INCLUDE file='rflaspc'.
index.
:INCLUDE file='rflacol'.
    for the index.
:DT.see_string
:I2 refid='lasee_s'.lasee_s
:DD.This attribute accepts a character string.
    The specified string precedes any :HP1.see:eHP1. text generated
    in the index.
:DT.see_also_string
:I2 refid='lasee_a'.lasee_a
:DD.This attribute accepts a character string.
    The specified string precedes any :HP1.see also:eHP1. text generated
    in the index.
:INCLUDE file='rflahdr'.
the index heading is generated.
:INCLUDE file='rflahdr2'.
:DT.index_string
:I2 refid='laindex'.laindex
:DD.This attribute accepts a character string.
If the index header is generated, the specified string is used
for the heading text.
:INCLUDE file='rflapge'.
:INCLUDE file='rflapgr'.
:INCLUDE file='rflafon'.
header attribute value.
The font value is linked to the
left_adjust, right_adjust, pre_top_skip and post_skip attributes
(see :HDREF refid='fnlink'.).
:EDL.
:H3.IXHEAD
:P.
Define the characteristics of the index headings.
In most cases, the index heading is the letter which starts the
index terms following it.
:INCLUDE file='rfltixh'.
:DL.
:SET symbol=local value='index heading.'.
:INCLUDE file='rflapes'.
:INCLUDE file='rflapos'.
:INCLUDE file='rflafon'.
&local.
The font value is linked to the
indent, pre_skip and post_skip attributes
(see :HDREF refid='fnlink'.).
:DT.indent
:I2 refid='lainden'.lainden
:DD.The indent attribute accepts any valid horizontal space unit.
    The attribute space value is added to the current left margin
    before the index heading is generated in the index.
    The left margin is reset to its previous value after the heading
    is generated.
:DT.frame
:I2 refid='laframe'.laframe
:DD.This attribute accepts the values
    :hp1.rule:ehp1.,
    :hp1.box:ehp1.,
    :hp1.none:ehp1.,
    and :hp1.'character string':ehp1..
    The specified attribute value determines the type of framing
    around the index heading.
    See the discussion of the frame attribute under
    :HDREF refid='gtfig'.
    for an explanation of the attribute values.
:DT.header
:I2 refid='laheade'.laheade
:DD.This attribute accepts the keyword values :hp1.yes:ehp1.
    and :hp1.no:ehp1..
    If 'no' is specified, the index heading is not displayed.
    The font and frame
    attributes are ignored, and the pre and post skip values are merged.
:EDL.
:H3.IXMAJOR
:P.
Define the characteristics of the major index signifiers.
:INCLUDE file='rfltixm'.
:DL.
:INCLUDE file='rflafon'.
major indexing signifier.
:EDL.
:H3.IXPGNUM
:P.
Define the characteristics of the index page numbers.
:INCLUDE file='rfltixp'.
:DL.
:INCLUDE file='rflafon'.
index page numbers.
:EDL.
:H3.In
:P
Define the characteristics of an index entry level, where
:HP1.n:eHP1. is 1, 2, or 3.
The :HP1.string_font:eHP1. attribute is only valide with
index entry levels one and two.
:INCLUDE file='rflti1'.
:I2 refid='lti2'.lti2
:I2 refid='lti3'.lti3
:DL.
:SET symbol=local value='index entry.'.
:INCLUDE file='rflapes'.
:INCLUDE file='rflapos'.
:INCLUDE file='rflaskp'.
each entry in an index level.
:INCLUDE file='rflafon'.
&local.
The font value is linked to the
wrap_indent, skip, pre_skip and post_skip attributes
(see :HDREF refid='fnlink'.).
:DT.indent
:I2 refid='lainden'.lainden
:DD.The indent attribute accepts any valid horizontal space unit.
    The attribute space value is added to the current left margin
    before the index entry is produced in the index.
    After the index entries under the current entry are produced,
    the left margin is reset to its previous value.
:DT.wrap_indent
:I2 refid='lawrap_'.lawrap_
:DD.This attribute accepts as a value any valid horizontal space unit.
    If the list of references for an index entry in the index
    does not fit on one output line, the specified attribute value
    indicates the indentation that is to occur on the following output
    lines.
:DT.index_delim
:I2 refid='laidxd'.laidxd
:DD.This attribute accepts a string value which is placed between
    the index text and the index page number(s).
    If the text, page number(s) and delimiter does not fit on one output
    line, the delimiter text is not used.
:DT.string_font
:I2 refid='lastrin'.lastrin
:DD.This attribute accepts a positive integer number,
and is valid with the :HP2.&gml.i1:eHP2. and :HP2.&gml.i2:eHP2. layout tags.
If a font number is used for which no font has been defined,
&WGML. will use font zero.
The font numbers from zero to three
correspond directly to highlighting levels specified
by the highlighting phrase GML tags.
The string_font attribute defines the font of the
the :HP1.see:eHP1. and :HP1.see_also:eHP1. attribute strings defined
by the &gml.INDEX layout tag.
:EDL.
:H3.LAYOUT
:P.
Start the layout definition.
:INCLUDE file='rfltlay'.
:H3.LETDATE
:P.
Define the characteristics of the date entity in the letter tag format.
:INCLUDE file='rfltletd'.
:DL.
:INCLUDE file='rfladaf'.
:DT.depth
:I2 refid='ladepth'.ladepth
:DD.The depth attribute specifies the amount of space to leave before the
    line of date text.
    This attribute accepts any valid vertical space unit.
    This attribute is often used to position
    beyond pre-printed letter head.
:INCLUDE file='rflafon'.
date text.
The font value is linked to the
date_form and depth attributes
(see :HDREF refid='fnlink'.).
:INCLUDE file='rflapgp'.
    date
:INCLUDE file='rflapgp2'.
:EDL.
:H3.LP
:P.
Define the characteristics of the list part entity.
:INCLUDE file='rfltlp'.
:DL.
:INCLUDE file='rflalin'.
list part.
:INCLUDE file='rflarin'.
list part.
:INCLUDE file='rflalii'.
    list part.
:SET symbol=local value='list part.'.
:INCLUDE file='rflapes'.
:INCLUDE file='rflapos'.
:INCLUDE file='rflaspc'.
list part.
:EDL.
:H3.LQ
:P.
Define the characteristics of the long quote entity.
:INCLUDE file='rfltlq'.
:DL.
:INCLUDE file='rflalin'.
long quote.
:INCLUDE file='rflarin'.
long quote.
:SET symbol=local value='long quote.'.
:INCLUDE file='rflapes'.
:INCLUDE file='rflapos'.
:INCLUDE file='rflaspc'.
long quote.
:INCLUDE file='rflafon'.
quote text.
The font value is linked to the
left_indent, right_indent, pre_skip and post_skip attributes
(see :HDREF refid='fnlink'.).
:EDL.
:H3.NOTE
:P.
Define the characteristics of the note entity.
:INCLUDE file='rfltnote'.
:DL.
:INCLUDE file='rflalin'.
note.
:INCLUDE file='rflarin'.
note.
:SET symbol=local value='note.'.
:INCLUDE file='rflapes'.
:INCLUDE file='rflapos'.
:INCLUDE file='rflafon'.
text specified by the :HP1.note_string:eHP1. attribute.
The font value is linked to the
left_indent, right_indent, pre_skip and post_skip attributes
(see :HDREF refid='fnlink'.).
:INCLUDE file='rflaspc'.
note.
:DT.note_string
:I2 refid='lanote_'.lanote_
:DD.This attribute accepts a character string.
The specified string precedes the text of the note.
The length of this string determines indentation of the note text.
:EDL.
:H3.OL
:P.
Define the characteristics of the ordered list entity.
:INCLUDE file='rfltol'.
:DL.
:SET symbol=local value='ordered list.'.
:SET symbol=loc2 value='ol'.
:INCLUDE file='rflalev'.
:INCLUDE file='rflalin'.
ordered list.
:INCLUDE file='rflarin'.
ordered list.
:INCLUDE file='rflapes'.
:INCLUDE file='rflaskp'.
    list items.
:INCLUDE file='rflaspc'.
list item.
:INCLUDE file='rflapos'.
:INCLUDE file='rflafon'.
ordered list text.
The font value is linked to the
left_indent, right_indent, pre_skip, post_skip and skip attributes
(see :HDREF refid='fnlink'.).
:SET symbol=loca1 value='list item number'.
:INCLUDE file='rflaaln'.
list item.
:INCLUDE file='rflanst'.
    list item number.
:INCLUDE file='rflanst2'.
:INCLUDE file='rflanfon'.
list item number.
The font value is linked to the
align attibute
(see :HDREF refid='fnlink'.).
:EDL.
:H3.OPEN
:P.
Define the characteristics of the open entity in the letter tag format.
:INCLUDE file='rfltopen'.
:DL.
:SET symbol=local value='opening salutation.'.
:INCLUDE file='rflapts'.
:INCLUDE file='rflafon'.
    opening salutation.
The font value is linked to the
pre_top_skip attribute
(see :HDREF refid='fnlink'.).
:DT.delim
:I2 refid='ladelim'.ladelim
:DD.The delim attribute sets the delimiter that is output
    following the opening salutation
    to a specific character.
:EDL.
:H3.P
:P.
Define the characteristics of the paragraph entity.
:INCLUDE file='rfltp'.
:DL.
:SET symbol=local value='paragraph.'.
:INCLUDE file='rflalii'.
    paragraph.
:INCLUDE file='rflapes'.
:INCLUDE file='rflapos'.
:EDL.
:H3.PAGE
:P.
Define the boundaries of the document on the output page.
:INCLUDE file='rfltpage'
:Dl.
:DT.top_margin
:I2 refid='latop_m'.latop_m
:DD.The top margin attribute specifies the amount of space
    between the top of the page and the start of the output text.
    This attribute accepts any valid vertical space unit.
:DT.left_margin
:I2 refid='lalefma'.lalefma
:DD.The left margin attribute specifies the amount of space
    between the left side of
    the page and the start of the output text.
    This attribute accepts any valid horizontal space unit.
:DT.right_margin
:I2 refid='laritma'.laritma
:DD.The right margin attribute specifies the amount of space
    between the left side of the page and the right margin of
    of the output text.
    This attribute accepts any valid horizontal space unit.
:DT.depth
:I2 refid='ladepth'.ladepth
:DD.The depth attribute specifies the depth of the output page.
    Output text starts at the top margin and ends at the bottom
    margin of the page.
    The bottom margin is the sum of the :HP1.top_margin:eHP1. and
    :HP1.depth:eHP1. attribute values.
    This attribute accepts any valid vertical space unit.
:EDL.
:H3.PC
:P.
Define the characteristics of the paragraph continuation entity.
:INCLUDE file='rfltpc'.
:DL.
:SET symbol=local value='paragraph continuation.'.
:INCLUDE file='rflalii'.
    paragraph continuation.
:INCLUDE file='rflapes'.
:INCLUDE file='rflapos'.
:EDL.
:H3.PREFACE
:P.
Define the characteristics of the preface section and preface
heading.
:INCLUDE file='rfltpref'
:DL
:SET symbol=local value='preface heading.'.
:INCLUDE file='rflapos'.
If the preface heading is not displayed (the header attribute has
a value of NO), the post-skip value has no effect.
:INCLUDE file='rflapts'.
:INCLUDE file='rflafon'.
&local.
The font value is linked to the
pre_top_skip and post_skip attributes
(see :HDREF refid='fnlink'.).
:INCLUDE file='rflaspc'.
preface.
:INCLUDE file='rflahdr'.
the preface heading is generated.
:INCLUDE file='rflahdr2'.
:DT.preface_string
:I2 refid='laprefa'.laprefa
:DD.This attribute accepts a character string.
If the preface header is generated, the specified string is used
for the heading text.
:INCLUDE file='rflapge'.
:INCLUDE file='rflapgr'.
:INCLUDE file='rflacol'.
    for the preface.
:EDL.
:H3 id='laysave'.SAVE
:P.
Save the current layout into the specified file name.
This tag is equivalent to the :hp2.&gml.convert:eHP2. tag.
:XMP.
&gml.SAVE file='filename'.
:EXMP.
:H3.SL
:P.
Define the characteristics of the simple list entity.
:INCLUDE file='rfltsl'.
:DL.
:SET symbol=local value='simple list.'.
:SET symbol=loc2 value='sl'.
:INCLUDE file='rflalev'.
:INCLUDE file='rflalin'.
simple list.
:INCLUDE file='rflarin'.
simple list.
:INCLUDE file='rflapes'.
:INCLUDE file='rflaskp'.
    list items.
:INCLUDE file='rflaspc'.
list items.
:INCLUDE file='rflapos'.
:INCLUDE file='rflafon'.
simple list text.
The font value is linked to the
left_indent, right_indent, pre_skip, post_skip and skip attributes
(see :HDREF refid='fnlink'.).
:EDL.
:H3.SUBJECT
:P.
Define the characteristics of the
subject entity in the letter tag format.
:INCLUDE file='rfltsubj'
:DL.
:SET symbol=local value='subject line.'.
:INCLUDE file='rflalad'.
:INCLUDE file='rflapgp'.
    subject line
:INCLUDE file='rflapgp2'.
:INCLUDE file='rflapts'.
:INCLUDE file='rflafon'.
subject line.
The font value is linked to the
left_adjust and pre_top_skip attributes
(see :HDREF refid='fnlink'.).
:EDL.
:H3.TITLE
:P.
Define the characteristics of the title line entity.
:INCLUDE file='rflttitl'.
:DL.
:INCLUDE file='rflalad'.
:INCLUDE file='rflarad'.
:INCLUDE file='rflapgp'.
    title line
:INCLUDE file='rflapgp2'.
:INCLUDE file='rflafon'.
title line.
The font value is linked to the
left_adjust, right_adjust, pre_top_skip and skip attributes
(see :HDREF refid='fnlink'.).
:SET symbol=local value='title lines.'.
:INCLUDE file='rflapts'.
:INCLUDE file='rflaskp'.
    title lines.
:EDL.
:H3.TITLEP
:P.
Define the characteristics of the title part section.
:INCLUDE file='rflttitp'.
:DL.
:INCLUDE file='rflaspc'.
title part.
:INCLUDE file='rflacol'.
    for the title part section.
:EDL.
:H3.TO
:P.
Define the characteristics of the TO entity in the letter tag format.
:INCLUDE file='rfltto'.
:DL.
:SET symbol=local value='TO text.'.
:INCLUDE file='rflalad'.
:INCLUDE file='rflapgp'.
TO text
:INCLUDE file='rflapgp2'.
:INCLUDE file='rflapts'.
:INCLUDE file='rflafon'.
TO text.
The font value is linked to the
left_adjust and pre_top_skip attributes
(see :HDREF refid='fnlink'.).
:eDL.
:H3.TOC
:P.
Define the characteristics of the table of contents.
:INCLUDE file='rflttoc'.
:DL.
:INCLUDE file='rflalad'.
:INCLUDE file='rflarad'.
:INCLUDE file='rflaspc'.
table of contents.
:INCLUDE file='rflacol'.
    for the table of contents.
:DT.toc_levels
:I2 refid='latoc_l'.latoc_l
:DD.This attribute accepts as its value a positive integer value.
    The attribute value specifies the maximum level of the entries
    that appear in the table of contents.
    For example, if the attribute value is four, heading levels
    zero through three will appear in the table of contents.
:INCLUDE file='rflafil'.
:EDL.
:H3.TOCHn
:P
Define the characteristics of a table of contents heading,
where :HP1.n:eHP1. is between zero and six inclusive.
:INCLUDE file='rflttoc0'.
:I2 refid='lttoch1'.lttoch1
:I2 refid='lttoch2'.lttoch2
:I2 refid='lttoch3'.lttoch3
:I2 refid='lttoch4'.lttoch4
:I2 refid='lttoch5'.lttoch5
:I2 refid='lttoch6'.lttoch6
:DL.
:SET symbol=local
 value='group of level &amp.hp2.n&amp.ehp2. table of contents heading entries.'.
:SET symbol=loca1 value='table of contents'.
:INCLUDE file='rflgroup'.
:DT.indent
:I2 refid='lainden'.lainden
:DD.The indent attribute accepts any valid horizontal space unit.
    The attribute space value is added to the current left margin
    before the table of contents entry is produced.
    After all of the subentries under
    the current entry are produced,
    the left margin is reset to its previous value.
:INCLUDE file='rflaskp'.
    the individual entries within
    the group of level :hp2.n:ehp2.
    table of contents heading entries.
:INCLUDE file='rflapes'.
:INCLUDE file='rflapos'.
:SET symbol=loca1 value='table of contents heading'.
:INCLUDE file='rflaaln'.
heading entry.
:INCLUDE file='rflafon'.
heading produced in the table of contents entry.
:DT.display_in_toc
:I2 refid='ladistc'.ladistc
:DD.This attribute accepts the keyword values :hp1.yes:ehp1.
    and :hp1.no:ehp1..
    The heading for the table of contents entry is not produced
    when the value :hp1.no:ehp1. is specified.
    The entries pre and post skips are still generated.
:EDL.
:H3.TOCPGNUM
:P.
Define the characteristics of the table of contents page numbers.
:INCLUDE file='rflttocp'.
:DL.
:INCLUDE file='rflasiz'.
    page number of a table of contents entry.
:INCLUDE file='rflafon'.
page number.
The font value is linked to the
size attribute
(see :HDREF refid='fnlink'.).
:EDL.
:H3.UL
:P.
Define the characteristics of the unordered list entity.
:INCLUDE file='rfltul'.
:DL.
:SET symbol=local value='unordered list.'.
:SET symbol=loc2 value='ul'.
:INCLUDE file='rflalev'.
:INCLUDE file='rflalin'.
unordered list.
:INCLUDE file='rflarin'.
unordered list.
:INCLUDE file='rflapes'.
:INCLUDE file='rflaskp'.
    list items.
:INCLUDE file='rflaspc'.
list items.
:INCLUDE file='rflapos'.
:INCLUDE file='rflafon'.
unordered list text.
The font value is linked to the
left_indent, right_indent, pre_skip, post_skip and skip attributes
(see :HDREF refid='fnlink'.).
:SET symbol=loca1 value='list item bullet'.
:INCLUDE file='rflaaln'.
list item.
:DT.bullet
:I2 refid='labulle'.labulle
:DD.This attribute specifies the single character
    value which annotates
    an unordered list item.
:DT.bullet_translate
:I2 refid='labullt'.labullt
:DD.This attribute accepts the keyword values :hp1.yes:ehp1.
    and :hp1.no:ehp1..
    If 'yes' is specified, input translation is performed on the
    annotation character.
:DT.bullet_font
:I2 refid='labullf'.labullf
:DD.This attribute accepts a non-negative integer number.
    If a font number is used for which no font has been defined,
    &WGML. will use font zero.
    The font numbers from zero to three
    correspond directly to the highlighting levels specified
    by the highlighting phrase GML tags.
    The bullet_font attribute defines the font of the
    annotation character.
    The font value is linked to the
    align attibute
    (see :HDREF refid='fnlink'.).
:EDL.
:H3.WIDOW
:P.
Define the widowing control of document elements.
:INCLUDE file='rfltwid'.
:DL.
:INCLUDE file='rflathr'.
A document element will be forced to the next page or column
if the threshold requirement is not met.
:EDL.
:H3.XMP
:P.
Define the characteristics of the example entity.
:INCLUDE file='rfltxmp'.
:DL.
:SET symbol=local value='example.'.
:INCLUDE file='rflalin'.
example.
:INCLUDE file='rflarin'.
example.
:INCLUDE file='rflapes'.
:INCLUDE file='rflapos'.
:INCLUDE file='rflaspc'.
example.
:INCLUDE file='rflafon'.
&local.
The font value is linked to the
left_indent, right_indent, pre_skip and post_skip attributes
(see :HDREF refid='fnlink'.).
:EDL.
