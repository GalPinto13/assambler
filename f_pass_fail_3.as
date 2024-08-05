K:		.data 22
MAIN:	mova S1.1, W
add #-4,r0
S2:		.struct -1	 , 	 "12345"

XYZ: 	jmp W
macro m1
inc K
; This is comment!!!
    mov S1.2 ,	r3, W
endmacro
prn 	#-50

sub r1
m1
bne #4
END:	hlt END
STR:	.string "abcdef"
LENGTH:	.data 6,-512,511
S1:		.struct 8, "ab"
    .entry K
    .entry XYZ
    .extern L3
    .extern W