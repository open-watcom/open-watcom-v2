:H1 id='scrcon'.Script Support
:P.
Script is a formatting language used at many installations for
:I1.Script
:I1.formatting with Script
creating documents.
The Script commands (control words)
are format directives which define how a document
is formatted.
This is in contrast with the GML tags, which define the content of a
document.
:P.
The Script directives are recognized and processed when the SCRIPT command
line option is specified.
:I2 refid='gropt'.script
:I1.script option
Each document record which begins with a period in the first column is
a Script control line.
The period is called the :HP1.control word indicator:eHP1..
:I1.indicator
:I1.control word indicator
:I1.Script indicator
A Script control line must contain a valid Script directive.
All of the directives defined by the Waterloo Script product are recognized.
Those control words not implemented are ignored.
See :HDREF refid='prcrule'. for details on the processing rules for
a source document, and :HDREF refid='unprocs'. for a list of Script control
words which are not processed.
:P.
The control words have been implemented based on version 90.1 of the
Waterloo Script product.
The documentation for this product is included with the &wgml. package
to provide the documentation for the Script support.
The control words for define macro (.DM), gml tag (.GT), and gml attribute
(.GA) are also described in this document.
These control words are the fundamental tools needed to build your own set
of GML tags.
:P.
Many of the Script directives cause a break.
A break will cause any text currently formatted on an output line to
be sent to the output device.
Any new text will not be joined with the previously processed document text.
:H2.Control Word Modifiers
:P.
Modifiers change the processing of the Script control line, and are
placed immediately after the control word indicator.
There are two modifiers for a control word specification.
:P.
The single quote(') modifier directs the processor to ignore control
word separators(;) in the input line.
The separator character will be treated as text data, and included
in the processing of the control word operand.
:P.
If there are two control word indicators at the beginning of the
logical record, the list of macros is not searched.
The characters which follow must be a Script control word.
:H2.DM Control Word
:XMP.
  .DM  name  BEGIN
     macro data
  .DM  name END

or

  .DM  name  DELETE | OFF

or

  .DM  name  /data line1/data line2/.../data linen\(/\)
:eXMP.
:P.
The &key.define macro&ekey. control word is used to create or remove
:I1.define macro control word
:I1.macros
a macro definition, and does not cause a break.
Macros contain source fragments which may be processed by specifying
the name of the macro.
See :HDREF refid='prcrule'. for details on the processing rules of
a source document.
:P.
The first form of the &mono.\.dm&emono. control word creates a macro.
The name of a macro may be one to eight characters in length.
All macro data lines are saved without processing (including symbol
substitution) until the define macro &mono.END&emono. is recognized.
The ending define macro control word must start at
the beginning of the physical input line in the document source.
:P.
The &mono.DELETE&emono. macro option removes the specified macro name
from the list of defined macros.
:P.
The last form of the &mono.\.dm&emono. control word creates a macro
from the operand line.
The first character of the operand (in this case the /\bcharacter) is
used to delimit individual lines in the macro definition.
:H3 id='invmac'.Invoking Macros
:P.
Macros are invoked in the document source by entering the control
:I1.macros, invoking
:I1.invoking macros
:I1.macro parameters
:I1.macro symbols
word indicator(.) in the first column of a logical record immediately
followed by the name of the macro.
All of the logical record text after the macro
name is processed as parameter data for the macro.
Invoking a macro does not cause a break.
:P.
Each operand value is separated by a space.
Operand values may be enclosed in quotation marks if they contain
a blank space.
If a valid symbol name is immediately followed by an equals(=) sign,
the value to the right hand side of the equals is assigned to the
symbol name.
All other operands are assigned to symbols local to the macro.
The symbol names used for these values are &mono.&amp.*1&emono.,
&mono.&amp.*2&emono.,
&mono.&amp.*3&emono., ..., &mono.&amp.*n&emono.
until all values have been assigned to a symbol.
The symbol &mono.&amp.*0&emono. contains the number of local symbols
that have been created.
The symbol &mono.&amp.*&emono. contains the entire macro operand line.
:P.
If &mono.screen&emono. is the name of a defined macro, then it could
be invoked as follows:
:XMP.
  .screen file='example' '2.5i'
:eXMP.
:PC.
The first operand value defines the symbol &mono.&amp.file&emono. with
the value &mono.example&emono.\.
The second operand value assigns &mono.2.5i&emono.
to the local macro symbol &mono.&amp.*1&emono.\.
See :HDREF refid='symsub'. for more information on symbols and symbol
substitution.
:INCLUDE file='rfscrgg'.
