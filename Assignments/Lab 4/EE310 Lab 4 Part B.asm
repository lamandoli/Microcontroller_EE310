;  Using BRANCH instructions write a program to blink the LED 5 times and then the program should stop.
;  Show your code to the instructor.

;  Follow the steps below:
;  Make sure you can blink the LED by itself alone without any additional delay or jump.
;  Add a function to count the number of times the LED is turned on and them stop after 5 times.
;  Add a delay function, similar to the previous example. Make sure you registers use different addresses.
;  Also when returning make sure your S bit is set properly.


MYREG	EQU 0x09              ;  use location 09 as DELAY counter
LEDREG	EQU 0X05              ;  use loction 0x05 as LED cycles counter

	ORG 0                 ;  Start at Register 0x00
	MOVLW 0X05            ;  Move literal(5) into WREG. This is for the 5 cycles.
	MOVWF 0X05            ;  Move WREG(value 5) into FREG location 0x05
BACK	MOVLW 0X00            ;  Move literal(0) into WREG
	MOVWF TRISC           ;  Move WREG into TRISC register
	MOVLW 0x01            ;  load WREG with 0X01
	MOVWF PORTC           ;  send 55H to port B
	CALL DELAY            ;  time delay
	MOVLW 0x00            ;  load WREG with 0X00
	MOVWF PORTC           ;  send AAH to port B
	CALL DELAY            ;  time delay
	DECF 0X05,1           ;  Decrement FREG location 0x05 in data memory address(hence ,1)
	BZ HERE               ;  Branch if zero. Will jump out of program when FREG location 0x05
	                      ;  reaches 0.
	GOTO BACK             ;  keep doing this indefinitely



	                      ;  -------- this is the delay subroutine --------

DELAY	MOVLW 0x10            ;  WREG = 10, the counter
	MOVWF MYREG


AGAIN	NOP                   ;  no operation wastes clock cycles
	NOP                   ;  no operation wastes clock cycles
	DECFSZ MYREG,1        ;  decrement MYREG, skip if 0
	BNZ AGAIN             ;  repeat until MYREG becomes 0
	RETURN                ;  return to caller


HERE	SLEEP
	END                   ;  end of asm file


