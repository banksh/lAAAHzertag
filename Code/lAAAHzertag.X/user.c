/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#if defined(__XC)
    #include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>        /* HiTech General Include File */
#endif

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */
#include <pic.h>
#include <pic12lf1571.h>

#include "user.h"


#define _XTAL_FREQ 16000000

#define __delay_us(x) _delay((unsigned long)((x)*(_XTAL_FREQ/4000000.0)))
#define __delay_ms(x) _delay((unsigned long)((x)*(_XTAL_FREQ/4000.0)))
/******************************************************************************/
/* User Functions                                                             */
/******************************************************************************/

/* <Initialize variables in user.h and insert code for user algorithms.> */

void Setup(void)
{
    TRISA |= (1 << TRISA4) | (1 << TRISA1); // MIC_IN and IR_IN to inputs
    //TRISA &= ^(1 << TRISA5) & (1 << TRISA3) & (1 << TRISA2) & (1 << TRISA0); // Everything else as output

    // ADC setup
    ANSELA |= (1 << ANSA4); // Config RA4 as analog input
    ADCON1 |= 0b00110000; // Use internal RC osc for ADC clock (allows ADC to function in sleep)
}

uint16_t ADC_read()
{
    ADCON0 |= (1 << CHS1) | (1 << CHS0) // Select channel AN3 (RA4)
    ADCON0 |= (1 << ADON); // Turn ADC on
    __delay_us(6);
    ADCON0 |= (1 << GO); // Set GO bit to start conversion
    while(^(ADCON0 & (1 << GO))){};
    return ADRESH << 2 + ADRESL >> 6;
}

void LED_on()
{
    TRISA &= ^(1 << TRISA5);
    PORTA &= ^(1 << RA5);
}