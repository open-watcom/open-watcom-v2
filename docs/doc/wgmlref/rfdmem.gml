:H4.MEMBER_NAME Attribute
:P.
The :HP2.member_name:eHP2.
:I1.member_name attribute
attribute specifies the member
name of the &local. definition.
The value of the member name attribute must be a
valid file name.
The member name must be unique among the member names of the
font, driver and device definitions.
When the GENDEV program processes the &local. block,
it places the &local. definition in a file with the specified
member name as the file name.
If the file extension part of the file name is not
specified, the GENDEV program will supply a default extension.
Refer to :HDREF refid='rundev'. for more information.
