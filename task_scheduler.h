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

#include "task_scheduler_private.h"
#include <inttypes.h>
#include <stdbool.h>
#include <avr/io.h>


/*** Public Variables --------------------------------------------------------*/
/*! \publicsection */
/*! The callback function called by the task scheduler is passed a single
 * parameter. This union represents the possible data types which may be in
 * that parameter. This can be for example used to pass some kind of state
 * information to a function.
 */
union callbackParamTypes_u {
    uint8_t     uint8;
    uint16_t    uint16;
    uint8_t *   uint8_ptr;
    uint16_t *  uint16_prt;
    struct {
        uint8_t *buff;          ///< pointer to buffer
        uint8_t iter;           ///< buffer iterator
        const uint8_t length;   ///< buffer length
    }           buffer;         ///< A simple buffer type
    void *      void_ptr;
};

/*! Status returned when a task is initialized.
 */
enum addStatus_e {
    TASK_INIT_OK     = 0,       ///< Task added to task manager queue
    TASK_INIT_ERROR             ///< Error, task not added.
};

/*** Type definitions to help make certain types less verbose ----------------*/

/*! Shortcut for `union callbackParamTypes_u`, which is passed to functions
 * called by the task scheduler.
 */
typedef union callbackParamTypes_u cbParam_t;

/*! A task function pointer
 *  A function returning void with a pointer to
 *  `cbParam_t` as a single argument.
 *  Task manager calls function when a task is run.
 */
typedef void (cb_t)(cbParam_t *);

/*! A task's data structure used by the task scheduler.
 */
typedef struct tsTask_s task_t;


/*** Private Variables -------------------------------------------------------*/
/*! \privatesection */


/*** Public Functions --------------------------------------------------------*/
/*! \publicsection */
enum addStatus_e tsAddTimedTask(task_t *task, cb_t *cb, cbParam_t *cbParam, int16_t period);
enum addStatus_e tsAddTimedSingleShotTask(task_t *task, cb_t *cb,
                                          cbParam_t *cbParam, int16_t period);
enum addStatus_e tsAddTask(task_t *task, cb_t *cb, cbParam_t *cbParam, bool singleShot);
enum addStatus_e tsAddConditionalTask(task_t *task, cb_t *cb, cbParam_t *cbParam,
                                      bool (*conditionalCheck)(cbParam_t *), cbParam_t *conditionalParam);
enum addStatus_e tsAddConditionalSingleShotTask(task_t *task, cb_t *cb, cbParam_t *cbParam,
                                                bool (*conditionalCheck)(cbParam_t *),
                                                cbParam_t *conditionalParam);
void tsRemoveTask(task_t *task);
task_t * tsGetCurrentTask(void);
void tsMain(void);
