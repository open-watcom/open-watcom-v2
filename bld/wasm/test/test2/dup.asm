.386
.model small
.data
ed  db "long string that is split to",
        " two lines."
a   dw  0x1234
fin_long db 0,1,2,3,4,5,6,7,8,9,
            1,0,0,0,0,0,0,0,
            2,0,0,0,0,0,0,0,
            3,0,0,0,0,0,0,0,
            4,0,0,0,0,0,0,0,
            5,
            0

da_end byte    0xd,0xe,0xa,0xd,0xb,0xe,0xe,0xf,0

.code
    xor ax, ax
end
