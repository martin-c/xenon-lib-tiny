/*! \file
 *  timer_counter_a.h
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

/*! Timer/Counter A Pre-scale factor. TCA prescaler always runs from the peripheral clock.
 */
enum timerCounterAPrescale_e {
    TCA_PRESCALE_DIV1               = TCA_SINGLE_CLKSEL_DIV1_gc,    ///< Prescale factor 1
    TCA_PRESCALE_DIV2               = TCA_SINGLE_CLKSEL_DIV1_gc,    ///< Prescale factor 2
    TCA_PRESCALE_DIV4               = TCA_SINGLE_CLKSEL_DIV1_gc,    ///< Prescale factor 4
    TCA_PRESCALE_DIV8               = TCA_SINGLE_CLKSEL_DIV1_gc,    ///< Prescale factor 8
    TCA_PRESCALE_DIV16              = TCA_SINGLE_CLKSEL_DIV1_gc,    ///< Prescale factor 16
    TCA_PRESCALE_DIV64              = TCA_SINGLE_CLKSEL_DIV1_gc,    ///< Prescale factor 64
    TCA_PRESCALE_DIV256             = TCA_SINGLE_CLKSEL_DIV1_gc,    ///< Prescale factor 256
    TCA_PRESCALE_DIV1024            = TCA_SINGLE_CLKSEL_DIV1_gc,    ///< Prescale factor 1024
};

/*! Timer/Counter A Waveform Mode (normal, non-split)
 */
enum timerCounterAWaveformMode_e {
    TCA_WAVEFORM_NORMAL             = TCA_SINGLE_WGMODE_NORMAL_gc,      ///< Normal mode
    TCA_WAVEFORM_FREQUENCY          = TCA_SINGLE_WGMODE_FRQ_gc,         ///< Frequency mode
    TCA_WAVEFORM_SINGLE_SLOPE       = TCA_SINGLE_WGMODE_SINGLESLOPE_gc, ///< Single-slope PWM
    TCA_WAVEFORM_DUAL_SLOPE_TOP     = TCA_SINGLE_WGMODE_DSTOP_gc,       ///< Dual-slope PWM, OVF TOP
    TCA_WAVEFORM_DUAL_SLOPE_BOTH    = TCA_SINGLE_WGMODE_DSBOTH_gc,      ///< Dual-slope PWM, OVF TOP and BOTTOM
    TCA_WAVEFORM_DUAL_SLOPE_BOTTOM  = TCA_SINGLE_WGMODE_DSBOTTOM_gc,    ///< Dual-slope PWM, OVF BOTTOM
};

/*! Timer/Counter A Event Action options. Event input is only supported in non-split mode.
 */

enum timerCounterAEventAction_e {
    TCA_EVENT_ACTION_POSITIVE_EDGE  = TCA_SINGLE_EVACT_POSEDGE_gc,  ///< Count on positive edge event
    TCA_EVENT_ACTION_ANY_EDGE       = TCA_SINGLE_EVACT_ANYEDGE_gc,  ///< Count on any edge event
    TCA_EVENT_ACTION_HIGH_LEVEL     = TCA_SINGLE_EVACT_HIGHLVL_gc,  ///< Count on prescaled clock while event line is high
    TCA_EVENT_ACTION_DIRECTION      = TCA_SINGLE_EVACT_UPDOWN_gc,   ///< Count on prescaled clock. The Event controls the count direction. Up- counting when the event line is low, down-counting when the event line is high.
};

struct timerCounterAConfig_s {
    enum timerCounterAPrescale_e prescale;
    enum timerCounterAWaveformMode_e waveformMode;
};



/*** Public Functions --------------------------------------------------------*/
/*! \publicsection */

void timerCounterASetPeriod(uint16_t period);
void timerCounterASetCompare(const uint16_t compare[3]);
uint16_t timerCounterAGetCounter(void);
void timerCounterAConfig(const struct timerCounterAConfig_s *config);
void timerCounterAConfigEventAction(const enum timerCounterAEventAction_e action);
void timerCounterAEnableEventAction(void);
void timerCounterADisableEventAction(void);
void timerCounterAEnable(void);
