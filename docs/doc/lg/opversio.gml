.*
.*
.option VERSION
.*
.np
The "VERSION" option can be used to identify the application so that
it can be distinguished from other versions (releases) of the same
application.
.np
This option is most useful when creating a DLL or NLM since
applications that use the DLL or NLM may only execute with a specific
version of the DLL or NLM.
.np
The format of the "VERSION" option (short form "VERS") is as follows.
.mbigbox
OS/2, Win16, Win32:
    OPTION VERSION=major[.minor]
Netware:
    OPTION VERSION=major[.minor[.revision]]
.embigbox
.synote
.mnote major
specifies the major version number.
.mnote minor
specifies the minor version number and must be less than 100.
.mnote revision
specifies the revision.
The revision should be a number or a letter.
If it is a number, it must be less than 27.
.esynote
