.*
.*
.mnote n
represents a value.
The complete form of
.sy n
is the following.
.millust begin
[0x]d{d}[k|m]
.millust end
.pc
.sy d
represents a decimal digit.
If
.sy 0x
is specified, the string of digits represents a hexadecimal number.
If
.sy k
is specified, the value is multiplied by 1024.
If
.sy m
is specified, the value is multiplied by 1024*1024.
