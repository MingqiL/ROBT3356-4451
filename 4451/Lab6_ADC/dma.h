/*
 * dma.h
 *
 *  Created on: Mar. 27, 2024
 *      Author: andrew_friesen
 */

#ifndef DMA_H_
#define DMA_H_

#define DMA0SRC

void configDMAx_ADC12(unsigned int sourceAddr, unsigned int *destAddr, unsigned int arraySize);
void configDMAy_ADC12(unsigned int sourceAddr, unsigned int *destAddr, unsigned int arraySize);



#endif /* DMA_H_ */
