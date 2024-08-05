K:		.data 22
MAIN:	mov S1.1, W
add r0,STR
S2:		.struct -1	 , 	 "12345
S3:		.struct "abc"	 , 	 12

XYZ: 	jmp W
macro m1
inc K
; This is comment!!!
    mov S1.2 ,	r3
endmacro
prn 	#-500
prn 	#500
prn 	#asf
prn 	#50

sub r1	, r4
m1
bne L3
END:	hlt
STR:	.string "abcd"saa"
LENGTH:	.data 5555,-513,asv
DATE:	.data 55,2,4 5
S1:		.struct 8, "ab"
    .entry K
    .entry XYZ
    .extern L3
    .extern W