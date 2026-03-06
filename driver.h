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

//frame buffer used to store the current brightness values of every led
extern volatile uint8_t framebuffer[ROWS][COLS];

//returns current ticks passed since isr routine started started
uint32_t getTicks();

//returns true once every time a cycle is completed
uint8_t getFrametick();

//sets the led at index to brightness, with or without gamma correction
void setLed(int index, int brightness, uint8_t gammaCorrection);

//sets up the ISR, ports, framebuffer and enables global interrupts, needs to be called once before actual animations
void setupDriver();

#endif