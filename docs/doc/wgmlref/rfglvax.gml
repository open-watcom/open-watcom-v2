:H2.Libraries with &decvax.
:H3.Creating and Updating a Library
:I1.library
:I2.creating
:I2.updating
:P.
A definition library is maintained with the use of the
VMS :HP2.LIBRARY:eHP2. command.
A library is a file which contains the contents of many individual
files.
The definition files produced by the WATCOM GENDEV program are
added to and deleted from the library.
The library member file can have an arbritrary file type.
The default file type produced by WATCOM GENDEV is :HP2.TXT:eHP2..
All library operations are in terms of the member name, not the
defined name of a definition.
:P.
The :HP2.LIBRARY/TEXT/CREATE:eHP2. command is used
to create a library.
:FIG place=inline frame=box.
LIBRARY/TEXT/CREATE gmllib.tlb wgmlst.txt
:FIGCAP.Creating a &decvax. Library
:eFIG.
:PC.
The first name after the command is the name of the library to be
created.
If this library already exists, a new version is created.
The second name after the command is the name of the member file
to place in the library.
Once a member is in a library, the member file can be erased.
:P.
The :HP2.LIBRARY/TEXT/DELETE:eHP2. command
is used to delete a member from a library.
:FIG place=inline frame=box.
LIBRARY/TEXT/DELETE=qume gmllib.tlb
:FIGCAP.Deleting a &decvax. Library Member
:eFIG.
:PC.
The member in the :HP1.gmllib:eHP1. library with the name
:HP1.qume:eHP1. is deleted.
Once a member is in the library, the file type is no longer needed.
:P.
The :HP2.LIBRARY/TEXT/INSERT:eHP2. command is used to add a member to
an existing library.
:FIG place=inline frame=box.
LIBRARY/TEXT/INSERT gmllib.tlb qume.txt
:FIGCAP.Adding a &decvax. Library Member
:eFIG.
:PC.
The member file with the name :HP1.qume:eHP1. is added to
the :HP1.gmllib:eHP1. library.
If a member already exists in the library with the same name,
the library command generates an error message.
A member must first be deleted from
the library before a new version is added.
:P.
The library command is more completely described by the documentation
available with the system.
:H3.Defining a Library List
:I1.library
:I2.name list
:P.
The name of a definition library can be any arbitrary file name.
To locate the library, &WGML. and WATCOM GENDEV must have
a list of library names.
This list is defined with the :HP2.ASSIGN:eHP2. command.
:FIG place=inline frame=box.
ASSIGN <gml>gmllib.tlb GMLLIB:
:FIGCAP.Defining the &decvax. Library List
:eFIG.
:PC.
The assign command must be performed before either &WGML. or
WATCOM GENDEV are invoked.
The assigned name :HP2.GMLLIB::eHP2. must be used.
If :HP1.gmllib:eHP1. is the shared library and :HP1.mylib:eHP1.
is the personal library, the following command specifies the
proper library list.
:FIG place=inline frame=box.
ASSIGN <dave>mylib.tlb,<gml>gmllib.tlb GMLLIB:
:FIGCAP.
:eFIG.
