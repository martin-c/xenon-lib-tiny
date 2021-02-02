/*! \file
 *  timer_counter_b.c
 *  xenon-lib-tiny
 *  Copyright (c) 2020 Martin Clemons
 */
#include "timer_counter_b.h"


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

/*! Get Timer/Counter B counter value.
 * @param tcb Pointer to TCB peripheral.
 * @return The timer/counter value.
 */
uint16_t timerCounterBGetCounter(TCB_t *tcb)
{
    return tcb->CNT;
}

/*! Set Timer/Counter B counter value.
 * @param tcb Pointer to TCB peripheral.
 * @cnt The timer/counter value to set.
 */
void timerCounterBSetCounter(TCB_t *tcb, const uint16_t cnt)
{
    tcb->CNT = cnt;
}

/*! Get Timer/Counter B capture/compare value.
 * @param tcb Pointer to TCB peripheral.
 * @return The capture/compare value.
 */
uint16_t timerCounterBGetCapture(TCB_t *tcb)
{
    return tcb->CCMP;
}

/*! Set Timer/Counter B capture/compare value.
 * @param tcb Pointer to the TCB peripheral.
 * @param cmp The value to set capture/compare registers.
 */
void timerCounterBSetCompare(TCB_t *tcb, const uint16_t cmp)
{
    tcb->CCMP = cmp;
}

/*! Configure Timer/Counter B input capture/event capture.
 * @param tcb Pointer to the TCB peripheral.
 * @param config Configuration parameters.
 */
void timerCounterBConfigEvent(TCB_t *tcb, const struct timerCounterBEventConfig_s *config)
{
    tcb->EVCTRL =
            (config->inputNoiseFilterEnable ? TCB_FILTER_bm : 0)    |
            (config->inputCaptureEnable ? TCB_CAPTEI_bm : 0)        |
            (config->edgeBit > 0 ? TCB_EDGE_bm : 0);
}

/*! Enable or disable Timer/Counter B interrupts.
 * @param tcb
 * @param captureInterruptEnable Setting of the capture event interrupt.
 */
void timerCounterBConfigInterrupts(TCB_t *tcb, const bool captureInterruptEnable)
{
    tcb->INTCTRL = captureInterruptEnable ? TCB_CAPT_bm : 0;
}

/*! Configure Timer/Counter B. Performs basic configuration of TCB.
 * @param tcb Pointer to the TCB peripheral.
 * @param config Configuration parameters.
 */
void timerCounterBConfig(TCB_t *tcb, const struct timerCounterBConfig_s *config)
{
    tcb->CTRLB = config->mode;
    tcb->CTRLA = config->clockSource;
}

/*! Enable (start) Timer/Counter B.
 * @param tcb Pointer to the TCB peripheral.
 */
void timerCounterBEnable(TCB_t *tcb)
{
    tcb->CTRLA |= TCB_ENABLE_bm;
}

/*! Disable (stop) Timer/Counter B.
 * @param tcb Pointer to the TCB peripheral.
 */
void timerCounterBDisable(TCB_t *tcb)
{
    tcb->CTRLA &= ~TCB_ENABLE_bm;
}
