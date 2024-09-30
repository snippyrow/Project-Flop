#include "Interrupts/idt.cpp"
#include "rand.cpp"

int vY = 0;
int vX = 4;
int pX = 320/2;
int pY = 200/2;

int sizeX = 10;
int sizeY = 10;

bool cheat = false;

bool needSwitch = true;

// Define buffers
unsigned char *video_buffer = (unsigned char *)0xa0000;
unsigned char *working_buffer = (unsigned char *)0xfffff; // why 0xfffff? I have no idea.
unsigned char *font = (unsigned char *)0xa000;
unsigned char *ragebait = (unsigned char *)0xe000;

void outb2(short Port, char Value){
    asm volatile("outb %1, %0" : : "dN" (Port), "a" (Value));
}

void putpixel(uint32_t x, uint32_t y, uint8_t color) {
    video_buffer[(y*320) + x] = color;
}

void testkey() {
    uint8_t scancode = inb(0x60);
    switch(scancode) {
        case 0x39:
            vY = -20;
            break;
        case 16:
            cheat = !cheat;
            break;
        default:
            break;
    }
}

void initPIT(int frequency) { // frequency is in hz
    uint32_t divisor = 1193182/frequency; // Divisor must be a maximum of 65535, or 0xffff. Frequency must be 18.2hz or higher.

    // Send command byte to PIT
    outb2(0x43, 0x36); // Binary, mode 3, low/high byte

    // Send the low byte
    outb2(0x40, divisor & 0xFF);
    // Send the high byte
    outb2(0x40, (divisor >> 8) & 0xFF);
    return;
}

void renderclear(uint8_t clearcolor) {
    for (int i=0;i<320*200;i++) {
        working_buffer[i] = clearcolor;
    }
}

void callSwitch() {
    if (!needSwitch) {
        return;
    }
    needSwitch = false;
    for (int i=0;i<(320*200);i++) {
        video_buffer[i] = working_buffer[i];
    }
}

void draw_rect(int x1, int y1, int sX, int sY, uint8_t color) {
    for (int x=0;x<sX;x++) {
        for (int y=0;y<sY;y++) {
            working_buffer[((y+y1)*320)+x+x1] = color;
        }
    }
}

int i=0;
void drawchar(int x, int y, uint8_t character, uint8_t color) {
    i++;
    int it = ((character/16)*8)+(character/16);
    int cX = (character%16)+(character%16)*8;
    int cY = 144*(it);

    for (int y1=0;y1<8;y1++) {
        for (int x1=0;x1<8;x1++) {
            int ptr = ((y1*144)+cY)+x1+cX;
            uint8_t var = font[ptr];

            if ((var)) {
                working_buffer[((y1+y)*320)+x1+x] = color;
            }
        }
    }
}


int trail[32][2];

bool dead = false;
bool won = false;
int deadtimer = 0;
int score = 0;
int goal = 50000;
char death[] = "Game over.";
char win[] = "You won!!!";

coin rage;



void gameloop() {
    renderclear(0);
    if (score >= goal) {
        won = true;
    }
    if (!dead && !won) {
        score = score + 20;
        vY += 1;
        pY += vY/4;
        pX += vX;
    } else {
        deadtimer += 3;
    }
    if (pX >= 320-sizeX) {
        vX = -4;
    } else if (pX <= 0) {
        vX = 4;
    } else if (pY < 0 && !cheat) {
        dead = true;
        rage.active = true;
        rage.x = pX;
        rage.y = pY;
    } else if (pY > 200-sizeY && !cheat) {
        dead = true;
        rage.active = true;
        rage.x = pX;
        rage.y = pY;
    }

    for (int i=1;i<32;i++) {
        trail[i-1][0] = trail[i][0];
        trail[i-1][1] = trail[i][1];

        trail[i][0] = pX;
        trail[i][1] = pY;
    }
    for (int i=1;i<16;i++) {
        int width = sizeX / (i + .5 - 1); // Slower division
        int height = sizeY / (i + .5 - 1); // Slower division
        draw_rect(trail[32-i][0],trail[32-i][1],sizeX-i,sizeY-i,0x7);
    }
    int player_color = 4;
    if (cheat) {
        player_color = 2;
    }
    for (int x=0;x<sizeX;x++) {
        for (int y=0;y<sizeY;y++) {
            working_buffer[((y+pY)*320)+x+pX] = player_color;
        }
    }
    // render coins
    for (int i=0;i<32;i++) {
        if (coins[i].active) {
            // check coin collision
            if (pX < coins[i].x + 8 && pX + sizeX > coins[i].x && pY < coins[i].y + 8 && pY + 8 > coins[i].y) {
                coins[i].active = false;
                score += 1000;

            } else {
                drawchar(coins[i].x,coins[i].y,0xb8,0xe);
            }
        }
    }
    // render spikes
    for (int i=0;i<6;i++) {
        draw_rect(315,spikes_l[i].y,5,15,0x7);

        // now right-side spike wall
        draw_rect(0,spikes_r[i].y,5,15,0x7);
        
        if (!cheat) {
            if (pX < 0 + 5 && pX + sizeX > 0 && pY < spikes_l[i].y + 15 && pY + 15 > spikes_l[i].y) {
                dead = true;
                rage.active = true;
                rage.x = pX;
                rage.y = pY;
            }
            if (pX < 315 + 5 && pX + sizeX > 315 && pY < spikes_r[i].y + 15 && pY + 15 > spikes_r[i].y) {
                dead = true;
                rage.active = true;
                rage.x = pX;
                rage.y = pY;
            }
        }

    }

    // we do a little trolling
    if (rage.active) {
        for (int x=0;x<64;x++) {
            for (int y=0;y<64;y++) {
                int ptr = (y*64)+x;
                int vptr = ((y+rage.y)*320)+x+rage.x;
                int v = ragebait[ptr];
                if (v == 1) {
                    working_buffer[vptr] = 0xf;
                } else if (v == 2) {
                    working_buffer[vptr] = 0x0;
                }
                
            }
        }
    }

    needSwitch = true;

    // write score to screen
    int dsp = score;
    int ind = 0;
    
    for (int i=0;i<5;i++) {
        int p = (8*5)-(ind*8);
        drawchar(25+p,10, (dsp % 10) + '0', 0x1f);
        dsp /= 10;
        ind++;
    }



    if (dead) {
        draw_rect(0,0,320,(deadtimer/16)*16,0x4);
        draw_rect(0,((deadtimer/8)*8)-8,320,8,0x7);

        int ind2 = 0;
        int start = (320/2)-40;
        uint8_t color = 0xf;
        if ((((deadtimer/15)*15)%2 == 0) && deadtimer <= 200) {
            color = 0xe;
        }
        while (death[ind2]) {
            drawchar(start+(ind2*8), 100-8, death[ind2], color);
            ind2++;
        }
    }

    if (won) {
        draw_rect(0,0,320,(deadtimer/16)*16,0x2);
        draw_rect(0,((deadtimer/8)*8)-8,320,8,0x7);

        int ind2 = 0;
        int start = (320/2)-40;
        uint8_t color = 0xf;
        if ((((deadtimer/15)*15)%2 == 0) && deadtimer <= 200) {
            color = 0xe;
        }
        while (win[ind2]) {
            drawchar(start+(ind2*8), 100-8, win[ind2], color);
            ind2++;
        }
    }


    callSwitch();
    if (deadtimer > 300 && !won) {
        pX = 320/2;
        pY = 200/2;
        dead = false;
        deadtimer = 0;
        score=0;

        rage.active = false;
    }
}

extern "C" void kmain() {
    // populate spikes @ runtime
    for (int i=0;i<5;i++) {
        spikes_l[i].y = i*200/5;
        spikes_l[i].dir = i%2==0;
    }
    for (int i=0;i<5;i++) {
        spikes_r[i].y = i*200/5;
        spikes_r[i].dir = i%2==0;
    }

    for (int i=0;i<32;i++) {
        coins[i].x = random_between(25,295);
        coins[i].y = random_between(25,175);

        coins[i].active = true;
    }

    kbd_hook = testkey;
    ch0_hook = gameloop;

    initPIT(30); // 30 FPS
    idt_install();
    asm volatile("sti");

    while(1){}
    return;
}