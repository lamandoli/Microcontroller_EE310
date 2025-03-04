;Lab 3
;Part A
	ORG 	   0x00 ; Directive, not executable command
	MOVLW	   0x0
	MOVFF      W,STATUS
	MOvLW      0X88
	ADDLW      0X88 ; 88 is in the working register. When we
			; add 88(b1000 1000) to 88(b1000 1000), we would
			; get b 1 0001 0000.
	                ; But we only have 8 bits so in the working register
	                ; we get 0x10 (b0001 0000) status register we get
	                ; 01011 which is an overflow,
	                ; a digit carry, and a carry/borrow.
	; W Reg = 0x10, Status Reg = 01011


	MOVLW      0X0
	MOVFF      W,STATUS
	MOVLW      0X80
	SUBLW      0X1 ; SUBLW means L-W-->W. 1-80 = -79. The status reg
		       ; show a negative, over-flow, and digit carry.
	; W Reg = 0x81, Status Reg = 11010


;Part B
	MOVLW	0x0
	MOVFF   W,STATUS
;LINE 1
	MOVLW	0X88
	ADDLW	0X88
	MOVWF	0X30
	MOVFF	STATUS, 0X20 ; STATUS OV, DC, C
			     ; 0X88+0X88 = 1000 1000 + 1000 1000 = 01 0001 0000
;RESET
	MOVLW	0x0
	MOVFF   W,STATUS
;LINE 2
	MOVLW	0X7F
	ADDLW	0X7F
	MOVWF	0X31
	MOVFF	STATUS, 0X21 ;STATUS N, OV, DC
			     ;0X7F + 0X7F = 0111 1111 + 0111 1111 = 0 1111 1110
;RESET
	MOVLW	0x0
	MOVFF   W,STATUS
;LINE 3
	MOVLW	0X50
	ADDLW	0XF0
	MOVWF	0X32
	MOVFF	STATUS, 0X22 ; STATUS C
			     ; 0X50 + 0XF0 = 0101 0000 + 1111 0000 = 01 0100 0000
;RESET
	MOVLW	0x0
	MOVFF   W,STATUS
;LINE 4
	MOVLW	0X50
	SUBLW	0XF0
	MOVWF	0X33
	MOVFF	STATUS, 0X23 ; STATUS N,DC,C
			     ; 0XF0 - 0X50 = 1111 0000 - 0101 0000 = 0 1010 0000 (160d)
;RESET
	MOVLW	0x0
	MOVFF   W,STATUS
;LINE 5
	MOVLW	0XF0
	SUBLW	0X50
	MOVWF	0X34
	MOVFF	STATUS, 0X24 ;STATUS DC
			     ;0X50 - 0XF0 = 0101 0000 - 1111 0000 = 1010 0000 (-160d)
;RESET
	MOVLW	0x0
	MOVFF   W,STATUS
;LINE 6
	MOVLW	0XFF
	SUBLW	0X1
	MOVWF	0X35
	MOVFF	STATUS, 0X25 ;STATUS NONE
	                     ;0X1-0XFF= 0000 0001 - 1111 1111 =  0000 0010
	                     ;(Two’s complement representation of -254)

;PART C
REG1    EQU      0x1

        ; ORG  0x20
        MOVLW   0x60    ; put 0x60 into WREG
        ADDLW   0x2	; add 0x2 to 0x60 in WREG
        ADDWF   0x1,1   ; add WREG and f and put results in register 0x01 because ,1
        MOVLW   0x5	; put 0x5 into WREG
        ADDWF   0x1,0   ; add WREG and f and keep in WREG becasue ,0

        MOVFF   0x1,0x2	; move contents of f 0x1 to f 0x2
        INCF    0x1,F	; increment value of fREG 0x1(location) by 1 (62-->63)
        DECF    0x2,F	; decrement value of fREG 0x2(location) by 1 (62-->61)

        MOVLW   0xFF 	; put 0xFF into WREG
        MOVWF   0x4	; move 0xFF into fREG 0x4(location)
        CLRF    0x4	; clear fREG 0x4(location)
        SETF    0x3	; set fREG 0x3(location) to 0xFF

        MOVLW   0x1	; put 0x1 into WREG
        MOVWF   0x5	; move contents of WREG to fREG 0x5(location)
        NEGF    0x5     ; TWO'S COMPLEMENT of 0x1 is oxFF

        MOVLW   0x1	; put 0x1 into WREG
        MOVWF   0x6	; move contents of WREG to fREG 0x6(location)
        SWAPF   0x6,1   ; SWAP nibble in fREG 0x6(location). It was 0x01, now it is 0x10

        MOVLW   0x1	; put 0x1 into WREG
        MOVWF   0x7	; move contents of WREG to fREG 0x7(location)
        COMF    0x7,1   ; 1'S COMPLEMENT of 0x1 is 0xFE. In binary 0000 0001-->1's comp.
			; ---> 1111 1110-->convert to hex-->0xFE
