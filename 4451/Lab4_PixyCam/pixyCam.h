/*************************************************************************************************
 * pixycam.h
 * - - C interface file for MSP430 UCSI UART A1, A0
 *
 *  Author: Mingqi Li
 *  Created on: March 1, 2017
 *  Modified: Feb 19, 2018
 **************************************************************************************************/

#ifndef PIXYCAM_H_
#define PIXYCAM_H_

#define MAIN_FEATURES 0
#define ALL_FEATURES 1
#define VECTORS 7
#define INTERSECTIONS 7
#define BARCODES 7

char pixyGetVersion(unsigned char slvAddr, unsigned char * rxBuffer);
char pixyGetVector(unsigned char slvAddr, unsigned char * rxBuffer);

#endif /* UART_LAB_H_ */
