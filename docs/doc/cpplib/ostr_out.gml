:CMT.========================================================================
:P.
.ix 'ostream output'
This chapter describes formatted and unformatted output.
:CMT.========================================================================
:SECTION.Formatted Output: Inserters
.ix 'formatted output'
The
:MONO.operator~b<<
function is used to write formatted values to a stream.
It is called an
:ITALICS.inserter
:PERIOD.
:I1.inserter
Values are formatted and written according to the type of object
being inserted and &fmtflags.:PERIOD.
:P.
All
:MONO.operator~b<<
functions perform the same basic sequence of operations.
First,
the output prefix function
:MONO.opfx
is called.  If it fails and returns zero, the
:MONO.operator~b<<
function also fails and returns immediately.
If the output prefix function succeeds, the object is formatted according
to its type and &fmtflags.:PERIOD.
The formatted sequence of characters is
then written to the specified stream.  Finally, the output suffix function
:MONO.osfx
is called.
:P.
The
:MONO.operator~b<<
functions return a reference to the specified stream so that multiple
insertions can be done in one statement.
:P.
For details on the interpretation of &fmtflags., see the &fmtflags.
section of the Library Functions and Types Chapter.
:P.
Errors are indicated via &iostate.:PERIOD.
&failbit. is set if the
:MONO.operator~b<<
function fails while writing the characters to the stream.
:CMT.========================================================================
:SECTION.Unformatted Output
.ix 'unformatted output'
The unformatted output functions are used to write characters
to the stream without conversion.
:P.
Like the inserters, the unformatted output functions follow a pattern.
First, they call the output prefix function
:MONO.opfx
and fail if it fails.
Then the characters are written without conversion.  Finally,
the output suffix function
:MONO.osfx
is called.
:P.
Errors are indicated via &iostate.:PERIOD.
&failbit. is set if the function fails while writing the characters to
the stream.
