:H2.Output Devices in &WGML.
:I1 id='outdev'.output devices
:IH1 seeid='outdev'.printers
:P.
When you process a document, &WGML. must create the resulting
output for a particular output device.
The format of the output may be different among
devices, or for a device which can be set with different characteristics.
To provide support for these differences, &WGML. specifies
a device as having several components.
:P.
Special characters, or
:HP2.control sequences:eHP2.,
:I1.control sequences
are sent to a device to perform various functions.
For example, a control sequence is needed when a new output
line or a new page is started.
The definition of the control sequences required
by &WGML. for a particular device is
called a :HP2.driver:eHP2..
:I1 id='drive'.driver
:IH1 seeid='drive'.device driver
:I1.device driver
:P.
The character sets, or
:HP2.fonts:eHP2.,
:I1 id='fonts'.fonts
:I1.character sets
:I1.device fonts
that are
available for the document is another component in the definition
of a device.
Each font definition specifies information such as
the size of the characters.
Some fonts have different sized characters to
produce more attractive output.
:P.
Some of the differences in the format of the output for a device are
related to the way in which the device is set up.
The number of lines on a page, continuous forms or single sheet
feeding, and default fonts are examples of set up differences.
The combination of the font and driver definitions with the
specification of the set up values create a
:HP2.device:eHP2. definition.
:I1 id='device'.devices
:I1.device definition
