.*
.*
.dirctv #
.*
.np
.ix 'directives' 'comment'
.ix 'comment (#) directive'
The "#" directive is used to mark the start of a comment.
All text from the "#" character to the end of the line is considered a comment.
The format of the "#" directive is as follows.
.mbigbox
    # comment
.embigbox
.synote
.mnote comment
is any sequence of characters.
.esynote
.np
The following directive file illustrates the use of comments.
.millust begin
file main, trigtest

# Use my own version of "sin" instead of the
# library version.

file mysin
library &pc.math&pc.trig
.millust end
