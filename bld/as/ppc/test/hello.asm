; Trivial hello world program for Linux

.data                       ; section declaration - variables only

msg:
        .string "Hello, world!\n"
;       len = . - msg       ; length of string

.text                       ; section declaration - code
.globl _start

_start:
                            ; write string to stdout

        li      r0,4        ; syscall number (sys_write)
        li      r3,1        ; first argument: file descriptor (stdout)
                            ; second argument: pointer to message to write
        lis     r4,ha^msg   ; load top 16 bits of &msg
        addi    r4,r4,l^msg ; load bottom 16 bits
        li      r5,14;len   ; third argument: message length
        sc                  ; call kernel

                            ; and exit

        li      r0,1        ; syscall number (sys_exit)
        li      r3,1        ; first argument: exit code
        sc                  ; call kernel
