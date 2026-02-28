#include "PWM-engine.c"
#include <math.h>
int random();
void delayFade(int n);
void fadeall(uint8_t speed);
void updateSnowfall(int delay, int lenghtTicks);
void updateBreathe(int cycles, int delay);
void updateSparkle();
void newSparkle(int i);

//upper and lower triangle animations
uint8_t tri1[16] = {
    1,
    2,
    3,
    4,
    5,
    6,
    7,
    24,
    25,
    26,
    27,
    28,
    29,
    30,
    31,
    32};

uint8_t tri2[16] = {
    14,
    13,
    12,
    11,
    10,
    9,
    8,
    23,
    22,
    21,
    20,
    19,
    18,
    17,
    16,
    15};

uint16_t randomval;
int count = 0;
uint32_t lastTicks = 0;
int n = 0;
int animation = 0;
uint8_t dir = 1;
int b = 0;



void delayFade(int n){ //fade for n * 4 ms
    int i = 0;
    while(i < n){
        if(frame_tick){
            fadeall(1);
            frame_tick = 0;
            i++;
        }
    }
}

int random(){
    randomval = randomval*2053u + 13849u; //uint16 for wraparound
    return randomval;
}

void updateSnowfall(int delay, int lenghtTicks){
    if(frame_tick){
        fadeall(1);
        frame_tick = 0;
    }
    if(ticks - lastTicks > delay){
        lastTicks = ticks;
        setLed((random() >> 11), 63);
        n++;
        if(n >= 5*lenghtTicks/6){
            delay += 2;
        }
    }
    if(n >= lenghtTicks){
        animation++;
        n = 0;
        delayFade(60);
    }
}

void updateBreathe(int cycles, int delay){

    if(ticks - lastTicks > delay){
        lastTicks = ticks;
        for(int led = 0; led < 32; led++){ //set all leds to b brightness
            setLed(led, b);
        }
        b += (dir) ? 1 : -1;//if direction is positive, increment by one, otherwise decrement by one
        if (b >= 63){
            b = 63;
            dir = 0;
        }
        if (b <= 1){
            b = 1;
            dir = 1;
            n++; //cycle completed
        }
    }
    if(n >= cycles){
        animation = 0;
        n = 0;
    }
}

void fadeall(uint8_t n){
    count++;
    if(count >= n){
        count = 0;
        for(int x = 0; x < ROWS; x++) {
            for(int y = 0; y < COLS; y++) {
                if(framebuffer[x][y] > 1) {
                    framebuffer[x][y] -= 1;
                }
            }
        }
    }
}