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

/* TODO User level functions prototypes (i.e. InitApp) go here */

void Setup(void);         /* I/O and Peripheral Initialization */
uint16_t ADC_read(void);
void LED_on(void);
void LED_off(void);
void Buzz(uint16_t freq, uint16_t dur_ms);
void Send_Packet(uint8_t data);
void Modulate_Serial(void);
void Disable_Modulation(void);
//void interrupt high_priority High_Priority_Interrupt();
