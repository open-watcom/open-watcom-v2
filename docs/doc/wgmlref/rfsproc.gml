:H2 id='prcrule'.Processing Rules
:P.&WGML. processes the source document text in a particular sequence.
:I1.processing rules
:I1.scanning rules
:I1.parsing rules
Each input record is divided into smaller :HP1.logical:eHP1. records, each
:I1.input records
:I1.logical records
containing a specific type of information.
The following rules are applied to the input record in sequence.
:OL.
:LI.The input record is searched for GML tags.
The input record is split into a new logical record
at each GML tag.
The one exception to this rule is the &gml.CMT tag which results in the
entire input line, including other GML tags, to be treated as a comment and
not processed.
Recognition of a GML tag in the text may be defeated by using the
&amp.GML. symbol instead of the GML tag separator.
:LI.As each logical record is needed for processing, substitution of
symbols is performed.
.sk
:LP.
If the SCRIPT or WSCRIPT option has been specified on the command line.
:I2 refid='gropt'.script
:I1.script option
:LI.If the value of a symbol starts with the Script control word
separator (default of ';'), the input record is split into two logical
:I1.separator
:I1.logical line end
records and substitution stops.
The separator character does not appear in either logical record.
:LI.If the first character of a logical record is the Script control
word indicator (default of '.'), the record must be a Script control
word or macro line.
:I1.indicator
:I1.control word indicator
:I1.Script indicator
The control word indicator will be recognized if it is the first character
in a symbol value being substituted at the beginning of the record.
:LI.When a Script control line is specified, the list of defined macros
is searched.
If a macro with the given name is not found, the value must be
a Script control word.
:LI.If a Script control line is specified with a second control word
indicator (..\bat the beginning of the logical record), the list of
defined macros is not searched.
:LI.If a Script control line is specified with an apostrophe after the
control word indicator, control word separators are not recognized in
the logical record.
:LI.Control word separators in a Script control line will cause a
split into a new logical record at that point and stop symbol substitution.
The separator character does not appear in either logical record.
:eOL.
:NOTE.When there is more than one pass over the document source, the layout
section is only processed on the first pass.
