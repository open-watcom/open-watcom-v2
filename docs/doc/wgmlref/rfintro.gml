:H2.What is GML?
:PC.GML (:HP2.G:eHP2.eneralized :HP2.M:eHP2.arkup
:HP2.L:eHP2.anguage)
:IH1.GML
:I2.definition of
is a language by which the
components of a document are specified.
A GML user creates a computer file containing a document specification,
and then causes a GML processor (a computer program) to be executed.
As the GML processor executes, the specification file is read
and the document is produced for an output device such as the
terminal or a printer.
:p.Only in rare situations is the document completed at this
point.
Usually, there are revisions which must be made.
The user enters these revisions by modifying the original specification
file and then causes the GML processor to be executed to obtain a
revised version of the document.
This revision process is typically repeated many times.
:p.The method outlined above where an input file containing
document content information is used to produce a document is often
called :hp2.text formatting:ehp2..
:IH1.text formatting
:I2.definition of
GML and many text formatting languages use this method.
There are a number of benefits to using the GML Language:
:ol.
:li.A GML document is described using high-level entities such as
headings, paragraphs and lists.
On the other hand, most text formatting languages
have formatting commands which describe how a document
is formatted.
This latter approach does not enforce consistency and
often restricts a document to one particular style.
:li.GML is used to describe the components
of a document.
These component descriptions do not contain specifications
about the appearance of the components once they have
been formatted.
Thus, the specification file is essentially
independent of the layout used.
A :hp2.layout:ehp2. is a set
:I1.layouts
:I2.definition of
of rules to determine the way in which a document is to be formatted.
Examples of these rules are the number of lines to be displayed
on a page, and the style and placement of page numbers on each page.
A default layout is supplied with the GML processor; optionally, a
GML user may create additional layouts for situations in which
the default is not satisfactory.
A common method of document preparation is to use one layout to
create a completed version of a document and then to use a
specialized layout in the final production run of the document.
:li.The GML language has been found to be easy to learn.
A user of the language seldom becomes involved in details concerning
layout specifications since there often exists a layout
suitable for use with a given document.
:eol
:p.The file(s) containing the GML specifications for a
document are usually prepared and revised using a text editor.
This manual does not describe the use of a specific editor.
