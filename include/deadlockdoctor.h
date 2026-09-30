/**
 * @file deadlockdoctor.h
 * @brief Main Umbrella Header for DeadlockDoctor.
 */

#ifndef DEADLOCKDOCTOR_H
#define DEADLOCKDOCTOR_H

#include "core/types.h"
#include "core/resource_mgr.h"
#include "core/graph.h"
#include "avoidance/banker.h"
#include "detection/detector.h"
#include "recovery/recovery.h"
#include "prevention/prevention.h"
#include "scenario/scenario.h"
#include "demo/live_demo.h"
#include "ui/visualizer.h"

#define DEADLOCKDOCTOR_VERSION "2.0.0"

#endif /* DEADLOCKDOCTOR_H */
