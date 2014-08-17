#define CMD_ACK 0x00
#define CMD_GET_RANDOM_NUMBER 0x01
#define CMD_TAKE_RANDOM_NUMBER 0x02
#define CMD_ASSIGN_ID 0x03
#define CMD_GET_LOG 0x04
#define CMD_TAKE_LOG 0x05
#define CMD_CLEAR_LOG 0x06
#define CMD_SET_HEALTH 0x07
#define CMD_SET_POWER 0x08
#define CMD_SET_DELAY 0x09

#define CB_SOF 0x10
#define CB_EOF 0x11

// Fill these in
#define CHECK_CHAR() (PIR1bits.RCIF)
#define AVAIL_CHAR() (RCREG)
#define GET_CHAR(gb) i=0; while(!PIR1bits.RCIF) {__delay_us(100); i++; if(i >= 1000) goto err;} gb = AVAIL_CHAR();
#define PUT_CHAR(pb) Send_Byte(pb);

// These should be good
#define READ_DATA_BYTE(b) GET_CHAR(x) GET_CHAR(y) if((x & (~0x0F)) || (y & (~0x0F))) goto err; b=((y << 4) | x); checksum += b;
#define SEND_DATA_BYTE(b) x = b & 0x0F; y = b >> 4; PUT_CHAR(x) PUT_CHAR(y) checksum -= b;

#define SEND_SOF() PUT_CHAR(CB_SOF) checksum = 0; SEND_DATA_BYTE(my_id)
#define SEND_EOF() SEND_DATA_BYTE(checksum) PUT_CHAR(CB_EOF)

#define ASSERT_SOF() checksum = 0; READ_DATA_BYTE(x) if(x != my_id) goto err;
#define ASSERT_EOF() READ_DATA_BYTE(x) if(checksum) goto err; GET_CHAR(x) if(x != CB_EOF) goto err;
