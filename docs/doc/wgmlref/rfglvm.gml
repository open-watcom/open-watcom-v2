:H2 id='vmlib'.Libraries with &ibmvm.
:H3.Creating and Updating a Library
:I1.library
:I2.creating
:I2.updating
:P.
A definition library is maintained with the use of the
:HP2.MACLIB:eHP2. command.
A maclib is a file which contains the contents of many individual
files.
The definition files produced by the WATCOM GENDEV program are
added to and deleted from a maclib.
The file type of a maclib member file must be :HP2.COPY:eHP2..
This is the default file type produced by WATCOM GENDEV.
All maclib operations are in terms of the member name, not the
defined name of a definition.
:P.
The :HP2.MACLIB GEN:eHP2. command is used
to create a maclib.
:FIG place=inline frame=box.
MACLIB GEN gmllib wgmlst
:FIGCAP.Creating an &ibmvm. Library
:eFIG.
:PC.
The first name after the command is the name of the library to be
created.
If this library already exists, it is erased first.
The file type of the library will always be :HP2.MACLIB:eHP2..
The second name after the command is the name of the member file
to place in the library.
Once a member is in a library, the member file may be erased.
:P.
The :HP2.MACLIB DEL:eHP2. command is used to delete a member from
a library.
:FIG place=inline frame=box.
MACLIB DEL gmllib qume
:FIGCAP.Deleting an &ibmvm. Library Member
:eFIG.
:PC.
The member in the :HP1.gmllib:eHP1. maclib with the name
:HP1.qume:eHP1. is deleted.
:P.
The :HP2.MACLIB ADD:eHP2. command is used to add a member to
an existing library.
:FIG place=inline frame=box.
MACLIB ADD gmllib qume
:FIGCAP.Adding an &ibmvm. Library Member
:eFIG.
:PC.
The member file with the name :HP1.qume:eHP1. is added to
the :HP1.gmllib:eHP1. maclib.
If a member already exists in the maclib with the same name,
it is NOT deleted from the maclib first.
There would then be two versions of the same member
in the maclib.
In this case,
&WGML. would find the old version.
It is therefore important to ensure a member is first deleted from
the maclib before a new version is added.
:P.
The maclib command is more completely described by the documentation
available with the system.
:H3.Defining a Library List
:I1.library
:I2.name list
:P.
The name of a definition library can be any arbitrary file name.
To locate the library, &WGML. and WATCOM GENDEV must have
a list of library names.
This list is defined with the :HP2.GLOBAL MACLIB:eHP2. command.
:FIG place=inline frame=box.
GLOBAL MACLIB gmllib
:FIGCAP.Defining the &ibmvm. Library List
:eFIG.
:PC.
The global maclib command must be performed before either &WGML. or
WATCOM GENDEV are invoked.
If :HP1.gmllib:eHP1. is the shared library and :HP1.mylib:eHP1.
is the personal library, the following command specifies the
proper library list.
:FIG place=inline frame=box.
GLOBAL MACLIB mylib gmllib
:FIGCAP.
:eFIG.
