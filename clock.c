/*! \file
 *  clock.c
 *  xenon-lib-tiny
 *
 * Copyright (c) 2019 Martin Clemons
 */
#include <util/delay.h>
#include "clock.h"


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

/*! Set system clock prescaler. **Not reentrant**
 * @param ps The prescale factor, set to `SYS_CLOCK_PRESCALE_DIV1` to disable
 * prescaler.
 */
void clockSetSysClockPrescaler(enum sysClockPrescaler_e ps)
{
    uint8_t mask;
    if (ps == SYS_CLOCK_PRESCALE_DIV1) {
        // clear the PEN bit, other bits don't matter
        mask = 0x00;
    } else {
        // set the PEN bit, bitshift mask 1 position
        mask = ps | CLKCTRL_PEN_bm;
    }
    // wait for any pending clock changes to finish
    loop_until_bit_is_clear(CLKCTRL.MCLKSTATUS, CLKCTRL_SOSC_bp);
    // enable changing protected registers
    CPU_CCP = CCP_IOREG_gc;
    // set mask
    CLKCTRL.MCLKCTRLB = mask;
}

/*! Set system clock source. **Not reentrant**
 * @param source The clock source to use.
 * Note: When changing clock sources and prescalers, ensure that even transient
 * combinations of clock source and prescaler do not exceed the manximum system
 * clock frequency for a given voltage and temperature range.
 */
void clockSetSysClockSource(enum sysClockSource_e source)
{
    // save state of CLKOUT pin
    uint8_t mask = CLKCTRL.MCLKCTRLA & CLKCTRL_CLKOUT_bm;
    mask |= source;
    // wait for any pending clock changes to finish
    loop_until_bit_is_clear(CLKCTRL.MCLKSTATUS, CLKCTRL_SOSC_bp);
    // enable changing protected registers
    CPU_CCP = CCP_IOREG_gc;
    // set mask
    CLKCTRL.MCLKCTRLA = mask;
    // wait for change to complete
    loop_until_bit_is_clear(CLKCTRL.MCLKSTATUS, CLKCTRL_SOSC_bp);
}

/*! Configure external 32.768kHz oscillator. **Not reentrant**
 * @param sut The oscillator start-up time.
 * @param src The oscillator source type (crystal / ext. clock on `TOSC1` pin).
 * @param runStdby Always run oscillator, sets the `RUNSTDBY` bit.
 * Function stops a running oscillator to make changes to configuration
 * register.
 * Function always waits for `XOSC32KS` flag bit to be cleared before making
 * changes to configuration register.
 * If oscillator is running, function starts it again with new settings
 * applied and waits for `XOSC32KS` flag bit to be set.
 * If oscillator is not running, it will not be started.
 */
void clockConfigXOsc32k(enum xOsc32kStartupTime_e sut,
        enum xOsc32kSourceType_e src,
        bool runStdby)
{
    // set mask to ENABLE bit state
    uint8_t mask = CLKCTRL.XOSC32KCTRLA & CLKCTRL_ENABLE_bm;
    if (mask > 0) {
        // disable oscillator if it's running
        CPU_CCP = CCP_IOREG_gc;
        CLKCTRL.XOSC32KCTRLA &= ~CLKCTRL_ENABLE_bm;
    }
    // wait for oscillator stable flag to be 0 (datasheet section 10.5.9)
    loop_until_bit_is_clear(CLKCTRL.MCLKSTATUS, CLKCTRL_XOSC32KS_bp);
    // compute configuration mask
    mask |= sut | src | ((runStdby == true) ? CLKCTRL_RUNSTDBY_bm: 0x0);
    // apply configuration
    CPU_CCP = CCP_IOREG_gc;
    CLKCTRL.XOSC32KCTRLA = mask;
    if (mask & CLKCTRL_ENABLE_bm) {
        // if oscillator was enabled wait for oscillator stable flag
        loop_until_bit_is_set(CLKCTRL.MCLKSTATUS, CLKCTRL_XOSC32KS_bp);
    }
}

/*! Start (enable) the external 32.768kHz oscillator. **not reentrant**
 *  Function waits for `XOSC32KS` flag bit to be set.
 */
void clockStartXOsc32k(void)
{
    uint8_t mask = CLKCTRL.XOSC32KCTRLA | CLKCTRL_ENABLE_bm;
    CPU_CCP = CCP_IOREG_gc;
    CLKCTRL.XOSC32KCTRLA = mask;
    loop_until_bit_is_set(CLKCTRL.MCLKSTATUS, CLKCTRL_XOSC32KS_bp);
}

/*! Stop (disable) the external 32.768kHz oscillator. **not reentrant**
 */
void clockStopXOsc32k(void)
{
    uint8_t mask = CLKCTRL.XOSC32KCTRLA & ~CLKCTRL_ENABLE_bm;
    CPU_CCP = CCP_IOREG_gc;
    CLKCTRL.XOSC32KCTRLA = mask;
}
