.data
	.word 1,2
.text
	lw $1,0($0)
	lw $2,4($0)
	noop $0,$0,$0
	noop $0,$0,$0
	add $3,$1,$2
	noop $0,$0,$0
	noop $0,$0,$0
	sw $3,8($0)
	halt $0,$0,$0