        .386
        .model flat

        .data

externdef var1:dword
        public var1
var1    dd 0

var2    dd 0

var3    dd 0

        public var2
externdef var2:dword

externdef var3:dword

        .code
start:
        ret

        END start
