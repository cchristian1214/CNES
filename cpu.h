#include <stdint.h>

struct cpu
{
    uint8_t reg_a;
    uint8_t reg_x;
    uint8_t reg_y;
    uint8_t reg_status;
    uint16_t program_counter;
    // Array representing 64KB of memory
    int memory[0xFFFF];
};

typedef struct cpu cpu_t;

cpu_t *init_cpu();

void free_cpu(cpu_t *cpu);

void load_and_run(cpu_t *cpu, int *program, int program_size);