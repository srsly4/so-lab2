//
// Created by sirius on 14.03.17.
//

#include <stdint.h>

#ifndef ZAD1_SYSOPERATIONS_H
#define ZAD1_SYSOPERATIONS_H

void sys_generate(char* fname, uint32_t record_count, uint32_t record_size);
void sys_sort(char* fname, uint32_t record_count, uint32_t record_size);

#endif //ZAD1_SYSOPERATIONS_H
