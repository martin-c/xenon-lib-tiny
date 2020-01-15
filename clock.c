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

/*! Stop (disable) 
void clockStopXOsc32k(void)
{
    uint8_t mask = CLKCTRL.XOSC32KCTRLA & ~CLKCTRL_ENABLE_bm;
    CPU_CCP = CCP_IOREG_gc;
    CLKCTRL.XOSC32KCTRLA = mask;
}


///*! Enable an oscillator.
// *  Function does not check if oscillator is stable before returning.
// *  \param mask Mask of oscillator(s) to enable.
// */
//void clockEnableOsc(enum clockSource_e src)
//{
//    uint8_t mask = 1 << src;    // mask for control and status registers corresponding to new clock source
//    OSC.CTRL |= mask;
//}
//
///*! Disable an oscillator.
// *  \param mask Mask of oscillator(s) to disable.
// */
//void clockDisableOsc(enum clockSource_e src)
//{
//    uint8_t mask = 1 << src;    // mask for control and status registers corresponding to new clock source
//    OSC.CTRL &= ~mask;
//}
//
///*! Configure external XTAL oscillator
// *  \param freq Enum defining XOSC frequency range.
// *  \param sel Enum defining XOSC selection and startup time.
// */
//void clockConfigXosc(enum clockXoscFreq_e freq, enum clockXoscSelection_e sel)
//{
//    OSC.XOSCCTRL = freq | sel;
//}
//
///*! Set XMega clock source.\n
// *  \param src New clock source.
// *  Function blocks until desired oscillator is stable, does not deactivate old clock source.
// */
//void clockSetSource(enum clockSource_e src)
//{
//    clockDisbaleDfllCalibration();
//    uint8_t mask = 1 << src;    // mask for control and status registers corresponding to new clock source
//
//    /* Loop until OSC.STATUS bit is set indicating new oscillator is stable */
//    while ((OSC.STATUS & mask) == 0) {
//        ;
//    }
//
//    /* new oscillator is running & stable, set it as the new clock source */
//    CCP = 0xD8;             // write signature to configuration change protection register
//                            // interrupts ignored for 4 instruction cycles
//    CLK.CTRL = src;         // set clock control register
//}
//
///*! Configure the PLL with desired settings.\n
// *  \param src The input source of the PLL.
// *  \param fact The multiplication factor of the PLL. The multiplication factor can be in the range
// *  from 1x to 31x. The output frequency from the PLL should not exceed 200 MHz.
// *  The PLL must have a minimum output frequency of 10 MHz.
// */
//void clockConfigPll(enum pllClockSource_e src, uint8_t fact)
//{
//    fact &= 0x1F;               // mask upper 3 bits of PLL factor, maximum PLL factor is 31.
//    OSC.PLLCTRL = src | fact;   // set PLL configuration register.
//}
//
///*! Configure Prescaler A division factor.\n
// *  \param ps Prescaler A division factor.
// */
//void clockSetPSA(enum prescaleAFactor_e ps)
//{
//    uint8_t mask = CLK.PSCTRL;
//    mask &= 0x03;           // clear all prescale A control bits
//    mask |= ps;             // set desired prescale A control bits
//    CCP = 0xD8;             // write signature to configuration change protection register
//                            // interrupts ignored for 4 instruction cycles
//    CLK.PSCTRL = mask;      // set prescale control register
//}
//
///*! Enable DFLL RC oscillator calibration on current oscillator.\n
// *  \param src Calibration source (internal 32.768kHz or external 32.768kHz).
// *  Oscillator calibration is disabled automatically when clock source is changed.
// */
//int8_t clockEnableDfllCalibration(enum clockDfllCalSource_e src)
//{
//    // enable required oscillator if necessary
//    if (src == DFLL_CAL_SOURCE_INT) {
//        OSC.CTRL |= OSC_RC32KEN_bm;             // enable internal oscillator for DFLL
//        // set both 32MHZ and 2MHZ to use the internal RC32K oscillator
//        OSC.DFLLCTRL = OSC_RC32MCREF_RC32K_gc;
//    } else {
//        OSC.XOSCCTRL |= OSC_XOSCSEL_32KHz_gc;   // configure external oscillator
//        OSC.CTRL |= OSC_XOSCEN_bm;              // enable external oscillator for DFLL
//        // set both 32MHZ and 2MHZ DFLL to use external 32K oscillator
//        #if defined(OSC_RC32MCREF_bm)
//        OSC.DFLLCTRL = OSC_RC2MCREF_bm | OSC_RC32MCREF_bm;
//        #elif defined(OSC_RC32MCREF_gm)
//        OSC.DFLLCTRL = OSC_RC2MCREF_bm | OSC_RC32MCREF_XOSC32K_gc;
//        #else
//        #error Unknown DFLL Control register (DFLLCTRL) bitmasks.
//        #endif
//    }
//    // wait for oscillator ready flag
//    uint8_t i = 0;
//    while ((src == DFLL_CAL_SOURCE_INT && bit_is_clear(OSC.STATUS, OSC_RC32KRDY_bp)) ||
//            (src != DFLL_CAL_SOURCE_INT && bit_is_clear(OSC.STATUS, OSC_XOSCRDY_bp))) {
//        if (i >= OSCILLATOR_STARTUP_TIMEOUT) {
//            // oscillator is not ready after timeout period
//            return -1;
//        }
//        i++;
//        _delay_ms(10.0);
//    }
//    // select which DFLL to activate
//    switch (CLK.CTRL & CLK_SCLKSEL_gm) {
//        case CLK_SCLKSEL_RC2M_gc:
//            // clock source RC 2MHz
//            DFLLRC2M.CTRL = DFLL_ENABLE_bm;
//            break;
//        case CLK_SCLKSEL_RC32M_gc:
//            // clock source RC 32MHz
//            DFLLRC32M.CTRL = DFLL_ENABLE_bm;
//            break;
//        case CLK_SCLKSEL_PLL_gc:
//            // clock source PLL
//            switch (OSC.PLLCTRL & OSC_PLLSRC_gm) {
//                case OSC_PLLSRC_RC2M_gc:
//                    // PLL source RC 2MHz
//                    DFLLRC2M.CTRL = DFLL_ENABLE_bm;
//                    break;
//                case OSC_PLLSRC_RC32M_gc:
//                    // PLL source RC 32MHz
//                    DFLLRC32M.CTRL = DFLL_ENABLE_bm;
//                    break;
//                default:
//                    // unsupported PLL source
//                    return -1;
//            }
//            break;
//        default:
//            // unsupported clock source for DFLL calibration
//            return -1;
//    }
//    return 0;
//}
//
///*! Disable DFLL RC oscillator calibration on any RC oscillator.\n
// *  Function is automatically called when oscillator is changed
// */
//void clockDisbaleDfllCalibration(void)
//{
//    DFLLRC2M.CTRL = 0;
//    DFLLRC32M.CTRL = 0;
//}
