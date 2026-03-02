#include "PWM-engine.c"
int random();
void delayFade(int n, int speed);
void fadeall(uint8_t speed);
void updateSnowfall(int delay, int lenghtTicks);
void updateBreathe(int cycles, int delay);
void updateBars(int delay, int n, uint8_t rand);
void update2dWaves(int speed, int n, uint8_t gammaCorrection);
void update1dWaves(int speed, int n, int freq, uint8_t gammaCorrection);
void lineTriWave(int speed, int freq, int gammaCorrection);
void planeTriWave(int speed, int freq, int gammaCorrection);

//all leds mapped into a 2d grid
const uint8_t coords[LEDCOUNT][2] = { //led, (x, y) grid is size (120, 200)
    {60, 200},
    {60, 186},
    {60, 170},
    {60, 155},
    {60, 139},
    {60, 124},
    {60, 108},
    {60, 93},
    {60, 77},
    {60, 61},
    {60, 41},
    {60, 30},
    {60, 14},
    {60, 0},
    {70, 14},
    {77, 30},
    {86, 46},
    {95, 62},
    {104, 78},
    {112, 94},
    {120, 110},
    {100, 107},
    {80, 103},
    {41, 97},
    {20, 93},
    {0, 90},
    {8, 105},
    {17, 122},
    {26, 138},
    {34, 154},
    {43, 169},
    {50, 186},
};

//lookup table for sin
const uint8_t sineLut[512] PROGMEM = {
    32, 32, 33, 33, 34, 34, 34, 35, 35, 35, 36, 36, 37, 37, 37, 38, 38, 38, 39, 39, 40, 40, 40, 41,
    41, 41, 42, 42, 42, 43, 43, 44, 44, 44, 45, 45, 45, 46, 46, 46, 47, 47, 47, 48, 48, 48, 49, 49,
    49, 50, 50, 50, 50, 51, 51, 51, 52, 52, 52, 53, 53, 53, 53, 54, 54, 54, 54, 55, 55, 55, 55, 56,
    56, 56, 56, 57, 57, 57, 57, 58, 58, 58, 58, 58, 59, 59, 59, 59, 59, 60, 60, 60, 60, 60, 60, 60,
    61, 61, 61, 61, 61, 61, 61, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 63, 63, 63, 63, 63, 63,
    63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 62,
    62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 61, 61, 61, 61, 61, 61, 61, 60, 60, 60, 60, 60, 60, 60,
    59, 59, 59, 59, 59, 58, 58, 58, 58, 58, 57, 57, 57, 57, 56, 56, 56, 56, 55, 55, 55, 55, 54, 54,
    54, 54, 53, 53, 53, 53, 52, 52, 52, 51, 51, 51, 50, 50, 50, 50, 49, 49, 49, 48, 48, 48, 47, 47,
    47, 46, 46, 46, 45, 45, 45, 44, 44, 44, 43, 43, 42, 42, 42, 41, 41, 41, 40, 40, 40, 39, 39, 38,
    38, 38, 37, 37, 37, 36, 36, 35, 35, 35, 34, 34, 34, 33, 33, 32, 32, 32, 31, 31, 30, 30, 30, 29,
    29, 29, 28, 28, 27, 27, 27, 26, 26, 26, 25, 25, 24, 24, 24, 23, 23, 23, 22, 22, 22, 21, 21, 20,
    20, 20, 19, 19, 19, 18, 18, 18, 17, 17, 17, 16, 16, 16, 15, 15, 15, 14, 14, 14, 14, 13, 13, 13,
    12, 12, 12, 11, 11, 11, 11, 10, 10, 10, 10, 9, 9, 9, 9, 8, 8, 8, 8, 7, 7, 7, 7, 6,
    6, 6, 6, 6, 5, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 6, 6, 6,
    6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 11, 11, 11, 11, 12, 12,
    12, 13, 13, 13, 14, 14, 14, 14, 15, 15, 15, 16, 16, 16, 17, 17, 17, 18, 18, 18, 19, 19, 19, 20,
    20, 20, 21, 21, 22, 22, 22, 23, 23, 23, 24, 24, 24, 25, 25, 26, 26, 26, 27, 27, 27, 28, 28, 29,
    29, 29, 30, 30, 30, 31, 31, 32};

//upper and lower triangle animations
const uint8_t tri1[16] = {
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

const uint8_t tri2[16] = {
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

const uint8_t bars[5][8]={{-1,26,27,28,29,30,31,0},   //upper hypotenuse
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

void lineTriWave(int speed, int freq, int gammaCorrection){ //untested
     //x axis is led index i, y axis is brightness
    for(int i = 0; i < LEDCOUNT; i++){
        int phase = (freq * i + speed * b) & 127;
        int brg = (phase > 63) ? 63 - phase % 63 : phase;
        if(b <= 64) brg = brg - (64 - b);
        setLed(i, brg, gammaCorrection);
    }
    b++;
    if(b >= n * 512 / speed){
        dir = 1;
        b = 0;
        animation++;
        delayFade(100, 1);
    }
}

void planeTriWave(int speed, int freq, int gammaCorrection){ //untested
    for(int i = 0; i < LEDCOUNT; i++){
        int x = coords[i][0];
        int y = coords[i][1];
        int phase = (freq * (y - 1/2 * x) + speed * b) & 127;
        int brg = (phase > 63) ? 63 - phase % 63 : phase;
        if(b <= 64) brg = brg - (64 - b);
        setLed(i, brg, gammaCorrection);
    }
    b++;
    if(b >= n * 512 / speed){
        dir = 1;
        b = 0;
        animation++;
        delayFade(100, 1);
    }
}

void updateSineBreathe(int speed, int n, int freq, int gammaCorrection){
    int phase = (freq + speed * b) & 511;
    int brg = pgm_read_byte(&sineLut[phase]);
    for(int i = 0; i < LEDCOUNT; i++){
        setLed(i, brg, gammaCorrection);
    }
    b++;
    if(b >= n * 512 / speed){
        dir = 1;
        b = 0;
        animation++;
        delayFade(100, 1);
    }
}

void update1dWaves(int speed, int n, int freq, uint8_t gammaCorrection){
    //x axis is led index i, y axis is brightness
    for(int i = 0; i < LEDCOUNT; i++){
        int phase = (15 * freq *i + speed * b) & 511;
        int brg = pgm_read_byte(&sineLut[phase]);
        if(b <= 64) brg = brg - (64 - b);
        setLed(i, brg, gammaCorrection);
    }
    b++;
    if(b >= n * 512 / speed){
        dir = 1;
        b = 0;
        animation++;
        delayFade(100, 1);
    }
}

void update2dWaves(int speed, int n, uint8_t gammaCorrection){
    //brightness = z = f(x, y) = 32 + 31*sin((1/40) * (y - 1/2x) + 0.05b) -> b grows by speed per tick
    for(int i = 0; i < LEDCOUNT; i++){
        int x = coords[i][0];
        int y = coords[i][1];
        //int brg = 32 + 31*sin(0.025*(y - x/2) + 0.05 * b); //runs about 1 fps
        int phase = ((y - (2*x/3)) * 3 + speed * b) & 511;//0b 0000 0001 1111 1111, effectively the same as taking the modulus %512
        int brg = pgm_read_byte(&sineLut[phase]);
        if(b <= 64) brg = brg - (64 - b);
        setLed(i, brg, gammaCorrection);
    }
    b++;

    if(b >= n * 512 / speed){
        dir = 1;
        b = 0;
        animation++;
        delayFade(100, 1);
    }
}

void updateBars(int delay, int n, uint8_t rand){
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
                setLed(bars[i][c], barBrg[i][0], 0);
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
    if(b >= n * 200){
        for(int i = 0; i < 5; i++){
            barBrg[i][0] = 0;
            barBrg[i][1] = 0;
        }
        animation++;
        b = 0;
        delayFade(100, 1);
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
    if(ticks - lastTicks > delay + b){
        lastTicks = ticks;
        setLed((random() >> 11), 63, 0);
        n++;
        if(n >= 9*lenghtTicks/10){
            b += 2;
        }
    }
    if(n >= lenghtTicks){
        animation++;
        n = 0;
        b = 0;
        delayFade(60, 1);
    }
}

void updateBreathe(int cycles, int delay){
    if(ticks - lastTicks > delay){
        lastTicks = ticks;
        for(int led = 0; led < 32; led++){ //set all leds to b brightness
            setLed(led, b, 0);
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

void delayFade(int n, int speed){ //fade for n * 4 ms ,,,todo non blocking solution to fading between animations
    int i = 0;
    while(i < n){
        if(frame_tick){
            fadeall(speed);
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