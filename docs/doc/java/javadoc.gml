
:cmt -------------------------------------------------------------------
:MSGGRP. JD
:MSGGRPTXT. Messages related to JavaDoc support
:MSGJGRPTXT.
:cmt -------------------------------------------------------------------

:MSGSYM. INF_JD_BLOCK
:MSGTXT. JavaDoc comment block is located %L
:MSGJTXT.
:INFO.
This informational message indicates the start of a JavaDoc comment block.

:MSGSYM. INF_JD_LINE
:MSGTXT. JavaDoc comment line is located %L
:MSGJTXT.
:INFO.
This informational message indicates the start of a JavaDoc comment line.

:MSGSYM. WARN_JD_MISPLACED_COMMENT
:MSGTXT. incorrect JavaDoc comment placement
:MSGJTXT.
:WARNING. 4
A JavaDoc comment was found to not immediately precede the definition of
a class, interface, method, constructor or class field.
This means that the text within the comment will be ignored and will have
no effect in the JavaDoc output files.
.np
If the comment was misplaced, then reposition the comment immediately
before the appropriate definition.
.np
A Javadoc comment starts with the three characters /** and completes in the
usual way with the two characters */.

:MSGSYM. WARN_JD_NO_STAR
:MSGTXT. expecting JavaDoc comment line with '*' at start
:MSGJTXT.
:WARNING. 4
A line within a JavaDoc comment did not start with a '*'.

:MSGSYM. WARN_JD_BAD_TAG
:MSGTXT. Javadoc tag not recognizable
:MSGJTXT.
:WARNING. 4
The comment text started with '@', but the name which followed was not a
recognizable JavaDoc tag.  The valid tags are:
.np
.id @author
.np
.id @exception
.np
.id @param
.np
.id @return
.np
.id @see
.np
.id @version

:MSGSYM. WARN_JD_TAG_MISPLACED
:MSGTXT. JavaDoc tag not allowed in this context
:MSGJTXT.
:WARNING. 4
A JavaDoc tag was encountered within a comment block in which
that tag is not allowed.  Tags are allowed in comment blocks as follows:
.np
.id @author
is allowed only in comment blocks for a
.kw class
or
.kw interface
.ct .li .
.np
.id @exception
is allowed only in comment blocks for a method or constructor.
.np
.id @param
is allowed only in comment blocks for a method or constructor.
.np
.id @return
is allowed only in comment blocks for a method, unless that
method has a
.kw void
return.
.np
.id @see
is allowed in any comment block
.np
.id @version
is allowed only in comment blocks for a
.kw class
or
.kw interface
.ct .li .

:MSGSYM. WARN_JD_TAG_NAKED
:MSGTXT. JavaDoc tag not followed by text
:MSGJTXT.
:WARNING. 4
A JavaDoc tag was not followed by any text which is not allowed for most tags and
is probably not intentional in other cases.

:MSGSYM. WARN_JD_BAD_SEE_CLASS
:MSGTXT. JavaDoc tag 'see' contains invalid class reference
:MSGJTXT.
:WARNING. 4
An invalid character or character sequence was detected within a class name
specification in the indicated
.kw see
tag.

:MSGSYM. WARN_JD_BAD_SEE_NAME
:MSGTXT. JavaDoc tag 'see' contains invalid name after '#'
:MSGJTXT.
:WARNING. 4
An invalid character or character sequence was detected within the name
specification, immediately following the '#' character, in the indicated
.kw see
tag.

:MSGSYM. WARN_JD_BAD_SEE_PARAM
:MSGTXT. JavaDoc tag 'see' has invalid parameter type name for method or constructor
:MSGJTXT.
:WARNING. 4
An invalid character or character sequence was detected within the parameters
specification of the method or constructor that was referenced in the indicated
.kw see
tag.

:MSGSYM. WARN_JD_SEE_EXTRA
:MSGTXT. JavaDoc tag 'see' contains extra characters after the closing ')'
:MSGJTXT.
:WARNING. 4

:eMSGGRP. JD
