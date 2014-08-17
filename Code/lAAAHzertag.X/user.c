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

config_t config;

void Setup(void)
{
    OSCCON = 0b01111010; //16MHz internal clock

    ANSELA = 0x00;
    TRISAbits.TRISA5 = 0;
    TRISAbits.TRISA2 = 0;
    TRISAbits.TRISA0 = 0;
    PORTAbits.RA5 = 1; // LED default to off

    // ADC setup
    //ANSELAbits.ANSA4 = 1; // RA4 as analog input
    ADCON1 |= 0b00110000; // Use internal RC osc for ADC clock (allows ADC to function in sleep)

    // USART setup
    SPBRGL = 103; // Baud rate is 2404
    TXSTAbits.SYNC = 0;  // Asynchronous mode
    RCSTAbits.SPEN = 1; // Enable EUSART
    BAUDCONbits.SCKP = 1; // Inverted mode (idle low)
    TXSTAbits.TXEN = 1; // Enable transmitter

    // RX setup
    RCSTAbits.CREN = 1; // Enable receiver

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
    PORTAbits.RA5 = 0;
}

void LED_off()
{
    PORTAbits.RA5 = 1;
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
    PWM3CONbits.PWM3OE = 0;
}

void Send_Byte(uint8_t data)
{
    RCSTAbits.CREN = 0; // Disable receiver
    Modulate_Serial();
    TXREG = data;
    while(!TXSTAbits.TRMT); // Wait for USART to send all data
    Disable_Modulation();
    RCSTAbits.CREN = 1; // Enable receiver
    RCREG;
}

void Modulate_Serial(void){
        DACCON0bits.DACEN = 1; // Enable DAC to send data over serial

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
    DACCON0bits.DACEN = 0;
}

uint16_t Read_Memory(uint16_t address){
    PMCON1bits.CFGS = 0; // Address program memory
    PMADR = address;
    PMCON1bits.RD = 1; // Read operation
    NOP();
    NOP();
    return PMDAT;
}

void Load(uint16_t address, uint16_t* ptr, uint8_t data_length){
    PMCON1bits.CFGS = 0; // Address program memory
    PMADR = address;
    for (uint8_t i=0; i<data_length; i++){
        PMCON1bits.RD = 1; // Read operation
        NOP();
        NOP();
        *ptr++ = PMDAT;
        PMADR ++;
    }
}

void Save(uint16_t address, uint16_t* ptr, uint8_t data_length){
    // Erase data
    INTCONbits.GIE = 0;
    PMCON1bits.CFGS = 0; // Program memory
    PMADR = address;
    PMCON1bits.FREE = 1; // Erase operation
    PMCON1bits.WREN = 1; // Write enable
    PMCON2 = 0x55;
    PMCON2 = 0xAA; // Flash memory unlock sequence
    PMCON1bits.WR = 1; // Begin erase
    NOP();
    NOP();
    PMCON1bits.WREN = 0;

    PMCON1bits.FREE = 0; // Write operation
    PMCON1bits.LWLO = 1; // Load write latches only
    PMCON1bits.WREN = 1; // Write enable

    for (uint8_t i=0; ; i++){
        PMDAT = *ptr++;
        if(i == data_length) break;
        PMCON2 = 0x55;
        PMCON2 = 0xAA; // Flash memory unlock sequence
        PMCON1bits.WR = 1; // Begin not erase
        NOP();
        NOP();
        PMADR ++;
    }
    PMCON1bits.LWLO = 0;
    PMCON2 = 0x55;
    PMCON2 = 0xAA; // Flash memory unlock sequence
    PMCON1bits.WR = 1; // DO IT
    NOP();
    NOP();
    PMCON1bits.WREN = 0;
    INTCONbits.GIE = 1;
}

void Get_hit(uint8_t ID){
    LED_on();
    Buzz(4000,100);
    LED_off();
    Buzz(3500,100);
    LED_on();
    Buzz(4000,100);
    LED_off();
    Buzz(4500,100);
    LED_on();
    Buzz(4000,100);
    LED_off();
}

// Interrupts

void interrupt High_Priority_Interrupt(){
    if(PIR1bits.TMR1IF == 1) // TMR1 register overflowed
   {
       asm("BANKSEL DACCON0"); // Use DAC to generate 38KHz carrier wave
       asm("MOVF DACCON0, W");
       asm("XORLW 1<<5");
       asm("MOVWF DACCON0");

       TMR1 += 65486;
       PIR1bits.TMR1IF = 0; // Clear the interupt flag
    }
}