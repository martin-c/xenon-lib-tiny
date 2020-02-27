/*! \file
 *  usart.c
 *  xenon-lib-tiny
 *  Copyright (c) 2020 Martin Clemons
 */
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
    USART0.CTRLA |=
            (config->rxcInterruptEnable ? USART_RXCIE_bm : 0)       |
            (config->txcInterruptEnable ? USART_TXCIE_bm : 0)       |
            (config->dreInterruptEnable ? USART_DREIE_bm : 0)       |
            (config->rxStartInterruptEnable ? USART_RXSIE_bm : 0)   |
            (config->autoBaudErrorInterruptEnable ? USART_ABEIE_bm : 0);
}

/*! Configure the USART peripheral in asynchronous serial mode
 * @param config Pointer to `usartAsyncSerialConfig_s` with the desired USART
 * configuration options.
 */
void usartConfigAsyncSerial(struct usartAsyncSerialConfig_s *config)
{
    // clear interrupt status bits
    USART0.STATUS = USART_TXCIF_bm | USART_RXSIF_bm | USART_ISFIF_bm | USART_ISFIF_bm |
            USART_BDF_bm;
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
    while (bit_is_clear(USART0.STATUS, USART_DREIE_bp)) { }
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
