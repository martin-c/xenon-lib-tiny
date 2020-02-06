/*! \file
 *  rtc.h
 *  xenon-lib-tiny
 *  Copyright (c) 2019 Martin Clemons
 *
 *  General utility functions for configuring the RTC peripheral.
 *  Note: The functions below are **not reentrant**, and should not be called
 *  from multiple threads simultaneously.
 */
#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <avr/io.h>


/*** Public Variables --------------------------------------------------------*/
/*! \publicsection */

/*! RTC prescaler selection */
enum rtcPrescale_e {
    RTC_PRESCALE_DIV1       = RTC_PRESCALER_DIV1_gc,        ///< Prescale factor 1, prescaler off
    RTC_PRESCALE_DIV2       = RTC_PRESCALER_DIV2_gc,        ///< Prescale factor 2
    RTC_PRESCALE_DIV4       = RTC_PRESCALER_DIV4_gc,        ///< Prescale factor 4
    RTC_PRESCALE_DIV8       = RTC_PRESCALER_DIV8_gc,        ///< Prescale factor 8
    RTC_PRESCALE_DIV16      = RTC_PRESCALER_DIV16_gc,       ///< Prescale factor 16
    RTC_PRESCALE_DIV32      = RTC_PRESCALER_DIV32_gc,       ///< Prescale factor 32
    RTC_PRESCALE_DIV64      = RTC_PRESCALER_DIV64_gc,       ///< Prescale factor 64
    RTC_PRESCALE_DIV128     = RTC_PRESCALER_DIV128_gc,      ///< Prescale factor 128
    RTC_PRESCALE_DIV256     = RTC_PRESCALER_DIV256_gc,      ///< Prescale factor 256
    RTC_PRESCALE_DIV512     = RTC_PRESCALER_DIV512_gc,      ///< Prescale factor 512
    RTC_PRESCALE_DIV1024    = RTC_PRESCALER_DIV1024_gc,     ///< Prescale factor 1024
    RTC_PRESCALE_DIV2048    = RTC_PRESCALER_DIV2048_gc,     ///< Prescale factor 2048
    RTC_PRESCALE_DIV4096    = RTC_PRESCALER_DIV4096_gc,     ///< Prescale factor 4096
    RTC_PRESCALE_DIV8192    = RTC_PRESCALER_DIV8192_gc,     ///< Prescale factor 8192
    RTC_PRESCALE_DIV16384   = RTC_PRESCALER_DIV16384_gc,    ///< Prescale factor 16384
    RTC_PRESCALE_DIV32768   = RTC_PRESCALER_DIV32768_gc,    ///< Prescale factor 32768
};

/*! RTC clock source selection
 *  Note: Although `RTC_CLOCK_SOURCE_XOSC32K` and `RTC_CLOCK_SOURCE_TOSC1` are distinct enum values,
 *  these values share the same `CLKSEL` value, `RTC_CLKSEL_TOSC32K_gc` or `0x02`. This enum assigns
 *  distinct values to these configurations to maintain a clearer representation of the actual
 *  hardware configuration.
 */
enum rtcClockSource_e {
    RTC_CLOCK_SOURCE_INT32K     = RTC_CLKSEL_INT32K_gc,     ///< 32.768 kHz from internal ULP oscillator
    RTC_CLOCK_SOURCE_INT1K      = RTC_CLKSEL_INT1K_gc,      ///< 1.024 kHz from internal ULP oscillator
    RTC_CLOCK_SOURCE_XOSC32K    = 0xF1,                     ///< 32.768 kHz from external 32KHz oscillator
    RTC_CLOCK_SOURCE_TOSC1      = 0xF2,                     ///< External clock from TOSC1 pin
    RTC_CLOCK_SOURCE_EXTCLK     = RTC_CLKSEL_EXTCLK_gc,     ///< External clock from EXTCLK pin
};

/*! RTC PIT (Periodic Interrupt Timer) period. This sets the number of RTC clock cycles between
 *  each PIT interrupt.
 */
enum rtcInterruptPeriod_e {
    RTC_PIT_DIV4        = RTC_PERIOD_CYC4_gc,       ///< PIT interrupt raised every 4 clock cycles
    RTC_PIT_DIV8        = RTC_PERIOD_CYC8_gc,       ///< PIT interrupt raised every 8 clock cycles
    RTC_PIT_DIV16       = RTC_PERIOD_CYC16_gc,      ///< PIT interrupt raised every 16 clock cycles
    RTC_PIT_DIV32       = RTC_PERIOD_CYC32_gc,      ///< PIT interrupt raised every 32 clock cycles
    RTC_PIT_DIV64       = RTC_PERIOD_CYC64_gc,      ///< PIT interrupt raised every 64 clock cycles
    RTC_PIT_DIV128      = RTC_PERIOD_CYC128_gc,     ///< PIT interrupt raised every 128 clock cycles
    RTC_PIT_DIV256      = RTC_PERIOD_CYC256_gc,     ///< PIT interrupt raised every 256 clock cycles
    RTC_PIT_DIV512      = RTC_PERIOD_CYC512_gc,     ///< PIT interrupt raised every 512 clock cycles
    RTC_PIT_DIV1024     = RTC_PERIOD_CYC1024_gc,    ///< PIT interrupt raised every 1024 clock cycles
    RTC_PIT_DIV2048     = RTC_PERIOD_CYC2048_gc,    ///< PIT interrupt raised every 2048 clock cycles
    RTC_PIT_DIV4096     = RTC_PERIOD_CYC4096_gc,    ///< PIT interrupt raised every 4096 clock cycles
    RTC_PIT_DIV8192     = RTC_PERIOD_CYC8192_gc,    ///< PIT interrupt raised every 8192 clock cycles
    RTC_PIT_DIV16384    = RTC_PERIOD_CYC16384_gc,   ///< PIT interrupt raised every 16384 clock cycles
    RTC_PIT_DIV32768    = RTC_PERIOD_CYC32768_gc,   ///< PIT interrupt raised every 32768 clock cycles
};

struct rtcConfig_s {
    enum rtcClockSource_e src;
    enum rtcPrescale_e ps;
    uint16_t period;
    uint16_t compare;
    bool rtcRunStdby;
    bool compareInterruptEnable;
    bool overflowInterruptEnable;
};



/*** Public Functions --------------------------------------------------------*/
/*! \publicsection */

void rtcSetClockSource(enum rtcClockSource_e src);
void rtcSetClockPrescaler(enum rtcPrescale_e ps);
void rtcSetPeriod(uint16_t period);
void rtcSetCompare(uint16_t compare);
uint16_t rtcGetCount(void);
void rtcSetCount(uint16_t count);
void rtcInitEnable(struct rtcConfig_s *config);
void rtcEnable(bool runStdby);
void rtcDisable(void);
