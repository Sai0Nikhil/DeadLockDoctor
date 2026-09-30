/**
 * @file banker.h
 * @brief Banker's Algorithm Implementation for Deadlock Avoidance.
 */

#ifndef DD_AVOIDANCE_BANKER_H
#define DD_AVOIDANCE_BANKER_H

#include "core/resource_mgr.h"
#include "core/types.h"
#include <stdbool.h>

bool banker_check_safety(const ResourceManager *rm, SafeSequence *out_seq);
bool banker_evaluate_request(const ResourceManager *rm, int pid, const int req_vector[], SafeSequence *out_seq);

#endif /* DD_AVOIDANCE_BANKER_H */
