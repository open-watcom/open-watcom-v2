/^\*\*/d
/^0000/d
/^Warning(1028)/d
/^file.*: undefined symbol/d
/^Current usage:/d
/^[0-9]* cache hits/d
/^File.*\\testin\\.*\.c/s/^.*\\testin\\\([0-9a-zA-Z_]*\.c\)/\1/
/declared in:/s/in:.*\\testin\\\([0-9a-zA-Z_]*\.c([0-9]*,[0-9]*)\)/in: \1/
s/[ENW][0-9]*: //
