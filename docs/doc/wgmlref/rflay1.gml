:H3.ABSTRACT
:P.
Define the characteristics of the abstract section
and the abstract heading.
:INCLUDE file='rfltabs'.
:DL
:SET symbol=local value='abstract heading.'.
:INCLUDE file='rflapos'.
If the abstract heading is not displayed (the header attribute
has a value of NO), the post-skip value has no effect.
:INCLUDE file='rflapts'.
:INCLUDE file='rflafon'.
&local.
The font value is linked to the
pre_top_skip and post_skip attributes
(see :HDREF refid='fnlink'.).
:INCLUDE file='rflaspc'.
abstract section.
:INCLUDE file='rflahdr'.
the abstract heading is generated.
:INCLUDE file='rflahdr2'.
:DT.abstract_string
:I2 refid='laabstr'.laabstr
:DD.This attribute accepts a character string.
If the abstract header is generated, the specified string is used
for the heading text.
:INCLUDE file='rflapge'.
:INCLUDE file='rflapgr'.
With the &gml.ABSTRACT tag only, a value of :HP1.yes:eHP1. will cause
the page number to always be reset after the title page.
:INCLUDE file='rflacol'.
    for the abstract.
:EDL.
:H3.ADDRESS
:P.
Define the characteristics of the address entity.
:INCLUDE file='rfltadd'.
:DL.
:SET symbol=local value='address.'.
:INCLUDE file='rflalad'.
:INCLUDE file='rflarad'.
:INCLUDE file='rflapgp'.
    address
:INCLUDE file='rflapgp2'.
:INCLUDE file='rflafon'.
&local.
The font value is linked to the
left_adjust, right_adjust and pre_skip attributes of the
&gml.ADDRESS tag, and the skip attribute of the &gml.ALINE tag
(see :HDREF refid='fnlink'.).
:INCLUDE file='rflapes'.
:EDL.
:H3.ALINE
:P.
Define the characteristics of the address line entity.
:INCLUDE file='rfltalin'.
:DL.
:INCLUDE file='rflaskp'.
    address lines.
:EDL.
:H3.APPENDIX
:P.
Define the characteristics of the appendix section and appendix
heading.
All of the attributes, with the exception of the spacing value, apply
to the :HP2.&gml.h1:eHP2. tag while in the appendix section.
:INCLUDE file='rfltapp'.
:DL.
:SET symbol=local value='appendix heading.'.
:INCLUDE file='rflaina'.
    appendix heading.
:INCLUDE file='rflapts'.
:INCLUDE file='rflapos'.
If the appendix heading is not displayed, the post-skip is
ignored.
:INCLUDE file='rflaspc'.
appendix section.
:INCLUDE file='rflafon'.
&local.
The font value is linked to the
indent, post_skip and pre_top_skip attributes
(see :HDREF refid='fnlink'.).
:INCLUDE file='rflanfon'.
    appendix number.
:INCLUDE file='rflanfo'.
    appendix heading
:INCLUDE file='rflanfo2'.
:INCLUDE file='rflapgp'.
    appendix heading
:INCLUDE file='rflapgp2'.
:INCLUDE file='rflanst'.
    appendix heading number.
:INCLUDE file='rflanst2'.
:INCLUDE file='rflapge'.
:DT.line_break
:I2 refid='laline_'.laline_
:DD.This attribute accepts the keyword values :hp1.yes:ehp1.
    and :hp1.no:ehp1..
    If the value :hp1.yes:ehp1. is specified,
    the skip value specified by the post_skip attribute
    will be issued.
    If the value :hp1.no:ehp1. is specified,
    the skip value specified by the post_skip attribute
    will be ignored.  If a paragraph follows the heading,
    the paragraph text will start on the same line as the heading.
:DT.display_heading
:I2 refid='ladispl'.ladispl
:DD.This attribute accepts the keyword values :hp1.yes:ehp1.
    and :hp1.no:ehp1..
    The heading is not produced when the value :hp1.no:ehp1. is specified.
    The heading pre and post skips are still generated.
:INCLUDE file='rflnumr'.
:INCLUDE file='rflcase'.
:SET symbol=loca1 value='appendix heading'.
:INCLUDE file='rflaaln'.
appendix heading.
:INCLUDE file='rflahdr'.
the appendix header (specified by the :HP1.appendix_string:eHP1.
attribute) is generated at the beginning of the heading text specified
by a :HP2.&gml.h1:eHP2. tag.
:INCLUDE file='rflahdr2'.
:DT.appendix_string
:I2 refid='laappen'.laappen
:DD.This attribute accepts a character string.
If the appendix header is generated, the specified string is inserted
before the :HP2.&gml.h1:eHP2. heading text.
:INCLUDE file='rflapgr'.
:DT.section_eject
:I2 refid='lasecti'.lasecti
:DD.This attribute accepts the keyword values :hp1.yes:ehp1.,
    :hp1.no:ehp1., :HP1.odd:eHP1., and :HP1.even:eHP1..
    If the value :HP1.no:eHP1. is specified, the section is not
    forced to a new page.
    The section is always placed on a new page when
    the value :hp1.yes:ehp1. is specified.
:P.
    The values :HP1.odd:eHP1. and :HP1.even:eHP1. will place the section
    on a new page if the parity (odd or even) of the current page number
    does not match the specified value.
:INCLUDE file='rflacol'.
    for the appendix.
:EDL.
:H3.ATTN
:P.
Define the characteristics of the attention entity in the letter format.
:INCLUDE file='rfltattn'.
:DL.
:SET symbol=local value='attention text.'.
:INCLUDE file='rflalad'.
:INCLUDE file='rflapgp'.
attention text
:INCLUDE file='rflapgp2'.
:INCLUDE file='rflapts'.
:INCLUDE file='rflafon'.
attention text.
The font value is linked to the
left_adjust and pre_top_skip attributes
(see :HDREF refid='fnlink'.).
:DT.attn_string
:I2 refid='laattn_'.laattn_
:DD.This attribute accepts a character string.
The specified string precedes the attention text in the output document.
:DT.string_font
:I2 refid='lastrin'.lastrin
:DD.This attribute accepts a non-negative integer number.
If a font number is used for which no font has been defined,
&WGML. will use font zero.
The font numbers from zero to three
correspond directly to the highlighting levels specified
by the highlighting phrase GML tags.
The :HP1.string_font:eHP1. attribute defines the font of the
the attention string defined by the :HP1.attn_string:eHP1. attribute.
:eDL.
:H3.AUTHOR
:P.
Define the characteristics of the author entity.
:INCLUDE file='rfltauth'.
:DL.
:SET symbol=local value='author lines.'.
:INCLUDE file='rflalad'.
:INCLUDE file='rflarad'.
:INCLUDE file='rflapgp'.
    author line
:INCLUDE file='rflapgp2'.
:INCLUDE file='rflafon'.
&local.
The font value is linked to the
left_adjust, right_adjust, pre_skip and skip attributes
(see :HDREF refid='fnlink'.).
:INCLUDE file='rflapes'.
:INCLUDE file='rflaskp'.
&local.
:EDL.
:H3.BACKM
:P.
Define the characteristics of the back material section.
:INCLUDE file='rfltback'.
:DL.
:SET symbol=local value='back material.'.
:INCLUDE file='rflapos'.
If the back material heading is not displayed (the header attribute
has a value of NO), the post-skip value has no effect.
:INCLUDE file='rflapts'.
:INCLUDE file='rflahdr'.
the back material heading is generated.
:INCLUDE file='rflahdr2'.
:DT.backm_string
:I2 refid='labackm'.labackm
:DD.This attribute accepts a character string.
If the back material header is generated, the specified string is used
for the heading text.
:INCLUDE file='rflapge'.
:INCLUDE file='rflapgr'.
:INCLUDE file='rflacol'.
    for the back material.
:INCLUDE file='rflafon'.
header attribute value.
The font value is linked to the
pre_top_skip and post_skip attributes
(see :HDREF refid='fnlink'.).
:EDL.
:H3.BANNER
:IH1 seeid='etbanne'.page banner
:IH1 seeid='etbanne'.running titles
:IH1 seeid='etbanne'.footers
:IH1 seeid='etbanne'.headers
:P.
Defines a page banner.
A page banner appears at the top and/or bottom of a page.
Information such as page numbers, running titles and the current
heading would be defined in a banner.
Banners may be defined for the top and/or bottom  of a page in
each section of the document.
The banner attributes specify the size of the banner
and the document section in which it is to be used.
:P.
A banner definition begins with the :HP2.&gml.banner:eHP2.
tag and ends with the :HP2.&gml.ebanner:eHP2. tag.
The banner is divided into a number of regions,
each defined by the :HP2.&gml.banregion:eHP2. tag.
The banner region definitions are placed after the banner
attributes and before the :HP2.&gml.ebanner:eHP2. tag.
:INCLUDE file='rfltbann'.
:DL.
:INCLUDE file='rflalad'.
:INCLUDE file='rflarad'.
:DT.depth
:I2 refid='ladepth'.ladepth
:DD.The depth attribute accepts as its value any valid vertical
    space unit.
    It specifies the vertical depth of the banner.
:DT.place
:I2 refid='laplace'.laplace
:DD.The place attribute specifies where on the odd or even numbered
    output page the banner is to be placed.
    The following values may be specified for this attribute&gml.
    :DL compact.
    :DT.top
    :DD.The top of pages.
    :DT.bottom
    :DD.The bottom of pages.
    :DT.topodd
    :DD.The top of odd pages.
    :DT.topeven
    :DD.The top of even pages.
    :DT.botodd
    :DD.The bottom of odd pages.
    :DT.boteven
    :DD.The bottom of even pages.
    :EDL.
:DT.refplace
:I2 refid='larefpl'.larefpl
:DD.The refplace attribute specifies the place value of an existing
    banner.
:DT.docsect
:I2 refid='ladocse'.ladocse
:DD.The document section for which the banner will be used.
    The following values may be specified for this attribute&gml.
    :DL compact.
    :DT.abstract
    :DD.The banner will appear in the abstract section of the document.
    :DT.appendix
    :DD.The banner will appear in the appendix section of the document.
    :DT.backm
    :DD.The banner will appear in the back material section of the document.
    :DT.body
    :DD.The banner will appear in the body section of the document.
    :DT.figlist
    :DD.The banner will appear in the figure list
        section of the document.
    :DT.HEADn
    :DD.The banner will appear when a heading of level
        :hp2.n:ehp2., where :hp2.n:ehp2. may have a value of zero
        through six inclusive, appears on the output page.
    :DT.letfirst
    :DD.The banner will appear on the first page of the letter when
        the letter format is used.
        If the letter has only one page, only the banner defined for
        the top of the page will be used.
        Even page banners are not allowed if :HP1.letfirst:eHP1.
        is the document section value.
    :DT.letlast
    :DD.The banner will appear on the last page of the letter when
        the letter format is used.
        If the letter has only one page, only the banner defined for
        the bottom of the page will be used.
    :DT.letter
    :DD.The banner will appear on the pages
        between the first and last page of the letter when
        the letter format is used.
    :DT.index
    :DD.The banner will appear in the index section of the document.
    :DT.preface
    :DD.The banner will appear in the preface section of the document.
    :DT.toc
    :DD.The banner will appear in the table of contents
        section of the document.
    :EDL.
:DT.refdoc
:I2 refid='larefdo'.larefdo
:DD.The refdoc attribute specifies the docsect value of an existing
    banner.
:EDL.
:P.
The :hp1.refplace:ehp1. and :hp1.refdoc:ehp1. attributes are
used in combination to specify an existing banner.
The referenced banner is copied to the banner being defined.
These attributes are most commonly used
when duplicating a banner for an odd or even page.
When these attributes are specified, only the :hp1.place:ehp1.
and :hp1.docsect:ehp1. attributes are required.
All other attributes will override the attribute values of the
banner being copied.
If the two reference attributes are not specified, all of the other
attributes are required.
:P.
To delete a banner, specify only the :hp1.place:ehp1. and
:hp1.docsect:ehp1. attributes, and delete the individual
banner regions.
:H3 id='rflbanr'.BANREGION
:P.
Define a banner region within a banner.
Each banner region specifies a rectangular section of the banner.
A banner region begins with a :HP2.&gml.banregion:eHP2. tag
and ends with an :HP2.&gml.ebanregion:eHP2. tag.
All banner regions are defined after the banner tag
attributes and before the :HP2.&gml.ebanner:eHP2. tag.
:INCLUDE file='rfltbanr'.
:DL.
:DT.indent
:I2 refid='lainden'.lainden
:DD.The indent attribute accepts any valid horizontal space unit.
    The specified space value is added to the value of
    the horizontal offset attribute (hoffset) to determine the start of
    banner region in the banner if the horizontal offset is
    specified as :hp1.left:ehp1., :hp1.centre:ehp1.,
    or :hp1.center:ehp1..
    If the horizontal offset is specified as :hp1.right:ehp1.,
    the indent value is subtracted from the right margin of the banner.
:DT.hoffset
:I2 refid='lahoffs'.lahoffs
:DD.The hoffset attribute specifies
    the horizontal offset from the left side of the banner where
    the banner region will start.
    The attribute value may be any valid horizontal space unit,
    or one of the keywords
    :hp1.left:ehp1.,
    :hp1.center:ehp1.,
    :hp1.centre:ehp1.,
    or :hp1.right:ehp1..
    The keyword values remove the dependence upon the left and right
    adjustment settings of the banner that occurs when using an
    absolute horizontal offset.
:DT.width
:I2 refid='lawidth'.lawidth
:DD.This attribute may be any valid horizontal space unit, or
    the keyword :hp1.extend:ehp1..
    If the width of the banner region is specified
    as :hp1.extend:ehp1., the width of the region will be increased
    until the start of another banner region
or the right margin of the banner
    is reached.
:DT.voffset
:I2 refid='lavoffs'.lavoffs
:DD.This attribute accepts any valid vertical space unit.
    It specifies the vertical offset from the top of the banner for
    the start of the banner region.
    A value of zero will be the first line of the banner,
    while the value one will be the second line of the banner.
:DT.depth
:I2 refid='ladepth'.ladepth
:DD.The depth attribute accepts a vertical space unit value.
    The attribute value specifies the number of output lines or
    vertical space of the banner region.
:INCLUDE file='rflafon'.
banner region's contents.
:DT.refnum
:I2 refid='larefnu'.larefnu
:DD.This attribute accepts a positive integer number.
    Each banner region must have a unique reference number.
    If this is the only attribute specified, the banner region
    is deleted from the banner.
:DT.region_position
:I2 refid='laregio'.laregio
:DD.This attribute specifies the position
    of the data within the banner region.
    The attribute value may be
    one of the keywords
    :hp1.left:ehp1.,
    :hp1.center:ehp1.,
    :hp1.centre:ehp1.,
    or :hp1.right:ehp1..
:DT.pouring
:I2 refid='lapouri'.lapouri
:DD.When the value of the contents attribute is a heading,
    and a heading of the specified level does
    not appear on the output page, the contents can be
    'poured' back to a previous heading level.
    When the attribute value :hp1.none:ehp1.
    is specified, no pouring occurs.
    In this case, the region will be empty.
    When the attribute value :hp1.last:ehp1.
    is specified, the last heading appearing in the document
    with the same level as the heading specified
    by the contents attribute is used.
    The attribute value :hp1.headn:ehp1., where
    :hp2.n:ehp2. may have a value of zero through six inclusive,
    may be specified.
    In this case,
    the last heading appearing in the document which has a level
    between zero and the pouring value is used.
:DT.script_format
:I2 refid='lascrip'.lascrip
:DD.This attribute determines if the contents region is processed
    as a Script content string in the same way as the operand of
    a Script running title control word.
    If the attribute value is
    :hp1.yes:ehp1.,
    then the value of the content attribute is treated as a Script
    format title string.
:DT.contents
:I2 refid='laconte'.laconte
:DD.This attribute defines the content of the banner region.
    If the content value does not fit in the banner region,
    the value is truncated.
:I3 refid='presyms'.banner symbols
    Symbols containing the values for each of the content keywords are
    also listed.
    Specifying these symbols as part of the string content may be used
    to create more complex banner region values.
    Note that when using a symbol in a content string of a banner definition,
    you will need to protect it from being substituted during the
    definition with the &amp.AMP symbol (ie &amp.AMP.AUTHOR.).
    The possible values are&gml.
    :DL.
    :DT.author
    :DD.The first author of the document will be used.
        The symbol $AUTHOR is also defined with this value.
    :DT.bothead
    :DD.The last heading on the output page is used.
        The symbol $BOTHEAD is also defined with this value.
    :DT.date
    :DD.The current date will be used.
    :DT.docnum
    :DD.The document number will be the content of the banner region.
        The symbol $DOCNUM is also defined with this value.
    :DT.HEADn
    :DD.The last heading of level
        :hp2.n:ehp2., where :hp2.n:ehp2. may have a value of zero
        through six inclusive.
        Both the heading number and heading text are both used.
        The symbols $HEAD0 through $HEAD6 are also defined with this value.
    :DT.HEADNUMn
    :DD.The heading number from the last heading of level
        :hp2.n:ehp2., where :hp2.n:ehp2. may have a value of zero
        through six inclusive.
        The symbols $HNUM0 through $HNUM6 are also defined with this value.
    :DT.HEADTEXTn
    :DD.The text of the heading from the last heading of level
        :hp2.n:ehp2., where :hp2.n:ehp2. may have a value of zero
        through six inclusive.
        If the :HP1.stitle:eHP1. attribute was specified for the
        selected heading, the stitle value is used.
        The symbols $HTEXT0 through $HTEXT6 are also defined with this value.
    :DT.none
    :DD.The banner region will be empty.
    :DT.pgnuma
    :DD.The content of the banner region will be the page number
        of the output page in the hindu-arabic numbering style.
        The symbol $PGNUMA is also defined with this value.
    :DT.pgnumad
    :DD.The content of the banner region will be the page number
        of the output page in the hindu-arabic numbering style
        followed by a decimal point.
        The symbol $PGNUMAD is also defined with this value.
    :DT.pgnumr
    :DD.The content of the banner region will be the page number
        of the output page in the lower case roman
        numbering style.
        The symbol $PGNUMR is also defined with this value.
    :DT.pgnumrd
    :DD.The content of the banner region will be the page number
        of the output page in the lower case roman numbering style
        followed by a decimal point.
        The symbol $PGNUMRD is also defined with this value.
    :DT.pgnumc
    :DD.The content of the banner region will be the page number
        of the output page in the upper case roman
        numbering style.
        The symbol $PGNUMC is also defined with this value.
    :DT.pgnumcd
    :DD.The content of the banner region will be the page number
        of the output page in the upper case roman numbering style
        followed by a decimal point.
        The symbol $PGNUMCD is also defined with this value.
    :DT.rule
    :DD.The content of the banner region will be a rule line which
        fills the entire region.
    :DT.sec
    :DD.The security value specified by the :hp1.sec:ehp1. attribute
        on the :hp2.&gml.gdoc:ehp2. tag is used.
        The symbol $SEC is also defined with this value.
    :DT.stitle
    :DD.The :HP1.stitle:eHP1. attribute value from the first
        title tag specified in the front material of the document is used.
        If the stitle attribute was not specified, the title
        text is used.
        The symbol $STITLE is also defined with this value.
    :DT.title
    :DD.The text of the first title tag specified
        in the front material of the document is used.
        The symbol $TITLE is also defined with this value.
    :DT.string
    :DD.Any character string enclosed in quotation marks.
    :DT.time
    :DD.The current time will be used.
    :DT.tophead
    :DD.The first heading on the output page is used.
        The symbol $TOPHEAD is also defined with this value.
    :EDL.
:EDL.
:P.
If a banner region does not already exist, then all attributes
must be specified.
If you wish to modify an existing banner region, the
:hp1.refnum:ehp1. attribute will uniquely identify the region.
When the reference number is that of an existing banner region,
all other attributes will modify the values of the existing
banner region.
:P.
To delete a banner region, specify only the :HP1.refnum:eHP1. attribute.
All banner regions must be deleted before a banner definition will be removed.
:H3.BODY
:P.
Define the characteristics of the body section.
:INCLUDE file='rfltbody'.
:DL.
:SET symbol=local value='body.'.
:INCLUDE file='rflapos'.
If the body heading is not displayed (the header attribute
has a value of NO), the post-skip value has no effect.
:INCLUDE file='rflapts'.
:INCLUDE file='rflahdr'.
the body heading is generated.
:INCLUDE file='rflahdr2'.
:DT.body_string
:I2 refid='labody_'.labody_
:DD.This attribute accepts a character string.
If the body header is generated, the specified string is used
for the heading text.
:INCLUDE file='rflapge'.
:INCLUDE file='rflapgr'.
:INCLUDE file='rflafon'.
header attribute value.
The font value is linked to the
pre_top_skip and post_skip attributes
(see :HDREF refid='fnlink'.).
:EDL.
:H3.CIT
:P.
Define the characteristics of the citation entity.
:INCLUDE file='rfltcit'.
:DL.
:INCLUDE file='rflafon'.
citation text.
:EDL.
:H3.CLOSE
:P.
Define the characteristics of the close entity in the letter format.
:INCLUDE file='rfltclos'.
:DL.
:SET symbol=local value='close.'.
:INCLUDE file='rflapes'.
:DT.depth
:I2 refid='ladepth'.ladepth
:DD.The depth attribute accepts as its value any valid vertical space
    unit.
    The value specifies the amount of space to be
    left after the text of the :HP2.&gml.close:eHP2. tag.
:INCLUDE file='rflafon'.
    closing statements.
The font value is linked to the
depth and pre_skip attributes
(see :HDREF refid='fnlink'.).
:INCLUDE file='rflapgp'.
closing statements
:INCLUDE file='rflapgp2'.
:DT.delim
:I2 refid='ladelim'.ladelim
:DD.The delimiter attribute sets the delimiter character to be used
    following the closing salutation.
:DT.extract_threshold
:I2 refid='laextra'.laextra
:DD.The depth attribute accepts as its value
    a positive integer number.
    If the text associated with the :HP2.&gml.close:eHP2. tag
    starts on a new page,
    the number of lines specified by the
    :HP1.extract_threshold:eHP1. attribute
    will move to the next page with the closing text.
:EDL.
:H3.CONVERT
:P.
Convert the current layout into the specified file name.
The resulting file will contain the entire layout
in a readable form.
:XMP.
&gml.CONVERT file='file name'.
:EXMP.
:I2 refid='ltconve'.ltconve
:H3.DATE
:P.
Defines the characteristics of the date entity in
the standard tag format.
The :HP2.&gml.letdate:eHP2. layout tag defines the characteristics
of the date entity in the letter tag format.
:INCLUDE file='rfltdate'.
:DL.
:INCLUDE file='rfladaf'.
:INCLUDE file='rflalad'.
:INCLUDE file='rflarad'.
:INCLUDE file='rflapgp'.
    date
:INCLUDE file='rflapgp2'.
:INCLUDE file='rflafon'.
date text.
The font value is linked to the
left_adjust, right_adjust and pre_skip attributes
(see :HDREF refid='fnlink'.).
:SET symbol=local value='date.'.
:INCLUDE file='rflapes'.
:EDL.
:H3.DD
:P.
Define the characteristics of the data description entity.
:INCLUDE file='rfltdd'.
:DL.
:DT.line_left
:I2 refid='lalinle'.lalinle
:DD.This attribute accepts any valid horizontal space unit.
    The specified amount of space must be available on the
    output line after the definition term which precedes the
    data description.
    If there is not enough space, the data description will
    be started on the next output line.
:INCLUDE file='rflafon'.
data description.
:EDL.
:H3.DDHD
:P.
Define the characteristics of the data description heading entity.
:INCLUDE file='rfltddhd'.
:DL.
:INCLUDE file='rflafon'.
data description heading.
:EDL.
:H3 id='deflay'.DEFAULT
:P.
Define default characteristics for document processing.
:INCLUDE file='rfltdef'.
:DL.
:INCLUDE file='rflaspc'.
document when there is no layout entry for spacing with a
specific document element.
:INCLUDE file='rflacol'.
    on each output page.
:INCLUDE file='rflafon'.
    document when the font is not explicitly determined by the
    document element.
:DT.justify
:I2 refid='lajusti'.lajusti
:DD.The justify attribute accepts the keyword values :hp1.yes:ehp1.
    and :hp1.no:ehp1..
    Right justification of text is performed if this attribute has a
    value of :hp1.yes:ehp1..
    If justification is not desired, the value should be :hp1.no:ehp1..
:DT.input_esc
:I2 refid='lainput'.lainput
:DD.The input escape attribute accepts the keyword value
    :HP1.none:eHP1. or a quoted character.
    Input escapes are not recognized if the attribute value is
    :HP1.none:eHP1. or a blank.
    If a character is specified as the attribute value,
    this character is used as the input escape delimiter.
    If an empty('') or :HP1.none:eHP1. value is specified,
    the blank value is used.
    Refer to :HDREF refid='intrans'. for more information.
:DT.gutter
:I2 refid='lagutte'.lagutte
:DD.The gutter attribute specifies the amount of space between
    columns in a multi-column document, and has no effect in
    a single column document.
    This attribute accepts any valid horizontal space unit.
:DT.binding
:I2 refid='labindi'.labindi
:DD.The binding attribute accepts any valid horizontal space unit.
    The binding value is added to the current left and right margins of
    those output pages which are odd numbered.
:EDL.
:H3.DISTRIB
:P.
Define the characteristics of the distribution list entity.
:INCLUDE file='rfltdist'.
:DL.
:SET symbol=local value='distribution list.'.
:INCLUDE file='rflapts'.
:INCLUDE file='rflaskp'.
items of the distribution list.
:INCLUDE file='rflafon'.
items of the distribution list.
The font value is linked to the
indent, skip and pre_top_skip attributes
(see :HDREF refid='fnlink'.).
:DT.indent
:I2 refid='lainden'.lainden
:DD.The indent attribute accepts any valid horizontal space unit.
    The indent value is the offset from the left margin for the
    &local.
:DT.page_eject
:I2 refid='lapage_'.lapage_
:DD.This attribute accepts the keyword values :hp1.yes:ehp1.
    and :hp1.no:ehp1..
    If the value :hp1.yes:ehp1. is specified, the distribution list is
    placed on a new page.
:EDL.
:H3.DOCNUM
:P.
Define the characteristics of the document number entity.
:INCLUDE file='rfltdoc'.
:DL.
:SET symbol=local value='document number.'.
:INCLUDE file='rflalad'.
:INCLUDE file='rflarad'.
:INCLUDE file='rflapgp'.
    document number
:INCLUDE file='rflapgp2'.
:INCLUDE file='rflafon'.
    text specified by the :HP1.document_string:eHP1.
    attribute and the document number.
The font value is linked to the
left_adjust, right_adjust and pre_skip attributes
(see :HDREF refid='fnlink'.).
:INCLUDE file='rflapes'.
:DT.docnum_string
:I2 refid='ladocnu'.ladocnu
:DD.This attribute accepts a character string.
The specified string precedes the document number in the output document.
:EDL.
:H3.DL
:P.
Define the characteristics of the definition list entity.
:INCLUDE file='rfltdl'.
:DL.
:SET symbol=local value='definition list.'.
:SET symbol=loc2 value='dl'.
:INCLUDE file='rflalev'.
:INCLUDE file='rflalin'.
definition list.
:INCLUDE file='rflarin'.
definition list.
:INCLUDE file='rflapes'.
:INCLUDE file='rflaskp'.
each item of the definition list.
:INCLUDE file='rflaspc'.
items of the definition list.
:INCLUDE file='rflapos'.
:SET symbol=loca1 value='definition term'.
:INCLUDE file='rflaaln'.
definition list item.
:DT.line_break
:I2 refid='laline_'.laline_
:DD.This attribute accepts the keyword values :hp1.yes:ehp1.
    and :hp1.no:ehp1..
    If the value :hp1.yes:ehp1. is specified,
    the data description starts a new line after the definition term
    if the length of the term is larger than align value.
    If the value :hp1.no:ehp1. is specified,
    the definition term is allowed to intrude into the data description area.
:EDL.
:H3.DT
:P.
Define the characteristics of the definition term entity.
:INCLUDE file='rfltdt'.
:DL.
:SET symbol=local value='definition term.'.
:INCLUDE file='rflafon'.
&local.
The font value is linked to the
left_indent, right_indent, pre_skip, post_skip, skip and align attributes
of the :HP2.&gml.dl:eHP2. tag,
and the line_left attribute of the :HP2.&gml.DD:eHP2. tag
(see :HDREF refid='fnlink'.).
:EDL.
:H3.DTHD
:P.
Define the characteristics of the definition term heading entity.
:INCLUDE file='rfltdthd'.
:DL.
:INCLUDE file='rflafon'.
definition term heading.
:EDL.
:H3.EBANNER
:P.
Mark the end of a banner definition.
:INCLUDE file='rflteban'.
:H3.EBANREGION
:P.
Mark the end of a banner region definition.
:INCLUDE file='rfltebre'.
:H3.ECLOSE
:P.
Mark the end of the close entity in the letter tag format.
:INCLUDE file='rflteclo'.
:DL.
:SET symbol=local value='typist mark.'.
:INCLUDE file='rflapes'.
:INCLUDE file='rflafon'.
&local.
The font value is linked to the
pre_skip attribute
(see :HDREF refid='fnlink'.).
:eDL.
:H3.ELAYOUT
:P.
Mark the end of a layout definition.
:INCLUDE file='rfltelay'.
:H3.FIG
:P.
Define the characteristics of the figure entity.
:INCLUDE file='rfltfig'.
:DL.
:SET symbol=local value='figure.'.
:INCLUDE file='rflalad'.
:INCLUDE file='rflarad'.
:INCLUDE file='rflapes'.
:INCLUDE file='rflapos'.
&local.
:INCLUDE file='rflaspc'.
figure.
:INCLUDE file='rflafon'.
figure text.
The font value is linked to the
left_adjust, right_adjust, pre_skip and post_skip attributes
(see :HDREF refid='fnlink'.).
:DT.default_place
:I2 refid='ladefpl'.ladefpl
:DD.This attribute accepts the values
    :hp1.top:ehp1.,
    :hp1.bottom:ehp1.,
    and :hp1.inline:ehp1..
    The specified attribute value is used as the default value for the
    place attribute of the GML figure tag.
:DT.default_frame
:I2 refid='ladefau'.ladefau
:DD.This attribute accepts the values
    :hp1.rule:ehp1.,
    :hp1.box:ehp1.,
    :hp1.none:ehp1.,
    and :hp1.'character string':ehp1..
    The specified attribute value is used as the default value for the
    frame attribute of the GML figure tag.
    See the discussion about the frame attribute under :HDREF refid='gtfig'.
    for an explanation of the attribute values.
:EDL.
:H3.FIGCAP
:P.
Define the characteristics of the figure caption entity.
:INCLUDE file='rfltfigc'.
:DL.
:SET symbol=local value='figure caption.'.
:INCLUDE file='rflapli'.
:INCLUDE file='rflafon'.
figure caption text.
The font value is linked to the
pre_lines attribute
(see :HDREF refid='fnlink'.).
:DT.figcap_string
:I2 refid='lafigca'.lafigca
:DD.This attribute accepts a character string.
The specified string is the first part of the figure caption
generated by &WGML..
:DT.string_font
:I2 refid='lastrin'.lastrin
:DD.This attribute accepts a non-negative integer number.
If a font number is used for which no font has been defined,
&WGML. will use font zero.
The font numbers from zero to three
correspond directly to the highlighting levels specified
by the highlighting phrase GML tags.
The :HP1.string_font:eHP1. attribute defines the font of the
the figure caption string defined from the text specified
by the :HP1.figcap_string:eHP1.
attribute to the figure caption delimiter inclusive.
:DT.delim
:I2 refid='ladelim'.ladelim
:DD.This attribute accepts a quoted character value.
    The delimiter value specifies the character which is inserted
    after the number of the figure.
    If a character other than a blank space is specified,
    that character followed by a blank space will be inserted.
    If a blank space is specified, only that blank space will be
    inserted.
:EDL.
:H3.FIGDESC
:P.
Define the characteristics of the figure description entity.
:INCLUDE file='rfltfigd'.
:DL.
:SET symbol=local value='figure description.'.
:INCLUDE file='rflapli'.
If the previous tag was :HP2.&gml.figcap:eHP2., this value is ignored.
:INCLUDE file='rflafon'.
figure description.
The font value is linked to the
pre_lines attribute
(see :HDREF refid='fnlink'.).
:EDL.
:H3.FIGLIST
:P.
Define the characteristics of the figure list.
:INCLUDE file='rfltfigl'.
:DL.
:INCLUDE file='rflalad'.
:INCLUDE file='rflarad'.
:INCLUDE file='rflaskp'.
    figure list items.
:INCLUDE file='rflaspc'.
    figure list.
:INCLUDE file='rflacol'.
    for the figure list.
:INCLUDE file='rflafil'.
:EDL.
:H3.FLPGNUM
:P.
Define the characteristics of the figure list page numbers.
:INCLUDE file='rfltflp'.
:DL.
:INCLUDE file='rflasiz'.
    figure page number.
:INCLUDE file='rflafon'.
page number.
The font value is linked to the
size attribute
(see :HDREF refid='fnlink'.).
:EDL.
:H3.FN
:P.
Define the characteristics of the footnote entity.
:INCLUDE file='rfltfn'.
:DL.
:SET symbol=local value='footnote.'.
:INCLUDE file='rflalii'.
    footnote.
:SET symbol=loca1 value='footnote number'.
:INCLUDE file='rflaaln'.
    footnote.
:SET symbol=local value='footnotes are output.'.
:INCLUDE file='rflapli'.
:SET symbol=local value='footnote.'.
:INCLUDE file='rflaskp'.
    the footnotes.
:INCLUDE file='rflaspc'.
    footnote.
:INCLUDE file='rflafon'.
footnote text.
The font value is linked to the
line_indent, pre_lines, skip and align attributes
(see :HDREF refid='fnlink'.).
:INCLUDE file='rflanfon'.
footnote number.
:INCLUDE file='rflanst'.
footnote number.
:INCLUDE file='rflanst2'.
:DT.frame
:I2 refid='laframe'.laframe
:DD.This attribute accepts the values
    :hp1.rule:ehp1. or
    :hp1.none:ehp1..
    If the value :HP1.rule:eHP1. is specified, a rule line is placed
    between the main body of text and the footnotes at the bottom of the
    output page.
    If the footnote is placed across the entire page, the width of the
    rule line is half the width of the page.
    If the footnote is one column wide, the rule line width is the
    width of a column minus twenty percent.
:EDL.
:H3.FNREF
:P.
Define the characteristics of the footnote reference entity.
:INCLUDE file='rfltfnre'.
:DL.
:INCLUDE file='rflafon'.
footnote reference text.
:INCLUDE file='rflanst'.
footnote reference number.
:INCLUDE file='rflanst2'.
:EDL.
:H3.FROM
:P.
Define the characteristics of the FROM entity in the letter tag format.
:INCLUDE file='rfltfrom'.
:DL.
:SET symbol=local value='FROM text.'.
:INCLUDE file='rflalad'.
:INCLUDE file='rflapgp'.
from text
:INCLUDE file='rflapgp2'.
:INCLUDE file='rflapts'.
:INCLUDE file='rflafon'.
&local.
The font value is linked to the
left_adjust, page_position and pre_top_skip attributes
(see :HDREF refid='fnlink'.).
:eDL.
:H3.GD
:P.
Define the characteristics of the glossary description entity.
:INCLUDE file='rfltgd'.
:DL.
:INCLUDE file='rflafon'.
glossary description.
:EDL.
:H3.GL
:P.
Define the characteristics of the glossary list entity.
:INCLUDE file='rfltgl'.
:DL.
:SET symbol=local value='glossary list.'.
:SET symbol=loc2 value='gl'.
:INCLUDE file='rflalev'.
:INCLUDE file='rflalin'.
glossary list.
:INCLUDE file='rflarin'.
glossary list.
:INCLUDE file='rflapes'.
:INCLUDE file='rflaskp'.
each item of the glossary list.
:INCLUDE file='rflaspc'.
items of the glossary list.
:INCLUDE file='rflapos'.
:SET symbol=loca1 value='glossary term'.
:INCLUDE file='rflaaln'.
glossary list item.
:DT.delim
:I2 refid='ladelim'.ladelim
:DD.The quoted character value is used to separate the glossary
    term from the glossary description.
:EDL.
:H3.GT
:P.
Define the characteristics of the glossary term entity.
:INCLUDE file='rfltgt'.
:DL.
:SET symbol=local value='glossary term.'.
:INCLUDE file='rflafon'.
&local.
The font value is linked to the
left_indent, right_indent, pre_skip, post_skip, skip and align attributes
of the :HP2.&gml.gl:eHP2. tag
(see :HDREF refid='fnlink'.).
:EDL.
:H3.HEADING
:P.
Specify information which applies to headings in general.
:INCLUDE file='rflthead'.
:DL.
:DT.delim
:I2 refid='ladelim'.ladelim
:DD.The delim attribute sets the heading number delimiter
    to a specific character.
:DT.stop_eject
:I2 refid='lastop_'.lastop_
:DD.This attribute accepts the keyword values :hp1.yes:ehp1.
    and :hp1.no:ehp1..
    If the value :hp1.yes:ehp1. is specified,
    a heading which would force the beginning of a new page will
    not cause a page ejection if it immediately follows another
    heading.
:DT.para_indent
:I2 refid='lapara_'.lapara_
:DD.This attribute accepts the keyword values :hp1.yes:ehp1.
    and :hp1.no:ehp1..
    If the value :hp1.no:ehp1. is specified,
    the indentation of the first line in a paragraph
    after a heading is suppressed.
:INCLUDE file='rflathr'.
The heading will be forced to the next page or column if the
threshold requirements are not met by the following document
element.
The threshold attribute of the heading overrides the default
threshold specified by the :HP2.&gml.widow:eHP2. tag.
:DT.max_group
:I2 refid='lamax_g'.lamax_g
:DD.This attribute accepts a positive integer number.
    If a group of headings are forced to a new page or column because of
    threshold requirements, the specified value will limit the
    number of headings forced as a group.
:EDL.
:H3.Hn
:P.
Define the characteristics of a heading tag, where
:hp2.n:ehp2. is between zero and six inclusive.
:INCLUDE file='rflth0'.
:I2 refid='lth1'.lth1
:I2 refid='lth2'.lth2
:I2 refid='lth3'.lth3
:I2 refid='lth4'.lth4
:I2 refid='lth5'.lth5
:I2 refid='lth6'.lth6
:DL.
:SET symbol=local value='heading.'.
:SET symbol=loca1 value='headings'.
:INCLUDE file='rflgroup'.
:INCLUDE file='rflaina'.
    heading.
:INCLUDE file='rflapts'.
:INCLUDE file='rflapos'.
:INCLUDE file='rflapes'.
:INCLUDE file='rflaspc'.
heading if it takes more than one line.
:INCLUDE file='rflafon'.
heading text.
The font value is linked to the
indent, pre_top_skip and post_skip attributes
(see :HDREF refid='fnlink'.).
:INCLUDE file='rflanfon'.
    heading number.
:INCLUDE file='rflanfo'.
    heading
:INCLUDE file='rflanfo2'.
:INCLUDE file='rflapgp'.
    heading
:INCLUDE file='rflapgp2'.
:INCLUDE file='rflanst'.
    heading number.
:INCLUDE file='rflanst2'.
:INCLUDE file='rflapge'.
:DT.line_break
:I2 refid='laline_'.laline_
:DD.This attribute accepts the keyword values :hp1.yes:ehp1.
    and :hp1.no:ehp1..
    If the value :hp1.yes:ehp1. is specified,
    the skip value specified by the post_skip attribute
    will be issued.
    If the value :hp1.no:ehp1. is specified,
    the skip value specified by the post_skip attribute
    will be ignored.  If a paragraph follows the heading,
    the paragraph text will start on the same line as the heading.
:DT.display_heading
:I2 refid='ladispl'.ladispl
:DD.This attribute accepts the keyword values :hp1.yes:ehp1.
    and :hp1.no:ehp1..
    If the value :hp1.no:ehp1. is specified,
    the heading line will not be displayed.
    The heading will still be internally created, and used
    in the table of contents.
:INCLUDE file='rflnumr'.
:INCLUDE file='rflcase'.
:SET symbol=loca1 value='heading'.
:INCLUDE file='rflaaln'.
heading.
:EDL.
