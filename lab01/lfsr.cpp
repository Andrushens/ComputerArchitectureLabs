#include <iostream>
#include "lfsr.h"

void lfsr_calculate(uint16_t *reg) {
    uint16_t bit = *reg;
    bit ^= *reg >> 2 & 1;
    bit ^= *reg >> 3 & 1;
    bit ^= *reg >> 5 & 1;
    *reg = (*reg >> 1) | (bit << 15);
}
