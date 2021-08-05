The
.id &funcb.
function returns a value between 0 and
.arg n
.ct , inclusive,
.mono (size_t)-2
or
.mono (size_t)-1
.period
The
.id &funcb.
function returns the first of the following that applies:
.begnote
.notehd1 Value
.notehd2 Meaning
.note 0
if the next
.arg n
or fewer bytes form the multibyte character that corresponds to the
null wide character.
.note >0
if the next
.arg n
or fewer bytes form a valid multibyte character; the value returned is
the number of bytes that constitute that multibyte character.
.note (size_t)-2
if the next
.arg n
bytes form an incomplete (but potentially valid) multibyte character,
and all
.arg n
bytes have been processed; it is unspecified whether this can occur
when the value of
.arg n
is less than that of the
.kw MB_CUR_MAX
macro.
.note (size_t)-1
if an encoding error occurs (when the next
.arg n
or fewer bytes do not form a complete and valid multibyte character);
.im _mbsret1
.endnote
