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

int get_sreg_bit(Atmega328p *mcu, int bit)
{
    return (mcu->ram[SREG_ADDR] >> bit) & 1;
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

    uint8_t old_d = mcu->ram[d];
    uint16_t result = (uint16_t)mcu->ram[d] + (uint16_t)mcu->ram[r];
    mcu->ram[d] = (uint8_t)result;

    set_sreg_bit(mcu, BIT_Z, (uint8_t)result == 0);
    set_sreg_bit(mcu, BIT_N, (result & 0x80) != 0);
    set_sreg_bit(mcu, BIT_C, (result & 0x0100) != 0);
    int v = ((mcu->ram[d] & 0x80) == (mcu->ram[r] & 0x80)) && ((result & 0x80) != (old_d & 0x80));
    set_sreg_bit(mcu, BIT_V, v);

    printf("ADD R%d, R%d | Nouveau contenu de R%d : %d\n", d, r, d, mcu->ram[d]);
}

void decode_rjmp(uint16_t opcode, Atmega328p *mcu)
{
    // RJMP => PC + k + 1
    // 1100 kkkk kkkk kkkk
    int16_t k = opcode & 0x0FFF;
    if (k & 0x0800)
        k -= 4096;
    mcu->pc += k;
    //printf("RJMP %d\n", mcu->pc);
}

void decode_cp(uint16_t opcode, Atmega328p *mcu)
{
    // 0011 dddd rrrr 
    uint16_t d = (opcode & 0x01F0) >> 4;
    uint16_t r = (opcode & 0x000F) | ((opcode & 0x0200) >> 5);
    printf("CP R%d, R%d\n", d, r);
    uint8_t result = mcu->ram[d] - mcu->ram[r];
    d = (int16_t)mcu->ram[d];
    r = (int16_t)mcu->ram[r];

    set_sreg_bit(mcu, BIT_Z, (result == 0));

    set_sreg_bit(mcu, BIT_N, (result & 0x80) != 0);

    set_sreg_bit(mcu, BIT_C, (r > d));

    int v = ((d & 0x80) && !(r & 0x80) && !(result & 0x80)) || (!(d & 0x80) && (r & 0x80) && (result & 0x80));
    set_sreg_bit(mcu, BIT_V, v);

    int n = (result & 0x80) != 0;
    set_sreg_bit(mcu, BIT_S, n ^ v);

    int h = (!(d & 0x08) && (r & 0x08)) || ((r & 0x08) && (result & 0x80)) || (!(d & 0x08) && (result & 0x08)); 
    set_sreg_bit(mcu, BIT_H, h);
}

void decode_breq(uint16_t opcode, Atmega328p *mcu)
{
    // 1111 00kk kkkk k001
    int16_t k = (opcode >> 3) & 0x7F;
    if (k & 0x40)
        k -= 128;
    if (get_sreg_bit(mcu, BIT_Z) == 1)
        mcu->pc = mcu->pc + k;
    printf("BREQ %d\n", k);
}

void decode_brne(uint16_t opcode, Atmega328p *mcu)
{
    // 1111 01kk kkkk k001
    int16_t k = (opcode >> 3) & 0x7F;
    if (k & 0x40)
        k -= 128;
    if (get_sreg_bit(mcu, BIT_Z) == 0)
        mcu->pc = mcu->pc + k - 1;
    printf("BRNE %d\n", k);
}

void dump_state(Atmega328p *mcu)
{
    printf("R16: %d ; R17: %d ; SREG: 0x%02X ; PC: %d\n", mcu->ram[16], mcu->ram[17], mcu->ram[SREG_ADDR], mcu->pc);
}
