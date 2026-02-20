#include "cpu.h"
#include "cpu_ops.h"
#include <stdint.h>

void set_sreg_bit(Atmega328p *mcu, int bit, int value)
{
    if (value)
        mcu->ram[SREG_ADDR] |= (1 << bit);
    else
        mcu->ram[SREG_ADDR] &= ~(1 << bit);
}

void decode_ldi(uint16_t opcode, Atmega328p *mcu)
{
    // 1110 KKKK dddd KKKK
    //
    // On prend les bits 0-3 et 8-11
    uint8_t k = (opcode & 0x000F) | ((opcode & 0x0F00) >> 4);

    // Extraire le registre d
    // Les bits 4-7 (LDI ne travaille que sur R16 -> R31)
    uint16_t d = ((opcode & 0x00F0) >> 4) + 16;

    mcu->ram[d] = k;
    printf("LDI R%d, %d (0x%02X)\n", d, k, k);
}

void decode_add(uint16_t opcode, Atmega328p *mcu)
{
    // 0000 11rd dddd rrrr
    // rrrr -> Index du registre source
    // dddd -> Index du registre de destination

    // Extraire Rd
    uint8_t d = (opcode & 0x01F0) >> 4;

    // Extraire Rr
    uint8_t r = ((opcode & 0x0200) >> 5) | (opcode & 0x000F);

    uint16_t result = (uint16_t)mcu->ram[d] + (uint16_t)mcu->ram[r];
    mcu->ram[d] = (uint8_t)result;

    set_sreg_bit(mcu, BIT_Z, (uint8_t)result == 0);
    set_sreg_bit(mcu, BIT_N, (result & 0x80) != 0);
    set_sreg_bit(mcu, BIT_C, (result & 0x0100) != 0);
    int v = ((mcu->ram[d] & 0x80) == (mcu->ram[r] & 0x80)) && ((result & 0x80) != (mcu->ram[d] & 0x80));
    set_sreg_bit(mcu, BIT_V, v);

    printf("ADD R%d, R%d | Nouveau contenu de R%d : %d\n", d, r, d, mcu->ram[d]);
}

void dump_state(Atmega328p *mcu)
{
    printf("R16: %d ; R17: %d ; SREG: 0x%02X\n", mcu->ram[16], mcu->ram[17], mcu->ram[SREG_ADDR]);
}
