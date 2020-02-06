/*! \file
 *  rtc_timer.c
 *  xenon-lib-tiny
 *  Copyright (c) 2020 Martin Clemons
 */
#include "rtc_timer.h"
#include "util/atomic.h"
#include "string.h"


/*** Private Global Variables ------------------------------------------------*/
/*! \privatesection */


/*** Public Global Variables -------------------------------------------------*/
/*! \publicsection */


/*** Private Function Prototypes ---------------------------------------------*/
/*! \privatesection */


/*** Interrupt Service Routines (ISR) ----------------------------------------*/
/*! \privatesection */


/*** Private Functions -------------------------------------------------------*/
/*! \privatesection */


/*** Public Functions --------------------------------------------------------*/
/*! \publicsection */

// Either include `rtc_isr.c` in compilation or implement this function in custom ISR code.
extern uint16_t rtcGetSoftCounter(void);

/*! Initialize a new timer.
 *  @param t Pointer to timer to initialize
 *  @param period Period in RTC "ticks" (overflows) for the new timer. Maximum value is 0x7FFF ticks.
 */
void rtcTimerInit(rtcTimer_t *t, uint16_t period)
{
    uint16_t count;
    if (t == NULL) {
        return;
    }
    period &= 0x7FFF;       // mask highest bit of period - this is reserved for overflow tracking
    // initialize timer
    count = rtcGetSoftCounter();
    // if expireCount overflows, so will timer. Therefore period should be preserved through overflow.
    t->expireCount = count + period;
}

/*! Add a time period to an active timer.
 *  @param t Pointer to timer to add time period.
 *  @param period Period in RTC "ticks" (overflows) to add.
 */
void rtcTimerAddPeriod(rtcTimer_t *t, uint16_t period)
{
    if (t == NULL) {
        return;
    }
    period &= 0x7FFF;
    t->expireCount += period;
}

/*! Check if a timer is active.
 *  @param timer Pointer to timer to check.
 *  @return Returns 0 if timer has elapsed, or 1 if timer is active.
 */
uint8_t rtcTimerActive(rtcTimer_t *t)
{
    uint16_t rtc;
    if (t == NULL) {
        return 0;
    }
    rtc = rtcGetSoftCounter();
    // compare overflow bits
    if ((t->expireCount & 0x8000) == (rtc & 0x8000)) {
        if ((rtc & 0x7FFF) < (t->expireCount & 0x7FFF)) {
            return 1;
        }
    } else {
        if ((rtc & 0x7FFF) >= (t->expireCount & 0x7FFF)) {
            return 1;
        }
    }
    return 0;
}
