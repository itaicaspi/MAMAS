	BR start
arg1:	.WORD	2
arg2:	.WORD	5
result:	.WORD	0
start:	LD	R1,arg1
		LD	R2,arg2
		ADD	R3,R1,#5
		ADD	R3,R1,R2
		ADD	R2,R3,R3
		ADD	R2,R1,R1
done:	BR 	done
