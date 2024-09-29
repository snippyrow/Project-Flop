// Output value to I/O port, 8 bits
void outb(short Port, char Value){
    asm volatile("outb %1, %0" : : "dN" (Port), "a" (Value));
}


// Get input for a specified I/O port, 8 bits
unsigned char inb(unsigned short port) {
    unsigned char result;
    __asm__("in %%dx, %%al" : "=a" (result) : "d" (port));
    return result;
}

void memcpy(int *source, int *dest, int nbytes) { // copy memory from A to B
    int i;
    for (i = 0; i < nbytes; i++) {
        *(dest + i) = *(source + i);
    }
}