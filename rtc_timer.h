/*! \file
 *  rtc_timer.h
 *  xenon-lib-tiny
 *  Copyright (c) 2019 Martin Clemons
 *
 *  A Soft-timer driven by the RTC.
 *
 */
#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <avr/io.h>


/*** Public Variables --------------------------------------------------------*/
/*! \publicsection */

/*  implementation of a timer */
struct rtcTimer_s {
    uint16_t expireCount;
};

/*! RTC Timer type */
typedef struct rtcTimer_s rtcTimer_t;

/*** Public Functions --------------------------------------------------------*/
/*! \publicsection */

void rtcTimerInit(rtcTimer_t *t, uint16_t period);
void rtcTimerAddPeriod(rtcTimer_t *t, uint16_t period);
uint8_t rtcTimerActive(rtcTimer_t *t);
