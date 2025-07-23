.data
x:	.word 0
y:	.word 0

.text
.globl main

	jump main

gcd:
    move r0, r5
    loadi r1, 0
    seq r2, r0, r1
    beq r2, r63, L0
    move r4, r4
    move r2, r4
    jump L1
L0:
    move r5, r5
    move r6, r4
    move r7, r4
    move r8, r5
    div r9, r7, r8
    move r10, r5
    mult r11, r9, r10
    sub r12, r6, r11
    # --- Tail-Call Optimization --- 
    move r4, r5
    move r5, r12
    jump gcd
L1:
    jr r62

main:
    in r14
    store r14, x
    in r15
    store r15, y
    load r16, x
    load r17, y
    move r4, r16
    move r5, r17
    jal gcd
    move r18, r2
    out r18
    jump exit_program


exit_program:
    hlt
