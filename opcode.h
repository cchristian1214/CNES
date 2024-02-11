#include <stdint.h>
#include <string.h>

enum AddressingMode
{
    IMMEDIATE,
    ZERO_PAGE,
    ZERO_PAGE_X,
    ZERO_PAGE_Y,
    ABSOLUTE,
    ABSOLUTE_X,
    ABSOLUTE_Y,
    INDIRECT,
    INDIRECT_X,
    INDIRECT_Y,
    NONE_ADDRESSING
};

typedef struct OpCode
{
    uint8_t code;
    char *name;
    int len;
    int cycles;
    enum AddressingMode mode;
} opcode_t;

opcode_t opcode_new(uint8_t code, char *name, int len, int cycles, enum AddressingMode mode);

opcode_t opcode_lookup(uint8_t code);