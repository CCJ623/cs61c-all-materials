#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "lfsr.h"

void lfsr_calculate(uint16_t *reg) {
  /* YOUR CODE HERE */
  uint16_t shift_in = (*reg) & 1;
  shift_in ^= (*reg >> 2) & 1;
  shift_in ^= (*reg >> 3) & 1;
  shift_in ^= (*reg >> 5) & 1;
  *reg = (*reg >> 1) + (shift_in << 15);
}
