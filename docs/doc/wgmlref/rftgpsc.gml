:H2.PSC
:xmp
Format&gml. &gml.PSC \(proc='character string'\).
:exmp
:I1.process specific control
:I2 refid='gtpsc' pg=major.gtpsc
:p.This tag allows you to specify process-specific controls in
your document.
The :hp2.&gml.psc:ehp2. tag may be used anywhere in the document,
and is terminated by the :hp2.&gml.epsc:ehp2. tag.
:p.
The :hp2.proc:ehp2.
:I2 refid='gaproc'.gaproc
attribute determines when the text in
the psc block will be processed.
If the proc attribute is not specified, the text in the psc
block will always be processed.
When the proc attribute is specified, the attribute value is
a character string composed of device names separated by
blanks.
If the device being used to format the document
matches one of the specified
names in the list, the process control block is processed.
In addition to the device names, one other process name may
be specified in the proc list.
This name is checked against the name set
by the :hp2.process:ehp2. command line option.
