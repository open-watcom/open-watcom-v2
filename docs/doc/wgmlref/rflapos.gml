:DT.post_skip
:I2 refid='lapost_'.lapost_
:DD.This attribute accepts vertical space units.
    A zero value means that no lines are skipped.
    If the skip value is a line unit,
    it is multiplied by the current line spacing
    (see :HDREF refid='specver'. for more information).
    The resulting amount of space is skipped after the
    &local.
The post-skip will be merged with the next document entity's
pre-skip value.
If a post-skip occurs at the end of an output page, any
remaining part of the skip is not carried over to the next
output page.
