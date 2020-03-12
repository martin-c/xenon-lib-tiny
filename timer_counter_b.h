/*! \file
 *  timer_counter_b.h
 *  xenon-lib-tiny
 *  Copyright (c) 2020 Martin Clemons
 *
 *  Enter file description here.
 */
#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <avr/io.h>


/*** Public Variables --------------------------------------------------------*/
/*! \publicsection */

/*! Timer/Counter B clock source selection.
 * Note: There is significant errata related to clock source for ATTiny16xx parts!
 * http://ww1.microchip.com/downloads/en/DeviceDoc/ATtiny1616_3216-Errata-and-Clarification-DS40002120B.pdf
 */
enum timerCounterBClockSource_e {
    TCB_CLOCK_SOURCE_PER        = TCB_CLKSEL_CLKDIV1_gc,    ///< Peripheral clock
    TCB_CLOCK_SOURCE_PER_DIV2   = TCB_CLKSEL_CLKDIV2_gc,    ///< Peripheral clock divided by 2
    TCB_CLOCK_SOURCE_CLK_TCA    = TCB_CLKSEL_CLKTCA_gc,     ///< TCA clock source
};

/*! Timer/Counter B modes.
 */
enum timerCounterBMode_e {
    TCB_MODE_PERIODIC_INTERRUPT     = TCB_CNTMODE_INT_gc,       ///< Periodic interrupt mode
    TCB_MODE_TIMEOUT_CHECK          = TCB_CNTMODE_TIMEOUT_gc,   ///< Time-out check mode
    TCB_MODE_CAPTURE                = TCB_CNTMODE_CAPT_gc,      ///< Capture on event mode
    TCB_MODE_FREQUENCY              = TCB_CNTMODE_FRQ_gc,       ///< Frequency measurement mode
    TCB_MODE_PULSE_WIDTH            = TCB_CNTMODE_PW_gc,        ///< Pulse-width measurement mode
    TCB_MODE_FREQUENCY_PULSE_WIDTH  = TCB_CNTMODE_FRQPW_gc,     ///< Frequency and pulse width measurement mode
    TCB_MODE_SINGLE_SHOT            = TCB_CNTMODE_SINGLE_gc,    ///< Single shot mode
    TCB_MODE_PWM                    = TCB_CNTMODE_PWM8_gc,      ///< 8-bit PWM mode
};

/*! Timer/Counter B basic configuration.
 */
struct timerCounterBConfig_s {
    enum timerCounterBClockSource_e clockSource;    ///< TCB clock source selection
    enum timerCounterBMode_e mode;                  ///< TCB mode selection
};

/*! Timer/Counter B event control configuration.
 */
struct timerCounterBEventConfig_s {
    bool inputNoiseFilterEnable;            ///< Enable input capture noise filter
    bool inputCaptureEnable;                ///< Enable input capture
    uint8_t edgeBit;                        ///< `EVCTRL` `EDGE` bit setting (0 or 1)
};



/*** Public Functions --------------------------------------------------------*/
/*! \publicsection */
uint16_t timerCounterBGetCounter(TCB_t *tcb);
uint16_t timerCounterBGetCapture(TCB_t *tcb);
void timerCounterBSetCompare(TCB_t *tcb, uint16_t cmp);
void timerCounterBConfigEvent(TCB_t *tcb, struct timerCounterBEventConfig_s *config);
void timerCounterBConfigInterrupts(TCB_t *tcb, bool captureInterruptEnable);
void timerCounterBConfig(TCB_t *tcb, struct timerCounterBConfig_s *config);
void timerCounterBEnable(TCB_t *tcb);
void timerCounterBDisable(TCB_t *tcb);
