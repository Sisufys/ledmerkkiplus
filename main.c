#include "animations.c"

uint8_t fadeSpeed[5] = {1,0,0,1,1};

void main (void){
    //set the pwm engine ready
    setup();

    //setup random value for the linear congruential generator (random number)
    uint8_t l = TCNT0L;
    uint8_t h = TCNT0H;
    randomval = (((uint16_t)h << 8) | l);
    randomval ^= TCNT1;
    if(!randomval) randomval = 1;

    int i = 1;
    int n = 0;
    int lastButtonPress = 0;
    while(1){
        if(frame_tick){ //fade leds if any on while keeping ye button responsive
            fadeall(fadeSpeed[animation]);
            frame_tick = 0;

            if(animation >= 5){
                animation = 0;
            }
            switch(animation){
                case(0):
                    ripple(3, 10, 4, 0);
                    //updateBreathe(10, 2);
                    break;
                case(1):
                    update1dWaves(2, 10, 1, 1);
                    break;
                case(2):
                    update2dWaves(2, 10, 1);
                    break;
                case(3):
                    updateBars(2, 10, 0);
                    break;
                case(4):
                    updateSnowfall(8, 160);
                    break;
            }
        }


        uint8_t buttonState = (PINB >> PB6) & 1; //state is 0 or 1, 0 being button pressed
        if(!buttonState && (ticks - lastButtonPress > 5)){ //if button is pressed and the last button press is over n * 4 ms ago, debouncing
            lastButtonPress = ticks;
            animation++;
            delayFade(100, 1);
            b = 0;
        }
    }
}