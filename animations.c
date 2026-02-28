#include "PWM-engine.c"
#include <math.h>
int random();
void delayFade(int n);
void fadeall(uint8_t speed);
void updateSnowfall(int delay, int lenghtTicks);
void updateBreathe(int cycles, int delay);
void updateBars(int delay, uint8_t rand);

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

uint8_t bars[5][8]={{-1,26,27,28,29,30,31,0},   //upper hypotenuse
                    {-1,1,2,3,4,5,6,-1},         //upper leg
                    {20,21,22,23,24,25,-1,-1},  //middle line
                    {7,8,9,10,11,12,-1,-1},     //lower leg
                    {13,14,15,16,17,18,19,-1}}; //lower hypotenuse

uint8_t barBrg[5][2]; //0 = brightness, 1 = direction

uint16_t randomval;
int count = 0;
uint32_t lastTicks = 0;
int n = 0;
int animation = 0;
uint8_t dir = 1;
int a = 0;
int b = 0;

void updateBars(int delay, uint8_t rand){
    for(int i = 0; i < 5; i++){
        if(!rand && ((b - (38 * i)) % 200 == 0)){
            barBrg[i][1] = 1;
            barBrg[i][0] = 1;
        }

        if(!barBrg[i][1]) continue;//if the bar wont change continue the loop

        if(barBrg[i][0] >= 63){
            barBrg[i][1] = -1;
        }

        barBrg[i][0] += barBrg[i][1];

        if(barBrg[i][0] <= 1){
            barBrg[i][1] = 0;
        }
        for(int c = 0; c < 8; c++){
            if(bars[i][c] >= 0){
                setLed(bars[i][c], barBrg[i][0]);
            }
        }
    }
    if(rand && (b % 48 == 0)){
        int tries = 0;
        int passed = 0;
        int i;
        while((tries <= 20) && (passed == 0)){
            i = (random() >> 11) % 5; //brute force an index where a bar is not already on
            if(barBrg[i][1] == 0){
                passed = 1;
            }
            tries++;
        }
        //turn the new bar on
        barBrg[i][1] = 1;
        barBrg[i][0] = 1;
    }
    b++;
    if(b >= 1000){
        for(int i = 0; i < 5; i++){
            barBrg[i][0] = 0;
            barBrg[i][1] = 0;
        }
        animation++;
        b = 0;
        delayFade(100);
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
        animation++;;
        n = 0;
        b = 0;
    }
}

void delayFade(int n){ //fade for n * 4 ms ,,,todo non blocking solution to fading between animations
    int i = 0;
    while(i < n){
        if(frame_tick){
            fadeall(1);
            frame_tick = 0;
            i++;
        }
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