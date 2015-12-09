		lw	0	1	six
		lw	1	2	5
start	add	1	2	1
		beq	0	1	1
		beq	0	0	start
done	halt
six	.fill	2
neg1	.fill	-1
staddr	.fill	start
