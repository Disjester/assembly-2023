; file ps.as
.entry LENGTH
.extern W
MAIN: mov @r3 ,LENGTH
LOOP: jmp L1
        mcro m1
            sub @r1, @r4
            bne L3
        endmcro
        prn -5
        bne W
        m1
L1:     inc K
.entry LOOP
        jmp     W
END:    stop

STR:    .string "abcdef"
LENGTH:     .data 6,-9,15
K:          .data 22
.extern L3