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
    if(cpu->reg_a != 0x05)
    {
        fprintf(stderr, "0xa9_lda_immediate_load_data failure: reg_a not correct");
        exit(1);
    }
    if((cpu->reg_status & 0b00000010) != 0)
    {
        fprintf(stderr, "0xa9_lda_immediate_load_data failure: zero flag not correct");
        exit(1);
    }
    if((cpu->reg_status & 0b10000000) != 0)
    {
        fprintf(stderr, "0xa9_lda_immediate_load_data failure: negative flag not correct");
        exit(1);
    }
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
    if((cpu->reg_status & 0b00000010) != 0b10)
    {
        fprintf(stderr, "0xa9_lda_zero_flag failure: zero flag not correct");
        exit(1);
    }
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
    if((cpu->reg_status & 0b10000000) != 0b10000000)
    {
        fprintf(stderr, "0xa9_lda_negative_flag failure: negative flag not correct");
        exit(1);
    }
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
    if(cpu->reg_x != 17)
    {
        fprintf(stderr, "0xaa_tax failure: reg_x not correct");
        exit(1);
    }
    if((cpu->reg_status & 0b00000010) != 0)
    {
        fprintf(stderr, "0xaa_tax failure: zero flag not correct");
        exit(1);
    }
    if((cpu->reg_status & 0b10000000) != 0)
    {
        fprintf(stderr, "0xaa_tax failure: negative flag not correct");
        exit(1);
    }
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
    if(cpu->reg_x != 0x02)
    {
        fprintf(stderr, "0xe8_inx failure: reg_x not correct");
        exit(1);
    }
    if((cpu->reg_status & 0b00000010) != 0)
    {
        fprintf(stderr, "0xe8_inx failure: zero flag not correct");
        exit(1);
    }
    if((cpu->reg_status & 0b10000000) != 0)
    {
        fprintf(stderr, "0xe8_inx failure: negative flag not correct");
        exit(1);
    }
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
    if(cpu->reg_x != 0x07)
    {
        fprintf(stderr, "0xe8_inx_nonzero failure: reg_x not correct");
        exit(1);
    }
    if((cpu->reg_status & 0b00000010) != 0)
    {
        fprintf(stderr, "0xe8_inx_nonzero failure: zero flag not correct");
        exit(1);
    }
    if((cpu->reg_status & 0b10000000) != 0)
    {
        fprintf(stderr, "0xe8_inx_nonzero failure: negative flag not correct");
        exit(1);
    }
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
    if(cpu->reg_x != 0xc1)
    {
        fprintf(stderr, "ops_together failure: reg_x not correct");
        exit(1);
    }
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
    if(cpu->reg_x != 1)
    {
        fprintf(stderr, "overflow_inx failure: reg_x not correct");
        exit(1);
    }
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