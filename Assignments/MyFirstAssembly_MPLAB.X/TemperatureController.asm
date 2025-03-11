//----------------------------- 
// Title: TemperatureController 
//----------------------------- 
// Purpose: Automatically measures and adjusts temperature 
// Dependencies: "MyConfigFile.inc" 
// Compiler: MPLAB X IDE v6.20 Pic-as (v3.0) 
// Author: Lee Amandoli 
// OUTPUTS: Turns on heating(PORTD.1) or cooling(PORTD.2)system and 
//          corresponding LED
// INPUTS: Reference temperature and measured temperature  
// Versions: 
//   v6.20: 3/8/2025 - First version  
//-----------------------------
    
;----------------
; FILE DIRECTIVES
;----------------
#include "MyConfigFile.inc"
#include <xc.inc>
    
;----------------
; PROGRAM INPUTS
;----------------
;The DEFINE directive is used to create macros or symbolic names for values.
;It is more flexible and can be used to define complex expressions or sequences 
;of instructions.
;It is processed by the preprocessor before the assembly begins.
#define  measuredTempInput 	-2 ; this is the input value
#define  refTempInput 		10 ; this is the input value

;---------------------
; Definitions
;---------------------
#define SWITCH    LATD,2  
#define LED0      PORTD,0
#define LED1	  PORTD,1
#define contReg   0x22 ;
#define count     0x31
#define number    0x32
#define sign      0x33
    
;---------------------
; Program Constants
;---------------------
; The EQU (Equals) directive is used to assign a constant value to a symbolic name or label.
; It is simpler and is typically used for straightforward assignments.
;It directly substitutes the defined value into the code during the assembly process.
REG10   equ     10h   // in HEX
REG11   equ     11h
REG01   equ     1h
   
;---------------------
;     MAIN CODE
;---------------------
    PSECT absdata,abs,ovrld  ; Do not change
    
    ORG          0           ;Reset vector
    GOTO        _start1

    ORG          0020H       ; Begin assembly at 0020H
    
_start1:
    CLRF    TRISD             ; Set PORTD as output (all bits = 0)
    MOVLW   0                 ; Move literal to WREG
    MOVWF   TRISD             ; Initialize TRISD, move WREG to TRISD
    BRA     HEXtoBCD
_start2:
    MOVLW   refTempInput      ; Move literal(refTempInput) to WREG
    MOVWF   0X20              ; Move literal(refTempInput) to location 0x20
    MOVLW   measuredTempInput ; Move literal(measuredTempInput) to WREG
    MOVWF   0X21              ; Move literal(measuredTempInput) to location 0x21
    MOVF    0x21, W           ; Load measuredTemp from register 0x21 into WREG

;;--------------------------------------------------
;; Check sign of measuredTempInput (see if negative)
;;--------------------------------------------------
;CLRF sign      ; Clear sign flag (default = positive)
;BTFSS 0x21, 7        ; Check if `measuredTempInput` is negative (bit 7 = 1)
;GOTO  HEXtoBCD   ; If measuredTempInput is positive, skip sign check
;; If measuredInputTemp is negative:
;MOVLW  0x01
;MOVWF  sign     ; Set the sign flag to 1 because measuredTempInput is negative
;COMF   0x21, W  ; Take 1's complement
;ADDLW  0x01     ; Add 1 to turn into 2's complement
;MOVWF  0x21
    
;If measuredTemp=refTemp then 
;set contReg=0 
;GOTO LED_OFF 
    CPFSEQ  0x20        ; Compare WREG with refTemp (Skip next line if Equal)
    GOTO    _SKIP1      ; If not equal, skip setting contReg, goto SKIP1
    CLRF    contReg     ; If equal, set contReg = 0
    GOTO    _LED_OFF    ; Jump to LED_OFF
    
;If measuredTemp>refTemp then 
;set contReg=2 
;GOTO LED_HOT 
    _SKIP1:
    MOVF    0x21, W      ; Load measuredTemp into WREG
    BTFSC   0X21, 7 ;
    GOTO    _LED_HOT
    CPFSGT  0x20         ; Compare WREG with refTemp
    GOTO    _SKIP2       ; If not greater than, skip setting contReg, goto SKIP2
    MOVLW   2            ; Load WREG with 2
    MOVWF   contReg      ; If greater than, set contReg = 2
    GOTO    _LED_HOT     ; Jump to LED_HOT
    
;ELSE
;set contReg=1 
;GOTO LED_COOL
    _SKIP2:
    MOVLW   1           ; Load WREG with 1
    MOVWF   contReg     ; If less than, set contReg = 1
    GOTO    _LED_COOL   ; Jump to LED_COOL

_LED_HOT:
    MOVLW 1
    MOVWF contReg
    MOVLW 2
    MOVWF PORTD
    GOTO _END_LOOP      ; Stay in an infinite loop 

_LED_COOL:
    MOVLW 2
    MOVWF contReg
    MOVLW 4
    MOVWF PORTD
    GOTO _END_LOOP      ; Stay in an infinite loop 

_LED_OFF:
    MOVLW 0
    MOVWF contReg
    MOVLW 0
    MOVWF PORTD
    GOTO _END_LOOP      ; Stay in an infinite loop 

_END_LOOP:
    NOP
    GOTO _END_LOOP

;---------------------------------------
; HEX to BCD conversion for refTempInput
;--------------------------------------- 
HEXtoBCD:
    CLRF   count          ; Clear counter 
    MOVLW  refTempInput   ; WREG = refTempInput
    MOVWF  number         ; store numerator in number
    BTFSC  number,7       ; Bit check bit 7, if clear skip next line
    NEGF   number         ; If negative, take 2's comp
    MOVLW  100            ; Sets up WREG for Loop100s conversion
			  ; Converts 100s place of refTempInput to BCD
Loop100sRef:
    INCF  count, F        ; increment quotient for every 10 subtraction
    SUBWF number, F       ; subtract 10 (F = F - W)
    BC    Loop100sRef     ; keep doing it until Carry = 0
    DECF  count, F        ; once too many
    ADDWF number, F       ; add 10 back to get remainder
    MOVFF count,0x62      ; Places 100's place value into 0x62
    CLRF  count           ; Clear counter
    MOVLW 10              ; Sets up WREG for Loop10s conversion
                          ; Converts 10s place of refTempInput to BCD
Loop10sRef:
    INCF  count, F        ; increment quotient for every 10 subtraction
    SUBWF number, F       ; subtract 10 (F = F - W)
    BC    Loop10sRef      ; keep doing it until Carry = 0
    DECF  count, F        ; once too many
    ADDWF number, F       ; add 10 back to get remainder
    MOVFF count,0x61      ; Places 10's place value into 0x61
    MOVFF number, 0x60    ; Places 1's place value into 0x60
                          ; Converts 10s place of refTempInput to BCD
;--------------------------------------------
; HEX to BCD conversion for measuredTempInput
;--------------------------------------------  
    CLRF   count              ; Clear counter 
    MOVLW  measuredTempInput  ; WREG = measuredTempInput
    MOVWF  number             ; store numerator in number
    BTFSC  number,7           ; Bit check bit 7, if clear skip next line
    NEGF   number             ; If negative, take 2's comp
    MOVLW  100                ; Sets up WREG for Loop100s conversion
			      ; Converts 100s place of measuredTempInput to BCD
			      
Loop100sMeas:
    INCF  count, F        ; increment quotient for every 10 subtraction
    SUBWF number, F       ; subtract 10 (F = F - W)
    BC    Loop100sMeas    ; keep doing it until Carry = 0
    DECF  count, F        ; once too many
    ADDWF number, F       ; add 10 back to get remainder
    MOVFF count,0x72      ; Places 100's place value into 0x72
    MOVLW 10              ; Sets up WREG for Loop10s conversion
                          ; Converts 10s place of measuredTempInput to BCD
    
Loop10sMeas:
    INCF  count, F        ; increment quotient for every 10 subtraction
    SUBWF number, F       ; subtract 10 (F = F - W)
    BC    Loop10sMeas     ; keep doing it until Carry = 0
    DECF  count, F        ; once too many
    ADDWF number, F       ; add 10 back to get remainder
    MOVFF count,0x71      ; Places 10's place value into 0x71
    MOVFF number, 0x70    ; Places 1's place value into 0x70
                          ; Converts 10s place of measuredTempInput to BCD
GOTO _start2
    
