.*
.*
.section Librarian Error Messages
.*
.np
The following messages may be issued by the &libname..
.begnote $break
.mnote Error! Could not open object file '%s'.
Object file '%s' could not be found.
This message is usually issued when an attempt is made to add a non-existent
object file to the library.
.mnote Error! Could not open library file '%s'.
The specified library file could not be found.
This is usually issued for input library files.
For example, if you are combining two library files, the library file you are
adding is an input library file and the library file you are adding to
or creating is an output library file.
.mnote Error! Invalid object module in file '%s' not added.
The specified file contains an invalid object module.
.mnote Error! Dictionary too large. Recommend split library into two libraries.
The size of the dictionary in a library file cannot exceed 64K.
You must split the library file into two separate library files.
.mnote Error! Redefinition of module '%s' in file '%s'.
This message is usually issued when an attempt is made to add a module
to a library that already contains a module by that name.
.mnote Warning! Redefinition of symbol '%s' in file '%s' ignored.
This message is issued if a symbol defined by a module already in the library
is also defined by a module being added to the library.
.mnote Error! Library too large. Recommend split library into two libraries or try a larger page_bound than %xH.
The record size of the library file does not allow the library file to increase
beyond its current size.
The record size of the library file must be increased using the "p" option.
.mnote Error! Expected '%s' in '%s' but found '%s'.
An error occurred while scanning command input.
.mnote Warning! Could not find module '%s' for deletion.
This message is issued if an attempt is made to delete a module that does not
exist in the library.
.mnote Error! Could not find module '%s' for extraction.
This message is issued if an attempt is made to extract a module that does not
exist in the library.
.mnote Error! Could not rename old library for backup.
The &libname creates a backup copy before making any changes (unless the "b"
option is specified).
This message is issued if an error occurred while trying to rename the original
library file to the backup file name.
.mnote Warning! Could not open library '%s' : will be created.
The specified library does not exist.
It is usually issued when you are adding to a non-existent library.
The &libname will create the library.
.mnote Warning! Output library name specification ignored.
This message is issued if the library file specified by the "o" option could
not be opened.
.mnote Warning! Could not open library '%s' and no operations specified: will not be created.
This message is issued if the library file specified on the command line does
not exist and no operations were specified.
For example, asking for a listing file of a non-existent library will cause this
message to be issued.
.mnote Warning! Could not open listing file '%s'.
The listing file could not be opened.
For example, this message will be issued when a "disk full" condition is
present.
.mnote Error! Could not open output library.
The output library could not be opened.
.mnote Error! Unable to write to output library.
An error occurred while writing to the output library.
.mnote Error! Unable to write to extraction file '%s'.
This message is issued when extracting an object module from a library file
and an error occurs while writing to the output file.
.mnote Error! Out of Memory.
There was not enough memory to process the library file.
.mnote Error! Could not open file '%s'.
This message is issued if the output file for a module that is being extracted
from a library could not be opened.
.mnote Error! Library '%s' is invalid. Contents ignored.
The library file does not contain the correct header information.
.mnote Error! Library '%s' has an invalid page size. Contents ignored.
The library file has an invalid record size.
The record size is contained in the library header and must be a power of 2.
.mnote Error! Invalid object record found in file '%s'.
The specified file contains an invalid object record.
.mnote Error! No library specified on command line.
This message is issued if a library file name is not specified on the command
line.
.mnote Error! Expecting library name.
This message is issued if the location of the library file name on the command
line is incorrect.
.mnote Warning! Invalid file name '%s'.
This message is issued if an invalid file name is specified.
For example, a file name longer that 127 characters is not allowed.
.mnote Error! Could not open command file '%s'.
The specified command file could not be opened.
.mnote Error! Could not read from file '%s'. Contents ignored as command input.
An error occurred while reading a command file.
.endnote
