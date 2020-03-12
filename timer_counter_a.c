/*! \file
 *  timer_counter_a.c
 *  xenon-lib-tiny
 *  Copyright (c) 2020 Martin Clemons
 */
#include "timer_counter_a.h"


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

/*! Directly set Timer/Counter A Period register. Bypass buffer registers and
 * set `PER` registers directly. Not recommended when timer is running.
 * @param period The value to set `PER` registers to.
 */
void timerCounterASetPeriod(uint16_t period)
{
    TCA0.SINGLE.PER = period;
}

/*! Directly set Timer/Counter A Compare registers. Bypass buffer registers and
 * set `CMP0`, `CMP1`, and `CMP2` registers directly. Not recommended when timer is running.
 * @param compare Array of 3 uint16_t values to set CMP registers to. `CMP0` is
 * set to the first element in the array (compare[0]), `CMP1` is set to the second
 * element in the array, and `CMP2` is set to the third element in the array.
 */
void timerCounterASetCompare(const uint16_t compare[3])
{
    TCA0.SINGLE.CMP0 = compare[0];
    TCA0.SINGLE.CMP1 = compare[1];
    TCA0.SINGLE.CMP2 = compare[2];
}

/*! Get the counter value for Timer/Counter A.
 * @return The 16-bit counter value.
 */
uint16_t timerCounterAGetCounter(void)
{
    return TCA0.SINGLE.CNT;
}

/*! Configure Timer/Counter A. This function configures only the minimum common
 * parameters between different applications for TCA. More specific functions
 * can be used to configure more specialized features.
 * @param config Pointer to `timerCounterAConfig_s` with basic configuration
 * values.
 */
void timerCounterAConfig(const struct timerCounterAConfig_s *config)
{
    // set prescale/clock select in CTRLA
    TCA0.SINGLE.CTRLA = config->prescale;
    // set waveform mode in CTRLB
    TCA0.SINGLE.CTRLB = config->waveformMode;
}

/*! Configure Timer/Counter A event action. The event action determines how TCA
 * responds to an incoming event.
 * @param action `timerCounterAEventAction_e` with the desired action.
 */
void timerCounterAConfigEventAction(const enum timerCounterAEventAction_e action)
{
    TCA0.SINGLE.EVCTRL = action;
}

/*! Enable Timer/Counter A event action. This function sets the `CNTEI` bit in
 * the `EVCTRL` register.
 */
void timerCounterAEnableEventAction(void)
{
    TCA0.SINGLE.EVCTRL |= TCA_SINGLE_CNTEI_bm;
}

/*! Disable Timer/Counter A event action. This function clears the `CNTEI` bit in
 * the `EVCTRL` register.
 */
void timerCounterADisableEventAction(void)
{
    TCA0.SINGLE.EVCTRL &= ~TCA_SINGLE_CNTEI_bm;
}

/*! Enable Timer/Counter A. This function sets the `ENABLE` bit in
 * the `CTRLA` register.
 */
void timerCounterAEnable(void)
{
    TCA0.SINGLE.CTRLA |= TCA_SINGLE_ENABLE_bm;
}
