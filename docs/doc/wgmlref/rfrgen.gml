:P.This chapter describes how you invoke &WGML. and the options
:I1.running &WGML.
:I1.invoking &WGML.
that may be specified.
The subsections provide information which is specific
to each of the systems supported by &WGML., followed
by a description of the available options.
:P.
&WGML. is invoked by entering:
:XMP.
WGML file-name options
:EXMP.
:P.
The "file-name" specifies the file containing the
source text and GML tags for the document.
If the file type part of the file name
(see :HDREF refid='files'.)
is not specified,
&WGML. searches for source files with the
alternate file extension followed by the file type
of :HP1.GML:eHP1..
When a file type is specified, &WGML. searches for source
files with that file type.
:P.
It is possible that many command line options
will be necessary to process a document.
The :HP1.command file:eHP1.
:I2 refid='grcmdf'.command file
support provided by &WGML.
allows you to place these options in a file.
The options in the command file are processed
by specifying the :HP1.file:eHP1. option
on the &WGML. command line.
:P.
The option file "default" is located and loaded before other
options are processed.
The search path for the default option file is the current disk
location, the device library path, followed by the document include path.
:NOTE.The :HP1.device:eHP1. option must always be specified.
