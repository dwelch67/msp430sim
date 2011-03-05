.global LFC00
LFC00:
        mov #0x4000,r1
        mov #0x0001,r14
LFC06:
        mov &0xFC24,r15
LFC0A:
        swpb r15
        swpb r15
        push r14
        mov @r1+,r14
        sub #0x0001,r15
        jnz LFC0A ; pc+0xFFF4
        sub #0x0001,r14
        jnz LFC06 ; pc+0xFFEC
        mov #0x0031,r15
        mov.b r15,&0x0077
        .word 0x0FFF ; halt
LFC24: .word 0x0123
.global LFC26
LFC26:
        jmp LFC26 ; pc+0xFFFE
