:H2 id='incltag'.INCLUDE
:xmp
Format&gml. &gml.INCLUDE file='file name'.
:exmp
:I2 refid='gtinclu' pg=major.gtinclu
:p.The value of the required attribute :hp2.file:ehp2.
:I2 refid='gafile'.gafile
is used as the name of the file to include.
The content of the included file is processed by &WGML. as if
the data was in the original file.
This tag provides the means whereby a document may be specified using
a collection of separate files.
Entering the source text into separate files, such as one file
for each chapter, may help in managing the document.
:P.
If the specified file does not have a file type, the default document
file type is used.
For example, if the main document file is &mono.manual.doc&emono.,
&mono.doc&emono. is the default document file type.
If the file is not found, the alternate extension supplied on the command
line is used.
If the file is still not found, the file type &mono.GML&emono. is used.
:INCLUDE file='rfpcincl'.
