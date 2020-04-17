/*! \file
 *  futures.h
 *  xenon-lib-tiny
 *  Copyright (c) 2020 Martin Clemons
 *
 *  Very basic 'future' data types with different `promise` types.
 *  See http://dist-prog-book.com/chapter/2/futures.html
 *  An unresolved future has `promise` set to 0 (or some undefined value), and
 *  resolved set to false. Once the asynchrounouse call completes, the result is
 *  written to `promise` and `resolved` is set to `true`. The code using the
 *  future may then read the value stored in `promise`.
 */
#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <avr/io.h>
#include "task_scheduler.h"


/*** Public Variables --------------------------------------------------------*/
/*! \publicsection */

/*! The callback function called by the task scheduler is passed a single
 * parameter. This union represents the possible data types which may be in
 * that parameter. This can be for example used to pass some kind of state
 * information to a function.
 */
typedef union callbackParamTypes_u promise_t;

/*! A basic data structure for a Future. Includes a promise value, a task
 * scheduler task, and a boolean field indicating whether the promise is
 * resolved.
 * Note: `task` must be first element of a future so the address of the future
 * can be inferred from the address of the task.
 */
typedef struct future_s {
    task_t task;           ///< Task scheduler task associated with this future.
    promise_t *promise;    ///< The value the future eventually `returns`.
    bool resolved;         ///< Boolean indicating whether promise is resolved.
} future_t;


/*** Public Functions --------------------------------------------------------*/
/*! \publicsection */

#define future_resolved(future) ((future).resolved)
#define future_unresolved(future) (!(future).resolved)
