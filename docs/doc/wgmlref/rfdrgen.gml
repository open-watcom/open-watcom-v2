:P.This
:I1.running WATCOM GENDEV
:I1.invoking WATCOM GENDEV
section describes how you invoke WATCOM GENDEV and the options
that may be specified.
:P.
WATCOM GENDEV is invoked by entering:
:XMP.
GENDEV file-name options
:EXMP.
:P.
The "file-name" specifies the file containing the
device, font and/or driver definitions.
If the file type
part of the file name
(see :HDREF refid='files'.)
is not specified,
WATCOM GENDEV searches for source files with
the default file type for device and driver definitions.
The font definition file type is the default alternate extension.
:DL compact.
:DTHD.File Type
:DDHD.Definition (&ibmvm.)
:DT.VMD
:DD.default file type for the device and driver definition.
:DT.FONT
:DD.default file type for the font definition.
:DT.COPY
:DD.default file type for the created member name.
:DTHD.File Type
:DDHD.Definition (&ibmpc.)
:DT.PCD
:DD.default file type for the device and driver definition.
:DT.FON
:DD.default file type for the font definition.
:DT.COP
:DD.default file type for the created member name.
:DTHD.File Type
:DDHD.Definition (&decvax.)
:DT.VXD
:DD.default file type for the device and driver definition.
:DT.FON
:DD.default file type for the font definition.
:DT.TXT
:DD.default file type for the created member name.
:eDL.
