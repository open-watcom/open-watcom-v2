:H2.First Example Document
:P.The first example illustrates a number of the features in the
GML language.
The GML file is as follows&gml.
:fig id=rfgbeg2 place=inline frame=box.
 :INCLUDE file='rfgbeg'.
 :figcap.First GML example
:efig.
:INCLUDE file='rffhead'.
:fig id=rflbeg2 place=inline frame=box.
 :INCLUDE file='rflbeg'.
 :figcap.
 :figdesc.Output of :figref refid=rfgbeg2 page=no.
:efig.
:pc
The first example consists of a heading and two paragraphs.
:p.It should be noted that the GML source contains two types
of information.
:dl.
:dt.Tags
:IH1.tags
:I2.definition of
:dd.Each tag in the example starts with a colon(&gml.)
and ends with a period(.).
Although the period is usually optional, it is
a good practice to always include it.
This convention avoids the necessity of learning the specific
instances in which the period would be required.
The tags used in the first example are
:hp2.&gml.gdoc:ehp2.,
:hp2.&gml.body:ehp2.,
:hp2.&gml.h0:ehp2.,
:hp2.&gml.p:ehp2. and
:hp2.&gml.egdoc:ehp2..
Tags can be entered using either upper or lower case letters.
The tags will be shown in upper case in the tutorial examples.
Ending tags, such as &gml.eGDOC., will be entered with the "e" in lower
case to emphasize that they are used in conjunction with other tags.
:dt.Text
:IH1.text
:I2.definition of
:dd.The words and punctuation to be processed is called text.
:edl.
:P.The following tags have been introduced in this example.
:dl.
:dt.&gml.gdoc
:I2 refid='ttgdoc'.ttgdoc
:dd.This tag indicates the start of the GML document.
It must precede all text to be formatted.
:dt.&gml.egdoc
:I2 refid='ttegdoc'.ttegdoc
:dd.This tag indicates the end of the GML document.
It must be the last tag in the GML source file.
:dt.&gml.body
:I2 refid='ttbody'.ttbody
:dd.This tag indicates the start of the main text for the document.
Since this is a simple document, it precedes
the specification of the document text.
:dt.&gml.h0
:I2 refid='tth0'.tth0
:dd.This tag specifies a level zero heading.
Heading tags define the structural divisions of the document
text, and are described in the section :HDREF refid='rftuhd'..
:dt.&gml.p
:I2 refid='ttp'.ttp
:dd.This tag indicates the start of a paragraph.
    The sentences of the paragraph may start on separate input lines;
    &WGML. processes the text into a paragraph for you.
    It is a common practice to start sentences on separate input lines
    to make it easier to modify the text in future revisions of the
    document.
:edl.
:P.
With most of the examples in the tutorial, the layout of the
resulting output shown in this book will be slightly different from
the tutorial output you will see.
This difference results from the use of a modified version
of the default layout for the examples in this book.
:H2.Processing the Examples
:P.
The simplest way to process the examples given in this
tutorial is to use one of the command formats shown below.
The format of the command will depend on the system on which you are
running the &WGML. processor.
For a complete description of the WGML command and the command line
options, see :HDREF refid='rungml'..
:P.
Create the file &mono.EX1.GML&emono.
with an editor, using
the document text shown in :FIGREF refid='rfgbeg2'..
Note that the source for these examples is supplied with the product
distribution.
:P.
The :HP2.WGML:eHP2. command
is used to invoke the &WGML. processor.
The command line option :HP2.DEVICE:eHP2.
selects the type of device
for which the document is being processed.
:DL tsize=15 compact.
:DT.&ibmvm.
:DD.WGML EX1 ( DEVICE TERM
:DT.&decvax.
:DD.WGML EX1/DEVICE=TERM
:DT.&ibmpc.
:DD.WGML EX1 ( DEVICE TERM
:eDL.
:P.
If you see an error message concerning invalid options or device/font
member not found, there has probably been an error in the installation
of &WGML..
Refer to the section of the installation guide
on setting up the device library for corrective action.
