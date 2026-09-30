/**
 * @file prevention.h
 * @brief Deadlock Prevention Strategies and Rule Enforcers.
 */

#ifndef DD_PREVENTION_PREVENTION_H
#define DD_PREVENTION_PREVENTION_H

#include "core/resource_mgr.h"
#include "core/types.h"
#include <stdbool.h>

bool prevention_check_resource_ordering(const ResourceManager *rm, int pid, int rid);
bool prevention_check_all_or_nothing(const ResourceManager *rm, int pid, int rid);
bool prevention_validate_request(const ResourceManager *rm, int pid, int rid, char *out_reason, size_t reason_len);

#endif /* DD_PREVENTION_PREVENTION_H */
