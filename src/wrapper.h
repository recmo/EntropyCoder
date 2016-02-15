#pragma once
#include <stdio.h>
#include <stdint.h>
#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif

struct HEntropyWriter;
typedef struct HEntropyWriter HEntropyWriter;

VISIBLE HEntropyWriter* entropy_writer_create(const char* filename);

VISIBLE int entropy_writer_finalize_destroy(HEntropyWriter* handle);

VISIBLE int entropy_writer_write(HEntropyWriter* handle, uint64_t start, uint64_t end);


struct HEntropyReader;
typedef struct HEntropyReader HEntropyReader;

VISIBLE HEntropyReader* entropy_reader_create(const char* filename);

VISIBLE int entropy_reader_destroy(HEntropyReader* handle);

VISIBLE int entropy_reader_eof(const HEntropyReader* handle);

VISIBLE uint64_t entropy_reader_value(const HEntropyReader* handle);

VISIBLE int entropy_reader_next(HEntropyReader* handle, uint64_t start, uint64_t end);

#ifdef __cplusplus
}
#endif
