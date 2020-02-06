/*! \file
 *  rtc_isr.h
 *  xenon-lib-tiny
 *  Copyright (c) 2019 Martin Clemons
 *
 *  The Interrupt Service Routine (ISR) for the RTC overflow interrupt (OVF)
 *  as used by the RTC soft-timer. This file may be included to utilize the
 *  simple interrupt service routine code provided below, or a custom ISR
 *  may be created which implements additional functionality.
 *
 */
#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <avr/io.h>


/*** Public Variables --------------------------------------------------------*/
/*! \publicsection */


/*** Public Functions --------------------------------------------------------*/
/*! \publicsection */

uint16_t rtcGetSoftCounter(void);
