#include <stdbool.h> 
bool correct; 
int count; 
 
void update7SD(void) {              //updating 7-segment display to reflect current count
    if (count == 0)                 //using pre-defined values for 7SD
        PORTD = num0;               
    else if (count == 1)
        PORTD = num1;
    else if (count == 2)
        PORTD = num2;
    else if (count == 2)
        PORTD = num2;
    else if (count == 3)
        PORTD = num3;
    else if (count == 4)
        PORTD = num4;
    else if (count == 5)
        PORTD = num5;
    else if (count == 6)
        PORTD = num6;
    else if (count == 7)
        PORTD = num7;
    else if (count == 8)
        PORTD = num8;
    else if (count == 9)
        PORTD = num9;
    return;
}
 
void passwordInput(void) //gets input 
{ 
    while(correct==false){ 
        if(PORTAbits.RA0==1){ 
            count++; 
            update7SD(); 
            __delay_ms(500); 
        } 
        if(PORTBbits.RB1==1){ //leaves when button pressed  
            correct=true; 
        } 
    } 
    return; 
} 
 
void passwordCheck(void) //checks if on 
{ 
    update7SD(); 
    if(password == secretPassword){ //spins motor if password is correct 
        PORTAbits.RA5 = 1;                   
        __delay_ms(5000); 
        PORTAbits.RA5 = 0;   
    } 
    else{ //else turns on buzzer 
        PORTBbits.RB2 = 1; 
        __delay_ms(3000); 
        PORTBbits.RB2 = 0; 
    } 
    return;     
} 
 
void __interrupt(irq(IRQ_INT0),base(0x3008)) INT0_ISR(void){ //interrupt  
    if(PIR1bits.INT0IF == 1){ //turns on buzzer 
        PORTBbits.RB2 = 1; 
        __delay_ms(2000); 
        PORTBbits.RB2 = 0; 
    } 
    PIR1bits.INT0IF = 0;                 
    return; 
}

