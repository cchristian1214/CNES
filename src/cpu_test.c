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
    load_and_run(cpu, program, 3);
    assert(cpu->reg_a == 0x05);
    assert((cpu->reg_status & 0b00000010) == 0);
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
    load_and_run(cpu, program, 3);
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
    load_and_run(cpu, program, 3);
    assert((cpu->reg_status & 0b10000000) == 0b10000000);
    free_cpu(cpu);

}

void test_0xaa_tax()
{
    cpu_t *cpu = init_cpu();
    int *program = malloc(sizeof(int)*2);
    program[0] = 0xa9;
    program[1] = 0x11;
    program[2] = 0xaa;
    program[3] = 0x00;
    load_and_run(cpu, program, 4);
    assert(cpu->reg_x == 17);
    assert((cpu->reg_status & 0b00000010) == 0);
    assert((cpu->reg_status & 0b10000000) == 0);
    free_cpu(cpu);
}

void test_0xe8_inx()
{
    cpu_t *cpu = init_cpu();
    int *program = malloc(sizeof(int)*5);
    program[0] = 0xa9;
    program[1] = 0x01;
    program[2] = 0xaa;
    program[3] = 0xe8;
    program[4] = 0x00;
    load_and_run(cpu, program, 5);
    assert(cpu->reg_x == 0x02);
    assert((cpu->reg_status & 0b00000010) == 0);
    assert((cpu->reg_status & 0b10000000) == 0);
    free_cpu(cpu);
}

void test_0xe8_inx_nonzero()
{
    cpu_t *cpu = init_cpu();
    int *program = malloc(sizeof(int)*2);
    program[0] = 0xa9;
    program[1] = 0x06;
    program[2] = 0xaa;
    program[3] = 0xe8;
    program[4] = 0x00;
    load_and_run(cpu, program, 5);
    assert(cpu->reg_x == 0x07);
    assert((cpu->reg_status & 0b00000010) == 0);
    assert((cpu->reg_status & 0b10000000) == 0);
    free_cpu(cpu);

}

void test_ops_together()
{
    cpu_t *cpu = init_cpu();
    int *program = malloc(sizeof(int)*5);
    program[0] = 0xa9;
    program[1] = 0xc0;
    program[2] = 0xaa;
    program[3] = 0xe8;
    program[4] = 0x00;
    load_and_run(cpu, program, 5);
    assert(cpu->reg_x == 0xc1);
    free_cpu(cpu);
}

void test_overflow_inx()
{
    cpu_t *cpu = init_cpu();
    int *program = malloc(sizeof(int)*3);
    program[0] = 0xa9;
    program[1] = 0xff;
    program[2] = 0xaa;
    program[3] = 0xe8;
    program[4] = 0xe8;
    program[5] = 0x00;
    load_and_run(cpu, program, 6);
    assert(cpu->reg_x == 1);
    free_cpu(cpu);
}

int main()
{
    test_0xa9_lda_immediate_load_data();
    test_0xa9_lda_zero_flag();
    test_0xa9_lda_negative_flag();
    test_0xaa_tax();
    test_0xe8_inx();
    test_0xe8_inx_nonzero();
    test_ops_together();
    test_overflow_inx();
    printf("All tests passed!\n");
    return 0;
}