:H2.Introduction
:P.
This chapter introduces the concept of
:HP1.files:eHP1. and :HP1.output devices:eHP1.
:I1.files
:I1.output devices
which are used to store and display data.
For example, a disk can be used to store a file
containing document text.
A device such as a printer can also be treated
as if it were a file, although it is only useful
for displaying data; an attempt to read information
from this device is invalid.
:P.
A software system such as the WATCOM Editor or &WGML.
can access a number of devices.
Some devices (such as disks) can be used to store
a number of files.
Other devices (such as printers or a screen) have
limited, special purpose uses.
:H2.File Specification
:P.
The general format of a file specification is as
:I1 id='filespe'.file specification
follows (items enclosed in brackets(\(\)) are
optional):
:XMP.
\((attribute\(:attribute...\))\)file-designation
:eXMP.
:PC.
There are two components of a file specification.
The attribute component describes the records which
:I2 refid='filespe'.attribute
are in the file.
The file-designation component describes
:I2 refid='filespe'.file designation
the location and name of the file.
