/*! \file
 *  clock.h
 *  xenon-lib-tiny
 *  Copyright (c) 2019 Martin Clemons
 *
 *  General utility functions for configuring the system clock.
 */
#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <avr/io.h>



/*** Public Variables --------------------------------------------------------*/
/*! \publicsection */

/*! Enum defining system clock prescaler.
 */
enum sysClockPrescaler_e {
    SYS_CLOCK_PRESCALE_DIV1     = 0x0F,                 ///< Prescale factor 1, prescaler is off
    SYS_CLOCK_PRESCALE_DIV2     = CLKCTRL_PDIV_2X_gc,   ///< Prescale factor 2
    SYS_CLOCK_PRESCALE_DIV4     = CLKCTRL_PDIV_4X_gc,   ///< Prescale factor 4
    SYS_CLOCK_PRESCALE_DIV8     = CLKCTRL_PDIV_8X_gc,   ///< Prescale factor 8
    SYS_CLOCK_PRESCALE_DIV16    = CLKCTRL_PDIV_16X_gc,  ///< Prescale factor 16
    SYS_CLOCK_PRESCALE_DIV32    = CLKCTRL_PDIV_32X_gc,  ///< Prescale factor 32
    SYS_CLOCK_PRESCALE_DIV64    = CLKCTRL_PDIV_64X_gc,  ///< Prescale factor 64
    SYS_CLOCK_PRESCALE_DIV6     = CLKCTRL_PDIV_6X_gc,   ///< Prescale factor 6
    SYS_CLOCK_PRESCALE_DIV10    = CLKCTRL_PDIV_10X_gc,  ///< Prescale factor 10
    SYS_CLOCK_PRESCALE_DIV12    = CLKCTRL_PDIV_12X_gc,  ///< Prescale factor 12
    SYS_CLOCK_PRESCALE_DIV24    = CLKCTRL_PDIV_24X_gc,  ///< Prescale factor 24
    SYS_CLOCK_PRESCALE_DIV48    = CLKCTRL_PDIV_48X_gc,  ///< Prescale factor 48
};

/*! Enum defining desired system clock source.
 */
enum sysClockSource_e {
    SYS_CLOCK_INT_OSC   = CLKCTRL_CLKSEL_OSC20M_gc,     ///< Internal 16MHz/20MHz oscillator
    SYS_CLOCK_ULP_32K   = CLKCTRL_CLKSEL_OSCULP32K_gc,  ///< Internal Ultra Low Power 32kHz oscillator
    SYS_CLOCK_EXT_32K   = CLKCTRL_CLKSEL_XOSC32K_gc,    ///< External 32.768 kHz crystal oscillator
    SYS_CLOCK_EXT       = CLKCTRL_CLKSEL_EXTCLK_gc,     ///< External clock
};

/*! Enum defining the external 32kHz oscillator start-up time.
 */
enum xOsc32kStartupTime_e {
    XOSC_32K_STARTUP_TIME_1K    = CLKCTRL_CSUT_1K_gc,   ///< 1k cycles start-up
    XOSC_32K_STARTUP_TIME_16K   = CLKCTRL_CSUT_16K_gc,  ///< 1k cycles start-up
    XOSC_32K_STARTUP_TIME_32K   = CLKCTRL_CSUT_32K_gc,  ///< 1k cycles start-up
    XOSC_32K_STARTUP_TIME_64K   = CLKCTRL_CSUT_64K_gc,  ///< 1k cycles start-up
};

/*! Enum defining the external 32kHz oscillator source type.
 */
enum xOsc32kSourceType_e {
    XOSC_32K_SRC_XTAL       = 0x0,              ///< External 32.768 kHz crystal
    XOSC_32K_SRC_CLK        = CLKCTRL_SEL_bm,   ///< External clock source on TOSC1 pin
};

/*** Public Functions --------------------------------------------------------*/
/*! \publicsection */

void clockSetSysClockPrescaler(enum sysClockPrescaler_e ps);
void clockSetSysClockSource(enum sysClockSource_e source);
void clockConfigXOsc32k(enum xOsc32kStartupTime_e sut,
                        enum xOsc32kSourceType_e src,
                        bool runStdby);
void clockStartXOsc32k(void);
void clockStopXOsc32k(void);
