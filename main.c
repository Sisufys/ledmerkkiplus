#include "animations.c"


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
    int nextSparkle = 0;
    while(1){
        if(frame_tick){ //fade leds if any on while keeping ye button responsive
            if(animation != 1) fadeall(1);
            frame_tick = 0;
            //if(sparkles && (ticks % 3 == 0)) updateSparkle();
            //if(ticks % 100 == 0) setLed((random() >> 11), 63);
        }
        
        switch(animation){
            case(0):
                updateSnowfall(8, 80);

                break;
            case(1):
                updateBreathe(3, 2);
                break;
        }
        
        
        //triangles(4,14);
        //roundaround(4,10);
        //doublereverseround(5,7);
        //mirror(2, 10);
        uint8_t buttonState = (PINB >> PB6) & 1; //state is 0 or 1, 0 being button pressed
        if(!buttonState && (ticks - lastButtonPress > 25)){ //if button is pulled low and the last button press is over 50 ms ago, debouncing
            lastButtonPress = ticks;
            setLed((random() >> 11), 63);
            //newSparkle(nextSparkle);
            //nextSparkle++;
            //if(nextSparkle > 2) nextSparkle = 0;
        }
    }
}