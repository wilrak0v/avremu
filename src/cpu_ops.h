#ifndef CPU_OPS_H
#define CPU_OPS_H

#include <stdint.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "cpu.h"

#define SREG_ADDR 0x5F
#define BIT_C 0
#define BIT_Z 1
#define BIT_N 2
#define BIT_V 3

void decode_ldi(uint16_t opcode, Atmega328p *mcu);
void decode_add(uint16_t opcode, Atmega328p *mcu);
void dump_state(Atmega328p *mcu);

#endif
