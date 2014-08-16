#define CB_SOF 0x10
#define CB_EOF 0x11

// Fill these in
#define GET_CHAR(b)
#define PUT_CHAR(b) 

// Fill these in
#define ENABLE_RX
#define DISABLE_RX

// These should be good
#define READ_DATA_BYTE(b) GET_CHAR(x) GET_CHAR(y) if(x & ~0x0F || y & ~0x0F) goto err; b=(y << 4 | x); checksum += b;
#define PUT_DATA_BYTE(b) checksum -= b; x = b & 0x0F; y = b >> 4; PUT_CHAR(x) PUT_CHAR(y)

#define SEND_SOF() DISABLE_RX PUT_CHAR(CB_SOF) checksum = 0; PUT_DATA_BYTE(my_id)
#define SEND_EOF() PUT_DATA_BYTE(checksum) PUT_CHAR(CB_EOF) ENABLE_RX

#define ASSERT_SOF() checksum = 0; READ_DATA_BYTE(x) if(x != my_id) goto err;
#define ASSERT_EOF() READ_DATA_BYTE(x) checksum += x; if(checksum) goto err; GET_CHAR(x) if(x != CB_EOF) goto err;
