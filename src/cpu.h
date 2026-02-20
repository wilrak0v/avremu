#ifndef CPU_H
#define CPU_H

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define FLASH_SIZE 16384
#define RAM_SIZE 2304

typedef struct {
    uint16_t flash[FLASH_SIZE];
    uint8_t ram[RAM_SIZE];
    uint16_t pc;
    uint16_t sp;
} Atmega328p;


#endif
