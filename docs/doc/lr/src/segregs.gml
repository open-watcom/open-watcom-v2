All of the segment registers must contain valid values.
Failure to do so will cause a segment violation when running
in protect mode.
If you don't care about a particular segment register, then it
can be set to 0 which will not cause a segment violation.
The function
.kw segread
can be used to initialize
.arg seg_regs
to their current values.
