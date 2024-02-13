#include <stdint.h>

enum CPUFlags {
    CARRY = 0b00000001,
    ZERO = 0b00000010,
    INTERRUPT_DISABLE = 0b00000100,
    DECIMAL_MODE = 0b00001000,
    BREAK = 0b00010000,
    OVERFLOW = 0b01000000,
    NEGATIVE = 0b10000000
};

struct cpu
{
    uint8_t reg_a;
    uint8_t reg_x;
    uint8_t reg_y;
    uint8_t reg_status;
    uint16_t program_counter;
    uint8_t stack_pointer;
    // Array representing 64KB of memory
    int memory[0xFFFF];
};

typedef struct cpu cpu_t;

cpu_t *init_cpu();

void free_cpu(cpu_t *cpu);

void load_and_run(cpu_t *cpu, int *program, int program_size);