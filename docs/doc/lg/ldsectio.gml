.*
.*
.dirctv SECTION
.*
.np
The "SECTION" directive is used to define the start of an overlay.
All object files in subsequent "FILE" directives, up to the next
"SECTION" or "END" directive, belong to that overlay.
The format of the "SECTION" directive (short form "S") is as follows.
.mbigbox
    SECTION [INTO ovl_file]
.embigbox
.synote
.mnote INTO
specifies that the overlay is to be placed into a separate file,
namely
.sy ovl_file.
If "INTO" (short form "IN") is not specified, the overlay is placed in
the executable file.
Note that more than one overlay can be placed in the same file by
specifying the same file name in multiple "SECTION" directives.
.mnote ovl_file
is the file specification for the name of an overlay file.
If no file extension is specified, a file extension of "ovl" is
assumed.
.esynote
.np
Placing overlays in separate files has a number of advantages.
For example, if your application was linked into one file, it may not
fit on a single diskette, making distribution of your application
difficult.
