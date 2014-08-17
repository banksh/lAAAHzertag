/******************************************************************************/
/* User Level #define Macros                                                  */
/******************************************************************************/

#define _XTAL_FREQ 16000000

#define __delay_us(x) _delay((unsigned long)((x)*(_XTAL_FREQ/4000000.0)))
#define __delay_ms(x) _delay((unsigned long)((x)*(_XTAL_FREQ/4000.0)))

/* TODO Application specific user parameters used in user.c may go here */

/******************************************************************************/
/* User Function Prototypes                                                   */
/******************************************************************************/

typedef struct {
    uint16_t id;
    uint16_t power;
    uint16_t health;
    uint16_t respawn_timer;
} config_t;

#define CONFIG_SIZE 4

/* TODO User level functions prototypes (i.e. InitApp) go here */

void Setup(void);         /* I/O and Peripheral Initialization */
uint16_t ADC_read(void);
void LED_on(void);
void LED_off(void);
void Buzz(uint16_t freq, uint16_t dur_ms);
void Send_Byte(uint8_t data);
void Modulate_Serial(void);
void Disable_Modulation(void);
void Fire(void);
void Save(uint16_t address, uint16_t* ptr, uint8_t data_length);
void Load(uint16_t address, uint16_t* ptr, uint8_t data_length);
void Get_hit(uint8_t ID);
//void interrupt high_priority High_Priority_Interrupt();
