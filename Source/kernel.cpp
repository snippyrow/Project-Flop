#include "Interrupts/idt.cpp"

int vY = 0;
int vX = 4;
int pX = 50;
int pY = 0;

int sizeX = 10;
int sizeY = 10;

bool needSwitch = true;

unsigned char *video_buffer = (unsigned char *)0xa0000;
unsigned char *bg = (unsigned char *)0xaaaa; // a0000

void outb2(short Port, char Value){
    asm volatile("outb %1, %0" : : "dN" (Port), "a" (Value));
}

void putpixel(uint32_t x, uint32_t y, uint8_t color) {
    video_buffer[(y*320) + x] = color;
}

void testkey() {
    uint8_t scancode = inb(0x60);
    if ((int)scancode == 0x39) {
        video_buffer[0] = 0x1f;
        vY = -20;
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

unsigned char *working_buffer = (unsigned char *)0xfffff; // why 0xfffff? I have no idea.

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

void drawchar(int x, int y, uint8_t character) {
    /*
    int cX = character/16;
    int cY = character%16;

    int ptr_start = (cY*8*144)+(cX*144);

    for (int i=0;i<64;i++) {
        int row = (cX)+(i/8);
        int col = (cY)+(i%8);

        uint8_t var = bg[((y1*144)+x1)+ptr_start];
        if (!(var/2)) {
            working_buffer[(((y+y1)*320)+x+x1)] = 0x1f;
        }


    }
    */
    
    /*
    for (int x1=0;x1<8;x1++) {
        for (int y1=0;y1<8;y1++) {
            uint8_t var = bg[((y1*144)+x1)+ptr_start];
            if (!(var/2)) {
                working_buffer[(((y+y1)*320)+x+x1)] = 0x1f;
            }
        }
    }
    
    for (int i=0;i<512*40;i++) {
        int row = i / 144*2; // Integer division gives the row
        int col = i % 144*2; // Modulo gives the column
        uint8_t var = bg[i];
        if (!(var/2)) {
            working_buffer[((row*320) + col)] = 0x1f;
            working_buffer[(((row+1)*320) + col)] = 0x1f;
            working_buffer[((row*320) + col+1)] = 0x1f;
            working_buffer[(((row+1)*320) + col+1)] = 0x1f;
        }

    }
    */
}

int trail[32][2] = {
    
};

bool dead = false;
int deadtimer = 0;

void gameloop() {
    renderclear(0);
    if (!dead) {
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
    } else if (pY < 0) {
        dead = true;
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
    for (int x=0;x<sizeX;x++) {
        for (int y=0;y<sizeY;y++) {
            working_buffer[((y+pY)*320)+x+pX] = 4;
        }
    }
    // render spikes
    for (int i=0;i<6;i++) {
        draw_rect(315,spikes_l[i].y,5,15,0x7);

        // now right-side spike wall
        draw_rect(0,spikes_r[i].y,5,15,0x7);
        
        if (pX < 0 + 5 && pX + sizeX > 0 && pY < spikes_l[i].y + 15 && pY + 15 > spikes_l[i].y) {
            dead = true;
        }
        if (pX < 315 + 5 && pX + sizeX > 315 && pY < spikes_r[i].y + 15 && pY + 15 > spikes_r[i].y) {
            dead = true;
        }

    }
    needSwitch = true;

    if (dead) {
        draw_rect(0,0,320,(deadtimer/16)*16,0x4);
        draw_rect(0,((deadtimer/8)*8)-8,320,8,0x7);
    }

    //drawchar(50,50, 'G');
    callSwitch();
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
    kbd_hook = testkey;
    ch0_hook = gameloop;

    initPIT(30); // 30 FPS
    idt_install();
    asm volatile("sti");

    while(1){}
    return;
}