:P.
When working on a PC/DOS system, the DOS environment
symbol :HP2.GMLINC:eHP2.
:I1.symbols
:I2.GMLINC
:I1.dos symbols
:I2.GMLINC
may be set with an include file list.
This symbol is defined in the same way as a library definition list
(see :HDREF refid='dpclbl'.), and provides a list of alternate
directories for file inclusion.
If an included file is not defined in the current directory, the
directories specified by the include path list are searched for the file.
If the file is still not found, the directories specified by the
DOS environment symbol :HP2.PATH:eHP2. are searched.
