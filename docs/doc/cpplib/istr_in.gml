:CMT.========================================================================
:P.
.ix 'istream input'
This chapter describes formatted and unformatted input.
:CMT.========================================================================
:SECTION.Formatted Input: Extractors
.ix 'formatted input'
The
:MONO.operator~b>>
function is used to read formatted values from a stream. It is called an
:ITALICS.extractor
:PERIOD.
:I1.extractor
Characters are read and interpreted according to the type of object
being extracted.
:P.
All
:MONO.operator~b>>
functions perform the same basic sequence of operations.
First,
the input prefix function
:MONO.ipfx
is called with a parameter of zero, causing
leading whitespace characters to be discarded if
:MONO.ios::skipws
is set in &fmtflags.:PERIOD.
If the input prefix function fails and returns zero, the
:MONO.operator~b>>
function also fails and returns immediately.
If the input prefix function succeeds, characters are read from the stream
and interpreted in terms of the type of object being extracted and
&fmtflags.:PERIOD.
Finally, the input suffix function
:MONO.isfx
is called.
:P.
The
:MONO.operator~b>>
functions return a reference to the specified stream so that multiple
extractions can be done in one statement.
:P.
Errors are indicated via &iostate.:PERIOD.
&failbit. is set if the
characters read from the stream could not be interpreted for the
required type. &badbit. is set if the extraction of characters from
the stream failed in such a way as to make subsequent extractions impossible.
&eofbit. is set if the stream was located at the end when the extraction was
attempted.
:CMT.========================================================================
:SECTION.Unformatted Input
.ix 'unformatted input'
The unformatted input functions are used to read characters
from the stream without interpretation.
:P.
Like the extractors, the unformatted input functions follow a pattern.
First, they call
:MONO.ipfx
:CONT.,
the input prefix function,
with a parameter of one, causing no leading whitespace characters to
be discarded. If the input prefix function fails and returns zero,
the unformatted input function also fails and returns immediately.
If the input prefix function succeeds, characters are read from the stream
without interpretation. Finally,
:MONO.isfx
:CONT.,
the input suffix function, is called.
:P.
Errors are indicated via the iostate bits. &failbit. is set if the extraction
of characters from the stream failed. &eofbit. is set if the stream was
located at the end of input when the operation was attempted.
