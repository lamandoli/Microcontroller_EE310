/*
 * ---------------------
 * Title: Calculator 
 * ---------------------
 * Program Details:
 *  The purpose of this program is to make a simple calculator with a 4x4 keypad,
 *  breadboard, 8 LED's, qty-8 100 ohm resistors, qty-4 10k ohm pulldown resistors,
 *  power bank, 9V battery, and a PIC18F47K42 microcontroller. The calculator takes
 *  three inputs from the user: the first value X_Input_REG, the operation code 
 *  Operation_REG, and the second value Y_Input_REG. The program calculates
 *  the result and stores it in Display_Result_REG and displays it in binary on
 *  8 LED's.
 * Inputs: decimal values from keypad.PORTB[4:7](To Control Keypad). 
 *         X_Input_REG, Operation_REG, Y_Input_REG       
 * Outputs: decimal number displayed in binary on LED's PORTB[0:3](To Control Keypad)
 *          PORTD[0:7](To Control the LED's). Display_Result_REG
 * Setup: C-Simulator. PIC18F47K42 microcontroller, breadboard, 8 LED's, power module
 *        qty-4 10k ohm resistors, qty-8 150 ohm resistors, various wiring, 9V battery
 * Date: April 5, 2025
 * File Dependencies / Libraries: It is required to include the Configuration Header File 
 *                                CProgramExample.h
 * Compiler: xc8, 6.2
 * Author: Lee Amandoli
 * Versions:
 *      V1.0: Original
 * Useful links:  
 *      Datasheet: https://ww1.microchip.com/downloads/en/DeviceDoc/PIC18(L)F26-27-45-46-47-55-56-57K42-Data-Sheet-40001919G.pdf 
 *      PIC18F Instruction Sets: https://onlinelibrary.wiley.com/doi/pdf/10.1002/9781119448457.app4 
 *      List of Instrcutions: http://143.110.227.210/faridfarahmand/sonoma/courses/es310/resources/20140217124422790.pdf 
 */
// CONFIG1L
#pragma config FEXTOSC = LP     // External Oscillator Selection (LP (crystal oscillator) optimized for 32.768 kHz; PFM set to low power)
#pragma config RSTOSC = EXTOSC  // Reset Oscillator Selection (EXTOSC operating per FEXTOSC bits (device manufacturing default))

// CONFIG1H
#pragma config CLKOUTEN = OFF   // Clock out Enable bit (CLKOUT function is disabled)
#pragma config PR1WAY = ON      // PRLOCKED One-Way Set Enable bit (PRLOCK bit can be cleared and set only once)
#pragma config CSWEN = ON       // Clock Switch Enable bit (Writing to NOSC and NDIV is allowed)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor enabled)

// CONFIG2L
#pragma config MCLRE = EXTMCLR  // MCLR Enable bit (If LVP = 0, MCLR pin is MCLR; If LVP = 1, RE3 pin function is MCLR )
#pragma config PWRTS = PWRT_OFF // Power-up timer selection bits (PWRT is disabled)
#pragma config MVECEN = ON      // Multi-vector enable bit (Multi-vector enabled, Vector table used for interrupts)
#pragma config IVT1WAY = ON     // IVTLOCK bit One-way set enable bit (IVTLOCK bit can be cleared and set only once)
#pragma config LPBOREN = OFF    // Low Power BOR Enable bit (ULPBOR disabled)
#pragma config BOREN = SBORDIS  // Brown-out Reset Enable bits (Brown-out Reset enabled , SBOREN bit is ignored)

// CONFIG2H
#pragma config BORV = VBOR_2P45 // Brown-out Reset Voltage Selection bits (Brown-out Reset Voltage (VBOR) set to 2.45V)
#pragma config ZCD = OFF        // ZCD Disable bit (ZCD disabled. ZCD can be enabled by setting the ZCDSEN bit of ZCDCON)
#pragma config PPS1WAY = ON     // PPSLOCK bit One-Way Set Enable bit (PPSLOCK bit can be cleared and set only once; PPS registers remain locked after one clear/set cycle)
#pragma config STVREN = ON      // Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
#pragma config DEBUG = OFF      // Debugger Enable bit (Background debugger disabled)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Extended Instruction Set and Indexed Addressing Mode disabled)

// CONFIG3L
#pragma config WDTCPS = WDTCPS_31// WDT Period selection bits (Divider ratio 1:65536; software control of WDTPS)
#pragma config WDTE = OFF       // WDT operating mode (WDT Disabled; SWDTEN is ignored)

// CONFIG3H
#pragma config WDTCWS = WDTCWS_7// WDT Window Select bits (window always open (100%); software control; keyed access not required)
#pragma config WDTCCS = SC      // WDT input clock selector (Software Control)

// CONFIG4L
#pragma config BBSIZE = BBSIZE_512// Boot Block Size selection bits (Boot Block size is 512 words)
#pragma config BBEN = OFF       // Boot Block enable bit (Boot block disabled)
#pragma config SAFEN = OFF      // Storage Area Flash enable bit (SAF disabled)
#pragma config WRTAPP = OFF     // Application Block write protection bit (Application Block not write protected)

// CONFIG4H
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot Block not write-protected)
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers not write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM not write-protected)
#pragma config WRTSAF = OFF     // SAF Write protection bit (SAF not Write Protected)
#pragma config LVP = ON         // Low Voltage Programming Enable bit (Low voltage programming enabled. MCLR/VPP pin function is MCLR. MCLRE configuration bit is ignored)

// CONFIG5L
#pragma config CP = OFF         // PFM and Data EEPROM Code Protection bit (PFM and Data EEPROM code protection disabled)

#include <xc.h> // must have this
#include "C:/Program Files/Microchip/xc8/v3.00/pic/include/proc/pic18f47k42.h"


#define _XTAL_FREQ 4000000                 // Fosc  frequency for _delay()  library
#define FCY    _XTAL_FREQ/4
 //Defines rows and columns for 4x4 keypad
#define R0 LATBbits.LATB0
#define R1 LATBbits.LATB1
#define R2 LATBbits.LATB2
#define R3 LATBbits.LATB3
#define C0 PORTBbits.RB4
#define C1 PORTBbits.RB5
#define C2 PORTBbits.RB6
#define C3 PORTBbits.RB7

// Keypad map
const char keymap[4][4] = {
    {'1', '2', '3', '+'},
    {'4', '5', '6', '-'},
    {'7', '8', '9', '*'},
    {'C', '0', '=', '/'}
};

// Program Variables
char scan_keypad(void);
int get_num  (char ch);
void keypad_init(void);

unsigned char X_Input_REG = 0; //First number
unsigned char Y_Input_REG = 0; //Second number
char op = '\0'; //Operation
char state = 0;  // 0 = entering num1, 1 = op, 2 = num2

void main(void) {
    keypad_init();

    TRISD = 0x00;     // Initialize ports. PORTD = output
    ANSELD = 0x00;
    LATD = 0x00;

    while (1) {
        char Operation_REG = scan_keypad(); //Check keypad for input

        if (Operation_REG != '\0') { //Checks if the variable Operation_REG does not equal the null character ('\0').
            __delay_ms(300);  // delay

            if (Operation_REG >= '0' && Operation_REG <= '9') { //Checks to make sure inputs is within range
                int digit = Operation_REG - '0'; //Converts the character value stored in Operation_REG into an integer
                if (state == 0) { //If state(input) changes, do this
                    X_Input_REG = X_Input_REG * 10 + digit;//Allows user to input first 2 digit number
                    LATD = X_Input_REG ;//Displays value. 
                } else if (state == 2) { //If state(input) changes, do this
                    Y_Input_REG = Y_Input_REG * 10 + digit; //Allows user to input second 2 digit number
                    LATD = Y_Input_REG;//Displays value.
                }
            }
            // Check if Operation_REG contains one of the arithmetic operation symbols
            else if (Operation_REG == '+' || Operation_REG == '-' || Operation_REG == '*' || Operation_REG == '/') {
                if (state == 0) {  // If we're currently in state 0 (meaning num1 has been entered, waiting for an operation)
                    op = Operation_REG ; //Store operation in 'op'
                    state = 2;  // Move to state 2, where the user will enter the second number (num2)
                    LATD = 0x00;  // Clear display
                }
            }

            else if (Operation_REG == '=') { 
                signed char Display_Result_REG = 0; // 'signed char' allows storing both positive and negative 8-bit values (-128 to +127)
                                                    // Variable to store the result of the operation
                switch (op) {  // Perform the appropriate operation based on the stored operator (op)
                    case '+': Display_Result_REG = X_Input_REG + Y_Input_REG; break; // Addition
                    case '-': Display_Result_REG = X_Input_REG - Y_Input_REG; break; // Subtraction
                    case '*': Display_Result_REG = X_Input_REG * Y_Input_REG; break; // Multiplication
                    case '/': if (Y_Input_REG != 0) {
                                    Display_Result_REG = X_Input_REG / Y_Input_REG;
                              } else {
                                  Display_Result_REG = 0;
                              }break; // Division.Check for divide-by-zero to avoid crash 
                    default: Display_Result_REG = 0; break; // If no valid operator was selected, default the result to 0
                }

                LATD = Display_Result_REG;  // Shows 2's complement if negative

                // Reset for next input
                X_Input_REG = 0; Y_Input_REG = 0; op = '\0'; state = 0;
                __delay_ms(2000);
                LATD = 0x00;
}


            else if (Operation_REG == 'C') {
                // Reset everything
                X_Input_REG = 0; Y_Input_REG = 0; op = '\0'; state = 0;
                LATD = 0x00;
            }
        }
    }
}

// Scan function for pull-downs (active-high key press)
char scan_keypad() {
    for (int row = 0; row < 4; row++) {
        // Set all rows LOW
        R0 = R1 = R2 = R3 = 0;

        // Drive current row HIGH
        switch (row) {
            case 0: R0 = 1; break;
            case 1: R1 = 1; break;
            case 2: R2 = 1; break;
            case 3: R3 = 1; break;
        }

        __delay_ms(5); // delay

        if (C0 == 1) { while (C0 == 1); return keymap[row][0]; }
        if (C1 == 1) { while (C1 == 1); return keymap[row][1]; }
        if (C2 == 1) { while (C2 == 1); return keymap[row][2]; }
        if (C3 == 1) { while (C3 == 1); return keymap[row][3]; }
    }

    return '\0'; // No key pressed
}

// Function to initialize keypad pins
void keypad_init() {
    TRISB = 0b11110000;   // RB0:RB3 = output (rows), RB4:RB7 = input (columns)
    LATB = 0x00;          // Set all rows high initially
    ANSELB = 0x00;        // Digital mode for PORTB
   
}
