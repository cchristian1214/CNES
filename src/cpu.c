#include <stdlib.h>
#include <stdbool.h>
#include "cpu.h"


cpu_t* init_cpu()
{
    cpu_t *new_cpu = malloc(sizeof(cpu_t));
    new_cpu->reg_a = 0;
    new_cpu->reg_status = 0;
    new_cpu->program_counter = 0;
    return new_cpu;
}

void free_cpu(cpu_t *cpu)
{
    free(cpu);
}

void interpret(cpu_t *cpu, int *program)
{
    cpu->program_counter = 0;

    while(true)
    {
        int opcode = program[cpu->program_counter];
        cpu->program_counter += 1;

        switch(opcode)
        {
            case 0xA9:
                int param = program[cpu->program_counter];
                cpu->program_counter += 1;
                cpu->reg_a = param;

                if(cpu->reg_a == 0)
                    cpu->reg_status = cpu->reg_status | 0b00000010;
                else
                    cpu->reg_status = cpu->reg_status & 0b11111101;
                
                if((cpu->reg_a & 0b10000000) != 0)
                    cpu->reg_status = cpu->reg_status | 0b10000000;
                else
                    cpu->reg_status = cpu->reg_status & 0b01111111;
            case 0x00:
                return;                
        }
    }
}