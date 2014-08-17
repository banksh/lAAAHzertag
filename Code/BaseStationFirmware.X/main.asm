    include <p10f200.inc>

    __CONFIG _WDTE_OFF & _CP_OFF & _MCLRE_ON

    org 0
    movlw b'1110'
    tris GPIO

loop:
    btfss GPIO,1
    bsf GPIO,0
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    bcf GPIO,0
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    goto loop
    end
