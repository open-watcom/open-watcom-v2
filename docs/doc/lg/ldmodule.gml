.*
.*
.dirctv MODULE
.*
.np
The "MODULE" directive is used to specify the DLLs or NLMs to be
loaded before this executable is loaded.
The format of the "MODULE" directive (short form "MODU") is as
follows.
.mbigbox
    MODULE module_name{,module_name}
.embigbox
.synote
.mnote module_name
is the file name of a DLL or NLM.
.esynote
.warn
Versions 3.0 and 3.1 of the NetWare operating system do not
support the automatic loading of modules specified in the "MODULE"
directive.
You must load them manually.
.ewarn
