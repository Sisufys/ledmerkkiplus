#ifndef ANIMATIONS_H

#define ANIMATIONS_H
#include <stdint.h>
#include <avr/pgmspace.h>

int animation;

//initialize random number generator
void initRandom();

//reset variables used to keep track of the current animation
void resetVars();

//random number generation
int random();

//fade for n amount of ticks, lower speed = faster
void delayFade(int n, int speed);

//fade all leds, speed=number of ticks needed to decrement every brightness by one
void fadeall(uint8_t speed);

//set random leds full brightness while fading every led at the same time
void updateSnowfall(int delay, int lenghtTicks);

//breathing effect, cycle all leds between full and 1 brightness
void updateBreathe(int cycles, int delay);

//light up the straight lines of the matrix in a sequential pattern, or in a random pattern
void updateBars(int delay, int n, uint8_t rand);

//a sine wave animation across a plane (brightness = f(x, y) = sin((y - x) + a), where x and y are the coordinates of a led)
void update2dWaves(int speed, int n, uint8_t gammaCorrection);

//a sine wave animation across the leds in a line pattern (brightness = f(x) = sin(x + a), where x is the led)
void update1dWaves(int speed, int n, int freq, uint8_t gammaCorrection);

//a sine wave animation from the center of the matrix that advances to every direction
//brightness = f(x, y) = sin(sqrt((x+y)²) + a), x and y are the coordinates of a corresponding led
void ripple(int speed, int n, int freq, uint8_t gammaCorrection);

//a pendulum wave type of animation, every led blinks at a slightly different frequency, cool patterns
void blinkSync(int n);

#endif