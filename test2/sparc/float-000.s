.global main
.global Cmm.global_area
.global Cmm.globalsig.aQOYZWMPACZAJaMABGMOZeCCPY
.section ".data"
! memory for global registers
Cmm.globalsig.aQOYZWMPACZAJaMABGMOZeCCPY:
Cmm.global_area:
.section ".data"
a:
.skip 20
b:
.byte 64
.byte 9
.byte 33
.byte -54
.byte -64
.byte -125
.byte 18
.byte 113
fmt:
.byte 112
.byte 105
.byte 32
.byte 119
.byte 105
.byte 116
.byte 104
.byte 32
.byte 50
.byte 32
.byte 117
.byte 108
.byte 112
.byte 115
.byte 32
.byte 105
.byte 115
.byte 32
.byte 37
.byte 49
.byte 56
.byte 103
.byte 10
.byte 0
.section ".text"
main:
	save %sp, -112, %sp
	mov %i0, %g1
	mov %i1, %g1
	st %i7, [%sp+100]
	st %i7, [%sp+96]
Linitialize_continuations_l5:
Lproc_body_start_l4:
	set fmt, %g1
	mov %g1, %o0
	set b, %g1
	set 4, %g2
	add %g1, %g2, %g1
	ld [%g1], %g1
	mov %g1, %o2
	set b, %g1
	ld [%g1], %g1
	mov %g1, %o1
	call printf, 0
	nop
Ljoin_l9:
	set 0, %g1
	mov %g1, %i0
	ld [%sp+100], %i7
	ld [%sp+96], %i7
	! Evil recognizer deleted add %sp, 112, %sp
	ret
	restore
.section ".pcmap_data"
Lstackdata_l17:
.byte 0
.byte 0
.byte 0
.byte 0
.section ".pcmap"
.word Ljoin_l9
.word Lframe_l18
.section ".pcmap_data"
Lframe_l18:
.byte -1
.byte -1
.byte -1
.byte -20
.byte -128
.byte 0
.byte 0
.byte 92
.byte -1
.byte -1
.byte -1
.byte -12
.word Lstackdata_l17
.byte 0
.byte 0
.byte 0
.byte 2
.byte 0
.byte 0
.byte 0
.byte 2
.byte 0
.byte 0
.byte 0
.byte 0
.byte 0
.byte 0
.byte 0
.byte 1
.byte 64
.byte 0
.byte 0
.byte 49
.byte -1
.byte -1
.byte -1
.byte -16
.byte 64
.byte 0
.byte 0
.byte 19
.byte 64
.byte 0
.byte 0
.byte 19
.byte 0
.byte 0
.byte 0
.byte 0
.section ".text"