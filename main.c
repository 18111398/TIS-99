#include "parser.h"
#include "puzzle.h"
#include "sim.h"
#include "streams.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc <= 1) {
        puts("Provide a file");
        return 0;
    }
    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
        puts("ERROR: Failed to open file");
        return -1;
    }

    char *file_name = argv[1] + strlen(argv[1]);
    while (*(file_name - 1) != '/' && *(file_name - 1) != '\\' && file_name > argv[1]) {
        file_name--;
    }

    int puzzle_idx = -1;
    // Temp method, very bad, TODO: optimize
    for (int i = 0; i < sizeof(all_puzzles) / sizeof(puzzle); i++) {
        char *handle = all_puzzles[i].handle;
        if (strncmp(file_name, handle, strlen(handle)) == 0) {
            puzzle_idx = i;
            break;
        }
    }

    if (puzzle_idx < 0) {
        printf("ERROR: Cannot find puzzle by filename \"%s\"", file_name);
        return -1;
    }

    puzzle puzzle = all_puzzles[puzzle_idx];
    puts(puzzle.name);
    sim_state sim = {0};
    parse_report report = parse_solution(fp, &puzzle, &sim);
    if (report.result != PARSE_SUCCESS) {
        printf("ERROR: parsing error code %d (pretty error info WIP)\n", report.result);
        return -1;
    }

    for (int i = 0; i < 12; i++) {
        sim.nodes[i].type = puzzle.node_layout[i];
    }

    int all_success = 1;
    int max_cycle = 0;
    for (int test_i = 0; test_i < 3; test_i++) {
        puzzle.gen(puzzle.base_rng_seed * 100 + test_i, &sim.test_case);
        test_case_buffer test = sim.test_case;
        sim_report sim_report = run(&sim, 600000);
        if (sim_report.result == SIM_SUCCESS) {
            printf("Test %d passed in %d cycles\n", test_i + 1, sim_report.cycles);
            if (max_cycle < sim_report.cycles) max_cycle = sim_report.cycles;
        } else {
            printf("Test %d failed (pretty error info WIP)\n", test_i + 1);
            all_success = 0;
        }
    }
    if (all_success) {
        printf("Success: %d/%d/%d\n", max_cycle, sim.metric_node, sim.metric_instrs);
    } else {
        printf("Failed: ?/%d/%d\n", sim.metric_node, sim.metric_instrs);
    }
}
