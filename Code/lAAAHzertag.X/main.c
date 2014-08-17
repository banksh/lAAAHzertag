/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#if defined(__XC)
    #include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>        /* HiTech General Include File */
#endif

#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>       /* For true/false definition */

#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "user.h"          /* User funct/params, such as InitApp */
#include "protocol.h"

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/

/* i.e. uint8_t <variable_name>; */

/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/

// CONFIG1
#pragma config FOSC = INTOSC    //  (INTOSC oscillator; I/O function on CLKIN pin)
#pragma config WDTE = OFF       // Watchdog Timer Enable (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable (PWRT disabled)
#pragma config MCLRE = ON       // MCLR Pin Function Select (MCLR/VPP pin function is MCLR)
#pragma config CP = OFF         // Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config BOREN = ON       // Brown-out Reset Enable (Brown-out Reset enabled)
#pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)

// CONFIG2
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
#pragma config PLLEN = OFF       // PLL Enable (4x PLL enabled)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
#pragma config LPBOREN = OFF    // Low Power Brown-out Reset enable bit (LPBOR is disabled)
#pragma config LVP = OFF         // Low-Voltage Programming Enable (Low-voltage programming enabled)

extern config_t config;

uint8_t my_id=0x80;
uint8_t my_power_level=0x00;
uint8_t my_random_number=0x00;



void control_transfer()
{
    uint8_t x,y,checksum; // Used by macros
    uint16_t i; // Used by macros

    uint8_t cmd;
    uint8_t power_level,random_number,id;

    ASSERT_SOF()
    READ_DATA_BYTE(cmd)

    switch(cmd)
    {
        case CMD_GET_RANDOM_NUMBER:
            ASSERT_EOF()
            SEND_SOF()
            SEND_DATA_BYTE(CMD_TAKE_RANDOM_NUMBER)
            SEND_DATA_BYTE(my_random_number)
            SEND_EOF()
            break;
        case CMD_ASSIGN_ID:
            READ_DATA_BYTE(random_number)
            READ_DATA_BYTE(id)
            ASSERT_EOF()
            if(random_number != my_random_number) goto err;
            SEND_SOF()
            SEND_DATA_BYTE(CMD_ACK)
            SEND_EOF()
            //my_id=id;
            //Write_Device_ID(id);
            break;
        case CMD_SET_POWER: // set power
            READ_DATA_BYTE(power_level)
            ASSERT_EOF()
            my_power_level = power_level;
            SEND_SOF()
            SEND_DATA_BYTE(CMD_ACK) // ack
            SEND_EOF()
            break;
        // more commands go here
        default:
            goto err;
    }
err:
    return;
}

void Fire(){
    if (PORTAbits.RA4) {
        Send_Byte(my_id);
        Buzz(3000,50);
    }
}

void main(void)
{
    uint8_t b;

    config.health = 16;
    config.id = 0x80;
    config.power = 0x16;
    config.respawn_timer = 10;
    
    Setup();

    while(1)
    {/*
        Send_Byte('E');
        Send_Byte('r');
        Send_Byte('i');
        Send_Byte('c');
        Send_Byte(',');
        Send_Byte(' ');
        Send_Byte('s');
        Send_Byte('u');
        Send_Byte('c');
        Send_Byte('k');
        Send_Byte(' ');
        Send_Byte('a');
        Send_Byte(' ');
        Send_Byte('d');
        Send_Byte('i');
        Send_Byte('c');
        Send_Byte('k');
        Send_Byte('\n');*/
        Fire();
        if(CHECK_CHAR())
        {
            b=AVAIL_CHAR();
            if(b == 0x10) {
                control_transfer();
            }
            if((b > 0x80) && (b != my_id)) {
                Get_hit(b);
            }
                //Buzz(5000,150);
        }


        Save(0x7D0, (uint16_t*) &config, CONFIG_SIZE);

        config.id = 0x92;
        Load(0x7D0, (uint16_t*) &config, CONFIG_SIZE);
        //Write_Device_ID(0x82);
        //my_id = Read_Device_ID();
        //LED_on();
        //Buzz(3500,200);
        //Buzz(4000,200);
        //__delay_ms(50);
        //Send_Packet(0b10101010);
        //LED_off();
        //Buzz(5500,200);
        //Buzz(4000,200);
        //__delay_ms(50);
        //Send_Packet(0b10110011);
    }

}

