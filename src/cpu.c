#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "cpu.h"


cpu_t* init_cpu()
{
    cpu_t *new_cpu = (cpu_t *)calloc(1, sizeof(cpu_t));
    new_cpu->reg_a = 0;
    new_cpu->reg_status = 0;
    new_cpu->program_counter = 0;
    return new_cpu;
}

void free_cpu(cpu_t *cpu)
{
    free(cpu);
}

uint8_t mem_read(cpu_t *cpu, uint16_t address)
{
    return cpu->memory[address];
}

void mem_write(cpu_t *cpu, uint16_t address, uint8_t data)
{
    cpu->memory[address] = data;
}

uint16_t mem_read_16(cpu_t *cpu, uint16_t address)
{
    uint16_t low_byte = mem_read(cpu, address);
    uint16_t high_byte = mem_read(cpu, address + 1);
    return (high_byte << 8) | low_byte;

}

void mem_write_16(cpu_t *cpu, uint16_t address, uint16_t data)
{
    uint8_t low_byte = (data & 0xFF);
    uint8_t high_byte = (data >> 8);
    mem_write(cpu, address, low_byte);
    mem_write(cpu, address + 1, high_byte);
}

void reset(cpu_t *cpu)
{
    cpu->reg_a = 0;
    cpu->reg_x = 0;
    cpu->reg_status = 0;
    cpu->program_counter = mem_read_16(cpu, 0xFFFC);
}

void load(cpu_t *cpu, int *program, int program_size)
{
    memcpy(&cpu->memory[0x8000], program, program_size * sizeof(int));
    mem_write_16(cpu, 0xFFFC, 0x8000);
}

void set_flags(cpu_t *cpu, uint8_t result)
{
    if(result == 0)
        cpu->reg_status = cpu->reg_status | 0b00000010;
    else
        cpu->reg_status = cpu->reg_status & 0b11111101;
    
    if((result & 0b10000000) != 0)
        cpu->reg_status = cpu->reg_status | 0b10000000;
    else
        cpu->reg_status = cpu->reg_status & 0b01111111;
}

void run(cpu_t *cpu)
{
    while(true)
    {
        int opcode = mem_read(cpu, cpu->program_counter);
        cpu->program_counter += 1;

        switch(opcode)
        {
            case 0xA9:
                int param = mem_read(cpu, cpu->program_counter);
                cpu->program_counter += 1;
                cpu->reg_a = param;
                set_flags(cpu, cpu->reg_a);
                break;
            case 0xAA:
                cpu->reg_x = cpu->reg_a;
                set_flags(cpu, cpu->reg_x);
                break;
            case 0xE8:
                cpu->reg_x = cpu->reg_x + 1;
                set_flags(cpu, cpu->reg_x);
                break;
            case 0x00:
                return;
            default:
                return;              
        }
    }
}

void load_and_run(cpu_t *cpu, int *program, int program_size)
{
    load(cpu, program, program_size);
    reset(cpu);
    run(cpu);
}