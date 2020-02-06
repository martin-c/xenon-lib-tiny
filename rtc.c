/*! \file
 *  rtc.c
 *  xenon-lib-tiny
 *  Copyright (c) 2019 Martin Clemons
 *  This file provides basic utility functions for configuring and running the
 *  RTC (Real Time Counter) peripheral.
 */
#include "rtc.h"
#include "util/atomic.h"


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

/*! Set the RTC clock source. ** Not Reentrant **
 * The desired clock source is not enabled by this function and must be enabled elsewhere.
 * @param src The RTC clock source.
 */
void rtcSetClockSource(enum rtcClockSource_e src)
{
    switch (src) {
        /* Although `RTC_CLOCK_SOURCE_XOSC32K` and `RTC_CLOCK_SOURCE_TOSC1` are
         * distinct enum values, these values share the same `CLKSEL` register value,
         * `RTC_CLKSEL_TOSC32K_gc` or `0x02`.*/
        case RTC_CLOCK_SOURCE_XOSC32K:
        case RTC_CLOCK_SOURCE_TOSC1:
            RTC.CLKSEL = RTC_CLKSEL_TOSC32K_gc;
            break;
        default:
            RTC.CLKSEL = src;
    }
}

/*! Set RTC prescaler. ** Not Reentrant **
 * Function waits for clock domain sync by polling the RTC_CTRLABUSY bit
 * before changing the CTRLA register.
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
}

/*! Set the RTC Period register. ** Not Reentrant **
 * The value supplied is written to the RTC Period register.
 * The RTC is *not* stopped before writing to register.
 * Function waits for clock domain sync before writing.
 * @param period The value to write to the RTC Period register.
 */
void rtcSetPeriod(uint16_t period)
{
    // wait for clock domain sync
    loop_until_bit_is_clear(RTC.STATUS, RTC_PERBUSY_bp);
    RTC.PER = period;
}

/*! Set the RTC Compare register. ** Not Reentrant **
 * The value supplied is written to the RTC Compare register.
 * The RTC is *not* stopped before writing to register.
 * Function waits for clock domain sync before writing.
 * @param compare The value to write to the RTC Compare register.
 */
void rtcSetCompare(uint16_t compare)
{
    // wait for clock domain sync
    loop_until_bit_is_clear(RTC.STATUS, RTC_CMPBUSY_bp);
    RTC.CMP = compare;
}

/*! Return the value of the 16-bit RTC count register. ** Not Reentrant, may be called from ISR **
 * Interrupts are temporarily disabled to ensure atomic access from ISR and/or
 * main thread.
 * @return Returns the 16-bit RTC count value.
 */
uint16_t rtcGetCount(void)
{
    uint16_t count;
    // The temp register is shared with other RTC registers, disable interrupts
    // to ensure atomic access.
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        count = RTC.CNT;
    }
    return count;
}

/*! Set the value of the 16-bit RTC count register. ** Not Reentrant, may be called from ISR **
 * Interrupts are temporarily disabled to ensure atomic access from ISR and/or
 * main thread.
 * @param: count The value to set the RTC Count register to.
 */
void rtcSetCount(uint16_t count)
{
    // try to wait outside of block where ISR is disabled
    loop_until_bit_is_clear(RTC.STATUS, RTC_CNTBUSY_bp);
    // The temp register is shared with other RTC registers, disable interrupts
    // to ensure atomic access.
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        // make sure bit is still clear
        loop_until_bit_is_clear(RTC.STATUS, RTC_CNTBUSY_bp);
        RTC.CNT = count;
    }
}

/*! Configure and start the RTC in a single function call. ** Not Reentrant **
 * Function waits for clock domain sync before writing RTC registers.
 * @param config Pointer to `rtcConfig_s` with RTC configuration values.
 */
void rtcInitEnable(struct rtcConfig_s *config)
{
    // configure interrupt flags
    RTC.INTCTRL = (config->compareInterruptEnable ? RTC_CMP_bm: 0x00) |
                  (config->overflowInterruptEnable ? RTC_OVF_bm: 0x00);
    // clear counter, set period and compare registers
    rtcSetCount(0);
    rtcSetPeriod(config->period);
    rtcSetCompare(config->compare);
    // set clock
    rtcSetClockPrescaler(config->ps);
    rtcSetClockSource(config->src);
    // set runstdby and enable
    rtcEnable(config->rtcRunStdby);
}

/*! Set the RTC enable and run standby bits in RTC CTRLA register. ** Not Reentrant **
 * Function waits for clock domain sync by polling the RTC_CTRLABUSY bit before
 * writing CTRLA register.
 * @param runStdby The state to set the RUNSTDBY bit.
 */
void rtcEnable(bool runStdby)
{
    // wait for clock domain sync
    loop_until_bit_is_clear(RTC.STATUS, RTC_CTRLABUSY_bp);
    RTC.CTRLA |= RTC_RTCEN_bm | (runStdby ? RTC_RUNSTDBY_bm: 0x00);
}

/*! Clear the RTC enable bit in RTC CTRLA register. ** Not Reentrant **
 * Function waits for clock domain sync by polling the RTC_CTRLABUSY bit before
 * writing CTRLA register.
 */
void rtcDisable(void)
{
    // wait for clock domain sync
    loop_until_bit_is_clear(RTC.STATUS, RTC_CTRLABUSY_bp);
    RTC.CTRLA &= ~RTC_RTCEN_bm;
}
