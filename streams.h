#ifndef STREAMS_H
#define STREAMS_H
#include <stdint.h>

typedef int16_t stream_buffer[39];

typedef uint8_t image_buffer[540];

typedef struct test_case_buffer {
    uint8_t input_sizes[4];
    stream_buffer inputs[4];
    uint8_t output_sizes[4];
    stream_buffer outputs[4];
    image_buffer image;
} test_case_buffer;

typedef struct output_buffer {
    stream_buffer outputs[4];
    image_buffer image;
} output_buffer;

#endif
