* Syntax:       fill <address> <value> <length>
*
* Synopsis:     fill a region of memory with a value
*
* Example:      fill es:0 ff 100
*
do dbg$fill=<3>
modify/byte <1>,,
while (dbg$fill!=0) {
    modify/byte ,<2>,,;
    /--dbg$fill
}
