#include <ctype.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "instr.h"
#include "puzzle.h"

typedef struct str_slice {
    char *begin;
    // Points to the pos AFTER the last char in the slice. Thus *end is not a part of the slice.
    char *end;
} str_slice;

// Extracts next substring until '@' or end of string.
// Argument str's begin is changed to the char after the next '@',
// pointing directly to the next T21 index in a valid solution file.
str_slice next_node(str_slice *str) {
    str_slice node = {str->begin, str->begin};
    while (*node.end != '@' && node.end < str->end ) {
        node.end++;
    }
    str->begin = node.end + 1;
    return node;
}

// Extracts next substring until '\r', '\n', "\r\n" or end of string.
// Argument str's begin is changed to the char after the line breaker delimiter.
str_slice next_code_line(str_slice *str) {
    str_slice line = {str->begin, str->begin};
    while (*line.end != '\r' && *line.end != '\n' && line.end < str->end ) {
        line.end++;
    }
    str->begin = line.end + 1;
    // Treat \r\n as one delimiter.
    if (str->begin < str->end && *line.end == '\r' && *str->begin == '\n') {
        str->begin++;
    }
    return line;
}

str_slice next_token(str_slice *line) {
    str_slice token;
    token.begin = line->begin;
    while ((isspace(*token.begin) || *token.begin == ',') && token.begin < line->end) {
        token.begin++;
    }
    token.end = token.begin;
    while (!isspace(*token.end) && *token.end != ',' && token.end < line->end) {
        token.end++;
    }
    line->begin = token.end;
    return token;
}

// Returns parsed int if the whole slice represents an integer in [-999, 999],
// returns -18111398 otherwise
int parse_code_integer(str_slice slice) {
    int sign = 1;
    if (slice.begin[0] == '-') {
        sign = -1;
        slice.begin++;
    }
    int res = 0;
    for (char *c = slice.begin; c < slice.end; c++) {
        if (*c < '0' || *c > '9') {
            return -18111398;
        }
        res = 10 * res + *c - '0';
        if (res > 999) {
            return -18111398;
        }
    }
    return res * sign;
}

char slice_is_empty(str_slice slice) {
    return slice.begin >= slice.end;
}

int slice_len(str_slice slice) {
    return slice.end - slice.begin;
}

int parse_port(str_slice slice) {
    for (int i = 0; i < 8; i++) {
        char len = PORT_NAME_LENGTHS[i];
        if (slice_len(slice) == len && memcmp(PORT_NAMES[i], slice.begin, len) == 0) {
            return i;
        }
    }
    return -18111398;
}

int parse_port_or_imm(str_slice slice) {
    int imm = parse_code_integer(slice);
    if (imm == -18111398) {
        return parse_port(slice) + 1000;
    }
    return imm;
}

typedef struct label_map_entry {
    char* label;
    char label_len;
    char value;
} label_map_entry;

typedef struct label_map {
    int size;
    label_map_entry entries[15]; // The game allows only one label per row, so this is the upper bound
} label_map;

void add_label(label_map *map, str_slice label, char value) {
    label_map_entry entry = {label.begin, label.end - label.begin, value};
    map -> entries[map -> size] = entry;
    map -> size++;
}

int get_label_value(label_map *map, char *label, char label_len) {
    // Linear probe suffices for avg. 2~3, max. 15 elements
    for (int i = 0; i < map -> size; i++) {
        label_map_entry _e = map -> entries[i];
        if (_e.label_len == label_len && memcmp(_e.label, label, label_len) == 0) {
            return _e.value;
        }
    }
    return -1;
}

typedef struct deferred_label_parsing {
    char* label;
    char label_len;
    char instr_pos;
} deferred_label_parsing;

typedef struct deferred_label_parsings {
    int size;
    deferred_label_parsing entries[15];
} deferred_label_parsings;

void append_deferred_label(deferred_label_parsings *l, str_slice label, char instr_pos) {
    l->entries[l->size] = (deferred_label_parsing) {label.begin, label.end - label.begin, instr_pos};
    l->size++;
}

// This method currently assumes that sim is zeroed. If not, more work should be done
parse_report parse_solution(FILE *fp, puzzle *puzzle, sim_state *sim) {
    fseek(fp, 0, SEEK_END);
    long expected_file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char *file_buf = malloc(expected_file_size + 1);
    int actual_size = fread(file_buf, 1, expected_file_size, fp);
    file_buf[actual_size] = '\0';

    str_slice file = {file_buf, file_buf + actual_size};

    char t21_count = 0;
    char t21_to_node_idx[12];
    for (int i = 0; i < 12; i++) {
        if (puzzle -> node_layout[i] == T21) {
            t21_to_node_idx[t21_count] = i;
            t21_count++;
        }
    }

    sim->metric_node = 0;
    sim->metric_instrs = 0;

    parse_report res = {0};

    // Discard all contents before the first '@'.
    next_node(&file);

    while (!slice_is_empty(file)) {
        str_slice node = next_node(&file);
        str_slice first_line = next_code_line(&node);

        const char t21_idx = parse_code_integer(first_line);

        if (t21_idx < 0 || t21_idx >= t21_count) {
            res.result = PARSE_INVALID_T21_IDX;
            goto parse_cleanup;
        }

        const char node_idx = t21_to_node_idx[t21_idx];
        // This is a very awkward way to write; I hope I can simplify this
        T21_state *t21 = &(sim->nodes[node_idx].state.t21);
        t21->program_size = 0;

        label_map label_map = {0};
        deferred_label_parsings deferred_label_parsings = {0};

        while (!slice_is_empty(node)) {
            str_slice line = next_code_line(&node);
            if (line.end - line.begin > 18) {
                res.result = PARSE_LINE_TOO_LONG;
                goto parse_cleanup;
            }

            // extract label, remove comment
            char *colon = NULL;
            for (char *c = line.begin; c < line.end; c++) {
                if (colon == NULL && *c == ':') {colon = c;}
                if (*c == '#') {line.end = c; break;}
            }

            if (colon != NULL) {
                // TODO: trim spaces off label, prevent empty label
                if (get_label_value(&label_map, line.begin, colon - line.begin) >= 0) {
                    res.result = PARSE_LABEL_REPEATED;
                    goto parse_cleanup;
                }
                add_label(&label_map, (str_slice){line.begin, colon}, t21->program_size);
                line.begin = colon + 1;
            }

            str_slice opcode_name = next_token(&line);
            if (slice_is_empty(opcode_name)) {
                continue;
            }
            opcode op = 18111398;

            if (opcode_name.end - opcode_name.begin != 3) {
                res.result = PARSE_UNKNOWN_OPCODE;
                goto parse_cleanup;
            }

            for (int i = 0; i <= HCF; i++) {
                if (memcmp(opcode_name.begin, OPCODE_NAMES + i * 3, 3) == 0) {
                    op = i;
                    break;
                }
            }

            if (op == 18111398) {
                res.result = PARSE_UNKNOWN_OPCODE;
                goto parse_cleanup;
            }
            instr ins = {.opcode = op};
            switch (op) {
                case MOV: {
                    str_slice src = next_token(&line);
                    if (slice_is_empty(src)) {
                        res.result = PARSE_TOO_FEW_ARGS;
                        goto parse_cleanup;
                    }
                    int src_arg = parse_port_or_imm(src);
                    if (src_arg < -1000) {
                        res.result = PARSE_UNKNOWN_ARG;
                        goto parse_cleanup;
                    }
                    ins.src = src_arg;
                    str_slice dst = next_token(&line);
                    if (slice_is_empty(dst)) {
                        res.result = PARSE_TOO_FEW_ARGS;
                        goto parse_cleanup;
                    }
                    int dst_arg = parse_port(dst);
                    if (dst_arg < -1000) {
                        res.result = PARSE_UNKNOWN_ARG;
                        goto parse_cleanup;
                    }
                    ins.dst = dst_arg;
                    if (!slice_is_empty(next_token(&line))) {
                        res.result = PARSE_TOO_MANY_ARGS;
                        goto parse_cleanup;
                    }
                    break;
                }
                case ADD:
                case SUB:
                case JRO: {
                    str_slice src = next_token(&line);
                    if (slice_is_empty(src)) {
                        res.result = PARSE_TOO_FEW_ARGS;
                        goto parse_cleanup;
                    }
                    int src_arg = parse_port_or_imm(src);
                    if (src_arg < -1000) {
                        res.result = PARSE_UNKNOWN_ARG;
                        goto parse_cleanup;
                    }
                    ins.src = src_arg;
                    if (!slice_is_empty(next_token(&line))) {
                        res.result = PARSE_TOO_MANY_ARGS;
                        goto parse_cleanup;
                    }
                    break;
                }
                case NOP:
                case SWP:
                case SAV:
                case NEG:
                case HCF: {
                    if (!slice_is_empty(next_token(&line))) {
                        res.result = PARSE_TOO_MANY_ARGS;
                        goto parse_cleanup;
                    }
                    break;
                }
                case JMP:
                case JEZ:
                case JNZ:
                case JGZ:
                case JLZ: {
                    str_slice label = next_token(&line);
                    if (slice_is_empty(label)) {
                        res.result = PARSE_TOO_FEW_ARGS;
                        goto parse_cleanup;
                    }
                    append_deferred_label(&deferred_label_parsings, label, t21->program_size);
                    if (!slice_is_empty(next_token(&line))) {
                        res.result = PARSE_TOO_MANY_ARGS;
                        goto parse_cleanup;
                    }
                    break;
                }
            }
            t21->program[t21->program_size] = ins;
            t21->program_size++;
            sim->metric_instrs++;
        }
        for (int i = 0; i < deferred_label_parsings.size; i++) {
            deferred_label_parsing deferred = deferred_label_parsings.entries[i];
            int dst_instr = get_label_value(&label_map, deferred.label, deferred.label_len);
            if (dst_instr < 0) {
                res.result = PARSE_LABEL_UNDEFINED;
                goto parse_cleanup;
            }
            if (dst_instr == t21->program_size) {
                dst_instr = 0;
            }
            t21->program[deferred.instr_pos].dst = dst_instr;
        }
        if (t21->program_size > 0) {
            sim->metric_node++;
        }
    }

    for (int i = 0; i < 12; i++) {
        if (puzzle->node_layout[i] == T21) {
            if (sim->nodes[i].state.t21.program_size > 0) {
                sim->functional_node_ids[sim->functional_node_count] = i;
                sim->functional_node_count++;
            }
        } else if (puzzle->node_layout[i] == T30) {
            sim->functional_node_ids[sim->functional_node_count] = i;
            sim->functional_node_count++;
            sim->t30_node_ids[sim->t30_count] = i;
            sim->t30_count++;
        }
    }

    sim->inputs = puzzle->input_count;
    memcpy(sim->input_pos, puzzle->inputs, 4);
    sim->outputs = puzzle->output_count;
    memcpy(sim->output_pos, puzzle->outputs, 4);
    sim->image_pos = puzzle->image;

    parse_cleanup:
    free(file_buf);
    return res;
}
