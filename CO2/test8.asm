.data
	.word 1,2,3,4,5
.text
	lw $6,16($0)
	lw $5,0($0)   
	sub $6,$6,$5  
	bne $6,$5,8    
	halt $0,$0,$0