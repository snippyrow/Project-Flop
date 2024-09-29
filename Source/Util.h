#define low_16(address) (short)((address) & 0xFFFF)
#define high_16(address) (short)(((address) >> 16) & 0xFFFF)

typedef struct {
    uint32_t y;
    bool dir;
} position;

position spikes_l[8];
position spikes_r[8];