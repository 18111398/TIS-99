#include "puzzle.h"
#include "random.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

int gen_self_test_diagnostic(uint32_t seed, test_case_buffer *test) {
    unity_rng rng0 = create_unity_rng(seed);
    for (int i = 0; i < 39; i++) {
        test->inputs[0][i] = unity_rng_next_between(&rng0, 10, 100);
        test->outputs[0][i] = test->inputs[0][i];
    }

    unity_rng rng1 = create_unity_rng(seed+1);
    for (int i = 0; i < 39; i++) {
        test->inputs[1][i] = unity_rng_next_between(&rng1, 10, 100);
        test->outputs[1][i] = test->inputs[1][i];
    }
    test->input_sizes[0] = 39;
    test->input_sizes[1] = 39;
    test->output_sizes[0] = 39;
    test->output_sizes[1] = 39;
    return 0;
}

int gen_signal_amplifier(uint32_t seed, test_case_buffer *test) {
    unity_rng rng = create_unity_rng(seed);
    for (int i = 0; i < 39; i++) {
        test->inputs[0][i] = unity_rng_next_between(&rng, 10, 100);
        test->outputs[0][i] = test->inputs[0][i] * 2;
    }
    test->input_sizes[0] = 39;
    test->output_sizes[0] = 39;
    return 0;
}

int gen_differential_converter(uint32_t seed, test_case_buffer *test) {
    unity_rng rng_a = create_unity_rng(seed);
    unity_rng rng_b = create_unity_rng(seed + 1);
    for (int i = 0; i < 39; i++) {
        int16_t a = unity_rng_next_between(&rng_a, 10, 100);
        int16_t b = unity_rng_next_between(&rng_b, 10, 100);
        test->inputs[0][i] = a;
        test->inputs[1][i] = b;
        test->outputs[0][i] = a - b;
        test->outputs[1][i] = b - a;
    }
    test->input_sizes[0] = 39;
    test->input_sizes[1] = 39;
    test->output_sizes[0] = 39;
    test->output_sizes[1] = 39;
    return 0;
}

int gen_signal_comparator(uint32_t seed, test_case_buffer *test) {
    memset(test->outputs[0], 0, 39 * 2);
    memset(test->outputs[1], 0, 39 * 2);
    memset(test->outputs[2], 0, 39 * 2);
    unity_rng rng = create_unity_rng(seed);
    for (int i = 0; i < 39; i++) {
        int16_t value = unity_rng_next_between(&rng, -2, 3);
        test->inputs[0][i] = value;
        test->outputs[(value == 0) + 2 * (value < 0)][i] = 1;
    }
    test->input_sizes[0] = 39;
    test->output_sizes[0] = 39;
    test->output_sizes[1] = 39;
    test->output_sizes[2] = 39;
    return 0;
}

int gen_signal_multiplexer(uint32_t seed, test_case_buffer *test) {
    unity_rng rng_a = create_unity_rng(seed);
    unity_rng rng_b = create_unity_rng(seed + 1);
    unity_rng rng_s = create_unity_rng(seed + 2);
    for (int i = 0; i < 39; i++) {
        int16_t a = unity_rng_next_between(&rng_a, -30, 1);
        int16_t b = unity_rng_next_between(&rng_b, 0, 31);
        int16_t s = unity_rng_next_between(&rng_s, -1, 2);
        test->inputs[0][i] = a;
        test->inputs[1][i] = s;
        test->inputs[2][i] = b;
        int16_t res = s <= 0 ? a : 0;
        if (s >= 0) res += b;
        test->outputs[0][i] = res;
    }
    test->input_sizes[0] = 39;
    test->input_sizes[1] = 39;
    test->input_sizes[2] = 39;
    test->output_sizes[0] = 39;
    return 0;
}

int gen_sequence_generator(uint32_t seed, test_case_buffer *test) {
    unity_rng rng = create_unity_rng(seed);
    unity_rng rng_1 = create_unity_rng(seed + 1);
    for (int i = 0; i < 13; i++) {
        test->inputs[0][i] = unity_rng_next_between(&rng, 10, 100);
        test->inputs[1][i] = unity_rng_next_between(&rng_1, 10, 100);
    }
    int16_t same_i = unity_rng_next_between(&rng_1, 0, 13);
    int16_t same_v = unity_rng_next_between(&rng_1, 10, 100);
    test->inputs[0][same_i] = same_v;
    test->inputs[1][same_i] = same_v;
    for (int i = 0; i < 13; i++) {
        int16_t a = test->inputs[0][i];
        int16_t b = test->inputs[1][i];
        test->outputs[0][i*3] = a < b ? a : b;
        test->outputs[0][i*3 + 1] = a < b ? b : a;
    }
    test->input_sizes[0] = 13;
    test->input_sizes[1] = 13;
    test->output_sizes[0] = 39;
    return 0;
}

int gen_sequence_counter(uint32_t seed, test_case_buffer *test) {
    unity_rng rng = create_unity_rng(seed);
    int total_size = 0;
    int seqs = 0;
    while (total_size < 39) {
        int16_t len = unity_rng_next_between(&rng, 0, 6);
        if (len > 38 - total_size) { len = 38 - total_size; }
        int16_t sum = 0;
        for (int i = 0; i < len; i++) {
            int16_t e = unity_rng_next_between(&rng, 10, 100);
            test->inputs[0][total_size + i] = e;
            sum += e;
        }
        test->inputs[0][total_size + len] = 0;
        test->outputs[0][seqs] = sum;
        test->outputs[1][seqs] = len;
        seqs++;
        total_size += len + 1;
    }
    test->input_sizes[0] = 39;
    test->output_sizes[0] = seqs;
    test->output_sizes[1] = seqs;
    return 0;
}

int gen_signal_pattern_detector(uint32_t seed, test_case_buffer *test) {
    unity_rng rng = create_unity_rng(seed);
    for (int i = 0; i < 39; i++) {
        test->inputs[0][i] = unity_rng_next_between(&rng, 0, 6);
    }
    for (int k = 0; k < 8; k++) {
        int i = unity_rng_next_between(&rng, 0, 36);
        test->inputs[0][i] = 0;
        test->inputs[0][i + 1] = 0;
        test->inputs[0][i + 2] = 0;
        i = unity_rng_next_between(&rng, 0, 35);
        test->inputs[0][i] = unity_rng_next_between(&rng, 1, 6);
        test->inputs[0][i + 1] = 0;
        test->inputs[0][i + 2] = 0;
        test->inputs[0][i + 3] = unity_rng_next_between(&rng, 1, 6);
    }

    int pattern = 0;
    for (int i = 0; i < 39; i++) {
        pattern = test->inputs[0][i] == 0 ? pattern + 1 : 0;
        test->outputs[0][i] = pattern >= 3;
    }
    test->input_sizes[0] = 39;
    test->output_sizes[0] = 39;
    return 0;
}

int gen_sequence_reverser(uint32_t seed, test_case_buffer *test) {
    unity_rng rng = create_unity_rng(seed);
    int total_size = 0;
    while (total_size < 39) {
        int16_t len = unity_rng_next_between(&rng, 0, 6);
        if (len > 38 - total_size) { len = 38 - total_size; }
        for (int i = 0; i < len; i++) {
            int16_t e = unity_rng_next_between(&rng, 10, 100);
            test->inputs[0][total_size + i] = e;
            test->outputs[0][total_size + len - i - 1] = e;
        }
        test->inputs[0][total_size + len] = 0;
        test->outputs[0][total_size + len] = 0;
        total_size += len + 1;
    }
    test->input_sizes[0] = 39;
    test->output_sizes[0] = 39;
    return 0;
}

int gen_signal_multiplier(uint32_t seed, test_case_buffer *test) {
    unity_rng rng_a = create_unity_rng(seed);
    unity_rng rng_b = create_unity_rng(seed + 1);
    for (int i = 0; i < 39; i++) {
        int16_t a = unity_rng_next_between(&rng_a, 0, 10);
        int16_t b = unity_rng_next_between(&rng_b, 0, 10);
        test->inputs[0][i] = a;
        test->inputs[1][i] = b;
        test->outputs[0][i] = a * b;
    }
    test->input_sizes[0] = 39;
    test->input_sizes[1] = 39;
    test->output_sizes[0] = 39;
    return 0;
}

int gen_image_test_pattern_1(uint32_t seed, test_case_buffer *test) {
    memset(test->image, 3, 540);
    return 0;
}

int gen_image_test_pattern_2(uint32_t seed, test_case_buffer *test) {
    for (int y = 0; y < 18; y++) {
        for (int x = 0; x < 30; x++) {
            test->image[x + y * 30] = (x ^ y) % 2 == 0 ? 3 : 0;
        }
    }
    return 0;
}

int gen_histogram_viewer(uint32_t seed, test_case_buffer *test) {
    unity_rng rng = create_unity_rng(seed);
    test->inputs[0][0] = unity_rng_next_between(&rng, 3, 14);
    for (int x = 1; x < 30; x++) {
        if (unity_rng_next_between(&rng, 0, 4)) {
            // MIN / MAX macros work incorrectly on things with side effects
            test->inputs[0][x] = test->inputs[0][x - 1] + unity_rng_next_between(&rng, -2, 3);
            test->inputs[0][x] = MIN(MAX(test->inputs[0][x], 1), 17);
        } else {
            test->inputs[0][x] = unity_rng_next_between(&rng, 3, 14);
        }
    }
    // The first row is always empty.
    for (int y = 1; y < 17; y++) {
        for (int x = 0; x < 30; x++) {
            test->image[x + y * 30] = 18 - y <= test->inputs[0][x] ? 3 : 0;
        }
    }
    // The last row is always filled.
    memset(test->image + 510, 3, 30);
    test->input_sizes[0] = 30;
    return 0;
}

int gen_signal_window_filter(uint32_t seed, test_case_buffer *test) {
    unity_rng rng = create_unity_rng(seed);
    for (int i = 0; i < 39; i++) {
        test->inputs[0][i] = unity_rng_next_between(&rng, 10, 100);
    }
    // If I can pad pre-inputs with zero this would be much simpler. Sadly I can't.
    int16_t sum3 = 0;
    for (int i = 0; i < 3; i++) {
        sum3 += test->inputs[0][i];
        test->outputs[0][i] = sum3;
        test->outputs[1][i] = sum3;
    }
    int16_t sum5 = sum3;
    for (int i = 3; i < 5; i++) {
        sum3 += test->inputs[0][i] - test->inputs[0][i - 3];
        sum5 += test->inputs[0][i];
        test->outputs[0][i] = sum3;
        test->outputs[1][i] = sum5;
    }
    for (int i = 5; i < 39; i++) {
        sum3 += test->inputs[0][i] - test->inputs[0][i - 3];
        sum5 += test->inputs[0][i] - test->inputs[0][i - 5];
        test->outputs[0][i] = sum3;
        test->outputs[1][i] = sum5;
    }
    test->input_sizes[0] = 39;
    test->output_sizes[0] = 39;
    test->output_sizes[1] = 39;
    return 0;
}

int gen_signal_divider(uint32_t seed, test_case_buffer *test) {
    unity_rng rng_a = create_unity_rng(seed);
    unity_rng rng_b = create_unity_rng(seed + 1);
    for (int i = 0; i < 39; i++) {
        int16_t a = unity_rng_next_between(&rng_a, 10, 100);
        int16_t b = unity_rng_next_between(&rng_b, 1, 10);
        test->inputs[0][i] = a;
        test->inputs[1][i] = b;
        test->outputs[0][i] = a / b;
        test->outputs[1][i] = a % b;
    }
    test->input_sizes[0] = 39;
    test->input_sizes[1] = 39;
    test->output_sizes[0] = 39;
    test->output_sizes[1] = 39;
    return 0;
}

int gen_sequence_indexer(uint32_t seed, test_case_buffer *test) {
    unity_rng rng_v = create_unity_rng(seed);
    unity_rng rng_i = rng_v;
    for (int i = 0; i < 10; i++) {
        test->inputs[0][i] = unity_rng_next_between(&rng_v, 100, 1000);
    }
    test->inputs[0][10] = 0;
    for (int i = 0; i < 39; i++) {
        int16_t id = unity_rng_next_between(&rng_i, 0, 10);
        test->inputs[1][i] = id;
        test->outputs[0][i] = test->inputs[0][id];
    }
    test->input_sizes[0] = 11;
    test->input_sizes[1] = 39;
    test->output_sizes[0] = 39;
    return 0;
}

int gen_sequence_sorter(uint32_t seed, test_case_buffer *test) {
    unity_rng rng = create_unity_rng(seed);
    int total_size = 0;
    while (total_size < 39) {
        int16_t len = unity_rng_next_between(&rng, 4, 8);
        if (len > 38 - total_size) { len = 38 - total_size; }
        for (int i = 0; i < len; i++) {
            int16_t e = unity_rng_next_between(&rng, 10, 100);
            test->inputs[0][total_size + i] = e;
            // Insert e into the sorted sequence
            int j = i;
            for (; j > 0 && test->outputs[0][total_size + j - 1] > e; j--) {
                test->outputs[0][total_size + j] = test->outputs[0][total_size + j - 1];
            }
            test->outputs[0][total_size + j] = e;
        }
        test->inputs[0][total_size + len] = 0;
        test->outputs[0][total_size + len] = 0;
        total_size += len + 1;
    }
    test->input_sizes[0] = 39;
    test->output_sizes[0] = 39;
    return 0;
}

int gen_stored_image_decoder(uint32_t seed, test_case_buffer *test) {
    int in_offset = 0;
    int image_offset = 0;
    unity_rng rng = create_unity_rng(seed);
    while (image_offset < 540) {
        int16_t len = unity_rng_next_between(&rng, 20, 45);
        int16_t col = unity_rng_next_between(&rng, 0, 4);
        test->inputs[0][in_offset] = len;
        test->inputs[0][in_offset + 1] = col;
        in_offset += 2;
        if (len > 540 - image_offset) {
            len = 540 - image_offset;
        }
        memset(test->image + image_offset, col, len);
        image_offset += len;
    }
    test->input_sizes[0] = in_offset;
    return 0;
}

int gen_sequence_merger(uint32_t seed, test_case_buffer *test) {
    mono_rng rng = create_mono_rng(seed);
    int i1 = 0;
    int i2 = 0;
    int i = 0;
    char prev_has_empty = 1;
    char can_be_zero = 1;
    while (i < 39) {
        int sup_len =
            i == 26 ? 11 :
            i < 28  ? 12 :
                      38 - i;
        // I made a confusion here.
        // sup_len is the exact length when i >= 28, but the max len + 1 when i < 28.
        // I hope I can improve to make this more readable.
        int len = sup_len;
        if (sup_len >= 11) {
            do {
                len = mono_rng_next_between(&rng, 0, sup_len);
            } while (len == 0 && can_be_zero == 0);
        }
        if (len == 0) { can_be_zero = 0; }
        int len1 = (prev_has_empty && len >= 2) ? mono_rng_next_between(&rng, 1, len) : mono_rng_next_between(&rng, 0, len + 1);
        int len2 = len - len1;

        prev_has_empty = (len1 == 0 || len2 == 0);

        // Generate sequences of the lengths. Will be sorting at the same time.
        // The generation demands avoiding any duplication in the merged sequence.
        // This implementation use 16 * 7 = 112 > 99 bits to track and check usage.
        // (16 * 6 = 96 suffices for [10, 99], but I want the code to be more readable)
        uint16_t used[7] = {0};
        for (int i_ = 0; i_ < len; i_++) {
            int16_t val;
            do {
                val = mono_rng_next_between(&rng, 10, 100);
            } while (used[val / 16] & (1 << (val % 16)));
            used[val / 16] |= 1 << (val % 16);
            // Insert into the corresponding sorted sequence.
            // Is this branch good?
            if (i_ < len1) {
                int j = i_;
                for (; j > 0 && test->inputs[0][i1 + j - 1] > val; j--) {
                    test->inputs[0][i1 + j] = test->inputs[0][i1 + j - 1];
                }
                test->inputs[0][i1 + j] = val;
            } else {
                int j = i_ - len1;
                for (; j > 0 && test->inputs[1][i2 + j - 1] > val; j--) {
                    test->inputs[1][i2 + j] = test->inputs[1][i2 + j - 1];
                }
                test->inputs[1][i2 + j] = val;
            }
        }
        // Merge the two sequences.
        test->inputs[0][i1 + len1] = 181;
        test->inputs[1][i2 + len2] = 181;
        int i_1 = 0, i_2 = 0;
        for (int i_ = 0; i_ < len; i_++) {
            if (test->inputs[0][i1 + i_1] < test->inputs[1][i2 + i_2]) {
                test->outputs[0][i + i_] = test->inputs[0][i1 + i_1];
                i_1++;
            } else {
                test->outputs[0][i + i_] = test->inputs[1][i2 + i_2];
                i_2++;
            }
        }
        test->inputs[0][i1 + len1] = 0;
        test->inputs[1][i2 + len2] = 0;
        test->outputs[0][i + len] = 0;
        i1 += len1 + 1;
        i2 += len2 + 1;
        i += len + 1;
    }
    test->input_sizes[0] = i1;
    test->input_sizes[1] = i2;
    test->output_sizes[0] = 39;
    return 0;
}

int gen_integer_series_calculator(uint32_t seed, test_case_buffer *test) {
    mono_rng rng = create_mono_rng(seed);
    for (int i = 0; i < 39; i++) {
        int16_t n = mono_rng_next_between(&rng, 1, 45);
        test->inputs[0][i] = n;
        test->outputs[0][i] = n * (n + 1) / 2;
    }
    test->input_sizes[0] = 39;
    test->output_sizes[0] = 39;
    return 0;
}

int gen_sequence_range_limiter(uint32_t seed, test_case_buffer *test) {
    mono_rng rng = create_mono_rng(seed);
    for (int i = 0; i < 6; i++) {
        test->inputs[0][i] = mono_rng_next_between(&rng, 3, 10) * 5;
    }
    for (int i = 0; i < 6; i++) {
        test->inputs[2][i] = mono_rng_next_between(&rng, 10, 18) * 5;
    }
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 5; j++) {
            int16_t v = mono_rng_next_between(&rng, 10, 100);
            test->inputs[1][j + i * 6] = v;
            test->outputs[0][j + i * 6] = MIN(MAX(v, test->inputs[0][i]), test->inputs[2][i]);
        }
        test->inputs[1][5 + i * 6] = 0;
        test->outputs[0][5 + i * 6] = 0;
    }
    test->input_sizes[0] = 6;
    test->input_sizes[1] = 36;
    test->input_sizes[2] = 6;
    test->output_sizes[0] = 36;
    return 0;
}

int gen_signal_prescaler(uint32_t seed, test_case_buffer *test) {
    mono_rng rng = create_mono_rng(seed);
    for (int i = 0; i < 39; i++) {
        int16_t a = mono_rng_next_between(&rng, 1, 121);
        test->inputs[0][i] = a * 8;
        test->outputs[0][i] = a * 4;
        test->outputs[1][i] = a * 2;
        test->outputs[2][i] = a;
    }
    test->input_sizes[0] = 39;
    test->output_sizes[0] = 39;
    test->output_sizes[1] = 39;
    test->output_sizes[2] = 39;
    return 0;
}

int gen_signal_averager(uint32_t seed, test_case_buffer *test) {
    mono_rng rng = create_mono_rng(seed);
    for (int i = 0; i < 39; i++) {
        test->inputs[0][i] = mono_rng_next_between(&rng, 100, 1000);
        test->inputs[1][i] = mono_rng_next_between(&rng, 100, 1000);
        test->outputs[0][i] = (test->inputs[0][i] + test->inputs[1][i]) / 2;
    }
    test->input_sizes[0] = 39;
    test->input_sizes[1] = 39;
    test->output_sizes[0] = 39;
    return 0;
}

int gen_submaximum_selector(uint32_t seed, test_case_buffer *test) {
    mono_rng rng = create_mono_rng(seed);
    for (int i = 0; i < 39; i++) {
        test->inputs[0][i] = mono_rng_next_between(&rng, 0, 100);
        test->inputs[1][i] = mono_rng_next_between(&rng, 0, 100);
        test->inputs[2][i] = mono_rng_next_between(&rng, 0, 100);
        test->inputs[3][i] = mono_rng_next_between(&rng, 0, 100);
        int16_t submax1 = MIN(MAX(test->inputs[0][i], test->inputs[1][i]), MAX(test->inputs[2][i], test->inputs[3][i]));
        int16_t submax2 = MAX(MIN(test->inputs[0][i], test->inputs[1][i]), MIN(test->inputs[2][i], test->inputs[3][i]));
        test->outputs[0][i] = MAX(submax1, submax2);
    }
    test->input_sizes[0] = 39;
    test->input_sizes[1] = 39;
    test->input_sizes[2] = 39;
    test->input_sizes[3] = 39;
    test->output_sizes[0] = 39;
    return 0;
}

int gen_decimal_decomposer(uint32_t seed, test_case_buffer *test) {
    mono_rng rng = create_mono_rng(seed);
    for (int i = 0; i < 39; i++) {
        // Obviously optimizable, but naive algo for now
        int16_t n;
        switch(mono_rng_next_between(&rng, 0, 3)) {
            case 0: n = mono_rng_next_between(&rng, 0, 10); break;
            case 1: n = mono_rng_next_between(&rng, 10, 100); break;
            case 2: n = mono_rng_next_between(&rng, 100, 1000); break;
        }
        test->inputs[0][i] = n;
        test->outputs[0][i] = n / 100;
        test->outputs[1][i] = (n % 100) / 10;
        test->outputs[2][i] = n % 10;
    }
    test->input_sizes[0] = 39;
    test->output_sizes[0] = 39;
    test->output_sizes[1] = 39;
    test->output_sizes[2] = 39;
    return 0;
}

int gen_sequence_mode_calculator(uint32_t seed, test_case_buffer *test) {
    mono_rng rng = create_mono_rng(seed);
    int seqs = 0;
    char lens[10] = {0};
    int prev_end = -1;
    for (int i = 0; i < 38; i++) {
        test->inputs[0][i] = mono_rng_next_between(&rng, 1, 6);
        if (i - prev_end > 3 && mono_rng_next(&rng) < 0.5 && i < 37) {
            test->inputs[0][i] = 0;
            lens[seqs] = i - prev_end;
            seqs++;
            prev_end = i;
        }
    }
    test->inputs[0][38] = 0;
    lens[seqs] = 38 - prev_end;
    seqs++;

    char begin = 0;
    for (int seq = 0; seq < seqs; seq++) {
        char len = lens[seq];
        char counts[6] = {0};
        for (int i = begin; i < begin + len - 1; i++) {
            counts[test->inputs[0][i]]++;
        }
        int16_t ret = 0;
        char mode = 0;
        for (int i = 1; i <= 5; i++) {
            if (counts[i] > mode) {
                mode = counts[i];
                ret = i;
            } else if (counts[i] == mode) {
                ret = 0;
            }
        }
        test->outputs[0][seq] = ret;

        begin += len;
    }
    test->input_sizes[0] = 39;
    test->output_sizes[0] = seqs;
    return 0;
}

int gen_sequence_normalizer(uint32_t seed, test_case_buffer *test) {
    mono_rng rng = create_mono_rng(seed);
    int begin = 0;
    int16_t min = 181;
    for (int i = 0; i < 38; i++) {
        test->inputs[0][i] = mono_rng_next_between(&rng, 1, 100);
        min = MIN(min, test->inputs[0][i]);
        if ((mono_rng_next_between(&rng, 1, 4) == 3 && i - begin >= 2) || (i - begin >= 7) || i == 36) {
            for (int oi = begin; oi <= i; oi++) {
                test->outputs[0][oi] = test->inputs[0][oi] - min;
            }
            i++;
            test->inputs[0][i] = -1;
            test->outputs[0][i] = -1;
            begin = i + 1;
            min = 181;
        }
    }
    test->input_sizes[0] = 38;
    test->output_sizes[0] = begin;
    return 0;
}

int gen_image_test_pattern_3(uint32_t seed, test_case_buffer *test) {
    for (int y = 0; y < 18; y++) {
        for (int x = 0; x < 30; x++) {
            int d = MIN(MIN(x, 29 - x), MIN(y, 17 - y));
            test->image[x + y * 30] = d % 2 == 0 ? 3 : 0;
        }
    }
    return 0;
}

int gen_image_test_pattern_4(uint32_t seed, test_case_buffer *test) {
    for (int y = 0; y < 18; y++) {
        for (int x = 0; x < 30; x++) {
            test->image[x + y * 30] = (x ^ y) % 4;
        }
    }
    return 0;
}

void random_spatial_path_coords(mono_rng *rng, char upper, char *res) {
    char remaining[30];
    for (int i = 1; i < upper; i++) {
        remaining[i] = i;
    }
    for (int i = upper - 1; i >= 2; i--) {
        int j = mono_rng_next_between(rng, 1, i + 1);
        char temp = remaining[i];
        remaining[i] = remaining[j];
        remaining[j] = temp;
    }
    char prev = 0;
    res[0] = 0;
    for (int i = 1; i < 11; i++) {
        char coord = remaining[i];
        for (int j = i + 1; 1; j++) {
            char d = abs(coord - prev);
            if (d >= 3 && d <= 14) {
                break;
            }
            char temp = coord;
            coord = remaining[j];
            remaining[j] = temp;
        }
        res[i] = coord;
        prev = coord;
    }
}
/*
 * I wrote this very bad. If possible, clean up
 */
int gen_spatial_path_viewer(uint32_t seed, test_case_buffer *test) {
    mono_rng rng = create_mono_rng(seed);
    memset(test->image, 0, 540);
    char x[11], y[11];
    random_spatial_path_coords(&rng, 30, x);
    random_spatial_path_coords(&rng, 18, y);
    int pos = 0;
    int ii = 0;
    for (int i = 0; 1; i++) {
        if (x[i] < x[i+1]) {
            test->inputs[0][ii++] = 0;
            test->inputs[0][ii++] = x[i+1] - x[i] + 1;
            for (int j = x[i]; j < x[i+1]; j++) {
                test->image[pos] = 3;
                pos++;
            }
        } else {
            test->inputs[0][ii++] = 180;
            test->inputs[0][ii++] = x[i] - x[i+1] + 1;
            for (int j = x[i]; j > x[i+1]; j--) {
                test->image[pos] = 3;
                pos--;
            }
        }
        if (i == 9) {
            test->image[pos] = 3;
            break;
        }
        if (y[i] < y[i+1]) {
            test->inputs[0][ii++] = 270;
            test->inputs[0][ii++] = y[i+1] - y[i] + 1;
            for (int j = y[i]; j < y[i+1]; j++) {
                test->image[pos] = 3;
                pos += 30;
            }
        } else {
            test->inputs[0][ii++] = 90;
            test->inputs[0][ii++] = y[i] - y[i+1] + 1;
            for (int j = y[i]; j > y[i+1]; j--) {
                test->image[pos] = 3;
                pos -= 30;
            }
        }
    }
    test->input_sizes[0] = 38;
    return 0;
}

int gen_character_terminal(uint32_t seed, test_case_buffer *test) {
    mono_rng rng = create_mono_rng(seed);
    memset(test->image, 0, 540);
    char line_ends[4] = {-1, 0, 0, 38};
    mono_rng_next(&rng);
    for (int i = 0; i < 38; i++) {
        test->inputs[0][i] = mono_rng_next_between(&rng, 1, 5);
    }
    test->inputs[0][38] = 0;
    line_ends[1] = mono_rng_next_between(&rng, 11, 16);
    line_ends[2] = mono_rng_next_between(&rng, 27, 31);
    test->inputs[0][line_ends[1]] = 0;
    test->inputs[0][line_ends[2]] = 0;
    static const char bitmaps[5] = {0b0000, 0b0101, 0b1001, 0b0110, 0b0111};
    static const char bit_offsets[4] = {0, 1, 30, 31};
    for (int line = 0; line < 3; line++) {
        int offset = line * 180;
        int col = 0; // only used for detecting line wrap
        for (int i = line_ends[line] + 1; i < line_ends[line + 1]; i++) {
            char bitmap = bitmaps[test->inputs[0][i]];
            for (int i = 0; i < 4; i++) {
                int pixel_offset = offset + bit_offsets[i];
                test->image[pixel_offset] = bitmap & 1 << i ? 3 : 0;
            }
            offset += 3;
            col++;
            if (col == 10) offset += 60;
        }
    }
    test->input_sizes[0] = 39;
    return 0;
}

int gen_backreference_reifier(uint32_t seed, test_case_buffer *test) {
    mono_rng rng = create_mono_rng(seed);
    for (int i = 0; i < 39; i++) {
        int16_t back = 0;
        if (mono_rng_next_between(&rng, 0, 2) == 0) {
            back = mono_rng_next_between(&rng, -4, 0);
            if (back < -i) {
                back = 0;
            }
        }
        test->inputs[0][i] = back;
        test->inputs[1][i] = mono_rng_next_between(&rng, 10, 100);
        test->outputs[0][i] = test->inputs[1][i + back];
    }
    test->input_sizes[0] = 39;
    test->input_sizes[1] = 39;
    test->output_sizes[0] = 39;
    return 0;
}

int gen_dynamic_pattern_detector(uint32_t seed, test_case_buffer *test) {
    mono_rng rng = create_mono_rng(seed);
    for (int i = 0; i < 12; i++) {
        mono_rng_next(&rng);
    }
    for (int i = 0; i < 3; i++) {
        test->inputs[1][i] = mono_rng_next_between(&rng, 1, 43);
    }
    test->inputs[1][3] = 0;
    for (int i = 0; i < 39; i++) {
        test->inputs[0][i] = mono_rng_next_between(&rng, 1, 43);
    }
    for (int k = 0; k < 2; k++) {
        int j = mono_rng_next_between(&rng, 0, 37);
        test->inputs[0][j] = test->inputs[1][0];
        test->inputs[0][j + 1] = test->inputs[1][1];
        test->inputs[0][j + 2] = test->inputs[1][2];
    }
    for (int k = 0; k < 3; k++) {
        int j = mono_rng_next_between(&rng, 0, 37);
        test->inputs[0][j + 1] = test->inputs[1][1];
        test->inputs[0][j + 2] = test->inputs[1][2];
    }
    int j = mono_rng_next_between(&rng, 0, 7);
    test->inputs[0][j] = test->inputs[1][0];
    test->inputs[0][j + 1] = test->inputs[1][1];
    test->inputs[0][j + 2] = test->inputs[1][2];

    j = mono_rng_next_between(&rng, 9, 13);
    test->inputs[0][j] = test->inputs[1][0];
    test->inputs[0][j + 1] = test->inputs[1][1];
    test->inputs[0][j + 2] = test->inputs[1][1];
    test->inputs[0][j + 3] = test->inputs[1][2];

    j = mono_rng_next_between(&rng, 16, 23);
    test->inputs[0][j] = test->inputs[1][0];
    test->inputs[0][j + 1] = test->inputs[1][0];
    test->inputs[0][j + 2] = test->inputs[1][1];
    test->inputs[0][j + 3] = test->inputs[1][2];

    j = mono_rng_next_between(&rng, 26, 35);
    test->inputs[0][j] = test->inputs[1][0];
    test->inputs[0][j + 1] = test->inputs[1][1];
    test->inputs[0][j + 2] = test->inputs[1][0];
    test->inputs[0][j + 3] = test->inputs[1][1];
    test->inputs[0][j + 4] = test->inputs[1][2];

    test->outputs[0][0] = 0;
    test->outputs[0][1] = 0;
    for (int i = 2; i < 39; i++) {
        test->outputs[0][i] =
            test->inputs[0][i - 2] == test->inputs[1][0] &&
            test->inputs[0][i - 1] == test->inputs[1][1] &&
            test->inputs[0][i - 0] == test->inputs[1][2];
    }
    test->input_sizes[0] = 39;
    test->input_sizes[1] = 4;
    test->output_sizes[0] = 39;
    return 0;
}

int gen_sequence_gap_interpolator(uint32_t seed, test_case_buffer *test) {
    mono_rng rng = create_mono_rng(seed);
    static const char lens[] = {5, 4, 4, 4, 5, 4, 5, 4, 4};
    int ii = 0;
    for (int seq = 0; seq < 9; seq++) {
        char len = lens[seq];
        int16_t min = mono_rng_next_between(&rng, 10, 91);
        int16_t gap_i = mono_rng_next_between(&rng, 1, len - 1);

        for (int i = 0; i < len - 1; i++) {
            test->inputs[0][ii + i] = min + i + (i >= gap_i);
        }
        for (int i = len - 2; i > 0; i--) {
            int j = mono_rng_next_between(&rng, 0, i + 1);
            int16_t temp = test->inputs[0][ii + i];
            test->inputs[0][ii + i] = test->inputs[0][ii + j];
            test->inputs[0][ii + j] = temp;
        }
        test->inputs[0][ii + len - 1] = 0;
        test->outputs[0][seq] = min + gap_i;
        ii += len;
    }
    test->input_sizes[0] = 39;
    test->output_sizes[0] = 9;
    return 0;
}

int gen_decimal_to_octal_converter(uint32_t seed, test_case_buffer *test) {
    mono_rng rng = create_mono_rng(seed);
    for (int i = 0; i < 39; i++) {
        int16_t in = mono_rng_next_between(&rng, 1, 64);
        test->inputs[0][i] = in;
        test->outputs[0][i] = in / 8 * 10 + in % 8;
    }
    test->input_sizes[0] = 39;
    test->output_sizes[0] = 39;
    return 0;
}

int gen_prolonged_sequence_sorter(uint32_t seed, test_case_buffer *test) {
    mono_rng rng = create_mono_rng(seed);
    uint8_t counts[10] = {0};
    // The test generation allows only 9 numbers of the input range to appear in the input.
    unsigned int remaining = 9;
    for (int i = 0; i < 38; i++) {
        int16_t val;
        do {
            val = mono_rng_next_between(&rng, 0, 10);
        } while(counts[val] == 0 && remaining == 0);
        remaining -= counts[val] == 0;
        counts[val]++;
        test->inputs[0][i] = val;
    }
    test->inputs[0][38] = -1;

    int out_offset = 0;
    for (int val = 0; val < 10; val++) {
        for (int i = 0; i < counts[val]; i++) {
            test->outputs[0][out_offset + i] = val;
        }
        out_offset += counts[val];
    }
    test->outputs[0][38] = -1;
    test->input_sizes[0] = 39;
    test->output_sizes[0] = 39;
    return 0;
}

int gen_prime_factor_calculator(uint32_t seed, test_case_buffer *test) {
    mono_rng rng = create_mono_rng(seed);
    // These represent the lengths of the whole prime factor sequence including the terminating zero.
    // Thus prime numbers have a length of 2.
    // This list is script-generated.
    char lens[90] = {3, 2, 4, 2, 3, 3, 5, 2, 4, 2, 4, 3, 3, 2, 5, 3, 3, 4, 4, 2, 4, 2, 6, 3, 3, 3, 5, 2, 3, 3, 5, 2, 4, 2, 4, 4, 3, 2, 6, 3, 4, 3, 4, 2, 5, 3, 5, 3, 3, 2, 5, 2, 3, 4, 7, 3, 4, 2, 4, 3, 4, 2, 6, 2, 3, 4, 4, 3, 4, 2, 6, 5, 3, 2, 5, 3, 3, 3, 5, 2, 5, 3, 4, 3, 3, 3, 7, 2, 4, 4};
    int total;
    do {
        total = 38;
        for (int i = 0; i < 10; i++) {
            int n = mono_rng_next_between(&rng, 10, 100);
            test->inputs[0][i] = n;
            total -= lens[n - 10];
        }
    } while (total != 0);

    const char small_primes[4] = {2, 3, 5, 7};
    int oi = 0;
    for (int i = 0; i < 10; i++) {
        int n = test->inputs[0][i];
        char len = lens[n - 10];
        char prime_i = 0;
        // I still compute the primes on the fly, but with the length known the termination detection is much simpler.
        for (int j = 0; j < len - 2; j++) {
            while (n % small_primes[prime_i] != 0) {
                prime_i++;
            }
            n /= small_primes[prime_i];
            test->outputs[0][oi + j] = small_primes[prime_i];
        }
        test->outputs[0][oi + len - 2] = n;
        test->outputs[0][oi + len - 1] = 0;
        oi += len;
    }

    test->input_sizes[0] = 10;
    test->output_sizes[0] = 38;
    return 0;
}

int gen_signal_exponentiator(uint32_t seed, test_case_buffer *test) {
    mono_rng rng = create_mono_rng(seed);
    static const int16_t sup_exp[10] = {11, 10, 7, 5, 5, 4, 4, 4, 4, 3};
    for (int i = 0; i < 39; i++) {
        int16_t base = mono_rng_next_between(&rng, 1, 11);
        int16_t exp = mono_rng_next_between(&rng, 1, sup_exp[base - 1]);
        int16_t pow = base;
        for (int i = 1; i < exp; i++) {
            pow *= base;
        }
        test->inputs[0][i] = base;
        test->inputs[1][i] = exp;
        test->outputs[0][i] = pow;
    }
    test->input_sizes[0] = 39;
    test->input_sizes[1] = 39;
    test->output_sizes[0] = 39;
    return 0;
}

int gen_t20_node_emulator(uint32_t seed, test_case_buffer *test) {
    mono_rng rng = create_mono_rng(seed);
    test->inputs[0][0] = 0;
    test->inputs[0][1] = 1;
    test->inputs[1][0] = 0;
    test->inputs[1][1] = 0;
    int16_t p = 0, q = 0;
    int vi = 2, oi = 0;
    for (int ii = 2; ii < 39; ii++) {
        int16_t instr = mono_rng_next_between(&rng, 0, 5);
        test->inputs[0][ii] = instr;
        switch (instr) {
            case 0:
                p = mono_rng_next_between(&rng, 10, 100);
                test->inputs[1][vi] = p;
                vi++;
                break;
            case 1:
                q = mono_rng_next_between(&rng, 10, 100);
                test->inputs[1][vi] = q;
                vi++;
                break;
            case 2:
                int16_t temp = p;
                p = q;
                q = temp;
                break;
            case 3:
                p += q;
                break;
            case 4:
                test->outputs[0][oi] = p;
                oi++;
                break;
        }
    }
    test->input_sizes[0] = 39;
    test->input_sizes[1] = vi;
    test->output_sizes[0] = oi;
    return 0;
}

int gen_t31_node_emulator(uint32_t seed, test_case_buffer *test) {
    mono_rng rng = create_mono_rng(seed);
    int16_t ram[8] = {0};
    int ii = 0;
    int oi = 0;
    do {
        int16_t i = mono_rng_next_between(&rng, 0, 8);
        int16_t v = mono_rng_next_between(&rng, 10, 100);
        int16_t rw = mono_rng_next_between(&rng, 0, 2);
        if (rw > 0) {
            if (ram[i] > 0) {
                test->inputs[0][ii] = rw;
                test->inputs[0][ii + 1] = i;
                test->outputs[0][oi] = ram[i];
                ii += 2;
                oi++;
            }
        } else {
            test->inputs[0][ii] = rw;
            test->inputs[0][ii + 1] = i;
            test->inputs[0][ii + 2] = v;
            ram[i] = v;
            ii += 3;
        }
    } while (ii <= 36);
    test->input_sizes[0] = ii;
    test->output_sizes[0] = oi;
    return 0;
}

int gen_wave_collapse_supervisor(uint32_t seed, test_case_buffer *test) {
    mono_rng rng = create_mono_rng(seed);
    uint8_t sums[4] = {0};
    uint8_t max_sum = 0;
    for (int i = 0; i < 39; i++) {
        for (int j = 0; j < 4; j++) {
            uint16_t n = mono_rng_next_between(&rng, 0, 2);
            if (i >= 1 && (test->outputs[0][i - 1] == j + 1)) {
                n = mono_rng_next_between(&rng, -1, 1);
            }
            test->inputs[j][i] = n;
            sums[j] += n;
            if (sums[j] > max_sum) {
                max_sum = sums[j];
                test->outputs[0][i] = j + 1;
            }
        }
        max_sum = 0;
    }
    test->input_sizes[0] = 39;
    test->input_sizes[1] = 39;
    test->input_sizes[2] = 39;
    test->input_sizes[3] = 39;
    test->output_sizes[0] = 39;
    return 0;
}

int gen_unknown(uint32_t seed, test_case_buffer *test) {
    // The algorithm used in the game is drastically different,
    // so there are a few weird-looking details that handles the artifacts of the original algorithm.
    unity_rng rng = create_unity_rng(seed);
    int offset = 0;
    int seq_i = 0;
    int16_t prev_val = 181;
    while (offset < 38) {
        int16_t val = unity_rng_next_between(&rng, 0, 4);
        int len = unity_rng_next_between(&rng, 2, 5);
        int16_t actual_len = MIN(len, 38 - offset);
        for (int i = 0; i < actual_len; i++) {
            test->outputs[0][offset + i] = val;
        }
        if (prev_val != val) {
            test->outputs[1][seq_i] = actual_len;
            test->outputs[1][seq_i + 1] = val;
            seq_i += 2;
        } else {
            test->outputs[1][seq_i - 2] += actual_len;
        }
        prev_val = val;
        offset += len;
    }
    test->outputs[0][38] = -1;
    if (offset == 38) {
        unity_rng_next(&rng);
        unity_rng_next(&rng);
    }

    for (int i = 0; i < 39; i++) {
        test->inputs[0][i] = test->outputs[0][i] * 25 + unity_rng_next_between(&rng, 6, 19);
    }
    test->inputs[0][38] = -1;
    test->input_sizes[0] = 39;
    test->output_sizes[0] = 39;
    test->output_sizes[1] = seq_i;
    return 0;
}

#undef MAX
#undef MIN
