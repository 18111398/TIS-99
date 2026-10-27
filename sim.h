#ifndef SIM_H
#define SIM_H
#include "instr.h"
#include "streams.h"
#include "puzzle.h"
#include <stdint.h>

typedef struct T21_state {
    instr program[15];
    uint8_t program_size;

    uint8_t pc;
    uint8_t last;
    int16_t acc;
    int16_t bak;
} T21_state;

typedef struct T30_state {
    int16_t stack[15];
    uint8_t size;
    uint8_t next_size;
} T30_state;

typedef struct node_state {
    uint8_t type;
    union {
        T21_state t21; T30_state t30;
    } state;
} node_state;

typedef struct sim_state {
    uint8_t metric_node;
    uint8_t metric_instrs;
    uint8_t functional_node_count;
    uint8_t functional_node_ids[12];
    uint8_t t30_count;
    uint8_t t30_node_ids[12];
    node_state nodes[12];
    uint8_t write_ports[24];
    uint8_t next_write_ports[24];
    int16_t write_contents[19];
    uint8_t input_state[4];
    uint8_t output_state[4];
    uint8_t image_state;
    uint8_t image_x;
    uint16_t image_xy;
    uint8_t out_diffs[4];
    uint16_t image_diff;
    output_buffer output;
    uint8_t inputs;
    uint8_t input_pos[4];
    uint8_t outputs;
    uint8_t output_pos[4];
    int8_t image_pos;
    test_case_buffer test_case;
} sim_state;

typedef enum sim_result {
    SIM_SUCCESS,
    SIM_WRONG_ANSWER,
    SIM_TIMEOUT,
    SIM_HCF
} sim_result;

typedef struct sim_report {
    sim_result result;
    int cycles;
} sim_report;

sim_report run(sim_state *sim, int cycle_limit);

static const uint8_t node_idx_to_commu_idx[] = {
     5,  6,  7,  8,
    10, 11, 12, 13,
    15, 16, 17, 18
};

// I hope I can get rid of this
static const uint8_t commu_idx_to_node_idx[] = {
    255, 255, 255, 255, 255,
      0,   1,   2,   3, 255,
      4,   5,   6,   7, 255,
      8,   9,  10,  11, 255,
    255, 255, 255, 255
};

static const int8_t write_idx_offsets[] = {0, -1, 1, -5, 5};
static const uint8_t opposite_port[] = {0, 2, 1, 4, 3};

#endif // SIM_H
