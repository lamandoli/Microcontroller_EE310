MYREG EQU   0x08         ; use location 08 as counter
            ORG     0
BACK       MOVLW   0x55   ; load WREG with 55H
            MOVWF   PORTB  ; send 55H to port B
            CALL    DELAY  ; time delay
            MOVLW   0xAA   ; load WREG with AA (in hex)
            MOVWF   PORTB  ; send AAH to port B
            CALL    DELAY  ; time delay
            GOTO    BACK   ; keep doing this indefinitely
; -------- this is the delay subroutine --------
            ORG     300H   ; put time delay at address 300H
DELAY      MOVLW   0xFF   ; WREG = 255, the counter
            MOVWF   MYREG
AGAIN      NOP             ; no operation wastes clock cycles
            NOP
            DECF    MYREG, F
            BNZ     AGAIN   ; repeat until MYREG becomes 0
            RETURN         ; return to caller
            END           ; end of asm file
