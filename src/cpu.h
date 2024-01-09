#include <stdint.h>

struct cpu
{
    uint8_t reg_a;
    uint8_t reg_x;
    uint8_t reg_status;
    uint16_t program_counter;
};

typedef struct cpu cpu_t;

cpu_t *init_cpu();

void free_cpu(cpu_t *cpu);

void interpret(cpu_t* cpu, int* program);