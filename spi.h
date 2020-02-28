/*! \file
 *  spi.h
 *  60345-100-digi-dac-firmware
 *  Copyright (c) 2020 Martin Clemons
 *
 *  General configuration and utility functions for the SPI peripheral.
 */
#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <avr/io.h>


/*** Public Variables --------------------------------------------------------*/
/*! \publicsection */

/*! The SPI interface data order (MSB first or LSB first).
 */
enum spiDataOrder_e {
    SPI_DO_MSB_FIRST        = 0,            ///< MSB first
    SPI_DO_LSB_FIRST        = SPI_DORD_bm   ///< LSB first
};

/*! The SPI transfer mode. See 'SPI Data Transfer Modes' in device datasheet for more
 * info.
 */
enum spiTransferMode_e {
    SPI_XFER_MODE_0     = SPI_MODE_0_gc,    ///< Leading edge: Rising, sample Trailing edge: Falling, setup
    SPI_XFER_MODE_1     = SPI_MODE_1_gc,    ///< Leading edge: Rising, setup Trailing edge: Falling, sample
    SPI_XFER_MODE_2     = SPI_MODE_2_gc,    ///< Leading edge: Falling, sample Trailing edge: Rising, setup
    SPI_XFER_MODE_3     = SPI_MODE_3_gc,    ///< Leading edge: Falling, setup Trailing edge: Rising, sample
};

/*! The SPI interface clock mode, selects between normal speed and double speed
 */
enum spiClockMode_e {
    SPI_CLOCK_NORMAL        = 0,            ///< SPI clock is normal speed
    SPI_CLOCK_DOUBLE        = SPI_CLK2X_bm  ///< SPI clock is double speed
};

/*! The SPI interface clock prescale factor (divisor).
 */
enum spiPrescale_e {
    SPI_PRESCALE_DIV4       = SPI_PRESC_DIV4_gc,    ///< Prescale factor 4
    SPI_PRESCALE_DIV16      = SPI_PRESC_DIV16_gc,   ///< Prescale factor 16
    SPI_PRESCALE_DIV64      = SPI_PRESC_DIV64_gc,   ///< Prescale factor 64
    SPI_PRESCALE_DIV128     = SPI_PRESC_DIV128_gc   ///< Prescale factor 128
};

/*! The SPI interrupt configuration. Set values to true to enable interrupt.
 */
struct spiInterruptConfig_s {
    bool rxcInterruptEnable;    ///< Receive Complete Interrupt Enable (buffered mode only)
    bool txcInterruptEnable;    ///< Transfer Complete Interrupt Enable (buffered mode only)
    bool dreInterruptEnable;    ///< Data Register Empty Interrupt Enable (buffered mode only)
    bool ssInterruptEnable;     ///< Slave Select Trigger Interrupt Enable (buffered mode only)
    bool spiInterruptEnable;    ///< Generic SPI Interrupt Enable (non-buffered mode only)
};

/*! Configuration options for SPI in master mode.
 */
struct spiMasterConfig_s {
    enum spiDataOrder_e dataOrder;          ///< SPI data order
    enum spiTransferMode_e transferMode;    ///< SPI transfer mode (supports modes 0 through 3)
    enum spiClockMode_e clockMode;          ///< SPI clock mode
    enum spiPrescale_e prescale;            ///< SPI clock prescaler
};


/*** Public Functions --------------------------------------------------------*/
/*! \publicsection */

void spiConfigInterrupts(struct spiInterruptConfig_s *config);
void spiConfigMaster(struct spiMasterConfig_s *config);
void spiIo(uint8_t buf[], uint8_t len);
void spiIo_24(uint8_t buf[3]);
void spiIo_16(uint8_t buf[2]);
