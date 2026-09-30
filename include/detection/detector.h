/**
 * @file detector.h
 * @brief Single-instance and Multi-instance Deadlock Detection Algorithms.
 */

#ifndef DD_DETECTION_DETECTOR_H
#define DD_DETECTION_DETECTOR_H

#include "core/resource_mgr.h"
#include "core/graph.h"
#include "core/types.h"
#include <stdbool.h>

void detector_build_wfg(const ResourceManager *rm, WaitForGraph *wfg);
void detector_build_rag(const ResourceManager *rm, ResourceAllocationGraph *rag);
bool detector_detect_wfg_cycle(const ResourceManager *rm, DeadlockReport *out_report);
bool detector_detect_multi_instance(const ResourceManager *rm, DeadlockReport *out_report);
bool detector_detect_deadlock(const ResourceManager *rm, DeadlockReport *out_report);

#endif /* DD_DETECTION_DETECTOR_H */
