/*! \file
 *  task_scheduler_private.h
 *  xenon-lib-tiny
 *  Copyright (c) 2020 Martin Clemons
 *
 *  Private data types for the task scheduler library. These data types are
 *  reserved for internal use, but are here so the function caller can allocate
 *  memory for a task.
 */
#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <avr/io.h>
#include "rtc_timer.h"


/*** Public Variables --------------------------------------------------------*/
/*! \publicsection */


/*** Private Variables -------------------------------------------------------*/
/*! \privatesection */

/*! The types of tasks currently supported by the task scheduler.
 */
enum taskType_e {
    TASK_EMPTY  = 0,                ///< Unallocated task
    TASK_RECURRING,                 ///< Recurring, unconditional task
    TASK_SINGLE_SHOT,               ///< Single-shot, unconditional task
    TASK_TIMED,                     ///< Timed task, due when timer expires
    TASK_CONDITIONAL,               ///< Conditional task, due when (reg & mask) > 0
    TASK_CONDITIONAL_SH,            ///< Conditional task, single shot
};

/*! Internal state data unique to timed tasks
 */
struct timedState_s {
    rtcTimer_t dueTimer;            ///< timer indicating when task is due again
    uint16_t period;                ///< period for rescheduling repeating task
};

/*! Internal data unique to queued tasks
 */
struct queuedState_s {
    ///< empty at this point
};

/*! Internal data unique to conditional tasks
 */
union callbackParamTypes_u;
struct conditionalState_s {
    bool (*cb)(union callbackParamTypes_u *);        ///< conditional check callback function
    union callbackParamTypes_u
            *conditionalParam;      ///< conditional check callback function parameter
};

/*! Data structure for a task
 */
struct tsTask_s {
    enum taskType_e type;
    union {
        struct timedState_s        timed;
        struct queuedState_s       queued;
        struct conditionalState_s  conditional;
    } state;
    void (*cb)(union callbackParamTypes_u *);
    union callbackParamTypes_u *cbParam;
    struct tsTask_s *next;
};

/*** Public Functions --------------------------------------------------------*/
/*! \publicsection */
