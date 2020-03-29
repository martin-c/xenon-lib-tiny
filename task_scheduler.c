/*! \file
 *  task_scheduler.c
 *  xenon-lib-tiny
 *  Copyright (c) 2020 Martin Clemons
 */


#include "task_scheduler.h"
#include <stdio.h>
#include <boards/current.h>
#include "rtc_timer.h"


/*** Private Global Variables ------------------------------------------------*/
/*! \privatesection */

/* Tasks are tracked using linked lists. There is a separate list for timed
 * tasks which is always iterated first in `tsMain`.
 * Since a list may be modified as it's being iterated (tasks may be added or
 * removed by callbacks or scheduler itself), modifications are performed as
 * follows:
 * Additions: New elements (tasks) are added to a separate `add_list`, which is
 * merged into the master list at the start of the next iteration.
 * Deletions: tasks are marked for deletion by setting their `type` to
 * `TASK_EMPTY`, then removed near the end of each iteration when it is safe to
 * do so.
 * By performing list modifications as defined above, the pointers used for
 * iterating the list always remain consistent.
 */
static struct taskList_s {
    task_t *first;
    struct {
        task_t *first;
        task_t *last;
    } add_list;
} timedTasks, conditionalTasks;


/*** Public Global Variables -------------------------------------------------*/
/*! \publicsection */


/*** Private Function Prototypes ---------------------------------------------*/
/*! \privatesection */
static void addTask(struct taskList_s *list, task_t *task);
static void removeTask(struct taskList_s *list, task_t *task, task_t *up);
static void mergeAddList(struct taskList_s *list);


/*** Interrupt Service Routines (ISR) ----------------------------------------*/
/*! \privatesection */


/*** Private Functions -------------------------------------------------------*/
/*! \privatesection */

/*! Add task to the 'add list' of a linked list of tasks.
 * @param list The list to add task to.
 * @param task The task to add to the list.
 * Task is inserted at the beginning of the `add_list`. The `add_list.first`
 * and `add_list.last` data structures are updated.
 */
static void addTask(struct taskList_s *list, task_t *task)
{
    // printf_P(PSTR("addTask: %p\r\n"), task);
    task->next = list->add_list.first;
    list->add_list.first = task;
    if (list->add_list.last == NULL) {
        // this is the first task added so it will always be last on the list
        list->add_list.last = task;
    }
}

/*! Remove a task from a linked list of tasks.
 * @param list The list ro remove task from.
 * @param task The task to remove from the list.
 * @param up Helper value pointing to the element 'above' (before) this element
 * in list (or `NULL` if `task` is the first list element).
 */
static void removeTask(struct taskList_s *list, task_t *task, task_t *up)
{
    if (up == NULL) {
        // first task in list
        list->first = task->next;
    } else {
        up->next = task->next;
    }
}

/*! Merge the add_list into the master list.
 * @param list The list to perform the merge on.
 */
static void mergeAddList(struct taskList_s *list)
{
    if (list->add_list.first == NULL) {
        // add list is empty
        return;
    }
    // last item in add list linked to first item in list
    list->add_list.last->next = list->first;
    // start of list points to first item in add list
    list->first = list->add_list.first;
    // reset add list
    list->add_list.first = NULL;
    list->add_list.last = NULL;
}


/*** Public Functions --------------------------------------------------------*/
/*! \publicsection */

/*! Add a new timed repeating task.
 * @param task Pointer to data structure where task is stored.
 * @param cb Pointer to the function which will be called by the scheduler (task).
 * @param cbParam The parameter passed to `cb` when it is called.
 * @param period Timer ticks between calls to task.
 * @return returns `TASK_ADD_OK` if task added to scheduler, `TASK_ADD_ERROR` otherwise.
 */
enum addStatus_e tsAddTimedTask(task_t *task, cb_t *cb, cbParam_t *cbParam, int16_t period)
{
    if (task == NULL || cb == NULL) {
        return TASK_INIT_ERROR;
    }
    // initialize new task
    task->cb = cb;
    task->cbParam = cbParam;
    task->type = TASK_TIMED;
    rtcTimerInit(&task->params.timed.dueTimer, period);
    task->params.timed.period = period;
    addTask(&timedTasks, task);
    return TASK_INIT_OK;
}

/*! Add a new timed one-shot task. Task is automatically removed after it is
 * called once.
 * @param task Pointer to data structure where task is stored.
 * @param cb Pointer to the function which will be called by the scheduler (task).
 * @param cbParam The parameter passed to `cb` when it is called.
 * @param period Timer ticks before task is called.
 * @return returns `TASK_ADD_OK` if task added to scheduler, `TASK_ADD_ERROR` otherwise.
 */
enum addStatus_e tsAddTimedSingleShotTask(task_t *task, cb_t *cb,
                                          cbParam_t *cbParam, int16_t period)
{
    if (task == NULL || cb == NULL) {
        return TASK_INIT_ERROR;
    }
    // initialize new task
    task->cb = cb;
    task->cbParam = cbParam;
    task->type = TASK_TIMED;
    rtcTimerInit(&task->params.timed.dueTimer, period);
    task->params.timed.period = 0;
    addTask(&timedTasks, task);
    return TASK_INIT_OK;
}

/*! Add a single-shot task. Task is called once the next time the scheduler runs.
 * @param task Pointer to data structure where task is stored.
 * @param cb Pointer to the function which will be called by the scheduler (task).
 * @param cbParam The parameter passed to `cb` when it is called.
 * @return returns `TASK_ADD_OK` if task added to scheduler, `TASK_ADD_ERROR` otherwise.
 */
enum addStatus_e tsAddTask(task_t *task, cb_t *cb, cbParam_t *cbParam)
{
    if (task == NULL || cb == NULL) {
        return TASK_INIT_ERROR;
    }
    // initialize new task
    task->cb = cb;
    task->cbParam = cbParam;
    task->type = TASK_QUEUED;
    addTask(&conditionalTasks, task);
    return TASK_INIT_OK;
}

/*! Add a conditional repeating task. A conditional task calls a check function
 * each time the scheduler runs to determine if task should be run. The check
 * function should be kept very short otherwise it will slow the scheduler.
 * @param task Pointer to data structure where task is stored.
 * @param cb Pointer to the function which will be called by the scheduler (task).
 * @param cbParam The parameter passed to `cb` when it is called.
 * @param conditionalCheck Pointer to the check function scheduler calls.
 * @param conditionalParam The parameter passed to `conditionalCheck` when it is called.
 * @return returns `TASK_ADD_OK` if task added to scheduler, `TASK_ADD_ERROR` otherwise.
 */
enum addStatus_e tsAddConditionalTask(task_t *task, cb_t *cb, cbParam_t *cbParam,
                                      bool (*conditionalCheck)(cbParam_t *), cbParam_t *conditionalParam)
{
    if (task == NULL || cb == NULL || conditionalCheck == NULL) {
        return TASK_INIT_ERROR;
    }
    // initialize new task
    task->cb = cb;
    task->cbParam = cbParam;
    task->type = TASK_CONDITIONAL;
    task->params.conditional.cb = conditionalCheck;
    task->params.conditional.conditionalParam = conditionalParam;
    addTask(&conditionalTasks, task);
    return TASK_INIT_OK;
}

/*! Add a conditional single-shot task. A conditional task calls a check function
 * each time the scheduler runs to determine if task should be run. The check
 * function should be kept very short otherwise it will slow the scheduler.
 * Task is automatically removed after it is called once.
 * @param task Pointer to data structure where task is stored.
 * @param cb Pointer to the function which will be called by the scheduler (task).
 * @param cbParam The parameter passed to `cb` when it is called.
 * @param conditionalCheck Pointer to the check function scheduler calls.
 * @param conditionalParam The parameter passed to `conditionalCheck` when it is called.
 * @return returns `TASK_ADD_OK` if task added to scheduler, `TASK_ADD_ERROR` otherwise.
 */
enum addStatus_e tsAddConditionalSingleShotTask(task_t *task, cb_t *cb, cbParam_t *cbParam,
                                                bool (*conditionalCheck)(cbParam_t *),
                                                cbParam_t *conditionalParam)
{
    enum addStatus_e status = tsAddConditionalTask(task, cb, cbParam,
                                                   conditionalCheck, conditionalParam);
    if (status != TASK_INIT_OK) {
        return status;
    } else {
        task->type = TASK_CONDITIONAL_SH;
    }
    return TASK_INIT_OK;
}

/*! Remove a task from the scheduler. Task will be marked for removal immediately
 * and will no longer be called by the scheduler. The memory where the data
 * structure is stored should not be released until after the next scheduler
 * execution however.
 * @param task Pointer to data structure where task is stored.
 */
void tsRemoveTask(task_t *task)
{
    // mark task for removal, the actual removal occurs when list is iterated
    if (task != NULL) {
        task->type = TASK_EMPTY;
    }
}

/*! Task scheduler function. Call this function at regular intervals.
 * Function first iterates through timed tasks, followed by conditional or
 * single-shot tasks.
 * As a performance example, `tsMain` runs for ~18µs to iterate through 3
 * timed tasks on an ATtiny32xx running at 16MHz. This is when no tasks are
 * actually called. Adding a single conditional task with a trivial
 * `conditionalCheck` function `return flase;` increases the runtime to 21µs.
 */
void tsMain(void)
{
    ERROR_LED_ON;
    // merge tasks added to list since the last iteration
    mergeAddList(&timedTasks);
    /* Iterate over the linked list of timed tasks, from the start of the list
     * toward the end. Check timers and execute callbacks if required. Then
     * remove list elements with `TASK_EMPTY` type from list.
     * Variable `t` points to the current element being iterated.
     * Variable 'up' points to the previously iterated element, so one 'up'
     * in the list (toward the start).
     * Variable `t->next` points to the next element, so one 'down' in the list.
     */
    task_t *t = timedTasks.first;
    task_t *up = NULL;

    while(t != NULL) {
        // check timers and call callbacks if necessary
        if (t->type == TASK_TIMED) {
            if (rtcTimerActive(&t->params.timed.dueTimer) == 0) {
                // task timer has elapsed
                // printf_P(PSTR("-> %p\r\n"), t);
                t->cb(t->cbParam);
                if (t->params.timed.period > 0) {
                    // task is not single shot, renew timer
                    rtcTimerAddPeriod(&t->params.timed.dueTimer, t->params.timed.period);
                } else {
                    t->type = TASK_EMPTY;
                }
            }
        }
        // remove items with `TASK_EMPTY` type from list
        // note that t->type may have been modified since previous `if` statement
        if (t->type == TASK_EMPTY) {
            removeTask(&timedTasks, t, up);
        } else {
            // only update up pointer if current element stays in list
            up = t;
        }
        t = t->next;
    }

    // next iterate through conditional and queued tasks
    // merge tasks added to list since the last iteration
    mergeAddList(&conditionalTasks);
    t = conditionalTasks.first;
    up = NULL;

    while(t != NULL) {
        switch (t->type) {
            case TASK_QUEUED:
                // printf_P(PSTR("-> %p\r\n"), t);
                t->cb(t->cbParam);
                t->type = TASK_EMPTY;
                break;

            case TASK_CONDITIONAL:
            case TASK_CONDITIONAL_SH:
                if (t->params.conditional.cb(t->params.conditional.conditionalParam) == true) {
                    // conditional check passed
                    // printf_P(PSTR("-> %p\r\n"), t);
                    t->cb(t->cbParam);
                    if (t->type == TASK_CONDITIONAL_SH) {
                        t->type = TASK_EMPTY;
                    }
                }
                break;

            default:
                break;
        }
        // remove items with `TASK_EMPTY` type from list
        // note that t->type may have been modified since previous `case` statement
        if (t->type == TASK_EMPTY) {
            removeTask(&conditionalTasks, t, up);
        } else {
            // only update up pointer if current element stays in list
            up = t;
        }
        t = t->next;
    }
    ERROR_LED_OFF;
}
