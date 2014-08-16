/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#if defined(__XC)
    #include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>        /* HiTech General Include File */
#endif

#include <stdint.h>         /* For uint8_t definition */
//#include <stdbool.h>        /* For true/false definition */
#include <pic.h>

#include "user.h"


/******************************************************************************/
/* User Functions                                                             */
/******************************************************************************/

/* <Initialize variables in user.h and insert code for user algorithms.> */

void Setup(void)
{
    OSCCON = 0b01111010; //16MHz internal clock

    ANSELA = 0x00;
    TRISAbits.TRISA5 = 0;
    TRISAbits.TRISA2 = 0;
    TRISAbits.TRISA0 = 0;

    // ADC setup
    ANSELAbits.ANSA4 = 1; // RA4 as analog input
    ADCON1 |= 0b00110000; // Use internal RC osc for ADC clock (allows ADC to function in sleep)
}

uint16_t ADC_read()
{
    ADCON0 |= (1 << CHS1) | (1 << CHS0); // Select channel AN3 (RA4)
    ADCON0 |= (1 << ADON); // Turn ADC on
    __delay_us(6);
    ADCON0 |= (1 << GO); // Set GO bit to start conversion
    while(ADCON0bits.GO){};
    return ADRESH << 2 + ADRESL >> 6;
}

void LED_on()
{
    PORTAbits.RA5 = 1;
}

void LED_off()
{
    PORTAbits.RA5 = 0;
}

void Buzz(uint16_t freq, uint16_t dur_ms)
{
    PWM3CONbits.PWM3EN = 1;
    PWM3CONbits.PWM3OE = 1;
    PWM3PR = (_XTAL_FREQ / freq) - 1;
    PWM3DC = PWM3PR >> 1;
    PWM3PH = 0x0000;
    PWM3LDCONbits.LDA = 1;

    for(uint16_t i = 0; i <= dur_ms; i++){
        __delay_ms(1);
    }
}

void Send_Packet(uint8_t data)
{
    DACCON0bits.DACEN = 1; // Enable DAC to send data over serial
    Modulate_Serial();

    SPBRGL = 54; // Baud rate is 4545
    TXSTAbits.SYNC = 0;  // Asynchronous mode
    RCSTAbits.SPEN = 1; // Enable EUSART
    BAUDCONbits.SCKP = 1; // Inverted mode (idle low)
    TXSTAbits.TXEN = 1; // Enable transmitter

    TXREG = data;
    while(!TXSTAbits.TRMT); // Wait for USART to send all data
    Disable_Modulation();
    TRISAbits.TRISA0 = 0; // Allow RA0 to drive low
    DACCON0bits.DACEN = 0;
}

void Modulate_Serial(void){
	INTCONbits.GIE = 1; // Enable interrupts
	INTCONbits.PEIE = 1; // Enable peripheral interrupts
	PIE1bits.TMR1IE = 1; // Enable Timer1 overflow interrupt

	T1CONbits.nT1SYNC = 1; // Don't synchronize external clock input
	T1CONbits.TMR1ON = 1; // Turn on timer
	TMR1 = 65486; // Should give 38.095 KHz
}

void Disable_Modulation(void){
    INTCONbits.GIE = 0;
    T1CONbits.TMR1ON = 0;
}


// Interrupts

void interrupt High_Priority_Interrupt(){
    if(PIR1bits.TMR1IF == 1) // TMR1 register overflowed
   {
       asm("BANKSEL DACCON0"); // Use DAC to generate 38KHz carrier wave
       asm("MOVF DACCON0, W");
       asm("XORLW 1<<5");
       asm("MOVWF DACCON0");
        
       asm("BANKSEL PORTA");
       asm("MOVF PORTA, W");
       asm("XORLW 1<<5");
       asm("MOVWF PORTA");
       
       TMR1 += 65486;
       PIR1bits.TMR1IF = 0; // Clear the interupt flag
    }
}