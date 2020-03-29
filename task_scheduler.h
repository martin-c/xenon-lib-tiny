/*! \file
 *  task_scheduler.h
 *  xenon-lib-tiny
 *  Copyright (c) 2020 Martin Clemons
 *  A lightweight, cooperatively scheduled, single-thread task scheduler.
 *  This is a very simple way to schedule call-back execution either directly,
 *  once when a timer expires (single-shot), repeatedly when a timer expires,
 *  or when a certain condition is met.
 */

#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "rtc_timer.h"


/*** Public Variables --------------------------------------------------------*/
/*! \publicsection */
/*! The callback function called by the task scheduler is passed a single
 * parameter. This union represents the possible data types which may be in
 * that parameter. This can be for example used to pass some kind of state
 * information to a function.
 */
typedef union callbackParamTypes_u {
    uint8_t     uint8;
    uint16_t    uint16;
    uint8_t *   uint8_ptr;
    uint16_t *  uint16_prt;
    void *      void_ptr;
} cbParam_t;

/*! Status returned when a task is initialized.
 */
enum addStatus_e {
    TASK_INIT_OK     = 0,       ///< Task added to task manager queue
    TASK_INIT_ERROR             ///< Error, task not added.
};

/*! A task function pointer
 *  A function returning void with a pointer to
 *  `union callbackParamTypes_u` as a single argument.
 *  Function is what the task manager calls when a task is run.
 */
typedef void (cb_t)(cbParam_t *);


/*** Private Variables -------------------------------------------------------*/
/*! \privatesection */
// These types are here so the compiler can allocate memory for `task_t`.
// Enum to track task type
enum taskType_e {
    TASK_EMPTY  = 0,                ///< Unallocated task
    TASK_TIMED,                     ///< Timed task, due when timer expires
    TASK_QUEUED,                    ///< Queued task, due at next call to tsMain()
    TASK_CONDITIONAL,               ///< Conditional task, due when (reg & mask) > 0
    TASK_CONDITIONAL_SH,            ///< Conditional task, single shot
};

// parameters unique to timed tasks
struct timedParams_s {
    rtcTimer_t dueTimer;            ///< timer indicating when task is due again
    uint16_t period;                ///< period for rescheduling repeating task
};

// parameters unique to queued tasks
struct queuedParams_s {
    ///< empty at this point
};

// parameters unique to conditional tasks
struct conditionalParams_s {
    bool (*cb)(cbParam_t *);        ///< conditional check callback function
    cbParam_t *conditionalParam;    ///< conditional check callback function parameter
};

// a single task, do not modify directly
typedef struct tsTask_s {
    enum taskType_e type;
    union {
        struct timedParams_s        timed;
        struct queuedParams_s       queued;
        struct conditionalParams_s  conditional;
    } params;
    cb_t *cb;
    cbParam_t *cbParam;
    struct tsTask_s *next;
} task_t;


/*** Public Functions --------------------------------------------------------*/
/*! \publicsection */
enum addStatus_e tsAddTimedTask(task_t *task, cb_t *cb, cbParam_t *cbParam, int16_t period);
enum addStatus_e tsAddTimedSingleShotTask(task_t *task, cb_t *cb,
                                          cbParam_t *cbParam, int16_t period);
enum addStatus_e tsAddTask(task_t *task, cb_t *cb, cbParam_t *cbParam);
enum addStatus_e tsAddConditionalTask(task_t *task, cb_t *cb, cbParam_t *cbParam,
                                      bool (*conditionalCheck)(cbParam_t *), cbParam_t *conditionalParam);
enum addStatus_e tsAddConditionalSingleShotTask(task_t *task, cb_t *cb, cbParam_t *cbParam,
                                                bool (*conditionalCheck)(cbParam_t *),
                                                cbParam_t *conditionalParam);
void tsRemoveTask(task_t *task);
void tsMain(void);
