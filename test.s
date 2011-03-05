
.global reset
reset:

  mov #0x4000,r1
  mov #1,r14
outer:
  mov &bob,r15
inner:
  swpb r15
  swpb r15
  push r14
  pop r14
  dec r15
  jnz inner
  dec r14
  jnz outer

  mov #0x31,r15
  mov.b r15,&0x77


.word 0x0FFF

bob: .word 0x123

.global hang
hang: jmp hang
