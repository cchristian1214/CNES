#include "cpu.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

void test_0xa9_lda_immediate_load_data()
{
    cpu_t *cpu = init_cpu();
    int *program = malloc(sizeof(int) * 3);
    program[0] = 0xa9;
    program[1] = 0x05;
    program[2] = 0x00;
    interpret(cpu, program);
    assert(cpu->reg_a == 0x05);
    assert((cpu->reg_status & 0b00000010) == 0b00);
    assert((cpu->reg_status & 0b10000000) == 0);
    free_cpu(cpu);
}

void test_0xa9_lda_zero_flag()
{
    cpu_t *cpu = init_cpu();
    int *program = malloc(sizeof(int)*3);
    program[0] = 0xa9;
    program[1] = 0x00;
    program[2] = 0x00;
    interpret(cpu, program);
    assert((cpu->reg_status & 0b00000010) == 0b10);
    free_cpu(cpu);
}

void test_0xa9_lda_negative_flag()
{
    cpu_t *cpu = init_cpu();
    int *program = malloc(sizeof(int)*3);
    program[0] = 0xa9;
    program[1] = 0xF0;
    program[2] = 0x00;
    interpret(cpu, program);
    assert((cpu->reg_status & 0b10000000) == 0b10000000);
    free_cpu(cpu);

}

int main()
{
    test_0xa9_lda_immediate_load_data();
    test_0xa9_lda_zero_flag();
    return 0;
}