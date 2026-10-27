#include "sim.h"
#include "instr.h"
#include "puzzle.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static inline void start_write_event(sim_state *sim, uint8_t commu_idx, uint8_t ports, int16_t value) {
    sim->next_write_ports[commu_idx] = ports;
    sim->write_contents[commu_idx] = value;
}

// Try to read from LEFT/RIGHT/UP/DOWN
static inline int16_t try_read_from_dir_port(sim_state *sim, uint8_t commu_idx, uint8_t port) {
    // printf("%d tries to read from %c\n", commu_idx, " <>^vX"[port]);
    uint8_t from_idx = commu_idx + write_idx_offsets[port];
    uint8_t available_port = (sim->write_ports[from_idx] & sim->next_write_ports[from_idx]);
    if (available_port == opposite_port[port] || available_port == ANY) {
        int16_t value = sim->write_contents[from_idx];
        sim->next_write_ports[from_idx] = 0;
        // The LAST setting part is a mess because I didn't plan for this
        uint8_t node_idx = commu_idx_to_node_idx[from_idx];
        if (available_port == ANY && sim->nodes[node_idx].type == T21) { // I don't like these branches
            sim->nodes[node_idx].state.t21.last = opposite_port[port];
        }
        return value;
    }
    return -1000;
}

static inline int16_t try_read_from_src(sim_state *sim, uint8_t commu_idx, T21_state *t21, int16_t src_arg) {
    if (src_arg < 1000) {
        return src_arg;
    }
    src_arg -= 1000;
    get_port:
    switch(src_arg) {
        case NIL:
            return 0;
        case LEFT:
        case RIGHT:
        case UP:
        case DOWN:
            return try_read_from_dir_port(sim, commu_idx, src_arg);
        case ACC:
            return t21->acc;
        case ANY:
            for (int port = 1; port <= 4; port++) {
                int16_t value = try_read_from_dir_port(sim, commu_idx, port);
                if (value > -1000) {
                    t21->last = port;
                    return value;
                }
            }
            return -1000;
        case LAST:
            src_arg = t21->last;
            goto get_port;   // Should I?
    }
}

sim_report run(sim_state *sim, int cycle_limit) {
    for (int i = 0; i < 12; i++) {
        switch(sim->nodes[i].type) {
            case T21:
                sim->nodes[i].state.t21.pc = 0;
                sim->nodes[i].state.t21.last = 0;
                sim->nodes[i].state.t21.acc = 0;
                sim->nodes[i].state.t21.bak = 0;
                break;
            case T30:
                sim->nodes[i].state.t30.size = 0;
                sim->nodes[i].state.t30.next_size = 0;
                break;
        }
    }

    memset(sim->write_ports, 0, 24);
    memset(sim->next_write_ports, 0, 24);
    memset(sim->input_state, 0, 4);
    memset(sim->output_state, 0, 4);
    memset(sim->out_diffs, 0, 4);
    for (int i = 0; i < sim->outputs; i++) {
        sim->out_diffs[i] = sim->test_case.output_sizes[i];
    }

    sim->image_state = 0;
    memset(sim->output.image, 0, 540);
    // This is at best done during test generation tbh
    sim->image_diff = 0;
    for (int i = 0; i < 540; i++) {
        if (sim->test_case.image[i] > 0) {
            sim->image_diff++;
        }
    }

    sim_report report = {0};
    while (1) {
        for (int _i = 0; _i < sim->functional_node_count; _i++) {
            int i = sim->functional_node_ids[_i];
            switch(sim->nodes[i].type) {
                case T30: {
                    T30_state *t30 = &sim->nodes[i].state.t30;
                    if (t30->next_size >= 15) { break; }
                    uint8_t commu_idx = node_idx_to_commu_idx[i];
                    for (int dir = 1; dir <= 4; dir++) {
                        int16_t value = try_read_from_dir_port(sim, commu_idx, dir);
                        if (value > -1000) {
                            t30->stack[t30->next_size] = value;
                            t30->next_size++;
                            if (t30->next_size >= 15) { break; }
                        }
                    }
                    break;
                }
                case T21: {
                    uint8_t commu_idx = node_idx_to_commu_idx[i];
                    if (sim->write_ports[commu_idx] > 0) { break; }
                    T21_state *t21 = &sim->nodes[i].state.t21;
                    // This will add a branch also on irrelevant operations.
                    // While this simplifies the code, would this hurt perf?
                    int16_t src_value = try_read_from_src(sim, commu_idx, t21, t21->program[t21->pc].src);
                    uint8_t dst = t21->program[t21->pc].dst;
                    if (src_value <= -1000) { break; }
                    char inc_pc = 1;
                    opcode op = t21->program[t21->pc].opcode;
                    switch (op) {
                        case ADD:
                            t21->acc += src_value;
                            if (t21->acc > 999) {
                                t21->acc = 999;
                            }
                            break;
                        case SUB:
                            t21->acc -= src_value;
                            if (t21->acc < -999) {
                                t21->acc = -999;
                            }
                            break;
                        case MOV:
                            get_dst_arg:
                            switch (dst) {
                                case NIL:
                                    break;
                                case LEFT:
                                case RIGHT:
                                case UP:
                                case DOWN:
                                case ANY:
                                    start_write_event(sim, commu_idx, dst, src_value);
                                    break;
                                case ACC:
                                    t21->acc = src_value;
                                    break;
                                case LAST:
                                    dst = t21->last;
                                    goto get_dst_arg;
                            }
                            // I am letting it just increment before the stall.
                            // This differs from the in-game behaviors, but the result won't change,
                            // and this implementation saves a state along with many extra logic.
                            break;
                        case NOP:
                            break;
                        case SWP:
                            int16_t temp = t21->acc;
                            t21->acc = t21->bak;
                            t21->bak = temp;
                            break;
                        case SAV:
                            t21->bak = t21->acc;
                            break;
                        case NEG:
                            t21->acc *= -1;
                            break;
                        case JMP:
                            t21->pc = dst;
                            inc_pc = 0;
                            break;
                        case JEZ:
                            if (t21->acc == 0) {
                                t21->pc = dst;
                                inc_pc = 0;
                            }
                            break;
                        case JLZ:
                            if (t21->acc < 0) {
                                t21->pc = dst;
                                inc_pc = 0;
                            }
                            break;
                        case JGZ:
                            if (t21->acc > 0) {
                                t21->pc = dst;
                                inc_pc = 0;
                            }
                            break;
                        case JNZ:
                            if (t21->acc != 0) {
                                t21->pc = dst;
                                inc_pc = 0;
                            }
                            break;
                        case JRO:
                            int next_pc = t21->pc + src_value;
                            if (next_pc < 0) next_pc = 0;
                            if (next_pc > t21->program_size - 1) next_pc = t21->program_size - 1;
                            t21->pc = next_pc;
                            inc_pc = 0;
                            break;
                        case HCF:
                            report.result = SIM_HCF;
                            return report;
                    }
                    if (inc_pc) {
                        t21->pc++;
                        if (t21->pc >= t21->program_size) {
                            t21->pc = 0;
                        }
                    }
                }
            }
        }

        // finalize T30
        for (int _i = 0; _i < sim->t30_count; _i++) {
            int i = sim->t30_node_ids[_i];
            T30_state *t30 = &sim->nodes[i].state.t30;
            uint8_t commu_index = node_idx_to_commu_idx[i];
            if (t30->size > 0 && sim->next_write_ports[commu_index] == 0) {
                for (int i = t30->size; i < t30->next_size; i++) {
                    t30->stack[i-1] = t30->stack[i];
                }
                t30->next_size--;
            }
            if (t30->next_size > 0) {
                sim->next_write_ports[commu_index] = ANY;
                sim->write_contents[commu_index] = t30->stack[t30->next_size - 1];
            } else {
                sim->next_write_ports[commu_index] = 0;
            }
            t30->size = t30->next_size;
        }

        // process inputs
        for (int i = 0; i < sim->inputs; i++) {
            int input_pos = sim->input_pos[i];
            int current_inputs = sim->input_state[i];
            if (sim->test_case.input_sizes[i] > current_inputs) {
                if (sim->write_ports[input_pos] == 0) {
                    sim->next_write_ports[input_pos] = DOWN;
                    sim->write_contents[input_pos] = sim->test_case.inputs[i][current_inputs];
                    sim->input_state[i]++;
                }
            }
        }

        // process outputs
        for (int i = 0; i < sim->outputs; i++) {
            int output_pos = sim->output_pos[i];
            int current_outputs = sim->output_state[i];
            if (sim->test_case.output_sizes[i] > current_outputs) {
                int16_t value = try_read_from_dir_port(sim, 20 + output_pos, UP);
                if (value > -1000) {
                    sim->out_diffs[i] -= (value == sim->test_case.outputs[i][current_outputs]);
                    sim->output.outputs[i][current_outputs] = value;
                    sim->output_state[i]++;
                }
            }
        }

        // process image
        if (sim->image_pos > 0) {
            int16_t value = try_read_from_dir_port(sim, 20 + sim->image_pos, UP);
            if (value > -1000) {
                if (value < 0) {
                    sim->image_state = 0;
                } else {
                    switch(sim->image_state) {
                        case 0:
                            sim->image_x = value;
                            sim->image_state = 1;
                            break;
                        case 1:
                            sim->image_xy = value * 30 + sim->image_x;
                            sim->image_state = 2;
                            break;
                        case 2:
                            if (sim->image_x < 30) {
                                int i = sim->image_xy;
                                uint8_t color = value >= 0 && value < 5 ? value : 0;
                                sim->image_diff -= sim->output.image[i] != sim->test_case.image[i];
                                sim->output.image[i] = color;
                                sim->image_diff += sim->output.image[i] != sim->test_case.image[i];
                                sim->image_x++;
                                sim->image_xy++;
                            }
                            break;
                    }
                }
            }
        }

        memcpy(sim->write_ports, sim->next_write_ports, 24);
        report.cycles++;

        if (sim->out_diffs[0] == 0 && sim->out_diffs[1] == 0 && sim->out_diffs[2] == 0 && sim->out_diffs[3] == 0
         && sim->image_diff == 0) {
            return report;
        }

        if (report.cycles > cycle_limit) {
            report.result = SIM_TIMEOUT;
            return report;
        }

        #ifdef TIS_DEBUG
        printf("Cycle %d\n", report.cycles);
        for (int i = 0; i < 12; i++) {
            printf("@%2d, ", i);
            switch(sim->nodes[i].type) {
                case T30: {
                    T30_state *t30 = &sim->nodes[i].state.t30;
                    int size = t30->size;
                    printf("T30: %2d, [", size);
                    for (int j = 0; j < size; j++) {
                        printf("%d, ", t30->stack[j]);
                    }
                    putchar(']');
                    break;
                }
                case T21: {
                    T21_state *t21 = &sim->nodes[i].state.t21;
                    printf("T21: acc: %d, bak: %d, last: %c, pc: %d", t21->acc, t21->bak, "_<>^vX"[t21->last], t21->pc);
                    break;
                }
                case XXX: {
                    printf("Damaged"); break;
                }
            }
            putchar('\n');
        }
        for (int row = 0; row < 4; row++) {
            for (int col = 0; col < 4; col++) {
                int commu_idx = row * 5 + col;
                int port = sim->write_ports[commu_idx];
                putchar("_<>^vX"[port]); putchar(' ');
                if (port == 0) {
                    printf("     ");
                } else {
                    printf("%4d ", sim->write_contents[commu_idx]);
                }
            }
            putchar('\n');
        }
        for (int i = 0; i < sim->outputs; i++) {
            int size = sim->output_state[i];
            printf("output %d: %d/%d/%d, [", i, size, sim->out_diffs[i], sim->test_case.output_sizes[i]);
            for (int j = 0; j < size; j++) {
                printf("%d, ", sim->output.outputs[i][j]);
            }
            putchar(']'); putchar('\n');
        }
        if (sim->image_pos >= 0) {
            printf("image: state: %d, x: %d, xy: %d, diff: %d\n", sim->image_state, sim->image_x, sim->image_xy, sim->image_diff);
            for (int y = 0; y < 18; y++) {
                for (int x = 0; x < 30; x++) {
                    char color = sim->output.image[x + y * 30];
                    printf("%d", color);
                }
                putchar('\n');
            }
        }
        getchar();
        #endif // TIS_DEBUG
    }
}
