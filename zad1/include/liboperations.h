//
// Created by sirius on 15.03.17.
//

#ifndef ZAD1_LIBOPERATIONS_H
#define ZAD1_LIBOPERATIONS_H

#include <inttypes.h>

void lib_generate(char* fname, uint32_t record_count, uint32_t record_size);
void lib_sort(char* fname, uint32_t record_count, uint32_t record_size);
void lib_shuffle(char* fname, uint32_t record_count, uint32_t record_size);

#endif //ZAD1_LIBOPERATIONS_H
