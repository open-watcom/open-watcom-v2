/^:MSGTXT/!d
/^:MSGTXT. filler/d
/^:MSGTXT/s/^:MSGTXT. //
/function's/s/function's/functions/g
s/'[^']*%S'/''/g
s/'[^']*%N'/''/g
s/'[^']*%T'/''/g
s/'[^']*%s'/''/g
/%/d
/'/s/'[^']*'/''/g
