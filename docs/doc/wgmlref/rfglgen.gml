:P.
When &WGML. processes a document, it must have information
about the output device and character sets being used.
The WATCOM GENDEV program is used to create and modify this information
(see :HDREF refid='crdev'.).
The definition of each device and character set is saved in
a separate file.
These definition files are grouped together in a :HP2.library:eHP2..
:I1.device library
Each of the individual files
is called a :HP2.member:eHP2. of the library.
:I1.library member
Libraries are used by &WGML. to localize the placement of
definitions needed to process a document.
:P.
Since a member in a library is created by the WATCOM GENDEV
program as a file, the length and character composition of the
member name is limited by the file name restrictions of the
computer system.
To minimize this restriction, every definition has two names
associated with it.
The :HP2.member name:eHP2. is the name of
:I1.library
:I2.member name
the file or library member which
contains the definition.
The :HP2.defined name:eHP2. is the name
:I2.defined name
used by &WGML. and WATCOM GENDEV
when referring to the definition.
A defined name can be up to 78 characters in length.
:P.
When a defined name is referenced, the member name associated with
that defined name must be known.
This is accomplished through the use of a "directory" file which
contains the defined name and its associated member name
for each definition contained in the library.
This file is named :HP2.WGMLST:eHP2., and is automatically created
:I1.wgmlst library member
when the WATCOM GENDEV program is used to process a definition.
The name WGMLST must not be used as a member name for any of the
definitions.
:P.
More than one definition library may be defined.
When there is a central place where a number of people share
data, a library containing all common definitions may be shared.
If an individual wishes to modify an existing definition
without affecting the shared library,
they may create a personal library containing their modifications.
:I1.user library
:I1.personal library
:I1.library
:I2.individual
When a definition is required, the personal library will be searched
first.
:I2.search order
If the definition is not found, then the shared library is searched.
The number of search levels can be extended to the requirements
of the people using the system.
:P.
Before &WGML. or WATCOM GENDEV is invoked, a list of the library
names must be defined.
The search order for the libraries is from the first name in the
list to the last name.
When the WATCOM GENDEV program is used, this library list should
:I1.library
:I2.name list
only contain the name of the library you wish to create or update.
In addition, WATCOM GENDEV will always try to find the WGMLST member
file on the current directory or disk before trying to find it in
a library.
The method for specifying the library list depends on the machine being
used, and is discussed further in the following subsections.
