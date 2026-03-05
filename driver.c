#include <avr/io.h>
#include <avr/pgmspace.h>
#include "driver.h"

//hand adjusted gamma correction table, does not look that good imo except on some of the sinewave animations
static const uint8_t gamma64[] PROGMEM = {
     1,  1,  1,  2,  2,  3,  3,  4,  
     5,  6,  7,  8,  9, 10, 11, 12,
    13, 15, 17, 19, 21, 23, 25, 27,
    29, 31, 33, 35, 37, 39, 41, 43,
    45, 47, 49, 50, 51, 52, 53, 54,
    55, 56, 57, 58, 59, 60, 61, 62,
    62, 63, 63, 63, 63, 63, 63, 63,
    63, 63, 63, 63, 63, 63, 63, 63
};

//used to store the brightness data for leds, we could have also a double or a triple buffer setup
//but i found that only overcomplicated things and did not actually fix anything, so the isr just reads from
//this buffer even if the frame is not completely ready.
volatile uint8_t framebuffer[ROWS][COLS];
uint8_t curRow = 0;                         //keeps track of the current row
volatile uint8_t frame_tick = 0;            //flag that is set 1 when the frame is ready
volatile uint8_t pwm_tick = 0;              // 0..63
uint16_t base_ticks = 6;                    //used to determine the lenght of one pwm slice in timer ticks, check isr
volatile uint32_t ticks = 0;                //counts up roughly every 6 ms
//math: 24 us per pwm slice, a frame consists of 64 slices for all 4 rows
//frame lenght = 24 us * 64 * 4 = 256 * 24us = 6144 us = roughly 6 ms. Framerate is roughly 160 fps

uint32_t getTicks(){
    return ticks;
}

uint8_t getFrametick(){
    if(frame_tick){
        frame_tick = 0;
        return 1;
    }
    return 0;
}

void setupDriver(){
    DDRB =  0b00111100; //Port B: /1 = output, 0 = input
    DDRA =  0b11111111; //Every port in porta as output
    PORTA = 0b00001111; //write ports
    PORTB = 0b01000000; //PB6 need to be written high (input-pullup for the button)

    DDRB  &= ~(1 << DDB6);   // PB6 as input
    //PORTB |=  (1 << PB6);    // enable pull-up on PB6

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

void setLed(int index, int brightness, uint8_t gammaCorrection){//gammaCorrection: 1 = on, 0 = off
    //smooth bounds alil, bad practice
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

//each iteration of isr has 24 us to run before it is ran again, so isr needs to be as lean as possible
//pll frequency = 64 MHz, prescaler 256 -> 256 * 1/64 000 000 = 4 us = tick, base_ticks = 6 so 6 ticks per pwm slice
//4 us * 6 = 24 us
ISR(TIMER1_COMPA_vect){
    //clear outputs
    PORTA = 0b11110000;
    PORTB &= ~COLB_MASK; //clear the ports used for leds, while keeping the others as they were (button pin has to always be 1)

    uint8_t cols = 0;
    uint8_t *rowptr = (uint8_t *)framebuffer[curRow];

    //faster than a for loop, creates the right column byte to use for the row
    if (rowptr[0] > pwm_tick) cols |= 0b00000001;
    if (rowptr[1] > pwm_tick) cols |= 0b00000010;
    if (rowptr[2] > pwm_tick) cols |= 0b00000100;
    if (rowptr[3] > pwm_tick) cols |= 0b00001000;
    if (rowptr[4] > pwm_tick) cols |= 0b00010000;
    if (rowptr[5] > pwm_tick) cols |= 0b00100000;
    if (rowptr[6] > pwm_tick) cols |= 0b01000000;
    if (rowptr[7] > pwm_tick) cols |= 0b10000000;

    uint8_t inv = ~cols;

    //PB2–PB5 maps to cols 4–7
    uint8_t pb = 0;
    pb |= (inv & 0b01110000) >> 1;
    pb |= (inv & 0b10000000) >> 5;
    PORTB = (PORTB & ~COLB_MASK) | (pb & COLB_MASK);

    //PA4–PA7 maps to cols 0–3
    PORTA = (inv << 4);

    //activate the right row
    PORTA |= (1 << curRow);

    //advance the row, go through all 4 rows/pwm slice, and then advance to next slice
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