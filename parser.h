#ifndef PARSER_H
#define PARSER_H
#include <stdio.h>

#include "puzzle.h"
#include "sim.h"

typedef enum parse_result {
    PARSE_SUCCESS,
    PARSE_LINE_TOO_LONG,
    PARSE_TOO_MANY_LINES,
    PARSE_UNKNOWN_OPCODE,
    PARSE_UNKNOWN_ARG,
    PARSE_CODE_BEFORE_T21,
    PARSE_INVALID_T21_IDX,
    PARSE_TOO_MANY_ARGS,
    PARSE_TOO_FEW_ARGS,
    PARSE_LABEL_REPEATED,
    PARSE_LABEL_UNDEFINED
} parse_result;

typedef struct parse_report {
    parse_result result;
    int line_num;
} parse_report;

parse_report parse_solution(FILE *fp, puzzle *puzzle, sim_state *sim);

#endif // PARSER_H
