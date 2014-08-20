#include <xc.h>         /* XC8 General Include File */
#include <stdint.h>         /* For uint8_t definition */
//#include <pic.h>

#include "user.h"
#include "main.h"

#include "protocol.h"

extern config_t config;

uint16_t* cur_song;
uint8_t cur_song_length;
uint8_t cur_song_ptr;
uint8_t cur_song_repeat;
uint16_t cur_song_duration;

#define NOTE_LOWC (_XTAL_FREQ / 261.6)
#define NOTE_C (_XTAL_FREQ / 523.3)
#define NOTE_F (_XTAL_FREQ / 698.5)
#define NOTE_E (_XTAL_FREQ / 659.3)
#define NOTE_D (_XTAL_FREQ / 587.3)
#define NOTE_LOWG (_XTAL_FREQ / 392.0)


const uint16_t death_song[] = {_XTAL_FREQ/260,_XTAL_FREQ/250,0,0,0,0,NOTE_C, NOTE_F, 0, NOTE_E, NOTE_F, NOTE_E, 0, NOTE_D, NOTE_C, NOTE_LOWG, 0, NOTE_LOWG, NOTE_LOWC};
const uint16_t fire_song[] = {_XTAL_FREQ/4000,_XTAL_FREQ/3500,_XTAL_FREQ/3000,_XTAL_FREQ/2500,_XTAL_FREQ/2000,_XTAL_FREQ/1500,_XTAL_FREQ/1000,_XTAL_FREQ/500,_XTAL_FREQ/450,_XTAL_FREQ/400,_XTAL_FREQ/350,_XTAL_FREQ/300,_XTAL_FREQ/250,0,0,0};
const uint16_t dead_song[] = {NOTE_LOWG, NOTE_LOWC, NOTE_LOWC};


void Setup(void)
{
    OSCCON = 0b01111010; //16MHz internal clock

    ANSELA = 0x00;
    TRISAbits.TRISA2 = 0;
    TRISAbits.TRISA0 = 0;

    // ADC setup
    ANSELAbits.ANSA4 = 1; // RA4 as analog input
    ADCON1 |= 0b00110000; // Use internal RC osc for ADC clock (allows ADC to function in sleep)

    // USART setup
    SPBRGL = 103; // Baud rate is 2404
    TXSTAbits.SYNC = 0;  // Asynchronous mode
    RCSTAbits.SPEN = 1; // Enable EUSART
    BAUDCONbits.SCKP = 1; // Inverted mode (idle low)
    TXSTAbits.TXEN = 1; // Enable transmitter

    // RX setup
    RCSTAbits.CREN = 1; // Enable receiver

    // Interrupts setup
    INTCONbits.GIE = 1; // Enable interrupts

    // TMR0 setup (serial modulation)
    OPTION_REGbits.TMR0CS = 0;

    // TMR1 setup (music)
    T1CONbits.T1CKPS=3;

    // TMR2 setup (counter)
    T2CONbits.T2CKPS = 3;
    T2CONbits.T2OUTPS = 15;
    T2CONbits.TMR2ON = 1;
}

uint16_t ADC_read()
{
    ADCON0bits.CHS = 3;
    ADCON0bits.ADON = 1; // Turn ADC on
    __delay_us(6);
    ADCON0bits.GO = 1; // Set GO bit to start conversion
    while(ADCON0bits.GO){};
    return ADRES >> 6;
}

void red_led_on()
{
    TRISAbits.TRISA5 = 0;
    PORTAbits.RA5 = 0;
}

void green_led_on()
{
    TRISAbits.TRISA5 = 0;
    PORTAbits.RA5 = 1;
}

void led_off()
{
    TRISAbits.TRISA5 = 1;
}

void play_song(uint16_t* song,uint8_t length,uint16_t note_duration,uint8_t repeat)
{
    cur_song=song;
    cur_song_length=length;
    cur_song_ptr=0;
    cur_song_repeat=repeat;
    cur_song_duration=note_duration;

    TMR1 = -cur_song_duration;
    PIR1bits.TMR1IF = 1;
    T1CONbits.TMR1ON = 1; // Turn on timer
}

void stop_song()
{
    tone_off();
    T1CONbits.TMR1ON=0;
    PIR1bits.TMR1IF=0;
}

void handle_music()
{
    if(!PIR1bits.TMR1IF) return;
    if(cur_song_ptr >= cur_song_length)
    {
        if(cur_song_repeat)
        {
            cur_song_ptr=0;
        }
        else
        {
            stop_song();
            return;
        }
    }
    tone(cur_song[cur_song_ptr]);
    TMR1 = -cur_song_duration;
    cur_song_ptr++;
    PIR1bits.TMR1IF=0;
}

void tone(uint16_t period)
{
    PWM3CONbits.PWM3EN = 1;
    PWM3CONbits.PWM3OE = 1;
    PWM3PR = period;
    PWM3DC = period >> 1;
    PWM3PH = 0x0000;
    PWM3LDCONbits.PWM3LD = 1;
}

void tone_off()
{
    PWM3CONbits.PWM3OE = 0;
}

void Buzz(uint16_t freq, uint16_t dur_ms)
{
    tone((_XTAL_FREQ / freq)-1);
    for(uint16_t i=0;i<dur_ms;i++) __delay_ms(1);
    tone_off();
}

void Send_Byte(uint8_t data)
{
    RCSTAbits.CREN = 0; // Disable receiver
    Modulate_Serial();
    TXREG = data;
    while(!TXSTAbits.TRMT); // Wait for USART to send all data
    Disable_Modulation();
    RCSTAbits.CREN = 1; // Enable receiver
    RCREG;
}

void Modulate_Serial(void){
    DACCON0bits.DACEN = 1; // Enable DAC to send data over serial
    TMR0=0;
    INTCONbits.TMR0IF=1;
    INTCONbits.TMR0IE=1; // Execute interrupt immediately
}

void Disable_Modulation(void){
    INTCONbits.TMR0IE=0; // Turn off interrupt
    DACCON0bits.DACEN=0;
}

uint16_t Read_Memory(uint16_t address){
    PMCON1bits.CFGS = 0; // Address program memory
    PMADR = address;
    PMCON1bits.RD = 1; // Read operation
    NOP();
    NOP();
    return PMDAT;
}

void Load(uint16_t address, uint16_t* ptr, uint8_t data_length){
    PMCON1bits.CFGS = 0; // Address program memory
    PMADR = address;
    for (uint8_t i=0; i<data_length; i++){
        PMCON1bits.RD = 1; // Read operation
        NOP();
        NOP();
        *ptr++ = PMDAT;
        PMADR ++;
    }
}

void Save(uint16_t address, uint16_t* ptr, uint8_t data_length){
    // Erase data
    INTCONbits.GIE = 0;
    PMADR = address;
    PMCON1bits.CFGS = 0; // Program memory
    PMCON1bits.FREE = 1; // Erase operation
    PMCON1bits.WREN = 1; // Write enable
    PMCON2 = 0x55;
    PMCON2 = 0xAA; // Flash memory unlock sequence
    PMCON1bits.WR = 1; // Begin erase
    NOP();
    NOP();
    PMCON1bits.WREN = 0;

    PMADR = address;
    PMCON1bits.CFGS = 0; // Program memory
    PMCON1bits.FREE = 0; // Write operation
    PMCON1bits.WREN = 1; // Write enable
    PMCON1bits.LWLO = 1; // Load write latches only

    for (uint8_t i=1; i<=data_length; i++){
        PMDAT = *ptr++;
        if(i == data_length) PMCON1bits.LWLO = 0; // last one
        PMCON2 = 0x55;
        PMCON2 = 0xAA; // Flash memory unlock sequence
        PMCON1bits.WR = 1; // Begin not erase
        NOP();
        NOP();
        PMADRL++;
    }
    PMCON1bits.WREN = 0;
    INTCONbits.GIE = 1;
}

uint8_t handle_fire(){
    static uint16_t timer = 0; // for holdoff
    static uint16_t counter = 0; // for power
    uint16_t a;

    a=ADC_read();
    if (a > config.fire_threshold && a < config.fire_cheating)
    {
        if(timer < config.fire_holdoff)
        {
            timer ++;
        }
        else
        {
            if(!config.power || counter <= config.power)
            {
                if(!counter)
                {
                    red_led_on();
                    play_song(fire_song,sizeof(fire_song)/sizeof(uint16_t),3000,!(config.power));
                }
                Send_Byte(config.id);
                counter++;
            }
            else
            {
                led_off();
            }
        }
    }
    else
    {
        led_off();
        if(!config.power && counter) stop_song();
        timer = 0;
        counter = 0;
        return 0;
    }
    return 1;
}

void Sleep() {
    // This is fucked, needs fixing
    INTCONbits.GIE = 1; // Enable interrupts
    INTCONbits.PEIE = 1; // Enable peripheral interrupts
    PIE1bits.TMR1IE = 1; // Enable Timer1 overflow interrupt
    T1CONbits.nT1SYNC = 1; // Don't synchronize external clock input
    T1CONbits.TMR1ON = 1; // Turn on timer
    TMR1 = 25536; // Sleep for 10ms
    asm("sleep");
    INTCONbits.GIE = 0;
    INTCONbits.PEIE = 0;
    PIE1bits.TMR1IE = 0;
    T1CONbits.TMR1ON = 0;
    Buzz(4000,5);
}

// Interrupts

void interrupt High_Priority_Interrupt(){
   if(INTCONbits.TMR0IF) // TMR1 register overflowed
   {
       asm("BANKSEL DACCON0"); // Use DAC to generate 38KHz carrier wave
       asm("MOVF DACCON0 & 0x7F, W");
       asm("XORLW 1<<5");
       asm("MOVWF DACCON0 & 0x7F");

       TMR0 -= 50;
       INTCONbits.TMR0IF = 0; // Clear the interupt flag
   }
}

uint8_t get_hitlist_length()
{
    uint8_t i;
    for(i=0;i<HITLIST_SIZE;i++)
    {
        if(!(hitlist[i] & 0x007F)) return 2*i;
        if(!(hitlist[i] & 0x3F80)) return 2*i+1;
    }
    return 2*i;
}

void add_to_hitlist(uint8_t gun)
{
    uint16_t gun16 = gun & 0x7F;
    for(uint8_t i=0;i<HITLIST_SIZE;i++)
    {
        if(!(hitlist[i] & 0x007F))
        {
            hitlist[i] |= gun16;
            return;
        }
        if(!(hitlist[i] & 0x3F80))
        {
            hitlist[i] |= gun16 << 7;
            return;
        }
    }
}

void super_dead_mode(){
    uint16_t counter = 0;
    while(!config.health){
        handle_music();
        // Manage base station comms
        uint8_t b;
        if(CHECK_CHAR()){
            b=AVAIL_CHAR();
            if(b == 0x10) {
                control_transfer();
            }
        }
        counter++;
        __delay_ms(1);
        if(counter > config.death_period){
            counter = 0;
            led_off();
            Send_Byte(config.id);
            play_song(dead_song,sizeof(dead_song)/sizeof(uint16_t),10000,0);
        }
        if(counter == config.death_period-50){
            red_led_on();
        }
    }
}

void hit_by(uint8_t who)
{
  uint16_t respawn_timer;

  add_to_hitlist(who);
  Save(FLASH_HITLIST,(uint16_t*)&hitlist,HITLIST_SIZE);

  config.health --;
  Save(FLASH_CONFIG,(uint16_t*)&config, CONFIG_SIZE);

  play_song(death_song,sizeof(death_song)/sizeof(uint16_t),60000,0);

  if(!config.health)
  {
    red_led_on();
    super_dead_mode();
    return;
  }

  respawn_timer = config.respawn_delay;
  while(respawn_timer)
  {
    respawn_timer--;
    red_led_on();
    for(uint8_t i=0;i<50;i++)
    {
        handle_music();
        __delay_ms(1);
    }

    if(respawn_timer < 30)
    {
        led_off();
    }
    for(uint8_t i=0;i<50;i++)
    {
        handle_music();
        __delay_ms(1);
    }
  }
}
