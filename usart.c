/*! \file
 *  usart.c
 *  xenon-lib-tiny
 *  Copyright (c) 2020 Martin Clemons
 */
#include <avr/sleep.h>
#include "usart.h"


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

/*! Configure the USART pin modes. This function is used to enable loopback and
 * open-drain USART modes.
 * @param config Pointer to `usartPinConfig_s` with the desired pin configuration.
 * Note: This function does NOT set the USART transmit pin as an output nor
 * does it set the receive pin as an input.
 */
void usartConfigPins(struct usartPinConfig_s *config)
{
    USART0.CTRLA |= config->loopBackEnable ? USART_LBME_bm : 0x00;
    USART0.CTRLB |= config->openDrainEnable ? USART_ODME_bm : 0x00;
}

/*! Configure the USART interrupt mask bits.
 * @param config Pointer to `usartInterruptConfig_s` with the desired interrupts
 * to be enabled.
 * Note: Function does not clear any existing interrupt flags when enabling interrupts.
 */
void usartConfigInterrupts(struct usartInterruptConfig_s *config)
{
    uint8_t rs485_bits = USART0.CTRLA & 0x03;
    USART0.CTRLA =
            (config->rxcInterruptEnable ? USART_RXCIE_bm : 0)           |
            (config->txcInterruptEnable ? USART_TXCIE_bm : 0)           |
            (config->dreInterruptEnable ? USART_DREIE_bm : 0)           |
            (config->rxStartInterruptEnable ? USART_RXSIE_bm : 0)       |
            (config->autoBaudErrorInterruptEnable ? USART_ABEIE_bm : 0) |
            rs485_bits;
}

/*! Configure the USART peripheral in asynchronous serial mode
 * @param config Pointer to `usartAsyncSerialConfig_s` with the desired USART
 * configuration options.
 */
void usartConfigAsyncSerial(struct usartAsyncSerialConfig_s *config)
{
    // clear interrupt status bits
    USART0.STATUS = USART_TXCIF_bm | USART_RXSIF_bm | USART_ISFIF_bm | USART_BDF_bm;
    // clear RS485 mode bits if set
    USART0.CTRLA &= ~0x03;
    // set USART mode, parityMode, stop bits, char size
    USART0.CTRLC = USART_CMODE_ASYNCHRONOUS_gc | config->parityMode | config->sbMode |
                   USART_CHSIZE_8BIT_gc;
    // set baud rate
    USART0.BAUD = config->baudPrescale;
    // enable tx, rx, set baud mode
    USART0.CTRLB =
            (config->rxEnable ? USART_RXEN_bm : 0)                      |
            (config->txEnable ? USART_TXEN_bm : 0)                      |
            (config->startFrameDetectionEnable ? USART_SFDEN_bm : 0 )   |
            config->baudMode;
}

/*! Flush the USART receive buffer and clear several interrupt flags.
 */
void usartFlush(void)
{
    USART0.RXDATAL;
    USART0.RXDATAL;
    USART0.RXDATAL;
    USART0.STATUS = USART_TXCIF_bm | USART_ISFIF_bm;
}

/*! Transmit a single character. Non-buffered, blocking. Function signature is
 * compatible with `fdevopen()` and can thus be used with `fdev_setup_stream()`
 * or `FDEV_SETUP_STREAM()`.
 * @param c Char to transmit.
 * @param file Pointer to FILE (ignored).
 * @return Return 0 if transmit is successful, nonzero otherwise.
 */
int usartPutChar(char c, FILE *file)
{
    // loop until USART tx register is ready for more data
    while (bit_is_clear(USART0.STATUS, USART_DREIF_bp)) { }
    USART0.TXDATAL = c;
    return 0;
}

/*! Receive a single character. Non-buffered, blocking. Function signature is
 * compatible with `fdevopen()` and can thus be used with `fdev_setup_stream()`
 * or `FDEV_SETUP_STREAM()`.
 * @param file Pointer to FILE (ignored).
 * @return Return char if receive is successful,  `_FDEV_ERR` if a framing error
 * is detected.
 */
int usartGetChar(FILE *file)
{
    // loop until USART has received data
    while (bit_is_clear(USART0.STATUS, USART_RXCIF_bp)) { }
    if (bit_is_set(USART0.RXDATAH, USART_FERR_bp)) {
        // return error if a framing error was detected
        return _FDEV_ERR;
    }
    return (int)USART0.RXDATAL;
}

/*! Send up to 255 bytes from `buffer`. This is a blocking send, so the function
 * will block until the last byte is written to the USART transmit buffer.
 * @param buffer Pointer to the buffer from which to send.
 * @param length Number of bytes to send (0 to 255).
 * @param sleep Boolean indicating whether to call `sleep_mode()` while waiting
 * for USART transmit buffer. Note: If `sleep` is `true` a wakeup source must be
 * enabled (such as Data Register Empty Interrupt) which will wake the processor
 * to send the next byte.
 * Function returns as soon as last byte is written to transmit buffer, check
 * `TXCIF` to determine when transmission is complete after function returns.
 */
void usartSendFromBuffer(const uint8_t *buffer, uint8_t length, bool sleep)
{
    uint8_t index = 0;
    while (index < length) {
        while(bit_is_clear(USART0.STATUS, USART_DREIF_bp)) {
            if (sleep) {
                sleep_mode();
            }
        }
        USART0.TXDATAL = buffer[index];
        index++;
    }
    // last byte(s) just written to buffer, clear TXCIF to enable
    // detection of transmission end
    USART0.STATUS |= USART_TXCIF_bm;
}

/*! Receive up to 255 bytes into `buffer`. This is a blocking receive, so the
 * function will block until the last byte is read from the USART receive buffer.
 * @param buffer Pointer to the buffer into which to receive.
 * @param length Number of bytes to receive (0 to 255).
 * @param sleep Boolean indicating whether to call `sleep_mode()` while waiting
 * for USART to receive data. Note: If `sleep` is `true` a wakeup source must be
 * enabled (such as Receive Complete Interrupt) which will wake the processor
 * to receive the next byte.
 */
void usartReceiveToBuffer(uint8_t *buffer, uint8_t length, bool sleep)
{
    uint8_t index = 0;
    while (index < length) {
        while (bit_is_clear(USART0.STATUS, USART_RXCIF_bp)) {
            if (sleep) {
                sleep_mode();
            }
        }
        buffer[index] = USART0.RXDATAL;
        index++;
    }
}