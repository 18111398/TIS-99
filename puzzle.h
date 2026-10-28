#ifndef PUZZLE_H
#define PUZZLE_H

#include <stdint.h>
#include <string.h>
#include "streams.h"

typedef int (*test_gen_func)(uint32_t seed, test_case_buffer *test);

typedef enum node_type {
    T21, T30, XXX
} node_type;

typedef struct puzzle {
    char *handle;
    char *name;
    int base_rng_seed;
    uint8_t input_count;
    uint8_t inputs[4];
    uint8_t output_count;
    uint8_t outputs[4];
    int8_t image;
    uint8_t node_layout[12];
    test_gen_func gen;
} puzzle;

int gen_self_test_diagnostic(uint32_t seed, test_case_buffer *test);
int gen_signal_amplifier(uint32_t seed, test_case_buffer *test);
int gen_differential_converter(uint32_t seed, test_case_buffer *test);
int gen_signal_comparator(uint32_t seed, test_case_buffer *test);
int gen_signal_multiplexer(uint32_t seed, test_case_buffer *test);
int gen_sequence_generator(uint32_t seed, test_case_buffer *test);
int gen_sequence_counter(uint32_t seed, test_case_buffer *test);
int gen_signal_pattern_detector(uint32_t seed, test_case_buffer *test);
int gen_sequence_reverser(uint32_t seed, test_case_buffer *test);
int gen_signal_multiplier(uint32_t seed, test_case_buffer *test);
int gen_image_test_pattern_1(uint32_t seed, test_case_buffer *test);
int gen_image_test_pattern_2(uint32_t seed, test_case_buffer *test);
int gen_histogram_viewer(uint32_t seed, test_case_buffer *test);
int gen_signal_window_filter(uint32_t seed, test_case_buffer *test);
int gen_signal_divider(uint32_t seed, test_case_buffer *test);
int gen_sequence_indexer(uint32_t seed, test_case_buffer *test);
int gen_sequence_sorter(uint32_t seed, test_case_buffer *test);
int gen_stored_image_decoder(uint32_t seed, test_case_buffer *test);
int gen_sequence_merger(uint32_t seed, test_case_buffer *test);
int gen_integer_series_calculator(uint32_t seed, test_case_buffer *test);
int gen_sequence_range_limiter(uint32_t seed, test_case_buffer *test);
int gen_signal_prescaler(uint32_t seed, test_case_buffer *test);
int gen_signal_averager(uint32_t seed, test_case_buffer *test);
int gen_decimal_decomposer(uint32_t seed, test_case_buffer *test);
int gen_sequence_normalizer(uint32_t seed, test_case_buffer *test);
int gen_image_test_pattern_3(uint32_t seed, test_case_buffer *test);
int gen_image_test_pattern_4(uint32_t seed, test_case_buffer *test);
int gen_backreference_reifier(uint32_t seed, test_case_buffer *test);
int gen_sequence_gap_interpolator(uint32_t seed, test_case_buffer *test);
int gen_decimal_to_octal_converter(uint32_t seed, test_case_buffer *test);
int gen_prolonged_sequence_sorter(uint32_t seed, test_case_buffer *test);
int gen_signal_exponentiator(uint32_t seed, test_case_buffer *test);
int gen_t20_node_emulator(uint32_t seed, test_case_buffer *test);
int gen_t31_node_emulator(uint32_t seed, test_case_buffer *test);
int gen_wave_collapse_supervisor(uint32_t seed, test_case_buffer *test);
int gen_unknown(uint32_t seed, test_case_buffer *test);

static const puzzle all_puzzles[] = {
    {
        "00150", "SELF TEST DIAGNOSTIC", 50,
        2, {0, 3},
        2, {0, 3},
        -1,
        {
            T21, XXX, T21, T21,
            T21, XXX, T21, XXX,
            T21, XXX, T21, T21
        },
        gen_self_test_diagnostic
    }, {
        "10981", "SIGNAL AMPLIFIER", 2,
        1, {1},
        1, {2},
        -1,
        {
            T21, T21, T21, XXX,
            T21, T21, T21, T21,
            XXX, T21, T21, T21
        },
        gen_signal_amplifier
    }, {
        "20176", "DIFFERENTIAL CONVERTER", 3,
        2, {1, 2},
        2, {1, 2},
        -1,
        {
            T21, T21, T21, T21,
            T21, T21, T21, XXX,
            T21, T21, T21, T21,
        },
        gen_differential_converter
    }, {
        "21340", "SIGNAL COMPARATOR", 4,
        1, {0},
        3, {1, 2, 3},
        -1,
        {
            T21, T21, T21, T21,
            T21, XXX, XXX, XXX,
            T21, T21, T21, T21,
        },
        gen_signal_comparator
    }, {
        "22280", "SIGNAL MULTIPLEXER", 22,
        3, {1, 2, 3},
        1, {2},
        -1,
        {
            T21, T21, T21, T21,
            T21, T21, T21, T21,
            XXX, T21, T21, T21,
        },
        gen_signal_multiplexer
    }, {
        "30647", "SEQUENCE GENERATOR", 5,
        2, {1, 2},
        1, {2},
        -1,
        {
            T21, T21, T21, T21,
            T21, T21, T21, T21,
            T21, XXX, T21, T21,
        },
        gen_sequence_generator
    }, {
        "31904", "SEQUENCE COUNTER", 9,
        1, {1},
        2, {1, 2},
        -1,
        {
            T21, T21, T21, XXX,
            T21, T21, T21, T21,
            T21, T21, T21, T21,
        },
        gen_sequence_counter
    }, {
        "40196", "SIGNAL PATTERN DETECTOR", 888,
        1, {1},
        1, {2},
        -1,
        {
            T21, T21, T21, XXX,
            T21, T21, T21, T21,
            T21, T21, T21, T21,
        },
        gen_signal_pattern_detector
    }, {
        "42656", "SEQUENCE REVERSER", 10,
        1, {1},
        1, {2},
        -1,
        {
            T21, T21, T30, T21,
            T21, T21, T21, T21,
            XXX, T30, T21, T21,
        },
        gen_sequence_reverser
    }, {
        "43786", "SIGNAL MULTIPLIER", 6,
        2, {1, 2},
        1, {2},
        -1,
        {
            T21, T21, T21, T21,
            T30, T21, T21, T30,
            XXX, T21, T21, T21,
        },
        gen_signal_multiplier
    }, {
        "50370", "IMAGE TEST PATTERN 1", 13,
        0, {},
        0, {},
        2,
        {
            T21, T21, T21, T21,
            XXX, T21, T21, T21,
            T21, T21, T21, T21,
        },
        gen_image_test_pattern_1
    }, {
        "51781", "IMAGE TEST PATTERN 2", 14,
        0, {},
        0, {},
        2,
        {
            XXX, T21, T21, T21,
            T21, T21, T21, T21,
            T21, T21, T21, T21,
        },
        gen_image_test_pattern_2
    }, {"53897", "HISTOGRAM VIEWER", 15,
        1, {1},
        0, {},
        2,
        {
            T21, T21, T21, T21,
            T21, T21, T21, T21,
            XXX, T21, T21, T21,
        },
        gen_histogram_viewer
    }, {
        "60099", "SIGNAL WINDOW FILTER", 55,
        1, {1},
        2, {1, 2},
        -1,
        {
            XXX, T21, T21, T30,
            T21, T21, T21, T21,
            T21, T21, T21, T30,
        },
        gen_signal_window_filter
    }, {
        "61212", "SIGNAL DIVIDER", 16,
        2, {1, 2},
        2, {1, 2},
        -1,
        {
            T21, T21, T21, T21,
            T30, T21, T21, T30,
            T21, T21, T21, XXX,
        },
        gen_signal_divider
    }, {
        "62711", "SEQUENCE INDEXER", 11,
        2, {0, 2},
        1, {2},
        -1,
        {
            T21, T30, T21, XXX,
            T21, T21, T21, T21,
            T21, T30, T21, T21,
        },
        gen_sequence_indexer
    }, {"63534", "SEQUENCE SORTER", 12,
        1, {1},
        1, {2},
        -1,
        {
            XXX, T21, T30, T21,
            T21, T21, T21, T21,
            T21, T30, T21, T21,
        },
        gen_sequence_sorter
    }, {
        "70601", "STORED IMAGE DECODER", 21,
        1, {1},
        0, {},
        2,
        {
            T21, T21, T21, T21,
            T21, T21, T21, T21,
            T21, T21, T21, T21,
        },
        gen_stored_image_decoder
    }, {
        "NEXUS.00.526.6", "SEQUENCE MERGER", 0,
        2, {1, 3},
        1, {2},
        -1,
        {
            T21, T21, XXX, T21,
            T30, T21, T21, T21,
            T21, T21, T21, T30,
        },
        gen_sequence_merger
    }, {
        "NEXUS.01.874.8", "INTEGER SERIES CALCULATOR", 1 * 23,
        1, {1},
        1, {1},
        -1,
        {
            T21, T21, T21, XXX,
            T21, T21, T21, T21,
            T21, T21, T21, XXX,
        },
        gen_integer_series_calculator
    }, {
        "NEXUS.02.981.2", "SEQUENCE RANGE LIMITER", 2 * 23,
        3, {0, 1, 2},
        1, {1},
        -1,
        {
            T21, T21, T21, XXX,
            T21, T21, T21, T21,
            T21, T21, T21, T21,
        },
        gen_sequence_range_limiter
    }, {
        "NEXUS.05.647.1", "SIGNAL PRESCALER", 5 * 23,
        1, {0},
        3, {1, 2, 3},
        -1,
        {
            T21, XXX, XXX, XXX,
            T21, T21, T21, T21,
            T21, T21, T21, T21,
        },
        gen_signal_prescaler
    }, {"NEXUS.06.786.0", "SIGNAL AVERAGER", 6 * 23,
        2, {1, 2},
        1, {2},
        -1,
        {
            T21, T21, T21, T21,
            XXX, T21, T21, T21,
            T21, T21, T21, T21,
        },
        gen_signal_averager
    }, {
        "NEXUS.08.633.9", "DECIMAL DECOMPOSER", 8 * 23,
        1, {1},
        3, {0, 1, 2},
        -1,
        {
            T21, T21, T21, XXX,
            T21, T21, T21, T21,
            T21, T21, T21, T21,
        },
        gen_decimal_decomposer
    }, {
        "NEXUS.10.656.5", "SEQUENCE NORMALIZER", 10 * 23,
        1, {1},
        1, {2},
        -1,
        {
            T21, T21, T30, T21,
            T21, T21, T21, T30,
            T21, XXX, T21, T21,
        },
        gen_sequence_normalizer
    }, {
        "NEXUS.11.711.2", "IMAGE TEST PATTERN 3", 11 * 23,
        0, {},
        0, {},
        2,
        {
            XXX, T21, T21, T21,
            T21, T21, T21, T21,
            T21, T21, T21, T21,
        },
        gen_image_test_pattern_3
    }, {
        "NEXUS.12.534.4", "IMAGE TEST PATTERN 4", 12 * 23,
        0, {},
        0, {},
        2,
        {
            XXX, T21, T21, T21,
            T21, T21, T21, T21,
            T21, T21, T21, T21,
        },
        gen_image_test_pattern_4
    }, {
        "NEXUS.15.897.9", "BACK-REFERENCE REIFIER", 15 * 23,
        2, {1, 2},
        1, {2},
        -1,
        {
            T21, T21, T21, T21,
            T21, T21, T21, T21,
            XXX, T21, T21, T21,
        },
        gen_backreference_reifier
    }, {
        "NEXUS.17.135.0", "SEQUENCE GAP INTERPOLATOR", 17 * 23,
        1, {2},
        2, {2},
        -1,
        {
            XXX, T21, T21, T21,
            XXX, T30, T21, T30,
            XXX, T21, T21, T21,
        },
        gen_sequence_gap_interpolator
    }, {
        "NEXUS.18.427.7", "DECIMAL TO OCTAL CONVERTER", 18 * 23,
        1, {1},
        1, {2},
        -1,
        {
            T21, T21, T21, T21,
            T21, T21, T21, T21,
            XXX, T21, T21, T21,
        },
        gen_decimal_to_octal_converter
    }, {
        "NEXUS.19.762.9", "PROLONGED SEQUENCE SORTER", 19 * 23,
        1, {2},
        1, {2},
        -1,
        {
            XXX, T30, T21, T21,
            T21, T21, T21, T21,
            T21, T30, T21, T21,
        },
        gen_prolonged_sequence_sorter
    }, {
        "NEXUS.21.601.6", "SIGNAL EXPONENTIATOR", 21 * 23,
        2, {1, 2},
        1, {2},
        -1,
        {
            T21, T21, T21, T21,
            T30, T21, T21, T30,
            XXX, T21, T21, T21,
        },
        gen_signal_exponentiator
    }, {"NEXUS.22.280.8", "T20 NODE EMULATOR", 22 * 23,
        2, {1, 2},
        1, {1},
        -1,
        {
            T21, T21, T21, T21,
            T21, T21, T21, T21,
            T21, T21, T21, XXX,
        },
        gen_t20_node_emulator
    }, {
        "NEXUS.23.727.9", "T31 NODE EMULATOR", 23 * 23,
        1, {2},
        1, {2},
        -1,
        {
            XXX, T30, T21, T21,
            T21, T21, T21, T21,
            T21, T30, T21, T21,
        },
        gen_t31_node_emulator
    }, {"NEXUS.24.511.7", "WAVE COLLAPSE SUPERVISOR", 24 * 23,
        4, {0, 1, 2, 3},
        1, {1},
        -1,
        {
            T21, T21, T21, T21,
            T21, T21, T21, T21,
            T21, T21, T21, T21,
        },
        gen_wave_collapse_supervisor
    }, {"UNKNOWN", "D.$A5R.5SRD..", 23,
        1, {1},
        2, {1, 2},
        -1,
        {
            T21, T21, T21, XXX,
            T21, T21, T21, XXX,
            XXX, T21, T21, T21,
        },
        gen_unknown
    }
};

#endif // PUZZLE_H
