	.file	"lnotuart.bc"
	.text
	.globl	uart_init
	.align	2
	.type	uart_init,@function
uart_init:                              ; @uart_init
; BB#0:                                 ; %entry
	ret
.Ltmp0:
	.size	uart_init, .Ltmp0-uart_init

	.globl	uart_send
	.align	2
	.type	uart_send,@function
uart_send:                              ; @uart_send
; BB#0:                                 ; %entry
	mov.b	r15, &119
	ret
.Ltmp1:
	.size	uart_send, .Ltmp1-uart_send

	.globl	hexstring
	.align	2
	.type	hexstring,@function
hexstring:                              ; @hexstring
; BB#0:                                 ; %entry
	mov.w	r15, &88
	ret
.Ltmp2:
	.size	hexstring, .Ltmp2-hexstring


