#include "stdint.h"

typedef struct {
    uint16_t offset_lo; // Address of function hook, low end
    uint16_t selector;  // Segment selector (0x8 for code seg)
    uint8_t always0;    // Reserved
    uint8_t flags;      // Flags
    uint16_t offset_hi; // Address of function hook, high end
} __attribute__((packed)) idt_gate;

typedef struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) idt_reg;

idt_gate idt[256];

idt_reg idt_desc;