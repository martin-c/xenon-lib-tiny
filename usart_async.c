/*! \file
 *  usart_async.c
 *  xenon-lib-tiny
 *  Copyright (c) 2020 Martin Clemons
 */
#include "usart_async.h"
#include <stdbool.h>
#include <stdio.h>
#include "task_scheduler.h"
#include "futures.h"



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
static inline void init(future_t *f)
{
    f->resolved = false;
    f->promise->uint8 = 0;
}

static void sendBytes(cbParam_t *param)
{
    uint8_t *buff = param->buffer.buff;
    uint8_t index = param->buffer.iter;
    const uint8_t length = param->buffer.length;
    while (bit_is_set(USART0.STATUS, USART_DREIF_bp) &&
            index < length) {
        USART0.TXDATAL = buff[index];
        (index)++;
    }
    param->buffer.iter = index;
    if (index == length) {
        // transmission complete
        task_t *task = tsGetCurrentTask();
        tsRemoveTask(task);
        // resolve future
        future_t *f = (future_t *)task; // task is first member of future_s
        f->promise->uint8 = param->buffer.length;
        f->resolved = true;
    }
}

/*** Public Functions --------------------------------------------------------*/
/*! \publicsection */

/*! Check if USART0 receive buffer has one or more bytes available to read.
 * @param param Task scheduler parameter (not used).
 * @return Returns `true` if receive buffer should be read, `false` otherwise.
 */
bool usartHasData(cbParam_t *param)
{
    return USART0.STATUS & USART_RXCIF_bm ? true : false;
}

bool usartReadyForData(cbParam_t *param)
{
    return USART0.STATUS & USART_DREIF_bm ? true : false;
}
/*
enum addStatus_e usartAsyncSend(future_t *f, promise_t *promise,
        uint8_t *buffer, uint8_t length)
{

}*/