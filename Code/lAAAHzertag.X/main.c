#include <xc.h>         /* XC8 General Include File */
#include <stdint.h>        /* For uint8_t definition */

#include "main.h"
#include "user.h"
#include "protocol.h"

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

config_t config;
hitlist_t hitlist;

static void LoadFromFlash()
{
    Load(FLASH_CONFIG,(uint16_t*)&config,CONFIG_SIZE);
    if(config.id == 0x3FFF)
    {
        config.health = 16;
        config.id = 0x80;
        config.power = 0;
        config.respawn_timer = 10;
        config.fire_threshold = 500;
        config.fire_holdoff = 8000;
        config.shield = 1;
        config.fire_cheating = 1000;
        Save(FLASH_CONFIG,(uint16_t*)&config,CONFIG_SIZE);
        Save(FLASH_HITLIST,(uint16_t*)&hitlist,HITLIST_SIZE);
    }
    Load(FLASH_HITLIST,(uint16_t*)&hitlist,HITLIST_SIZE);
}

void main(void)
{
    Setup();
    LoadFromFlash();

    while(1)
    {
        if(!handle_serial() & !Fire());// Sleep();
    }
}

void hit_by(uint8_t who)
{
  add_to_hitlist(who);
  Save(FLASH_HITLIST,(uint16_t*)&hitlist,HITLIST_SIZE);
  Buzz(1000,50);
  if(get_hitlist_length() > config.health)
  {
      Buzz(100,2000);
  }
}

