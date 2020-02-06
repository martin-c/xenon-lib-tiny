/*! \file
 *  rtc_isr.c
 *  xenon-lib-tiny
 *  Copyright (c) 2020 Martin Clemons
 */
#include "rtc_isr.h"
#include "util/atomic.h"
#include "avr/interrupt.h"


/*** Private Global Variables ------------------------------------------------*/
/*! \privatesection */
/* RTC soft counter, should be incremented in RTC ISR */
static volatile uint16_t rtcCount;


/*** Public Global Variables -------------------------------------------------*/
/*! \publicsection */


/*** Private Function Prototypes ---------------------------------------------*/
/*! \privatesection */


/*** Interrupt Service Routines (ISR) ----------------------------------------*/
/*! \privatesection */
ISR(RTC_CNT_vect) {
    RTC.INTFLAGS |= RTC_OVF_bm;
    PORTB_OUT ^= PIN4_bm;
    rtcCount++;
}

/*** Private Functions -------------------------------------------------------*/
/*! \privatesection */


/*** Public Functions --------------------------------------------------------*/
/*! \publicsection */

/*! Get 16-bit RTC soft-counter value.
 *  \return 16-bit RTC counter value.\n
 *  Counter starts at 0 on RTC initialization, and increments by 1 on every RTC overflow.
 */
uint16_t rtcGetSoftCounter(void)
{
    uint16_t c;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        c = rtcCount;
    }
    return c;
}
