#define _XTAL_FREQ 16000000

extern uint8_t my_random_number;

#define CHECK_CHAR() (PIR1bits.RCIF)
#define AVAIL_CHAR() (RCREG^0b10101010)

void Setup(void);
void cheat(void);
uint16_t ADC_read(void);
void green_led_on(void);
void red_led_on(void);
void led_off(void);
void Buzz(uint16_t freq, uint16_t dur_ms);
void Send_Byte(uint8_t data);
uint8_t handle_fire(void);
void Save(uint16_t address, uint16_t* ptr, uint8_t data_length);
void Load(uint16_t address, uint16_t* ptr, uint8_t data_length);
void Get_hit(uint8_t ID);
//void interrupt high_priority High_Priority_Interrupt();
uint8_t get_hitlist_length();
void add_to_hitlist(uint8_t gun);
void play_song(uint16_t* song,uint8_t length,uint16_t note_duration,uint8_t repeat);
void stop_song();
void handle_music();
void tone(uint16_t period);
void tone_off();
void hit_by(uint8_t who);
void super_dead_mode();
void delay_1_ms();

#define led_off() {TRISAbits.TRISA5 = 1;}

#define tone_off() {PWM3CONbits.PWM3OE = 0;}
