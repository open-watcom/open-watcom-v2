.np
This section lists the various warning and error messages that may
be issued by the &makname..
In the messages below, %? character sequences indicate places in the
message that are replaced with some other string.
.*
.dm makenote begin
.   .se *ctxn=&'substr(&*,1,&'pos(' ',&*)-1)
.   .se *ttl$=&'substr(&*,&'pos(' ',&*)+1)
.   .note &*ctxn &*ttl$
.*  .us &*ttl$
.dm makenote end
.*
.begnote
.*
.makenote 1 Out of memory
.makenote 2 Make execution terminated
.makenote 3 Option %c%c invalid
.makenote 4 %c%c must be followed by a filename
.makenote 5 No targets specified
.makenote 6 Ignoring first target in MAKEINIT
.makenote 7 Expecting a %M
.makenote 8 Invalid macro name %E
.makenote 9 Ignoring out of place %M
.makenote 10 Macros nested too deep
.makenote 11 Unknown internal command
.makenote 12 Program name is too long
.makenote 13 No control characters allowed in options
.makenote 14 Cannot execute %E: %Z
.makenote 15 Syntax error in %s command
.makenote 16 Nested %s loops not allowed
.makenote 17 Token too long, maximum size is %d chars
.makenote 18 Unrecognized or out of place character '%C'
.makenote 19 Target %E already declared %M
.makenote 20 Command list does not belong to any target
.makenote 21 Extension(s) %E not defined
.makenote 22 No existing file matches %E
.makenote 23 Extensions reversed in implicit rule
.makenote 24 More than one command list found for %E
.makenote 25 Extension %E declared more than once
.makenote 26 Unknown preprocessor directive: %s
.makenote 27 Macro %E is undefined
.makenote 28 !If statements nested too deep
.makenote 29 !%s has no matching !if
.makenote 30 Skipping !%1 block after !%2
.makenote 31 %1 not allowed after !%2
.makenote 32 Opening file %E: %Z
.*
.makenote 34 !%s pending at end of file
.makenote 35 Trying to !%s an undefined macro
.makenote 36 Illegal attempt to update special target %E
.makenote 37 Target %E is defined recursively
.makenote 38 %E does not exist and cannot be made from existing files
.makenote 39 Target %E not mentioned in any makefile
.makenote 40 Could not touch %E
.makenote 41 No %s commands for making %E
.makenote 42 Last command making (%L) returned a bad status
.makenote 43 Deleting %E: %Z
.makenote 44 %s command returned a bad status
.makenote 45 Maximum string length exceeded
.makenote 46 Illegal character value %xH in file
.makenote 47 Assuming target(s) are .%s
.makenote 48 Maximum %%make depth exceeded
.makenote 49 Opening (%s) for write: %Z
.makenote 50 Unable to write: %Z
.makenote 51 CD'ing to %E: %Z
.makenote 52 Changing to drive %C:
.makenote 53 DOS memory inconsistency detected! System may halt ...
.makenote 53 OS corruption detected
.makenote 54 While reading (%s): %Z
.*
.*
.*
.*
.makenote 59 !IF Parse Error
.makenote 60 TMP Path/File Too Long
.makenote 61 Unexpected End of File
.makenote 62 Only NO(KEEP) allowed here
.makenote 63 Non-matching "
.makenote 64 Invalid String Macro Substitution
.makenote 65 File Name Length Exceeded
.makenote 66 Redefinition of .DEFAULT Command List
.makenote 67 Non-matching { In Implicit Rule
.makenote 68 Invalid Implicit Rule Definition
.makenote 69 Path Too Long
.makenote 70 Cannot Load/Unload DLL %E
.makenote 71 Initialization of DLL %E returned a bad status
.makenote 72 DLL %E returned a bad status
.makenote 73 Illegal Character %C in macro name
.makenote 74 in closing file %E
.makenote 75 in opening file %E
.makenote 76 in writing file %E
.makenote 77 User Break Encountered
.makenote 78 Error in Memory Tracking Encountered
.makenote 79 Makefile may be Microsoft; try /ms switch
.*
.endnote
