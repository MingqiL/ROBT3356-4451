/*
 * dma.c
 *
 *  Created on: Mar. 27, 2024
 *      Author: andrew_friesen
 */
#include <msp430.h>
#include "dma.h"

void configDMAx_ADC12(unsigned int sourceAddr, unsigned int *destAddr, unsigned int arraySize)
{
//    DMA1CTL &= ~DMAEN;

    DMA0SA = sourceAddr;     // source address
    DMA0DA = destAddr; // destination address

    DMACTL0 |= DMA0TSEL__ADC12IFG;       // DMA1 trigger input
    DMA0SZ = arraySize;   // DMA0 transfer size (1 word)

    // single transfer, source increment, interrupt enable
    DMA0CTL = DMADT_0 | DMADSTINCR_3 | DMAEN;// | DMAIE;
}

void configDMAy_ADC12(unsigned int sourceAddr, unsigned int *destAddr, unsigned int arraySize)
{
//    DMA2CTL &= ~DMAEN;

    DMA2SA = sourceAddr;     // source address
    DMA2DA = destAddr; // destination address

    DMACTL1 |= DMA2TSEL__ADC12IFG;       // DMA2 trigger input
    DMA2SZ = arraySize;   // DMA0 transfer size (1 word)

    // single transfer, source increment, interrupt enable
    DMA2CTL = DMADT_0 | DMADSTINCR_3 | DMAEN;// | DMAIE;
}
