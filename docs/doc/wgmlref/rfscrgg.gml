.cp 1.25i
:H2.GA Control Word
:XMP.
  .GA  tagname | *   attname | *   \(options(A)\)  \(options(B)\)

where options(A) are:

        OFF | ON
        UPpercase
        REQuired

where options(B) are:

        AUTOmatic   'string'
        LENgth      integer number
        RANge       minvalue   maxvalue   \(default1  \(default2\)\)
        VALue       'valname'  \(USE       'string'\)  \(DEFault\)
        ANY         \('string'\)
        RESET       'valname' | 'string' | integer
:eXMP.
:P.
The &key.GML attribute&ekey. control word defines or modifies an attribute
:I1.GML attribute control word
:I1.attribute control word
for a GML tag.
The :HP1.tagname:eHP1. value must have been previously defined by
a .GT control word.
If an asterisk(*) is used, the last GML tag defined or operated on will
be referenced.
The :HP1.attname:eHP1. value defines a new or modifies an existing tag
attribute name.
An attribute name must contain no more than nine alphanumeric characters.
If an asterisk(*) is used, the last attribute name specified for the
current tag will be referenced.
:P.
One or more of the option(A) values may be specified with the GML attribute
control word.
:DL.
:DT.OFF
:DD.The attribute will be ignored when specified on a GML tag by the user.
:DT.ON
:DD.Processing of an attribute which was previously ignored due to the
    OFF option is restarted.
:DT.UPpercase
:DD.The value of the attribute is converted to uppercase before being processed.
:DT.REQuired
:DD.The attribute must always be specified when the GML tag is used.
:eDL.
:P.
More than one option(B) value may be specified for a GML attribute,
each of which must be specified by a separate .GA control word.
:DL.
:DT.AUTOmatic
:DD.The string value specified with this option is processed as the
    value of the attribute as if it was specified by the user.
    An automatic attribute may not be actually specified by the user
    with the GML tag.
:DT.LENgth
:DD.The number specified with this option is the maximum number of
    characters accepted as an attribute value.
:DT.RANge
:DD.The first two numbers specify the minimum and maximum numeric
    values allowed with the current attribute.
    The optional number :HP1.default1:eHP1. provides a default value
    if the attribute is not specified with the tag.
    If the attribute is specified without a value, the optional
    number :HP1.default2:eHP1. provides a default value (:HP1.default1:eHP1.
    will be the default if :HP1.default2:eHP1. is not specified).
:DT.VALue
:DD.The option operand :HP1.valname:eHP1. is defined as one of the possible
    values for an attribute.
    The VALUE option must be specified for each possible VALNAME you wish to
    define as a possible attribute value.
    If the &mono.USE&emono. keyword is specified, the USE string value
    is processed as the attribute value when the VALNAME value is specified.
    The &mono.DEFault&emono. keyword defines the default attribute value
    if the attribute is not specified with the GML tag.
:DT.ANY
:DD.Any character string may be specified as the attribute value.
    If the optional string operand is also specified, it is used as the
    default value if the attribute is not specified with the GML tag.
:DT.RESET
:DD.The reset option resets the current attribute values.
    With an AUTOMATIC or ANY attribute, the default string operand is reset.
    With a RANGE attribute, two numbers may be specified to reset the
    two default range numbers.
    With a VALUE attribute, the option will reset the default value to
    the specified value name.
:eDL.
.cp 1.25i
:H2.GT Control Word
:XMP.
  .GT  tagname       ADD     macro-name   \(tag options\)

or

  .GT  tagname       CHAnge  macro-name

or

  .GT  tagname | *   DELete | PRint

or

  .GT  tagname       OFF | ON

where tag options are:

        ATTributes
        CONTinue
        CSOFF
        NOCONTinue
        TAGnext
        TEXTDef         'string'
        TEXTError
        TEXTReqd
:eXMP.
:P.
The &key.GML tag&ekey. control word defines or modifies a
:I1.GML tag control word
:I1.tag control word
GML tag.
The :HP1.tagname:eHP1. value must have been previously defined by
a .GT control word for all but the &mono.ADD&emono. operand,
and may not contain more than fifteen alphanumeric characters.
:DL.
:DT.ADD
:DD.Specifies a new GML tag and assigns the macro 'macro-name' to
    process the tag information.
    The tag options 'continue', 'nocontinue', and 'tagnext' are
    recognized but not currently supported.
    :DL.
    :DT.ATTributes
    :DD.The GML tag has one or more attributes.
    :DT.CONTinue
    :DD.Each tag is treated as though it starts on a new input line.
        The 'continue' option causes a continue character to be
        generated before processing the tag.
    :DT.CSOFF
    :DD.This option will terminate any active process control
        (or conditional) sections.
    :DT.NOCONTinue
    :DD.The current tag cannot be continued by a previous tag.
    :DT.TAGnext
    :DD.Document text is not allowed after the current tag.
        Another GML tag must follow in the input.
    :DT.TEXTDef
    :DD.The specified character string is used if tag text is
        not specified with the tag.
    :DT.TEXTError
    :DD.Tag text is not allowed with the tag.
    :DT.TEXTLine
    :DD.All data to the end of the input line is treated as tag text.
    :DT.TEXTReqd
    :DD.Tag text must be specified with the tag.
    :eDL.
:DT.CHAnge
:DD.The macro processor for the current GML tag is reassigned to
    be the macro 'macro-name'.
:DT.DELete
:DD.The current GML tag and its associated attributes are deleted,
    and will no longer be recognized as a GML tag.
    If an asterisk(*) is specified as the tag name, all GML tags
    are deleted.
:DT.OFF
:DD.The GML tag will be not be processed if found in the document.
:DT.ON
:DD.Processing of a tag which was previously ignored due to the
    OFF option is restarted.
:DT.PRint
:DD.The current GML tag and its associated attributes are printed
    on the output screen.
    If an asterisk(*) is specified as the tag name, all GML tags
    are printed.
:eDL.
