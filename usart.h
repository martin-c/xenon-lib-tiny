/*! \file
 *  usart.h
 *  xenon-lib-tiny
 *  Copyright (c) 2020 Martin Clemons
 *
 *  General configuration and utility functions for the USART peripheral.
 */
#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <avr/io.h>


/*** Public Variables --------------------------------------------------------*/
/*! \publicsection */

/*! USART transmitter stop bit mode. Note: The USART receiver ignores this setting.
 */
enum usartStopBitMode_e {
    USART_SBMODE_1_BIT      = 0x00,                     ///< Transmit one stop bit
    USART_SBMODE_2_BIT      = USART_SBMODE_bm,          ///< Transmit two stop bits
};

/*! USART parity mode.
 */
enum usartParityMode_e {
    USART_PARITY_DISABLED   = USART_PMODE_DISABLED_gc,  ///< No parity bit
    USART_PARITY_EVEN       = USART_PMODE_EVEN_gc,      ///< Even parity bit
    USART_PARITY_ODD        = USART_PMODE_ODD_gc,       ///< Odd parity bit
};

/*! The baud rate mode for the receiver and possibly transmitter.
 */
enum usartBaudMode_e {
    USART_BAUD_NORMAL       = USART_RXMODE_NORMAL_gc,   ///< Normal transmission speed, clock divisor 16
    USART_BAUD_DOUBLE       = USART_RXMODE_CLK2X_gc,    ///< Double transmission speed, clock divisor 8
    USART_BAUD_AUTO         = USART_RXMODE_GENAUTO_gc,  ///< Generic auto-baud mode
};

/*! USART Interrupts.
 */
struct usartInterruptConfig_s {
    bool rxcInterruptEnable;            ///< Receive Complete Interrupt Enable
    bool txcInterruptEnable;            ///< Transmit Complete Interrupt Enable
    bool dreInterruptEnable;            ///< Data Register Empty Interrupt Enable
    bool rxStartInterruptEnable;        ///< Receiver Start Frame Interrupt Enable
    bool autoBaudErrorInterruptEnable;  ///< Auto-baud Error Interrupt Enable
};

/*! USART pin configuration.
 */
struct usartPinConfig_s {
    bool loopBackEnable;                ///< Set to true to enable internal loop-back connection
    bool openDrainEnable;               ///< Set to true to set TX pin to open-drain
};

/*! General configuration for USART in asynchronous serial mode.
 */
struct usartAsyncSerialConfig_s {
    bool txEnable;                      ///< Transmitter enable
    bool rxEnable;                      ///< Receiver enable
    bool startFrameDetectionEnable;     ///< Enable the USART Start Frame Detection mode
    enum usartStopBitMode_e sbMode;     ///< Transmitter stop bit mode
    enum usartParityMode_e parityMode;  ///< The desired USART parity mode
    enum usartBaudMode_e baudMode;      ///< The desired baud rate mode
    uint16_t baudPrescale;              ///< 16-bit baud pre-scale register
};



/*** Public Functions --------------------------------------------------------*/
/*! \publicsection */

void usartConfigPins(struct usartPinConfig_s *config);
void usartConfigInterrupts(struct usartInterruptConfig_s *config);
void usartConfigAsyncSerial(struct usartAsyncSerialConfig_s *config);
void usartFlush(void);
int usartPutChar(char c, FILE *file);
int usartGetChar(FILE *file);
void usartSendFromBuffer(const uint8_t *buffer, uint8_t length, bool sleep);
void usartReceiveToBuffer(uint8_t *buffer, uint8_t length, bool sleep);
