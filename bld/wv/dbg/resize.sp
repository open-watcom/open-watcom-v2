* Syntax:	resize <new_size>
*
* Synopsis:	Set the memory control block containing the PSP to a new size.
*		Too small values will cause erratic operation.
*
* Example:	resize 0x1000
*
if _dbg@dbg$os!=1 {error OS must be DOS}
call/int %0:(0x21*4)(/ah 0x4a, /es _dbg@dbg$psp, /bx <1>) (void)(_dbg@sc=fl.c),(void)(_dbg@sax=ax)
if _dbg@sc {print {Error resizing block: code %d} _dbg@sax}
