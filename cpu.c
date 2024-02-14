#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "cpu.h"
#include "opcode.h"

const uint16_t STACK_BASE = 0x0100;
const uint8_t STACK_RESET = 0xfd;

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
        cpu->reg_status |= ZERO;
    else
        cpu->reg_status &= ~ZERO;
    
    if((result & 0b10000000) != 0)
        cpu->reg_status |= NEGATIVE;
    else
        cpu->reg_status &= ~NEGATIVE;
}

void set_reg_a(cpu_t *cpu, uint8_t val)
{
    cpu->reg_a = val;
    set_flags(cpu, cpu->reg_a);
}

void ldy(cpu_t *cpu, enum AddressingMode mode)
{
    uint16_t address = get_operand_address(cpu, mode);
    cpu->reg_y = mem_read(cpu, address);
    set_flags(cpu, cpu->reg_y);
}

void ldx(cpu_t *cpu, enum AddressingMode mode)
{
    uint16_t address = get_operand_address(cpu, mode);
    cpu->reg_x = mem_read(cpu, address);
    set_flags(cpu, cpu->reg_x);
}

void lda(cpu_t *cpu, enum AddressingMode mode)
{
    uint16_t address = get_operand_address(cpu, mode);
    uint8_t val = mem_read(cpu, address);
    set_reg_a(cpu, val);
}

void sta(cpu_t *cpu, enum AddressingMode mode)
{
    uint16_t address = get_operand_address(cpu, mode);
    mem_write(cpu, address, cpu->reg_a);
}

void and(cpu_t *cpu, enum AddressingMode mode)
{
    uint16_t address = get_operand_address(cpu, mode);
    uint8_t val = mem_read(cpu, address);
    set_reg_a(cpu, (cpu->reg_a & val));
}

void eor(cpu_t *cpu, enum AddressingMode mode)
{
    uint16_t address = get_operand_address(cpu, mode);
    uint8_t val = mem_read(cpu, address);
    set_reg_a(cpu, (cpu->reg_a ^ val));
}

void ora(cpu_t *cpu, enum AddressingMode mode)
{
    uint16_t address = get_operand_address(cpu, mode);
    uint8_t val = mem_read(cpu, address);
    set_reg_a(cpu, (cpu->reg_a | val));
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

void iny(cpu_t *cpu, enum AddressingMode mode)
{
    cpu->reg_y = cpu->reg_y + 1;
    set_flags(cpu, cpu->reg_y);
}

void set_carry_flag(cpu_t *cpu)
{
    cpu->reg_status |= CARRY;
}

void clear_carry_flag(cpu_t *cpu)
{
    cpu->reg_status &= ~CARRY;
}

void add_to_reg_a(cpu_t *cpu, uint8_t data)
{
    uint16_t sum = (uint16_t)cpu->reg_a + (uint16_t)data;
    if(cpu->reg_status & CARRY != 0)
    {
        sum += 1;
    }
    bool carry = sum > 0xff;
    if(carry)
    {
        cpu->reg_status |= CARRY;
    }
    else
    {
        cpu->reg_status &= ~CARRY;
    }
    uint8_t result = (uint8_t)sum;
    if(((data ^ result) & (result ^ cpu->reg_a) & 0x80) != 0)
    {
        cpu->reg_status |= OVERFLOW;
    }
    else
    {
        cpu->reg_status &= ~OVERFLOW;
    }
    set_reg_a(cpu, result);
}

void sbc(cpu_t *cpu, enum AddressingMode mode)
{
    uint16_t address = get_operand_address(cpu, mode);
    uint8_t data = mem_read(cpu, address);
    add_to_reg_a(cpu, (~data + 1));
}

void adc(cpu_t *cpu, enum AddressingMode mode)
{
    uint16_t address = get_operand_address(cpu, mode);
    uint8_t val = mem_read(cpu, address);
    add_to_reg_a(cpu, val);
}

uint8_t stack_pop(cpu_t *cpu)
{
    cpu->stack_pointer += 1;
    return mem_read(cpu, (STACK_BASE + (uint16_t)cpu->stack_pointer));
}

void stack_push(cpu_t *cpu, uint8_t data)
{
    mem_write(cpu, (STACK_BASE + (uint16_t)cpu->stack_pointer), data);
    cpu->stack_pointer -= 1;
}

void stack_push_16(cpu_t *cpu, uint16_t data)
{
    uint8_t hi = (uint8_t)(data >> 8);
    uint8_t lo = (uint8_t)(data & 0xff);
    stack_push(cpu, hi);
    stack_push(cpu, lo);
}

uint16_t stack_pop_16(cpu_t *cpu)
{
    uint8_t lo = (uint16_t)stack_pop(cpu);
    uint8_t hi = (uint16_t)stack_pop(cpu);
    return (((uint16_t)hi << 8) | (uint16_t)lo);
}

void asl_accumulator(cpu_t *cpu)
{
    uint8_t data = cpu->reg_a;
    if ((data >> 7) == 1)
    {
        set_carry_flag(cpu);
    }
    else
    {
        clear_carry_flag(cpu);
    }
    data = data << 1;
    set_reg_a(cpu, data);
}

uint8_t asl(cpu_t *cpu, enum AddressingMode mode)
{
    uint16_t address = get_operand_address(cpu, mode);
    uint8_t data = mem_read(cpu, address);
    if((data >> 7) == 1)
    {
        set_carry_flag(cpu);
    }
    else
    {
        clear_carry_flag(cpu);
    }
    data = data << 1;
    mem_write(cpu, address, data);
    set_flags(cpu, data);
    return data;
}

void lsr_accumulator(cpu_t *cpu)
{
    uint8_t data = cpu->reg_a;
    if ((data & 1) == 1)
    {
        set_carry_flag(cpu);
    }
    else
    {
        clear_carry_flag(cpu);
    }
    data = data >> 1;
    set_reg_a(cpu, data);
}

uint8_t lsr(cpu_t *cpu, enum AddressingMode mode)
{
    uint16_t address = get_operand_address(cpu, mode);
    uint8_t data = mem_read(cpu, address);
    if((data & 1) == 1)
    {
        set_carry_flag(cpu);
    }
    else
    {
        clear_carry_flag(cpu);
    }
    data = data >> 1;
    mem_write(cpu, address, data);
    set_flags(cpu, data);
    return data;
}

uint8_t rol(cpu_t *cpu, enum AddressingMode mode)
{
    uint16_t address = get_operand_address(cpu, mode);
    uint8_t data = mem_read(cpu, address);
    bool carry = ((cpu->reg_status & CARRY) != 0);
    if((data >> 7) == 1)
    {
        set_carry_flag(cpu);
    }
    else
    {
        clear_carry_flag(cpu);
    }
    data = data << 1;
    if(carry)
    {
        data = data | 1;
    }
    mem_write(cpu, address, data);
    set_flags(cpu, data);
    return data;
}

void rol_accumulator(cpu_t *cpu)
{
    uint8_t data = cpu->reg_a;
    bool carry = ((cpu->reg_status & CARRY) != 0);
    if((data >> 7) == 1)
    {
        set_carry_flag(cpu);
    }
    else
    {
        clear_carry_flag(cpu);
    }
    data = data << 1;
    if(carry)
    {
        data = data | 1;
    }
    set_reg_a(cpu, data);
}

uint8_t ror(cpu_t *cpu, enum AddressingMode mode)
{
    uint16_t address = get_operand_address(cpu, mode);
    uint8_t data = mem_read(cpu, address);
    bool carry = ((cpu->reg_status & CARRY) != 0);
    if((data & 1) == 1)
    {
        set_carry_flag(cpu);
    }
    else
    {
        clear_carry_flag(cpu);
    }
    data = data >> 1;
    if(carry)
    {
        data = data | NEGATIVE;
    }
    mem_write(cpu, address, data);
    set_flags(cpu, data);
    return data;
}

void ror_accumulator(cpu_t *cpu)
{
    uint8_t data = cpu->reg_a;
    bool carry = ((cpu->reg_status & CARRY) != 0);
    if ((data & 1) == 1)
    {
        set_carry_flag(cpu);
    }
    else
    {
        clear_carry_flag(cpu);
    }
    data = data >> 1;
    if(carry)
    {
        data = data | NEGATIVE;
    }
    set_reg_a(cpu, data);
}

uint8_t inc(cpu_t *cpu, enum AddressingMode mode)
{
    uint16_t address = get_operand_address(cpu, mode);
    uint8_t data = mem_read(cpu, address);
    data = data + 1;
    mem_write(cpu, address, data);
    set_flags(cpu, data);
    return data;
}

void dey(cpu_t *cpu)
{
    cpu->reg_y = cpu->reg_y - 1;
    set_flags(cpu, cpu->reg_y);
}

void dex(cpu_t *cpu)
{
    cpu->reg_x = cpu->reg_x - 1;
    set_flags(cpu, cpu->reg_x);
}

uint8_t dec(cpu_t *cpu, enum AddressingMode mode)
{
    uint16_t address = get_operand_address(cpu, mode);
    uint8_t data = mem_read(cpu, address);
    data = data - 1;
    mem_write(cpu, address, data);
    set_flags(cpu, data);
    return data;
}

void pla(cpu_t *cpu)
{
    uint8_t data = stack_pop(cpu);
    set_flags(cpu, data);
}

void plp(cpu_t *cpu)
{
    cpu->reg_status = stack_pop(cpu);
    cpu->reg_status &= ~BREAK;
    cpu->reg_status |= BREAK2;
}

void php(cpu_t *cpu)
{
    uint8_t flags = cpu->reg_status;
    flags |= BREAK;
    flags |= BREAK2;
    stack_push(cpu, flags);
}

void bit(cpu_t *cpu, enum AddressingMode mode)
{
    uint16_t address = get_operand_address(cpu, mode);
    uint8_t data = mem_read(cpu, address);
    uint8_t res = cpu->reg_a & data;
    if(res == 0)
    {
        cpu->reg_status |= ZERO;
    }
    else
    {
        cpu->reg_status &= ~ZERO;
    }
    if((data & NEGATIVE) > 0)
    {
        cpu->reg_status |= NEGATIVE;
    }
    if((data & OVERFLOW) > 0)
    {
        cpu->reg_status |= OVERFLOW;
    }
}

void compare(cpu_t *cpu, enum AddressingMode mode, uint8_t comp)
{
    uint16_t address = get_operand_address(cpu, mode);
    uint8_t data = mem_read(cpu, address);
    if (data <= comp)
    {
        cpu->reg_status |= CARRY;
    }
    else
    {
        cpu->reg_status &= ~CARRY;
    }
    set_flags(cpu, (comp - data));
}

void branch(cpu_t *cpu, bool condition)
{
    if (condition)
    {
        int8_t offset = (int8_t)mem_read(cpu, cpu->program_counter);
        uint16_t new_pc = ((cpu->program_counter + 1) + (uint16_t)offset);
        cpu->program_counter = new_pc;
    }
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
            case 0xD8:
                cpu->reg_status &= ~DECIMAL_MODE;
                break;
            case 0x58:
                cpu->reg_status &= ~INTERRUPT_DISABLE;
                break;
            case 0xB8:
                cpu->reg_status &= ~OVERFLOW;
                break;
            case 0x18:
                clear_carry_flag(cpu);
                break;
            case 0x38:
                set_carry_flag(cpu);
                break;
            case 0x78:
                cpu->reg_status |= INTERRUPT_DISABLE;
                break;
            case 0xF8:
                cpu->reg_status |= DECIMAL_MODE;
                break;
            case 0x48:
                stack_push(cpu, cpu->reg_a);
                break;
            case 0x68:
                pla(cpu);
                break;
            case 0x08:
                php(cpu);
                break;
            case 0x28:
                plp(cpu);
                break;
            case 0x69:
            case 0x65:
            case 0x75:
            case 0x6D:
            case 0x7D:
            case 0x79:
            case 0x61:
            case 0x71:
                adc(cpu, op.mode);
                break;
            case 0xE9:
            case 0xE5:
            case 0xF5:
            case 0xED:
            case 0xFD:
            case 0xF9:
            case 0xE1:
            case 0xF1:
                sbc(cpu, op.mode);
                break;
            case 0x29:
            case 0x25:
            case 0x35:
            case 0x2D:
            case 0x3D:
            case 0x39:
            case 0x21:
            case 0x31:
                and(cpu, op.mode);
                break;
            case 0x49:
            case 0x45:
            case 0x55:
            case 0x4D:
            case 0x5D:
            case 0x59:
            case 0x41:
            case 0x51:
                eor(cpu, op.mode);
                break;
            case 0x09:
            case 0x05:
            case 0x15:
            case 0x0D:
            case 0x1D:
            case 0x19:
            case 0x01:
            case 0x11:
                ora(cpu, op.mode);
                break;
            case 0x4A:
                lsr_accumulator(cpu);
                break;
            case 0x46:
            case 0x56:
            case 0x4E:
            case 0x5E:
                lsr(cpu, op.mode);
                break;
            case 0x0A:
                asl_accumulator(cpu);
                break;
            case 0x06:
            case 0x16:
            case 0x0E:
            case 0x1E:
                asl(cpu, op.mode);
                break;
            case 0x2A:
                rol_accumulator(cpu);
                break;
            case 0x26:
            case 0x36:
            case 0x2E:
            case 0x3E:
                rol(cpu, op.mode);
                break;
            case 0x6A:
                ror_accumulator(cpu);
                break;
            case 0x66:
            case 0x76:
            case 0x6E:
            case 0x7E:
                ror(cpu, op.mode);
                break;
            case 0xE6:
            case 0xF6:
            case 0xEE:
            case 0xFE:
                inc(cpu, op.mode);
                break;
            case 0xC8:
                iny(cpu, op.mode);
                break;
            case 0xC6:
            case 0xD6:
            case 0xCE:
            case 0xDE:
                dec(cpu, op.mode);
                break;
            case 0xCA:
                dex(cpu);
                break;
            case 0x88:
                dey(cpu);
                break;
            case 0xC9:
            case 0xC5:
            case 0xD5:
            case 0xCD:
            case 0xDD:
            case 0xD9:
            case 0xC1:
            case 0xD1:
                compare(cpu, op.mode, cpu->reg_a);
                break;
            case 0xC0:
            case 0xC4:
            case 0xCC:
                compare(cpu, op.mode, cpu->reg_y);
                break;
            case 0xE0:
            case 0xE4:
            case 0xEC:
                compare(cpu, op.mode, cpu->reg_x);
                break;
            case 0x4C:{
                uint16_t addr = mem_read_16(cpu, cpu->program_counter);
                cpu->program_counter = addr;
                break;}
            case 0x6C:{
                uint16_t addr = mem_read_16(cpu, cpu->program_counter);
                if((addr & 0x00FF) == 0x00FF)
                {
                    uint8_t lo = mem_read(cpu, addr);
                    uint8_t hi = mem_read(cpu, (addr & 0xFF00));
                    cpu->program_counter = (((uint16_t)hi << 8) | (uint16_t) lo);
                }
                else
                {
                    cpu->program_counter = mem_read_16(cpu, addr);
                }
                break;}
            case 0x20:{
                stack_push_16(cpu, (cpu->program_counter + 2 - 1));
                uint16_t addr = mem_read_16(cpu, cpu->program_counter);
                cpu->program_counter = addr;
                break;}
            case 0x60:
                cpu->program_counter = (stack_pop_16(cpu) + 1);
                break;
            case 0x40:
                cpu->reg_status = stack_pop(cpu);
                cpu->reg_status &= ~BREAK;
                cpu->reg_status |= BREAK2;
                cpu->program_counter = stack_pop_16(cpu);
                break;
            case 0xD0:
                branch(cpu , ((cpu->reg_status & ZERO) == 0)); 
                break;
            case 0x70:
                branch(cpu, ((cpu->reg_status & OVERFLOW) > 0));
                break;
            case 0x50:
                branch(cpu, ((cpu->reg_status & OVERFLOW) == 0));
                break;
            case 0x10:
                branch(cpu, ((cpu->reg_status & NEGATIVE) == 0));
                break;
            case 0x30:
                branch(cpu, ((cpu->reg_status & NEGATIVE) > 0));
                break;
            case 0xF0:
                branch(cpu, ((cpu->reg_status & ZERO) > 0));
                break;
            case 0xB0:
                branch(cpu, ((cpu->reg_status & CARRY) > 0));
                break;
            case 0x90:
                branch(cpu, ((cpu->reg_status & CARRY) == 0));
                break;
            case 0x24:
            case 0x2C:
                bit(cpu, op.mode);
                break;
            case 0x86:
            case 0x96:
            case 0x8E:{
                uint16_t addr = get_operand_address(cpu, op.mode);
                mem_write(cpu, addr, cpu->reg_x);
                break;}
            case 0x84:
            case 0x94:
            case 0x8C:{
                uint16_t addr = get_operand_address(cpu, op.mode);
                mem_write(cpu, addr, cpu->reg_y);
                break;}
            case 0xA2:
            case 0xA6:
            case 0xB6:
            case 0xAE:
            case 0xBE:
                ldx(cpu, op.mode);
                break;
            case 0xA0:
            case 0xA4:
            case 0xB4:
            case 0xAC:
            case 0xBC:
                ldy(cpu, op.mode);
                break;
            case 0xEA:
                break;
            case 0xA8:
                cpu->reg_y = cpu->reg_a;
                set_flags(cpu, cpu->reg_y);
                break;
            case 0xBA:
                cpu->reg_x = cpu->stack_pointer;
                set_flags(cpu, cpu->reg_x);
                break;
            case 0x8A:
                cpu->reg_a = cpu->reg_x;
                set_flags(cpu, cpu->reg_a);
                break;
            case 0x9A:
                cpu->stack_pointer = cpu->reg_x;
                break;
            case 0x98:
                cpu->reg_a = cpu->reg_y;
                set_flags(cpu, cpu->reg_a);
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