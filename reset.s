
.global reset
reset:

  mov #0x4000,r1
  mov #1,r14
  call notmain
  jmp hang

.global hang
hang:
  .word 0x0FFF ;force an undefined instruction to stop the sim
  jmp hang


.global dummy
dummy:
    ret
