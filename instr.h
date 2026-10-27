#ifndef INSTR_H
#define INSTR_H
#include <stdint.h>

typedef enum opcode {
    ADD,
    SUB,
    JRO,
    MOV,
    NOP,
    SWP,
    SAV,
    NEG,
    JMP,
    JEZ,
    JLZ,
    JGZ,
    JNZ,
    HCF
} opcode;

static const char *OPCODE_NAMES =
    "ADD"
    "SUB"
    "JRO"
    "MOV"
    "NOP"
    "SWP"
    "SAV"
    "NEG"
    "JMP"
    "JEZ"
    "JLZ"
    "JGZ"
    "JNZ"
    "HCF"
;

typedef enum port {
    NIL,
    LEFT,
    RIGHT,
    UP,
    DOWN,
    ANY,
    ACC,
    LAST
} port;

static const char *PORT_NAMES[] = {
    "NIL",
    "LEFT",
    "RIGHT",
    "UP",
    "DOWN",
    "ANY",
    "ACC",
    "LAST",
};

static const char PORT_NAME_LENGTHS[] = {
    3, 4, 5, 2, 4, 3, 3, 4
};

typedef struct instr {
    uint8_t opcode;
    uint8_t dst;
    int16_t src;
} instr;

#endif // INSTR_H
