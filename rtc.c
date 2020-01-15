/*! \file
 *  rtc.c
 *  xenon-lib-tiny
 *  Copyright (c) 2019 Martin Clemons
 */
#include "rtc.h"



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

/*! Set RTC prescaler. **Not reentrant**
 * @param ps The RTC clock prescale factor.
 */
void rtcSetClockPrescaler(enum rtcPrescale_e ps)
{
    // wait for clock domain sync
    loop_until_bit_is_clear(RTC.STATUS, RTC_CTRLABUSY_bp);
    // don't change state of RUNSTDBY and RTCEN bits
    uint8_t mask = RTC.CTRLA & (RTC_RUNSTDBY_bm | RTC_RTCEN_bm);
    mask |= ps;
    RTC.CTRLA = mask;
    // wait for clock domain sync
    loop_until_bit_is_clear(RTC.STATUS, RTC_CTRLABUSY_bp);
}

void rtcSetClockSource(enum rtcClockSource_e src)
{

}

/*! Initialize RTC based on settings in intialization struct.
 *  \param prescale RTC clock prescaler value.
 *  \param clkSource RTC clock source.
 *  \param ovfInterrupt RTC overflow interrupt priority level.
 *  \param period RTC period register value, sets overflow rate.
 */
void rtcInit(enum rtcPrescale_e prescale,
             enum rtcClockSource_e clkSource,
             enum rtCOvfInterrupt_e ovfInterrupt,
             uint16_t period) {
    
    CLK.RTCCTRL = clkSource;        // set RTC clock source
    RTC.INTCTRL = ovfInterrupt;     // set overflow interrupt priority level
    RTC.PER = period;               // set period register
    /* From AVR 1314:
     * The four synchronized registers are CTRL, CNT, PER and COMP. Note that the PER register does
     * not have a separate synchronizer; the synchronization of PER is triggered by synchronization
     * of the CNT, CTRL or COMP registers.
     */
    RTC.CTRL = prescale;            // set prescaler
}

/*! Start initialized RTC
 *
 */
void rtcStart(void) {
    rtcCount = 0;                   // reset RTC counter
    CLK.RTCCTRL |= CLK_RTCEN_bm;    // set enable bit for RTC clock signal
}

/*! Stop RTC
 *
 */
void rtcStop(void) {
    CLK.RTCCTRL &= ~(CLK_RTCEN_bm); // clear enable bit for RTC clock signal
}

/*  Initialize a new timer.
 *  \param t Pointer to timer to initialize
 *  \param period Period in RTC "ticks" (overflows) for the new timer. Maximum value is 0x7FFF ticks.
 */
void rtcTimerInit(rtcTimer_t *t, uint16_t period) {
    
    uint16_t count;
    
    if (t == NULL) {
        return;
    }
    period &= 0x7FFF;           // mask highest bit of period - this is reserved for overflow tracking
    // initialize timer
    count = rtcGetCounter();
    // if expireCount overflows, so will timer. Therefore period should be preserved through overflow.
    t->expireCount = count + period;
}

/*  Add a time period to an active timer.
 *  \param t Pointer to timer to add time period.
 *  \param period Period in RTC "ticks" (overflows) to add.
 */
void rtcTimerAddPeriod(rtcTimer_t *t, uint16_t period) {
    
    if (t == NULL) {
        return;
    }
    period &= 0x7FFF;
    t->expireCount += period;
}

/*! Check if a timer is active.
 *  \param timer Pointer to timer to check.
 *  \return Returns 0 if timer has elapsed, or 1 if timer is active.
 */
uint8_t rtcTimerActive(rtcTimer_t *t) {
    
    uint16_t rtc;
    
    if (t == NULL) {
        return 0;
    }
    
    rtc = rtcGetCounter();
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

/*! Get 16-bit RTC counter.
 *  
 *  \return 16-bit RTC counter value.\n
 *  Counter starts at 0 on RTC initialization, and increments by 1 on every RTC overflow.
 */
uint16_t rtcGetCounter(void) {
    
    uint16_t c;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        c = rtcCount;
    }
    return c;
}

