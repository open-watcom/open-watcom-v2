:H2 id='pcldef'.Libraries with &ibmpc.
:H3.Creating and Updating a Library
:I1.library
:I2.creating
:I2.updating
:P.
A definition library is a specific directory on a device.
Creating a directory on a disk and placing the member files
in it is accomplished with standard &ibmpc. file commands.
:H3 id='dpclbl'.Defining a Library List
:I1.library
:I2.name list
:P.
To locate the library, &WGML. and WATCOM GENDEV must have
a list of library directories.
This list is defined with the DOS :HP2.SET:eHP2. command.
:FIG place=inline frame=box.
SET GMLLIB=A:\\wgmlib\\
:FIGCAP.Defining the &ibmpc. Library List
:eFIG.
:PC.
The set command must be performed before either &WGML. or
WATCOM GENDEV are invoked.
The name :HP2.GMLLIB:eHP2.
:I1.symbols
:I2.GMLLIB
:I1.dos symbols
:I2.GMLLIB
must be used, and there should be no
blanks between it and the equals sign.
If :HP1.wgmlib:eHP1. is the shared library directory and :HP1.mylib:eHP1.
is the personal library directory, the following command specifies the
proper library list.
:FIG place=inline frame=box.
SET GMLLIB=A:\\mylib\\;A:\\wgmlib\\
:FIGCAP.
:eFIG.
:PC.
Each library directory is separated from the next by a semi-colon.
