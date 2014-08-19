#define _XTAL_FREQ 16000000

#define __delay_us(x) _delay((unsigned long)((x)*(_XTAL_FREQ/4000000.0)))
#define __delay_ms(x) _delay((unsigned long)((x)*(_XTAL_FREQ/4000.0)))


void Setup(void);         /* I/O and Peripheral Initialization */
uint16_t ADC_read(void);
void green_led_on(void);
void red_led_on(void);
void led_off(void);
void Buzz(uint16_t freq, uint16_t dur_ms);
void Send_Byte(uint8_t data);
void Modulate_Serial(void);
void Disable_Modulation(void);
uint8_t handle_fire(void);
void Save(uint16_t address, uint16_t* ptr, uint8_t data_length);
void Load(uint16_t address, uint16_t* ptr, uint8_t data_length);
void Get_hit(uint8_t ID);
void Sleep();
//void interrupt high_priority High_Priority_Interrupt();
uint8_t get_hitlist_length();
void add_to_hitlist(uint8_t gun);
void play_song(uint16_t* song,uint8_t length,uint16_t note_duration,uint8_t repeat);
void stop_song();
void handle_music();
void tone(uint16_t period);
void tone_off();