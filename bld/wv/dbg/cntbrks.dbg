* Syntax:       cntbrks <address>
*
* Synopsis:     Count the number of times <address> is executed.
*
* Example:      cntbrks StageD
*
/_dbg@cnt$<0>=0
break/set <1> {
    /_dbg@cnt$<0>=_dbg@cnt$<0>+1;
    print/program {cnt$<0> = %u} _dbg@cnt$<0>;
    go/keep
}
? {The count variable is "_dbg@cnt$<0>"}
