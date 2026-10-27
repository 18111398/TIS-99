#include "puzzle.h"
#include "random.h"
#include <stdint.h>
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

#undef MAX
#undef MIN
