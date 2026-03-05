#ifndef ANIMATIONS_H

#define ANIMATIONS_H
#include <stdint.h>
#include <avr/pgmspace.h>

int animation;

void initRandom();
void resetVars();//reset variables used to keep track of the current animation
int random();
void delayFade(int n, int speed);
void fadeall(uint8_t speed);
void updateSnowfall(int delay, int lenghtTicks);
void updateBreathe(int cycles, int delay);
void updateBars(int delay, int n, uint8_t rand);
void update2dWaves(int speed, int n, uint8_t gammaCorrection);
void update1dWaves(int speed, int n, int freq, uint8_t gammaCorrection);
void ripple(int speed, int n, int freq, uint8_t gammaCorrection);

#endif