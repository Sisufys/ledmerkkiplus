#define __AVR_ATtiny861A__
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

void setLed(int index, int brightness, uint8_t gammaCorrection);
void setup();

#define ROW_MASK  0b00001111   //PA0–PA3
#define COLA_MASK 0b11110000   //PA4–PA7
#define COLB_MASK 0b00111100   //PB2–PB5
#define ROWS 4
#define COLS 8
#define LEDCOUNT 32
#define FRAMELEN 4

//gamma correction table, enable in setLed() if wanted
const uint8_t gamma64[] PROGMEM = {
     1,  1,  1,  2,  2,  3,  3,  4,  
     5,  6,  7,  8,  9, 10, 11, 12,
    13, 15, 17, 19, 21, 23, 25, 27,
    29, 31, 33, 35, 37, 39, 41, 43,
    45, 47, 49, 50, 51, 52, 53, 54,
    55, 56, 57, 58, 59, 60, 61, 62,
    62, 63, 63, 63, 63, 63, 63, 63,
    63, 63, 63, 63, 63, 63, 63, 63
};

volatile uint8_t framebuffer[ROWS][COLS];
uint8_t curRow = 0;
volatile uint8_t frame_tick = 0;
volatile uint8_t pwm_tick = 0;   // 0..63
uint16_t base_ticks = 6;
volatile uint32_t ticks = 0;

void setup(){
    DDRB =  0b00111100; //Port B: /1 = output, 0 = input
    DDRA =  0b11111111; //Every port in porta as output
    PORTA = 0b00001111; //write ports
    PORTB = 0b01000000; //PB6 need to be written high (input-pullup)

    DDRB  &= ~(1 << DDB6);   // PB6 as input
    PORTB |=  (1 << PB6);    // enable pull-up on PB6

    //set every brightness 0
    for(int i = 0; i < LEDCOUNT; i++){
        setLed(i, 0, 0);
    }

    //Timer1 uses PLL clock, setup PLL
    PLLCSR |= (1 << PLLE); 
    while (!(PLLCSR & (1 << PLOCK)));
    PLLCSR |= (1 << PCKE);

    //timer1 setup
    TCCR1A = 0;
    TCCR1B = (1 << 7) | (1 << CS13) | (1 << CS10); // CTC, prescaler 256
    //TCCR1B = (1 << 7) | (1 << CS13) | (1 << CS11);// CTC, prescaler 512
    TCNT1  = 0;
    OCR1C  = base_ticks;              // first slice
    TIMSK |= (1 << OCIE1A);           // enable Timer1 Compare A interrupt

    sei(); // Enable global interrupts
}

void setLed(int index, int brightness, uint8_t gammaCorrection){
    //smooth bounds alil
    if(index < 0) index += 32;
    if(index > 31) index -= 32;
    if(brightness < 0) brightness = 0;
    if(brightness > 63) brightness = 63;

    if(index >= 0 && index <= 31){
        if(gammaCorrection){
            framebuffer[index / 8][index % 8] = pgm_read_byte(&gamma64[brightness]);
        }
        else{
            framebuffer[index / 8][index % 8] = brightness;
        }
    }
}

ISR(TIMER1_COMPA_vect){
    //clear outputs
    PORTA = 0b11110000;
    PORTB &= ~COLB_MASK; //clear the ports used for leds, while keeping the others as they were

    uint8_t cols = 0;
    uint8_t *rowptr = (uint8_t *)framebuffer[curRow];

    if (rowptr[0] > pwm_tick) cols |= 0b00000001;
    if (rowptr[1] > pwm_tick) cols |= 0b00000010;
    if (rowptr[2] > pwm_tick) cols |= 0b00000100;
    if (rowptr[3] > pwm_tick) cols |= 0b00001000;
    if (rowptr[4] > pwm_tick) cols |= 0b00010000;
    if (rowptr[5] > pwm_tick) cols |= 0b00100000;
    if (rowptr[6] > pwm_tick) cols |= 0b01000000;
    if (rowptr[7] > pwm_tick) cols |= 0b10000000;

    uint8_t inv = ~cols;


    //PB2–PB5 cols 4–7
    uint8_t pb = 0;
    pb |= (inv & 0b01110000) >> 1;
    pb |= (inv & 0b10000000) >> 5;
    PORTB = (PORTB & ~COLB_MASK) | (pb & COLB_MASK);

    //PA4–PA7 cols 0–3
    PORTA = (inv << 4);

    //activate the right row
    PORTA |= (1 << curRow);

    //advance the row, go through all 4 rows/pwm slice, and then advance slice
    curRow++;
    if (curRow >= ROWS) {
        curRow = 0;

        pwm_tick++;
        if (pwm_tick >= 64) {
            pwm_tick = 0;
            frame_tick = 1;   // one full PWM frame completed
            ticks++;
        }
    }
}