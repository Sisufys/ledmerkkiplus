#include "animations.h"
#include "driver.h"

uint8_t fadeSpeed[5] = {0,0,0,0,0}; //set 1 for animations that need constant fading
uint8_t debounceTicks = 10;

void main (void){
    //setup the matrix driver
    setupDriver();
    //initialize pseudorandom number generator
    initRandom();

    uint32_t lastButtonPress = 0; //uint32_t to not overflow, uint16_t overflows in about 6.5 minutes, 32 bit overflows in 298 days
    while(1){
        if(getFrametick()){ //fade leds if any on while keeping ye button responsive
            fadeall(fadeSpeed[animation]);

            if(animation >= 5){
                animation = 0;
            }
            switch(animation){
                case(0):
                    ripple(3, 20, 4, 0);
                    //updateBreathe(10, 2);
                    break;
                case(1):
                    update1dWaves(2, 20, 1, 1);
                    break;
                case(2):
                    update2dWaves(2, 20, 1);
                    break;
                case(3):
                    updateBars(2, 20, 0);
                    break;
                case(4):
                    blinkSync(1);
                    //updateSnowfall(8, 160);
                    break;
            }
        }

        uint8_t buttonState = (PINB >> PB6) & 1; //read PB6 for button presses, 0 being pressed and 1 not pressed
        if(!buttonState && (getTicks() - lastButtonPress > debounceTicks)){ //if the last button press is over n * 6 ms ago, debouncing
            lastButtonPress = getTicks();
            animation++;
            resetVars();
            delayFade(100, 1);
        }
    }
}