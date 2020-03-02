/*! \file
 *  timer.c
 *  60345-100-digi-dac-firmware
 *  Copyright (c) 2020 Martin Clemons
 */
#include "timer_counter_d.h"

/*
 22.3.2.1 Register Synchronization Categories
 Most of the I/O registers need to be synchronized to the asynchronous TCD core clock domain.
 This is done in different ways for different register categories:
 Table 22-3. Categorization of Registers
|-----------------------------------------------------------------------------------------------|
| Enable and Command | Double-Buffered  | Static Registers  | Read-Only     | Normal I/O        |
| Registers          | Registers        |                   | Registers     | Registers         |
| -------------------|------------------|-------------------|---------------|-------------------|
| CTRLA (ENABLE bit) | TCDn.DLYCTRL     | TCDn.CTRLA (other)| TCDn.STATUS   | TCDn.INTCTRL      |
| CTRLE              | TCDn.DLYVAL      | TCDn.CTRLB        | TCDn.CAPTUREA | TCDn.INTFLAGS     |
|                    | TCDn.DITCTRL     | TCDn.CTRLC        | TCDn.CAPTUREB |                   |
|                    | TCDn.DITVAL      | TCDn.CTRLD        |               |                   |
|                    | TCDn.DBGCTRL     | TCDn.EVCTRLA      |               |                   |
|                    | TCDn.CMPASET     | TCDn.EVCTRLB      |               |                   |
|                    | TCDn.CMPACLR     | TCDn.INPUTCTRLA   |               |                   |
|                    | TCDn.CMPBSET     | TCDn.INPUTCTRLB   |               |                   |
|                    | TCDn.CMPBCLR     | TCDn.FAULTCTRL    |               |                   |
|                    |                  |                   |               |                   |
|--------------------|------------------|-------------------|---------------|-------------------|
*/

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

/*! Configure Timer/Counter D event inputs and resulting actions.
 * @param config Pointer to `timerCounterDEventConfig_s` with desired configuration.
 * Note: This function must be called with timer disabled (not running) because
 * static registers are modified (see table 22-3).
 */
void timerCounterDConfigEvents(struct timerCounterDEventConfig_s *config)
{
    TCD0.INPUTCTRLA = config->eventA.eventActionType;
    TCD0.INPUTCTRLB = config->eventB.eventActionType;
    TCD0.EVCTRLA =
            config->eventA.inputOptions |
            config->eventA.eventType |
            config->eventA.eventAction |
            (config->eventA.inputEnabled ? TCD_TRIGEI_bm : 0);
    TCD0.EVCTRLB =
            config->eventB.inputOptions |
            config->eventB.eventType |
            config->eventB.eventAction |
            (config->eventB.inputEnabled ? TCD_TRIGEI_bm : 0);
}

/*! Configure Timer/Counter D.
 * @param config Pointer to `timerCounterDConfig_s` with desired configuration.
 * Note: This function must be called with timer disabled (not running) because
 * static registers are modified (see table 22-3).
 */
void timerCounterDConfig(struct timerCounterDConfig_s *config)
{
    TCD0.CTRLB = config->waveformMode;
    TCD0.CTRLA = config->clockSource | config->counterPrescale |
            config->syncPrescale;
}

/*! Enable Timer/Counter D.
 * Note: This function blocks until the appropriate synchronization bit (`TCD_ENRDY`)
 * is set.
 */
void timerCounterDEnable(void)
{
    loop_until_bit_is_set(TCD0.STATUS, TCD_ENRDY_bp);
    TCD0.CTRLA |= TCD_ENABLE_bm;
}

/*! Set Timer/Counter D compare registers.
 * Function may be called while timer is running. Buffers must subsequently be synced.
 * @param cmp Pointer to `timerCounterDCompareRegister_s` with desired register values.
 */
void timerCounterDSetCompareRegisters(struct timerCounterDCompareRegister_s *cmp)
{
    TCD0.CMPASET = cmp->cmpASet;
    TCD0.CMPACLR = cmp->cmpAClr;
    TCD0.CMPBSET = cmp->cmpBSet;
    TCD0.CMPBCLR = cmp->cmpBClr;
}

/*! Synchronize the double-buffers in the TCD configuration registers.
 * There are two synchronization options: Immediate and at timer cycle end.
 * @param type A `timerCounterDSyncType_e` specifying sync type.
 * Note: This function blocks until `CMDRDY` bit is set in status register.
 */
void timerCounterDSyncBuffers(enum timerCounterDSyncType_e type)
{
    loop_until_bit_is_set(TCD0.STATUS, TCD_CMDRDY_bp);
    TCD0.CTRLE = type;
}

/*! Get the Timer/Counter D capture register A value.
 * Note: The captured value must be synchronized to the CPU clock domain before
 * the returned value is valid.
 * @return The TCD `CAPTUREA` value.
 */
uint16_t timerCounterDGetCaptureA(void)
{
    return TCD0.CAPTUREA;
}

/*! Get the Timer/Counter D capture register B value.
 * Note: The captured value must be synchronized to the CPU clock domain before
 * the returned value is valid.
 * @return The TCD `CAPTURED` value.
 */
uint16_t timerCounterDGetCaptureB(void)
{
    return TCD0.CAPTUREB;
}
