#include "animations.h"
#include "driver.h"

uint8_t fadeSpeed[5] = {1,0,0,1,1};

void main (void){
    //set the matrix driver
    setupDriver();
    //initialize pseudorandom number generator
    initRandom();

    int lastButtonPress = 0;
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

        uint8_t buttonState = (PINB >> PB6) & 1; //state is 0 or 1, 0 being button pressed
        if(!buttonState && (getTicks() - lastButtonPress > 10)){ //if button is pressed and the last button press is over n * 6 ms ago, debouncing
            lastButtonPress = getTicks();
            animation++;
            resetVars();
            delayFade(100, 1);
        }
    }
}