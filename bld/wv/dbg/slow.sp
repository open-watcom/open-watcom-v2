* Syntax:	slow <delay>
*
* Synopsis:	Perform slow motion execution of a program.
*
* Example:	slow 2
*
set lang C
while 1 {
    if *((char *)ip) == 0xcd {t/o} {t/i};
    d;
    /dbg$x=0;while dbg$x<<1> {/dbg$x=dbg$x+1}
}
