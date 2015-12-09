		lw 		0	4	four
		add		4	0	5
		nand 	5	5	6
		beq		6	0	1
		sw		0	7	0
done	halt
four  .fill 6