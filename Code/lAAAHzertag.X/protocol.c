#include <xc.h>         /* XC8 General Include File */
#include <stdint.h>         /* For uint8_t definition */
#include <pic.h>

#include "protocol.h"
#include "main.h"
#include "user.h"

#define SERIAL_TIMEOUT 30000
#define SEND_SPACING 100

#define CMD_ACK 0x00
#define CMD_GET_RANDOM_NUMBER 0x01
#define CMD_TAKE_RANDOM_NUMBER 0x02
#define CMD_ASSIGN_ID 0x03
#define CMD_GET_FLASH_PAGE 0x04
#define CMD_TAKE_FLASH_PAGE 0x05
#define CMD_SET_FLASH_PAGE 0x06
#define CMD_SUCCESS 0x7

#define ID_INIT_FLAG (1<<9)

#define CB_SOF 0x10
#define CB_EOF 0x11

uint8_t _x,_y,_checksum; // Used by macros
uint16_t _i; // Used by macros

const uint16_t one_up[] = {_XTAL_FREQ/659.3,_XTAL_FREQ/784,_XTAL_FREQ/1318.5,_XTAL_FREQ/1046.5,_XTAL_FREQ/1174.7,_XTAL_FREQ/1568};

static uint8_t GET_CHAR_HELPER(uint8_t* val) {_i=0; while(!CHECK_CHAR()) {_i++; if(_i >= SERIAL_TIMEOUT) return 1;} *val = AVAIL_CHAR(); return 0;}
#define GET_CHAR(gb) if(GET_CHAR_HELPER(&(gb))) goto err;
#define PUT_CHAR(pb) Send_Byte(pb);

#define READ_DATA_BYTE(b) GET_CHAR(_x) GET_CHAR(_y) if((_x & (~0x0F)) || (_y & (~0x0F))) goto err; b=((_y << 4) | _x); _checksum += b;

static void SEND_DATA_BYTE(uint8_t b) {_x = b & 0x0F; _y = b >> 4; PUT_CHAR(_x) PUT_CHAR(_y) _checksum -= b;}
static void SEND_SOF() {PUT_CHAR(CB_SOF) _checksum = 0; SEND_DATA_BYTE(config.id);}
static void SEND_EOF() {SEND_DATA_BYTE(_checksum); PUT_CHAR(CB_EOF);}

#define ASSERT_SOF() _checksum = 0; READ_DATA_BYTE(_x) if(_x != (uint8_t)config.id) goto err;
#define ASSERT_EOF() READ_DATA_BYTE(_x) if(_checksum) goto err; GET_CHAR(_x) if(_x != CB_EOF) goto err;

void control_transfer()
{
    uint8_t i;
    uint8_t cmd;
    uint8_t random_number,id;

    uint16_t page;
    uint16_t data[16];
    uint8_t* dptr;
    uint8_t x;

    ASSERT_SOF()
    READ_DATA_BYTE(cmd)

    switch(cmd)
    {
        case CMD_GET_RANDOM_NUMBER:
            ASSERT_EOF()
            SEND_SOF();
            SEND_DATA_BYTE(CMD_TAKE_RANDOM_NUMBER);
            SEND_DATA_BYTE(my_random_number);
            SEND_EOF();
            break;
        case CMD_ASSIGN_ID:
            READ_DATA_BYTE(random_number)
            READ_DATA_BYTE(id)
            ASSERT_EOF()
            if(random_number != my_random_number) goto err;
            SEND_SOF();
            SEND_DATA_BYTE(CMD_ACK);
            SEND_EOF();
            config.id=id | ID_INIT_FLAG;
            Save(FLASH_CONFIG,(uint16_t*)&config,CONFIG_SIZE);
            break;
        case CMD_GET_FLASH_PAGE:
            READ_DATA_BYTE(x)
            *((uint8_t*)(&page))=x;
            READ_DATA_BYTE(x)
            *(((uint8_t*)(&page))+1)=x;
            ASSERT_EOF()
            Load(page,data,16);
            SEND_SOF();
            SEND_DATA_BYTE(CMD_TAKE_FLASH_PAGE);
            dptr=(uint8_t*)data;
            for(i=0;i<32;i++)
            {
                SEND_DATA_BYTE(*dptr++);
            }
            SEND_EOF();
            break;
        case CMD_SET_FLASH_PAGE:
            READ_DATA_BYTE(x)
            *((uint8_t*)(&page))=x;
            READ_DATA_BYTE(x)
            *(((uint8_t*)(&page))+1)=x;
            dptr=(uint8_t*)data;
            for(i=0;i<32;i++)
            {
                READ_DATA_BYTE(x);
                *dptr++=x;
            }
            ASSERT_EOF()
            Save(page,data,16);
            switch(page)
            {
                case FLASH_CONFIG:
                    Load(FLASH_CONFIG,(uint16_t*)&config,CONFIG_SIZE);
                    break;
                case FLASH_HITLIST:
                    Load(FLASH_HITLIST,(uint16_t*)&hitlist,HITLIST_SIZE);
                    break;
            }
            SEND_SOF();
            SEND_DATA_BYTE(CMD_ACK);
            SEND_EOF();
            break;
        case CMD_SUCCESS:
            ASSERT_EOF()
            play_song(one_up,sizeof(one_up)/sizeof(uint16_t),65000,0);
            SEND_SOF();
            SEND_DATA_BYTE(CMD_ACK);
            SEND_EOF();
            break;

        default:
            goto err;
    }
err:
    return;
}

uint8_t handle_serial()
{
    static uint8_t rx_id=0;
    static uint8_t rx_counter=0;
    uint8_t b;

    if(!CHECK_CHAR()) return 0;
    b=AVAIL_CHAR();
    if(b == 0x10) {
        control_transfer();
    }

    else if((b > 0x80) && (b != config.id) && (config.id != 0x80) && !(config.id & ID_INIT_FLAG)) {
        if((b != rx_id) || PIR1bits.TMR2IF) rx_counter=0;
        TMR2 = 0; // Using timer2 to time out hits
        PIR1bits.TMR2IF = 0;
        rx_id = b;
        rx_counter++;
        if(rx_counter > config.shield) {
            RCSTAbits.CREN = 0; // Disable receiver
            hit_by(b);
            rx_id = 0;
            AVAIL_CHAR();
            RCSTAbits.CREN = 1;
        }
    }
    return 1;
}
