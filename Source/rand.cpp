unsigned long long seed = 12345;  // You can change the seed value for variability
const unsigned long long a = 1664525;
const unsigned long long c = 1013904223;
const unsigned long long m = 4294967296;  // 2^32 (to keep numbers within the 32-bit range)

// Linear Congruential Generator (LCG)
unsigned int lcg_random() {
    seed = (a * seed + c) % m;
    return (unsigned int)seed;  // Return the random value (32-bit)
}

// Function to generate a random number between X and Y
int random_between(int x, int y) {
    unsigned int rand_val = lcg_random();
    // Scale the random value to be between X and Y
    int range = y - x + 1;
    return (rand_val % range) + x;
}