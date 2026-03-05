#ifndef DRIVER_H

#include <stdint.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#define DRIVER_H
#define __AVR_ATtiny861A__

#define COLA_MASK 0b11110000   //PA4–PA7
#define COLB_MASK 0b00111100   //PB2–PB5
#define ROWS 4
#define COLS 8
#define LEDCOUNT 32

extern volatile uint8_t framebuffer[ROWS][COLS];

uint32_t getTicks();
uint8_t getFrametick();
void setLed(int index, int brightness, uint8_t gammaCorrection);
void setupDriver();

#endif