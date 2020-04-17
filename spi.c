/*! \file
 *  spi.c
 *  xenon-lib-tiny
 *  Copyright (c) 2020 Martin Clemons
 */
#include "spi.h"


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

/*! Configure the SPI interrupt mask bits.
 * @param config Pointer to `spiInterruptConfig_s` with the desired interrupts
 * to be enabled.
 * The RXC, TXC, DRE, and SS interrupts are only used when the SPI peripheral
 * is in buffered mode.
 * The generic SPI interrupt (IE bit) is only used when the SPI peripheral is
 * in non-buffered mode.
 * Note: Function does not clear any existing interrupt flags when enabling interrupts.
 */
void spiConfigInterrupts(struct spiInterruptConfig_s *config)
{
    SPI0.INTCTRL =
            (config->rxcInterruptEnable ? SPI_RXCIE_bm : 0)     |
            (config->txcInterruptEnable ? SPI_TXCIE_bm : 0)     |
            (config->dreInterruptEnable ? SPI_DREIE_bm : 0)     |
            (config->ssInterruptEnable ? SPI_SSIE_bm : 0)       |
            (config->spiInterruptEnable ? SPI_IE_bm : 0);
}

/*! Configure and enable the SPI peripheral in master SPI mode. Enables the hardware
 * tx/rx buffers in the peripheral.
 * @param config Pointer to `spiMasterConfig_s` with the desired configuration.
 * Note: function does not set the data direction of the MOSI, SCK, and !SS pins,
 * these must be set as outputs elsewhere.
 */
void spiConfigMaster(struct spiMasterConfig_s *config)
{
    // clear interrupt flags
    SPI0.INTFLAGS = SPI_RXCIF_bm | SPI_TXCIF_bm | SPI_DREIF_bm | SPI_SSIF_bm;
    // configure SPI peripheral
    SPI0.CTRLB = SPI_BUFEN_bm | SPI_SSD_bm | config->transferMode;
    // configure and enable
    SPI0.CTRLA = config->dataOrder | SPI_MASTER_bm | config->clockMode |
            config->prescale | SPI_ENABLE_bm;
    // make sure rx buffers don't have stale data
    SPI0.DATA;
    SPI0.DATA;
    SPI0.DATA;
}

/*! Send and receive data through the SPI interface.
 * This is a blocking send/receive, and the CS line must be asserted and released
 * externally. The function will not return until `len` bytes have been transmitted
 * and received. The received bytes are written into `buf`.
 * @param buf Buffer to send and receive from.
 * @param len Number of bytes to send and receive.
 * Note: This function assumes that the SPI peripheral is buffered: `BUFEN`
 * bit in `CTRLB` register is set.
 */
void spiIo(uint8_t buf[], uint8_t len)
{
    uint8_t tx_i = 0;       // transmit index
    uint8_t rx_i = 0;       // receive index
    while (tx_i < len || rx_i < len) {
        if (bit_is_set(SPI0.INTFLAGS, SPI_DREIF_bp) && tx_i < len) {
            SPI0.DATA = buf[tx_i];
            tx_i++;
        }
        if (bit_is_set(SPI0.INTFLAGS, SPI_RXCIF_bp) && rx_i < len) {
            buf[rx_i] = SPI0.DATA;
            rx_i++;
        }
    }
}

/*! Efficiently send and receive exactly 3 bytes (24 bits).
 * This is a blocking send/receive, and the CS line must be asserted and released
 * externally. The function will not return until 3 bytes have been transmitted
 * and received. The received bytes are written into `buf`.
 * This function is identical to `spiIo` except that it maximizes the use of
 * the SPI buffers and eliminates any internal counters and indexes, making
 * it slightly faster than using `spiIo` to transmit exactly 3 bytes (assuming
 * fast SPI clock frequency).
 * @param buf Buffer to send and receive from (exactly 3 bytes).
 * Note: This function assumes that the SPI peripheral is buffered: `BUFEN`
 * bit in `CTRLB` register is set.
 */
void spiIo_24(uint8_t buf[3])
{
    SPI0.DATA = buf[0];
    SPI0.DATA = buf[1];
    loop_until_bit_is_set(SPI0.INTFLAGS, SPI_DREIF_bp);
    buf[0] = SPI0.DATA;
    SPI0.INTFLAGS |= SPI_TXCIF_bm;
    SPI0.DATA = buf[2];
    loop_until_bit_is_set(SPI0.INTFLAGS, SPI_TXCIF_bp);
    buf[1] = SPI0.DATA;
    buf[2] = SPI0.DATA;
}

/*! Same as spiIo24 except bytes are sent and received reversed. Useful for
 * communicating with systems that have multi-bit variables that have the high
 * byte first.
 * @param buf Buffer to send and receive from (exactly 3 bytes).
 */
void spiIo_24_r(uint8_t buf[3])
{
    SPI0.DATA = buf[2];
    SPI0.DATA = buf[1];
    loop_until_bit_is_set(SPI0.INTFLAGS, SPI_DREIF_bp);
    buf[2] = SPI0.DATA;
    SPI0.INTFLAGS |= SPI_TXCIF_bm;
    SPI0.DATA = buf[0];
    loop_until_bit_is_set(SPI0.INTFLAGS, SPI_TXCIF_bp);
    buf[1] = SPI0.DATA;
    buf[0] = SPI0.DATA;
}

/*! Efficiently send and receive exactly 2 bytes (16 bits).
 * Same as `spiIo_24` except for 2 byte length.
 * @param buf Buffer to send and receive from (exactly 2 bytes).
 * Note: This function assumes that the SPI peripheral is buffered: `BUFEN`
 * bit in `CTRLB` register is set.
 */
void spiIo_16(uint8_t buf[2])
{
    SPI0.INTFLAGS |= SPI_TXCIF_bm;
    SPI0.DATA = buf[0];
    SPI0.DATA = buf[1];
    loop_until_bit_is_set(SPI0.INTFLAGS, SPI_DREIF_bp);
    buf[0] = SPI0.DATA;
    loop_until_bit_is_set(SPI0.INTFLAGS, SPI_TXCIF_bp);
    buf[1] = SPI0.DATA;
}
