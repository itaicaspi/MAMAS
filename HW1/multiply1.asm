	BR start
arg1:	.WORD	2
arg2:	.WORD	5
result:	.WORD	0
start:	LD	R1,arg1
	LD	R2,arg2
	ADD R2,R2,R1
	ST	R2,result
done:	BR 	done
