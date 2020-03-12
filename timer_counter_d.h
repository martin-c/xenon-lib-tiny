/*! \file
 *  timer.h
 *  xenon-lib-tiny
 *  Copyright (c) 2020 Martin Clemons
 *
 *  Configuration and utility functions for Timer/Counter D peripheral.
 */
#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <avr/io.h>


/*** Public Variables --------------------------------------------------------*/
/*! \publicsection */

/****** General Timer/Counter D configuration options ******/
/*! Timer/Counter D clock source selection
 */
enum timerCounterDClockSource_e {
    TCD_CLOCK_SOURCE_OSC20M     = TCD_CLKSEL_20MHZ_gc,      ///< Clock source is 16MHz/20MHz RC oscillator
    TCD_CLOCK_SOURCE_EXTCLK     = TCD_CLKSEL_EXTCLK_gc,     ///< External clock from EXTCLK pin
    TCD_CLOCK_SOURCE_SYSCLK     = TCD_CLKSEL_SYSCLK_gc,     ///< System clock
};

/*! Timer/Counter D synchronization clock pre-scale factor. The output of the
 * synchronization prescaler block drives the clock domain synchronization, as
 * well as the inputs to the counter prescaler and delay prescaler.
 */
enum timerCounterDSyncPrescale_e {
    TCD_SYNC_PRESCALE_DIV1      = TCD_SYNCPRES_DIV1_gc,     ///< Synchronization pre-scale factor 1
    TCD_SYNC_PRESCALE_DIV2      = TCD_SYNCPRES_DIV2_gc,     ///< Synchronization pre-scale factor 2
    TCD_SYNC_PRESCALE_DIV4      = TCD_SYNCPRES_DIV4_gc,     ///< Synchronization pre-scale factor 4
    TCD_SYNC_PRESCALE_DIV8      = TCD_SYNCPRES_DIV8_gc,     ///< Synchronization pre-scale factor 8
};

/*! Timer/Counter D counter pre-scale factor. The output of the
 * counter prescaler block drives the timer/counter counter.
 * The input to this block is the output of the synchronization prescaler.
 */
enum timerCounterDCounterPrescale_e {
    TCD_COUNTER_PRESCALE_DIV1   = TCD_CNTPRES_DIV1_gc,      ///< Counter pre-scale factor 1
    TCD_COUNTER_PRESCALE_DIV4   = TCD_CNTPRES_DIV4_gc,      ///< Counter pre-scale factor 4
    TCD_COUNTER_PRESCALE_DIV32  = TCD_CNTPRES_DIV32_gc,     ///< Counter pre-scale factor 32
};

/*! Timer/Counter D waveform mode.
 * The TCD provides four different Waveform Generation modes. The Waveform Generation
 * modes determine how the counter is counting during a TCD cycle, and when the
 * compare values match.
 */
enum timerCounterDWaveformMode_e {
    TCD_WAFEFORM_ONE_RAMP       = TCD_WGMODE_ONERAMP_gc,    ///< One ramp waveform mode
    TCD_WAVEFORM_TWO_RAMP       = TCD_WGMODE_TWORAMP_gc,    ///< Two ramp waveform mode
    TCD_WAVEFORM_FOUR_RAMP      = TCD_WGMODE_FOURRAMP_gc,   ///< Four ramp waveform mode
    TCD_WAVEFORM_DUAL_SLOPE     = TCD_WGMODE_DS_gc,         ///< Dual-slope waveform mode
};

/****** Timer/Counter D Event input and action configuration options ******/
/*! Timer/Counter D event input options.
 * Each event input may have a digital noise filter enabled or asynchronous event detection
 * enabled, but not both.
 */
enum timerCounterDEventInputOptions_e {
    TCD_EVENT_INPUT_NORMAL      = TCD_CFG_NEITHER_gc,       ///< No noise filter, no async event
    TCD_EVENT_INPUT_FILTER      = TCD_CFG_FILTER_gc,        ///< Enable digital filter on event input (22.3.2.4.2)
    TCD_EVENT_INPUT_ASYNC       = TCD_CFG_ASYNC_gc,         ///< Enable async event detection (22.3.2.4.3)
};

/*! Timer/Counter D event type options.
 * The type of event or event polarity which triggers the input.
 */
enum timerCounterDEventType_e {
    TCD_EVENT_TYPE_FALLING_LOW  = TCD_EDGE_FALL_LOW_gc,     ///< Falling edge or low level of event
    TCD_EVENT_TYPE_RISING_HIGH  = TCD_EDGE_RISE_HIGH_gc,    ///< Rising edge or high level of event
};

/*! Timer/Counter D event actions.
 * The event may trigger a fault only or trigger a fault and capture.
 */
enum timerCounterDEventAction_e {
    TCD_EVENT_ACTION_FAULT      = TCD_ACTION_FAULT_gc,      ///< Event triggers a fault
    TCD_EVENT_ACTION_CAPTURE    = TCD_ACTION_CAPTURE_gc,    ///< Event triggers a fault and capture
};

/*! Timer/Counter D event action type.
 */
enum timerCounterDEventActionType_e {
    TCD_EVENT_ACTION_TYPE_NONE          = TCD_INPUTMODE_NONE_gc,         ///< Input has no action
    TCD_EVENT_ACTION_TYPE_JMPWAIT       = TCD_INPUTMODE_JMPWAIT_gc,      ///< Stop output, jump to opposite compare cycle, and wait
    TCD_EVENT_ACTION_TYPE_EXECWAIT      = TCD_INPUTMODE_EXECWAIT_gc,     ///< Stop output, execute opposite compare cycle, and wait
    TCD_EVENT_ACTION_TYPE_EXECFAULT     = TCD_INPUTMODE_EXECFAULT_gc,    ///< Stop output, execute opposite compare cycle while fault active
    TCD_EVENT_ACTION_TYPE_FREQ          = TCD_INPUTMODE_FREQ_gc,         ///< Stop all outputs, maintain frequency
    TCD_EVENT_ACTION_TYPE_EXECDT        = TCD_INPUTMODE_EXECDT_gc,       ///< Stop all outputs, execute dead time while fault active
    TCD_EVENT_ACTION_TYPE_WAIT          = TCD_INPUTMODE_WAIT_gc,         ///< Stop all outputs, jump to next compare cycle, and wait
    TCD_EVENT_ACTION_TYPE_WAITSW        = TCD_INPUTMODE_WAITSW_gc,       ///< Stop all outputs, wait for software action
    TCD_EVENT_ACTION_TYPE_EDGETRIG      = TCD_INPUTMODE_EDGETRIG_gc,     ///< Stop output on edge, jump to next compare cycle
    TCD_EVENT_ACTION_TYPE_EDGETRIGFREQ  = TCD_INPUTMODE_EDGETRIGFREQ_gc, ///< Stop output on edge, maintain frequency
    TCD_EVENT_ACTION_TYPE_LVLTRIGFREQ   = TCD_INPUTMODE_LVLTRIGFREQ_gc   ///< Stop output at level, maintain frequency
};


/*! Timer/Counter D Interrupts.
 */
struct timerCounterDInterruptConfig_s {
    bool ovfInterruptEnable;            ///< Timer overflow Interrupt Enable
    bool trigAInterruptEnable;          ///< Trigger A Interrupt Enable
    bool trigBInterruptEnable;          ///< Trigger B Interrupt Enable
};

/*! Configure the EventA and EventB inputs to Timer/Counter D.
 */
struct timerCounterDEventConfig_s {
    struct {
        bool inputEnabled;
        enum timerCounterDEventInputOptions_e inputOptions;
        enum timerCounterDEventType_e eventType;
        enum timerCounterDEventAction_e eventAction;
        enum timerCounterDEventActionType_e eventActionType;
    } eventA, eventB;
};

/*! Timer/Counter D compare register values.
 * Since this is a 12-bit timer the maximum value is 2^12 - 1 = 4095.
 */
struct timerCounterDCompareRegister_s {
    uint16_t cmpASet;       ///< Compare A Set (assert)
    uint16_t cmpAClr;       ///< Compare A clear (de-assert)
    uint16_t cmpBSet;       ///< Compare B Set (assert)
    uint16_t cmpBClr;       ///< Compare B Set (de-assert)
};

/*! Timer/Counter D double buffer synchronization type.
 * Values in double-buffered registers can either be synced immediately or
 * after timer cycle.
 */
enum timerCounterDSyncType_e {
    TCD_SYNC_IMMEDIATE  = TCD_SYNC_bm,      ///< synchronize immediately
    TCD_SYNC_CYCLE_END  = TCD_SYNCEOC_bm    ///< synchronize at the end of timer cycle
};

/*! Configure basic Timer/Counter D settings.
 */
struct timerCounterDConfig_s {
    enum timerCounterDClockSource_e clockSource;
    enum timerCounterDSyncPrescale_e syncPrescale;
    enum timerCounterDCounterPrescale_e counterPrescale;
    enum timerCounterDWaveformMode_e waveformMode;
};



/*** Public Functions --------------------------------------------------------*/
/*! \publicsection */

void timerCounterDConfigEvents(struct timerCounterDEventConfig_s *config);
void timerCounterDConfig(struct timerCounterDConfig_s *config);
void timerCounterDEnable(void);
void timerCounterDSetCompareRegisters(struct timerCounterDCompareRegister_s *cmp);
void timerCounterDSyncBuffers(enum timerCounterDSyncType_e type);
uint16_t timerCounterDGetCaptureA(void);
uint16_t timerCounterDGetCaptureB(void);
