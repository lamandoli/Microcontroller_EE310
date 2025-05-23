//-----------------------------
// TITLE: LCD_Display
//-----------------------------
// PURPOSE: Learn about IO ports, learn about LCD's, and understand basic
//          operation and configurations of an ADC peripheral. LCD display will
//          show real-time lux(ambient light reading) value. If button is pressed
//          all functions will be halted and program will go into INTERRUPT mode
//          for 10 seconds. A Red LED will blink during the 10 second interrupt.
//          Upon interrupt mode completion, program will return to normal operation
//          of displaying lux value. 
// DEPENDICIES: 
//              #include <xc.h>            
//              #include <stdio.h>
//              #include <string.h>
//              #include <stdlib.h>
// COMPILER: XC8 v3.00
// AUTHOR: Lee Amandoli
// INPUTS: photoresistor[RA0], interrupt button[RC2], 
// OUTPUTS: LCD[RB0:7], Red LED[RC3],Register select(RS pin)[RD0], Enable(E pin)[RD1]
//
//         Detailed explanation of pin and port uses below
//---------------------------------------------------------------------------------
//      Port	Pin     Configured As	Used For
//---------------------------------------------------------------------------------
//      PORTA	RA0     Analog Input	Light sensor (ADC)
//      PORTB	RB0?RB7	Digital Output	LCD D0?D7 data lines
//      PORTC	RC2     Digital Input	Interrupt-On-Change button
//      PORTC	RC3     Digital Output	LED blinking output
//      PORTD	RD0     Digital Output	LCD RS control
//      PORTD	RD1     Digital Output	LCD EN control
// HARDWARE: PIC18F47K42 microcontroller, qty-1 power supply (with 9V battery), qty-1 photoresistors, qty-1 switch, qty-1 LCD display,
//           qty-1 Red LED, qty-1 potentiometer, qty-2 breadboards, qty-2 resistors, various wiring
//           various wiring. 
// VERSIONS: First and final version. 4/28/2025.

// PIC18F46K42 Configuration Bit Settings


// CONFIG1L
#pragma config FEXTOSC = LP      
#pragma config RSTOSC = EXTOSC  

// CONFIG1H
#pragma config CLKOUTEN = OFF   
#pragma config PR1WAY = ON      
#pragma config CSWEN = ON       
#pragma config FCMEN = ON       

// CONFIG2L
#pragma config MCLRE = EXTMCLR  
#pragma config PWRTS = PWRT_OFF 
#pragma config MVECEN = ON      
#pragma config IVT1WAY = ON     
#pragma config LPBOREN = OFF    
#pragma config BOREN = SBORDIS  

// CONFIG2H
#pragma config BORV = VBOR_2P45 
#pragma config ZCD = OFF        
#pragma config PPS1WAY = ON     
#pragma config STVREN = ON      
#pragma config DEBUG = OFF      
#pragma config XINST = OFF      

// CONFIG3L
#pragma config WDTCPS = WDTCPS_31
#pragma config WDTE = OFF       

// CONFIG3H
#pragma config WDTCWS = WDTCWS_7
#pragma config WDTCCS = SC      

// CONFIG4L
#pragma config BBSIZE = BBSIZE_512
#pragma config BBEN = OFF       
#pragma config SAFEN = OFF      
#pragma config WRTAPP = OFF     

// CONFIG4H
#pragma config WRTB = OFF       
#pragma config WRTC = OFF       
#pragma config WRTD = OFF       
#pragma config WRTSAF = OFF     
#pragma config LVP = ON         

// CONFIG5L
#pragma config CP = OFF         

#include <xc.h>            // Standard Microchip header
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define _XTAL_FREQ 4000000      // 4 MHz clock
#define FCY    _XTAL_FREQ/4

// LCD Control Pins
#define RS LATD0                 
#define EN LATD1                 
#define ldata LATB               

#define Vref 5.0                // ADC voltage reference
#define LCD_Port TRISB          
#define LCD_Control TRISD       

// Function Prototypes
void LCD_Init();
void LCD_Command(char);
void LCD_Char(char);
void LCD_String(const char*);
void LCD_String_xy(char, char, const char*);
void MSdelay(unsigned int);
void ADC_Init(void);
void Interrupt_Init(void);

// Global Variables
int digital;                 
float voltage;               
float lux;                   
char data[10];
unsigned char adcReady = 0;          
unsigned char interruptTriggered = 0;
unsigned char systemState = 0;        

/***************************** Main Program *******************************/
void main(void) {
    // Disable all analog features
    ANSELA = 0; ANSELB = 0; ANSELC = 0; ANSELD = 0;

    // Initialize modules
    LCD_Init();
    ADC_Init();
    Interrupt_Init();

    // Start first ADC manually
    ADCON0bits.GO = 1;
    while (ADCON0bits.GO);  // Wait for conversion

    // Process first ADC result manually
    digital = (ADRESH << 8) | ADRESL;
    voltage = (float)digital * (5.0 / 4096.0);
    lux = voltage * 200.0;

    LCD_Command(0x01);  // Clear LCD
    LCD_String_xy(1, 0, "Input Light:");
    sprintf(data, "%.0f Lux", lux);
    LCD_String_xy(2, 3, data);

    PIE1bits.ADIE = 1;  // Enable ADC interrupts
    ADCON0bits.GO = 1;  // Start continuous ADC

    while (1) {
        if (interruptTriggered) {
            systemState = 1;  // Enter HALT mode

            LCD_Command(0x01);
            LCD_String_xy(1, 0, "INTERRUPT MODE");

            PIE1bits.ADIE = 0; // Disable ADC interrupts temporarily

            // 10-second countdown
            for (int secondsLeft = 10; secondsLeft > 0; secondsLeft--) {
                char countdownMsg[17];
                sprintf(countdownMsg, "Time: %2d sec ", secondsLeft);
                LCD_String_xy(2, 0, countdownMsg);

                LATCbits.LATC3 = 1;
                __delay_ms(500);
                LATCbits.LATC3 = 0;
                __delay_ms(500);
            }

            // Reset interrupt state
            interruptTriggered = 0;
            systemState = 0;
            LATCbits.LATC3 = 0;
            IOCCFbits.IOCCF2 = 0;
            PIR0bits.IOCIF = 0;
            PIE1bits.ADIE = 1;

            // Restore normal display
            LCD_Command(0x01);
            LCD_String_xy(1, 0, "Input Light:");
            LCD_String_xy(2, 3, "--- Lux");

            adcReady = 0;
            ADCON0bits.GO = 1;
        }

        // Handle ADC conversion ready
        if (adcReady && systemState == 0) {
            adcReady = 0;

            LCD_Command(0xC3);          // Move to row 2, col 3
            sprintf(data, "%-3.0f Lux", lux);
            LCD_String(data);           // Update lux value

            __delay_ms(200);             // Slow update slightly
        }

        // Start new ADC if needed
        if (!ADCON0bits.GO && !adcReady && systemState == 0) {
            ADCON0bits.GO = 1;
        }
    }
}

/**************************** Functions ********************************/

void LCD_Init() {
    MSdelay(30);         
    LCD_Port = 0x00;      
    LCD_Control = 0x00;   
    LCD_Command(0x01);    // Clear display
    LCD_Command(0x38);    // 2 lines, 5x7 matrix
    LCD_Command(0x0C);    // Display ON, cursor OFF
    LCD_Command(0x06);    // Increment cursor
}

void LCD_Clear() {
    LCD_Command(0x01);    // Clear display
}

void LCD_Command(char cmd) {
    ldata = cmd;          
    RS = 0;               
    EN = 1;
    __delay_us(50);       
    EN = 0;
    MSdelay(3);           
}

void LCD_Char(char dat) {
    ldata = dat;          
    RS = 1;               
    EN = 1;
    __delay_us(50);       
    EN = 0;
    MSdelay(1);           
}

void LCD_String(const char *msg) {
    while (*msg != 0) {      
        LCD_Char(*msg);
        msg++;
    }
}

void LCD_String_xy(char row, char pos, const char *msg) {
    char location = (row <= 1) ? (0x80 | (pos & 0x0F)) : (0xC0 | (pos & 0x0F));
    LCD_Command(location);
    LCD_String(msg);
}

void MSdelay(unsigned int val) {
    unsigned int i, j;
    for (i = 0; i < val; i++)
        for (j = 0; j < 165; j++);  // 1ms for 8MHz
}

void ADC_Init(void) {
    TRISAbits.TRISA0 = 1;   
    ANSELAbits.ANSELA0 = 1; 
    ADCON0bits.FM = 1;      
    ADCON0bits.CS = 1;      
    ADPCH = 0x00;           
    ADCON0bits.ON = 1;      
    ADCLK = 0x00;           
    ADRESL = 0x00;
    ADRESH = 0x00;
    ADPREL = 0x00;
    ADPREH = 0x00;
    ADACQL = 0x00;
    ADACQH = 0x00;
}

void __interrupt(irq(default), base(0x6008)) my_ISR(void) {
    if (PIR0bits.IOCIF) {  
        if (IOCCFbits.IOCCF2) {  
            interruptTriggered = 1;
            unsigned char portValue = PORTC;
            IOCCFbits.IOCCF2 = 0;
            PIR0bits.IOCIF = 0;
        }
    }
    
    if (PIR1bits.ADIF) {        
        if (systemState == 0) { 
            digital = (ADRESH << 8) | ADRESL; 
            voltage = digital * (5.0 / 4096.0);
            lux = voltage * 200;
            adcReady = 1;
        }
        PIR1bits.ADIF = 0;  
    }
}

void Interrupt_Init(void) {
    INTCON0bits.GIE = 0;

    // Setup LED pin (RC3)
    TRISCbits.TRISC3 = 0; 
    LATCbits.LATC3 = 0;   
    ODCONCbits.ODCC3 = 0; 
    ANSELCbits.ANSELC3 = 0;

    // Setup button pin (RC2)
    TRISCbits.TRISC2 = 1;   
    ANSELCbits.ANSELC2 = 0;
    WPUCbits.WPUC2 = 1;     
    INLVLCbits.INLVLC2 = 1;
    SLRCONCbits.SLRC2 = 0; 
    ODCONCbits.ODCC2 = 0;   

    // Setup Interrupt Vector Table
    IVTBASEU = 0x00;
    IVTBASEH = 0x60;
    IVTBASEL = 0x08;

    // Configure Interrupt-On-Change for RC2
    IOCCFbits.IOCCF2 = 0;
    PIE0bits.IOCIE = 0;
    IOCCPbits.IOCCP2 = 1;
    IOCCNbits.IOCCN2 = 1;
    PIR0bits.IOCIF = 0;
    PIE0bits.IOCIE = 1;

    // Enable Interrupt Priority and Global Interrupts
    INTCON0bits.IPEN = 1;
    INTCON0bits.GIEH = 1;
    INTCON0bits.GIEL = 1;
    INTCON0bits.GIE = 1;
}
