/* 20/02/2026 -> start
 *
 * AvrEmulator --- written in C by wilrak0v
 *
 * Fonctionnement dans une boucle `while`:
 *  - fetch
 *  - decode
 *  - execute
 * */

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "cpu_ops.h"
#include "cpu.h"


void clear_memories(Atmega328p *mcu)
{
    memset(mcu->flash, 0, sizeof(mcu->flash));
    memset(mcu->ram, 0, sizeof(mcu->ram));
    mcu->pc = 0;
    mcu->sp = RAM_SIZE-1;
}

long get_file_size(FILE *f)
{
    fseek(f, 0, SEEK_END);
    return ftell(f);
}

int load_into_flash(const char *path, Atmega328p *mcu, int *size)
{
    FILE *prog = fopen(path, "rb");
    if (prog == NULL)
    {
        perror("Error");
        return -1;
    }

    *size = get_file_size(prog);
    if (*size > FLASH_SIZE)
    {
        printf("Error: the file is to biggest\n");
        return -1;
    }

    fseek(prog, 0, SEEK_SET);
    if (fread(mcu->flash, sizeof(uint16_t), *size / 2, prog) != (size_t)*size / 2)
    {
        perror("Error");
        return -1;
    }

    if (fclose(prog) != 0)
    {
        perror("Error");
        return -1;
    }
    return 0;
}

void decode_instruction(uint16_t opcode, Atmega328p *mcu)
{
    if (((opcode & 0xF000) >> 12) == 0xE)
        decode_ldi(opcode, mcu);
    else if ((opcode & 0xFC00) == 0x0C00)
        decode_add(opcode, mcu);
    else if ((opcode & 0xF000) == 0xC000)
        decode_rjmp(opcode, mcu);
    else if ((opcode & 0xFC00) == 0x1400)
        decode_cp(opcode, mcu);
    else if ((opcode & 0xFC07) == 0xF001)
        decode_breq(opcode, mcu);
    else if ((opcode & 0xFC07) == 0xF401)
        decode_brne(opcode, mcu);
    else if (opcode == 0x0000)
        printf("NOP\n");
    else
        printf("Error: unknow instruction at %04x : %04x\n", mcu->pc, opcode);
}

int main()
{
    Atmega328p mcu;
    clear_memories(&mcu);
    int size = 0;
    load_into_flash("test.bin", &mcu, &size);

    while(mcu.pc < size / 2)
    {
        // Fetch
        uint16_t instruction = mcu.flash[mcu.pc];
        mcu.pc++;
        // Decode
        decode_instruction(instruction, &mcu);
        // Execute
    }

    dump_state(&mcu);
    return 0;
}
