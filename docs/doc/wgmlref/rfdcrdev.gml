:H2 id='crdev'.Creating a Definition
:IH1.devices
:I2.creating
:P.
The WATCOM GENDEV (:HP2.GEN:eHP2.erate :HP2.DEV:eHP2.ice) program
(see :HDREF refid='rundev'.)
creates a definition for use by &WGML..
The font, driver and device definitions are each created separately.
The font and driver definitions appropriate for a particular device
are selected when the device definition is created.
:P.
A definition is first specified with a text editor.
The resulting data file is then processed by the WATCOM GENDEV program,
which produces a new file that is used by &WGML. when processing
a document.
The definition files produced by the WATCOM GENDEV program are collected
together in a definition library.
:I1.definition library
This library will contain all of the definitions that may be required
for the production of a document.
Each definition in the library
is referred to as a :HP2.member:eHP2. of the library.
Refer to :HDREF refid='rflib'. for more information.
:P.
Many computer systems limit the size of a library member name.
To minimize this restriction, every definition has two names
associated with it.
The :HP2.member name:eHP2.
:I1.member name
is the name of the library member which
contains the definition.
The :HP2.defined name:eHP2.
:I1.defined name
is the name used by &WGML. and WATCOM GENDEV
when referring to the definition.
:P.
When a defined name is referenced, the member name associated with
that defined name must be known.
This is accomplished through the use of a "directory" file which
:I1.library directory
contains the defined name and the associated member name
for each definition in the library.
This file is named :HP2.WGMLST:eHP2.,
:IH1.devices
:I2.directory
:I1.WGMLST directory
and is automatically created
when the WATCOM GENDEV program is used to process a definition.
The name WGMLST must not be used as a member name for any of the
definitions.
:P.
If the file 'device1' contains a definition, the following
example shows how the definition can be generated.
:FIG place=inline frame=box.
GENDEV device1
:FIGCAP.Generating a Definition
:eFIG.
:PC.
For more information on running the GENDEV program, refer to
:HDREF refid='rundev'..
:H2.Deleting a Definition
:IH1.devices
:I2.deleting
:P.
Specifying the :HP2.&gml.delete:eHP2. tag in the input for the GENDEV
program will delete a definition.
The WGMLST directory file is re-created with the entry for the
specified definition name removed.
The newly created WGMLST file must be updated to the definition library,
and the definition member associated with the specified definition
name removed.
:FIG place=inline frame=box.
&gml.DELETE
   defined_name = 'character string'.
:FIGCAP.Deleting a Definition
:eFIG.
:FIG place=inline frame=box.
&gml.DELETE
   defined_name = 'epson'.
:FIGCAP.Example of a Definition Deletion
:eFIG.
