#define low_16(address) (short)((address) & 0xFFFF)
#define high_16(address) (short)(((address) >> 16) & 0xFFFF)

typedef struct {
    uint32_t y;
    bool dir;
} position;

typedef struct {
    uint32_t x;
    uint32_t y;
    bool active;
} coin;

position spikes_l[8];
position spikes_r[8];

coin coins[32];