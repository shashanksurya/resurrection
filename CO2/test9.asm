.data
	.word 1,15,3
.text
	lw $1,0($0)
	lw $2,4($0)
	lw $3,9($0)
	add $3,$3,$1
	bne $3,$2,12
	halt $0,$0,$0 

