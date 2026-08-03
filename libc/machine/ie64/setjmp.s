.section .text,"ax"

.global setjmp
.type setjmp,@function
setjmp:
    store.q r18, 0(r1)
    store.q r19, 8(r1)
    store.q r20, 16(r1)
    store.q r21, 24(r1)
    store.q r22, 32(r1)
    store.q r23, 40(r1)
    store.q r24, 48(r1)
    store.q r25, 56(r1)
    fstore f8, 64(r1)
    fstore f9, 72(r1)
    fstore f10, 80(r1)
    fstore f11, 88(r1)
    fstore f12, 96(r1)
    fstore f13, 104(r1)
    fstore f14, 112(r1)
    fstore f15, 120(r1)
    store.q r31, 128(r1)
    load.q r2, (r31)
    store.q r2, 136(r1)
    li r1, #0
    rts
.size setjmp,.-setjmp

.global longjmp
.type longjmp,@function
longjmp:
    move.q r3, r1
    bne r2, r0, .Llongjmp_value
    li r2, #1
.Llongjmp_value:
    load.q r18, 0(r3)
    load.q r19, 8(r3)
    load.q r20, 16(r3)
    load.q r21, 24(r3)
    load.q r22, 32(r3)
    load.q r23, 40(r3)
    load.q r24, 48(r3)
    load.q r25, 56(r3)
    fload f8, 64(r3)
    fload f9, 72(r3)
    fload f10, 80(r3)
    fload f11, 88(r3)
    fload f12, 96(r3)
    fload f13, 104(r3)
    fload f14, 112(r3)
    fload f15, 120(r3)
    load.q r31, 128(r3)
    load.q r4, 136(r3)
    store.q r4, (r31)
    move.l r1, r2
    rts
.size longjmp,.-longjmp
