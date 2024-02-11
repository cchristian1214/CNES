#include "opcode.h"

opcode_t opcode_new(uint8_t code, char *name, int len, int cycles, enum AddressingMode mode)
{
    opcode_t op;
    op.code = code;
    op.name = name;
    op.len = len;
    op.cycles = cycles;
    op.mode = mode;
    return op;
}

static opcode_t codes[256];

void fill_codes()
{
    codes[0x00] = opcode_new(0x00, "BRK", 1, 7, NONE_ADDRESSING);
    codes[0xaa] = opcode_new(0xaa, "TAX", 1, 2, NONE_ADDRESSING);
    codes[0xe8] = opcode_new(0xe8, "INX", 1, 2, NONE_ADDRESSING);
    codes[0xa9] = opcode_new(0xa9, "LDA", 2, 2, IMMEDIATE);
    codes[0xa5] = opcode_new(0xa5, "LDA", 2, 3, ZERO_PAGE);
    codes[0xb5] = opcode_new(0xb5, "LDA", 2, 4, ZERO_PAGE_X);
    codes[0xad] = opcode_new(0xad, "LDA", 3, 4, ABSOLUTE);
    codes[0xbd] = opcode_new(0xbd, "LDA", 3, 4, ABSOLUTE_X);
    codes[0xb9] = opcode_new(0xb9, "LDA", 3, 4, ABSOLUTE_Y);
    codes[0xa1] = opcode_new(0xa1, "LDA", 2, 6, INDIRECT_X);
    codes[0xb1] = opcode_new(0xb1, "LDA", 2, 5, INDIRECT_Y);
    codes[0x85] = opcode_new(0x85, "STA", 2, 3, ZERO_PAGE);
    codes[0x95] = opcode_new(0x95, "STA", 2, 4, ZERO_PAGE_X);
    codes[0x8d] = opcode_new(0x8d, "STA", 3, 4, ABSOLUTE);
    codes[0x9d] = opcode_new(0x9d, "STA", 3, 5, ABSOLUTE_X);
    codes[0x99] = opcode_new(0x99, "STA", 3, 5, ABSOLUTE_Y);
    codes[0x81] = opcode_new(0x81, "STA", 2, 6, INDIRECT_X);
    codes[0x91] = opcode_new(0x91, "STA", 2, 6, INDIRECT_Y);
}

opcode_t opcode_lookup(uint8_t code)
{
    fill_codes();
    return codes[code];
}