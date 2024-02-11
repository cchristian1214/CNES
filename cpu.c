#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "cpu.h"
#include "opcode.h"


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

uint16_t get_operand_address(cpu_t *cpu, enum AddressingMode mode)
{
    uint16_t address;
    switch(mode)
    {
        case IMMEDIATE:
            address = cpu->program_counter;
            break;
        case ZERO_PAGE:
            address = (uint16_t)mem_read(cpu, cpu->program_counter);
            break;
        case ABSOLUTE:
            address = mem_read_16(cpu, cpu->program_counter);
            break;
        case ZERO_PAGE_X:
            address = (uint16_t)(mem_read(cpu, cpu->program_counter) + cpu->reg_x);
            break;
        case ZERO_PAGE_Y:
            address = (uint16_t)(mem_read(cpu, cpu->program_counter) + cpu->reg_y);
            break;
        case ABSOLUTE_X:
            address = (mem_read_16(cpu, cpu->program_counter) + (uint16_t)cpu->reg_x);
            break;
        case ABSOLUTE_Y:
            address = (mem_read_16(cpu, cpu->program_counter) + (uint16_t)cpu->reg_y);
            break;
        case INDIRECT_X: {
            uint8_t base = mem_read(cpu, cpu->program_counter);
            uint8_t ptr = base + cpu->reg_x;
            uint8_t lo = mem_read(cpu, (uint16_t)ptr);
            uint8_t hi = mem_read(cpu, (uint16_t)(ptr + 1));
            address = ((uint16_t)hi << 8) | (uint16_t)lo;
            break; }
        case INDIRECT_Y: {
            uint8_t base = mem_read(cpu, cpu->program_counter);
            uint8_t lo = mem_read(cpu, (uint16_t)base);
            uint8_t hi = mem_read(cpu, (uint16_t)(base + 1));
            uint16_t deref_base = ((uint16_t)hi << 8) | (uint16_t)lo;
            address = deref_base + (uint16_t)cpu->reg_y;
            break; }
        default:
            exit(1);
    }
    return address;
}

void reset(cpu_t *cpu)
{
    cpu->reg_a = 0;
    cpu->reg_x = 0;
    cpu->reg_y = 0;
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

void lda(cpu_t *cpu, enum AddressingMode mode)
{
    uint16_t address = get_operand_address(cpu, mode);
    cpu->reg_a = mem_read(cpu, address);
    set_flags(cpu, cpu->reg_a);
}

void sta(cpu_t *cpu, enum AddressingMode mode)
{
    uint16_t address = get_operand_address(cpu, mode);
    mem_write(cpu, address, cpu->reg_a);
}

void tax(cpu_t *cpu, enum AddressingMode mode)
{
    cpu->reg_x = cpu->reg_a;
    set_flags(cpu, cpu->reg_x);
}

void inx(cpu_t *cpu, enum AddressingMode mode)
{
    cpu->reg_x = cpu->reg_x + 1;
    set_flags(cpu, cpu->reg_x);
}

void run(cpu_t *cpu)
{
    while(true)
    {
        int code = mem_read(cpu, cpu->program_counter);
        cpu->program_counter += 1;
        uint16_t program_counter_state = cpu->program_counter;
        opcode_t op = opcode_lookup(code);

        switch(op.code)
        {
            case 0xA9:
            case 0xA5: 
            case 0xB5: 
            case 0xAD: 
            case 0xBD: 
            case 0xB9: 
            case 0xA1: 
            case 0xB1:
                lda(cpu, op.mode);
                break;
            case 0x85:
            case 0x95: 
            case 0x8D: 
            case 0x9D: 
            case 0x99: 
            case 0x81: 
            case 0x91:
                sta(cpu, op.mode);
                break;
            case 0xAA:
                tax(cpu, op.mode);
                break;
            case 0xE8:
                inx(cpu, op.mode);
                break;
            case 0x00:
                return;
            default:
                return;              
        }
        if(program_counter_state == cpu->program_counter)
        {
            cpu->program_counter += ((uint16_t)(op.len - 1));
        }
    }
}

void load_and_run(cpu_t *cpu, int *program, int program_size)
{
    load(cpu, program, program_size);
    reset(cpu);
    run(cpu);
}