.*
.*
.option CUSTOM
.*
.np
The format of the "CUSTOM" option (short form "CUST") is as follows.
.mbigbox
    OPTION CUSTOM=file_name
.embigbox
.synote
.mnote file_name
specifies the file name of the custom data file.
.esynote
.np
The custom data file is placed into the executable file when the
application is linked but is really not part of the program.
When the application is loaded into memory, the information extracted
from a custom data file is not loaded into memory.
Instead, information is passed to the program (as arguments) which
allows the access and processing of this information.
